// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2017 Theobroma Systems Design und Consulting GmbH
 */

#include <common.h>
#include <dm.h>
#include <env.h>
#include <init.h>
#include <log.h>
#include <misc.h>
#include <spl.h>
#include <syscon.h>
#include <u-boot/crc.h>
#include <usb.h>
#include <dm/pinctrl.h>
#include <dm/uclass-internal.h>
#include <asm/io.h>
#include <asm/setup.h>
#include <asm/arch-rockchip/clock.h>
#include <asm/arch-rockchip/hardware.h>
#include <asm/arch-rockchip/grf_rk3399.h>
#include <asm/arch-rockchip/periph.h>
#include <asm/arch-rockchip/misc.h>
#include <power/regulator.h>
#include <u-boot/sha256.h>

static void setup_iodomain(void)
{
	const u32 GRF_IO_VSEL_GPIO4CD_SHIFT = 3;
	struct rk3399_grf_regs *grf =
	    syscon_get_first_range(ROCKCHIP_SYSCON_GRF);

	/*
	 * Set bit 3 in GRF_IO_VSEL so PCIE_RST# works (pin GPIO4_C6).
	 * Linux assumes that PCIE_RST# works out of the box as it probes
	 * PCIe before loading the iodomain driver.
	 */
	rk_setreg(&grf->io_vsel, 1 << GRF_IO_VSEL_GPIO4CD_SHIFT);
}

/*
 * Swap mmc0 and mmc1 in boot_targets if booted from SD-Card.
 *
 * If bootsource is uSD-card we can assume that we want to use the
 * SD-Card instead of the eMMC as first boot_target for distroboot.
 * We only want to swap the defaults and not any custom environment a
 * user has set. We exit early if a changed boot_targets environment
 * is detected.
 */
static int setup_boottargets(void)
{
	const char *boot_device =
		ofnode_read_chosen_string("u-boot,spl-boot-device");
	char *env_default, *env;

	if (!boot_device) {
		debug("%s: /chosen/u-boot,spl-boot-device not set\n",
		      __func__);
		return -1;
	}
	debug("%s: booted from %s\n", __func__, boot_device);

	env_default = env_get_default("boot_targets");
	env = env_get("boot_targets");
	if (!env) {
		debug("%s: boot_targets does not exist\n", __func__);
		return -1;
	}
	debug("%s: boot_targets current: %s - default: %s\n",
	      __func__, env, env_default);

	if (strcmp(env_default, env) != 0) {
		debug("%s: boot_targets not default, don't change it\n",
		      __func__);
		return 0;
	}

	/*
	 * Only run, if booting from mmc1 (i.e. /mmc@fe320000) and
	 * only consider cases where the default boot-order first
	 * tries to boot from mmc0 (eMMC) and then from mmc1
	 * (i.e. external SD).
	 *
	 * In other words: the SD card will be moved to earlier in the
	 * order, if U-Boot was also loaded from the SD-card.
	 */
	if (!strcmp(boot_device, "/mmc@fe320000")) {
		char *mmc0, *mmc1;

		debug("%s: booted from SD-Card\n", __func__);
		mmc0 = strstr(env, "mmc0");
		mmc1 = strstr(env, "mmc1");

		if (!mmc0 || !mmc1) {
			debug("%s: only one mmc boot_target found\n", __func__);
			return -1;
		}

		/*
		 * If mmc0 comes first in the boot order, we need to change
		 * the strings to make mmc1 first.
		 */
		if (mmc0 < mmc1) {
			mmc0[3] = '1';
			mmc1[3] = '0';
			debug("%s: set boot_targets to: %s\n", __func__, env);
			env_set("boot_targets", env);
		}
	}

	return 0;
}

int misc_init_r(void)
{
	const u32 cpuid_offset = 0x7;
	const u32 cpuid_length = 0x10;
	u8 cpuid[cpuid_length];
	int ret;

	ret = rockchip_cpuid_from_efuse(cpuid_offset, cpuid_length, cpuid);
	if (ret)
		return ret;

	ret = rockchip_cpuid_set(cpuid, cpuid_length);
	if (ret)
		return ret;

	ret = rockchip_setup_macaddr();
	if (ret)
		return ret;

	setup_iodomain();
	setup_boottargets();

	return 0;
}

#ifdef CONFIG_SERIAL_TAG
void get_board_serial(struct tag_serialnr *serialnr)
{
	char *serial_string;
	u64 serial = 0;

	serial_string = env_get("serial#");

	if (serial_string)
		serial = simple_strtoull(serial_string, NULL, 16);

	serialnr->high = (u32)(serial >> 32);
	serialnr->low = (u32)(serial & 0xffffffff);
}
#endif

#if defined(CONFIG_SPL_BUILD)

#if defined(SPL_GPIO_SUPPORT)
static void rk3399_force_power_on_reset(void)
{
	ofnode node;
	struct gpio_desc sysreset_gpio;

	debug("%s: trying to force a power-on reset\n", __func__);

	node = ofnode_path("/config");
	if (!ofnode_valid(node)) {
		debug("%s: no /config node?\n", __func__);
		return;
	}

	if (gpio_request_by_name_nodev(node, "sysreset-gpio", 0,
				       &sysreset_gpio, GPIOD_IS_OUT)) {
		debug("%s: could not find a /config/sysreset-gpio\n", __func__);
		return;
	}

	dm_gpio_set_value(&sysreset_gpio, 1);
}
#endif

void rk_spl_board_init(void)
{
#if defined(SPL_GPIO_SUPPORT)
	struct rockchip_cru *cru = rockchip_get_cru();

	/*
	 * The RK3399 resets only 'almost all logic' (see also in the TRM
	 * "3.9.4 Global software reset"), when issuing a software reset.
	 * This may cause issues during boot-up for some configurations of
	 * the application software stack.
	 *
	 * To work around this, we test whether the last reset reason was
	 * a power-on reset and (if not) issue an overtemp-reset to reset
	 * the entire module.
	 *
	 * While this was previously fixed by modifying the various places
	 * that could generate a software reset (e.g. U-Boot's sysreset
	 * driver, the ATF or Linux), we now have it here to ensure that
	 * we no longer have to track this through the various components.
	 */
	if (cru->glb_rst_st != 0)
		rk3399_force_power_on_reset();
#endif

#if defined(SPL_DM_REGULATOR)
	/*
	 * Turning the eMMC and SPI back on (if disabled via the Qseven
	 * BIOS_ENABLE) signal is done through a always-on regulator).
	 */
	if (regulators_enable_boot_on(false))
		debug("%s: Cannot enable boot on regulator\n", __func__);
#endif
}
#endif /* CONFIG_SPL_BUILD */
