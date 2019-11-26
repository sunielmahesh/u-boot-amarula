// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2016 Amarula Solutions B.V.
 * Copyright (C) 2016 Engicam S.r.l.
 * Author: Jagan Teki <jagan@amarulasolutions.com>
 */

#include <common.h>
#include <env.h>
#include <mmc.h>
#include <asm/arch/sys_proto.h>
#include <watchdog.h>

#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_ENV_IS_IN_MMC
static void mmc_late_init(void)
{
	char cmd[32];
	char mmcblk[32];
	u32 dev_no = mmc_get_env_dev();

	env_set_ulong("mmcdev", dev_no);

	/* Set mmcblk env */
	sprintf(mmcblk, "/dev/mmcblk%dp2 rootwait rw", dev_no);
	env_set("mmcroot", mmcblk);

	sprintf(cmd, "mmc dev %d", dev_no);
	run_command(cmd, 0);
}
#endif

static void setenv_fdt_file(void)
{
	const char *cmp_dtb = CONFIG_DEFAULT_DEVICE_TREE;

	if (!strcmp(cmp_dtb, "imx6q-icore")) {
		if (is_mx6dq())
			env_set("fdt_file", "imx6q-icore.dtb");
		else if (is_mx6dl() || is_mx6solo())
			env_set("fdt_file", "imx6dl-icore.dtb");
	} else if (!strcmp(cmp_dtb, "imx6q-icore-mipi")) {
		if (is_mx6dq())
			env_set("fdt_file", "imx6q-icore-mipi.dtb");
		else if (is_mx6dl() || is_mx6solo())
			env_set("fdt_file", "imx6dl-icore-mipi.dtb");
	} else if (!strcmp(cmp_dtb, "imx6q-icore-rqs")) {
		if (is_mx6dq())
			env_set("fdt_file", "imx6q-icore-rqs.dtb");
		else if (is_mx6dl() || is_mx6solo())
			env_set("fdt_file", "imx6dl-icore-rqs.dtb");
	} else if (!strcmp(cmp_dtb, "imx6ul-geam"))
		env_set("fdt_file", "imx6ul-geam.dtb");
	else if (!strcmp(cmp_dtb, "imx6ul-isiot-emmc"))
		env_set("fdt_file", "imx6ul-isiot-emmc.dtb");
	else if (!strcmp(cmp_dtb, "imx6ul-isiot-nand"))
		env_set("fdt_file", "imx6ul-isiot-nand.dtb");
}

#ifdef CONFIG_BOARD_SPECIFIC_OPTIONS

/**
 * Boot syslinux conf types
 *
 * SYSLINUX_CONF_DEFAULT	: Default boot syslinux
 * SYSLINUX_CONF_ROLLBACK	: For redundent boot syslinux
 * SYSLINUX_CONF_A		: Boot syslinux for partition A type
 * SYSLINUX_CONF_B		: Boot syslinux for partition B type
 */
enum boot_conf_type {
	SYSLINUX_CONF_DEFAULT	= 0,
	SYSLINUX_CONF_ROLLBACK	= 1,
	SYSLINUX_CONF_A		= 2,
	SYSLINUX_CONF_B		= 3,
};

enum upgrade_type {
	UPGRADE_NO_NEED	= 0,
	UPGRADE_DONE	= 1,
};

static void upgrade_switch_boot_part(int conf_type)
{
	char *syslinux_conf = NULL;

	switch (conf_type) {
	case SYSLINUX_CONF_DEFAULT:
		syslinux_conf = "extlinux/extlinux.conf";
		break;
	case SYSLINUX_CONF_ROLLBACK:
		syslinux_conf = "extlinux-rollback/extlinux-rollback.conf";
		break;
	case SYSLINUX_CONF_A:
		syslinux_conf = "extlinux-a/extlinux-a.conf";
		break;
	case SYSLINUX_CONF_B:
		syslinux_conf = "extlinux-b/extlinux-b.conf";
		break;
	}

	env_set("boot_syslinux_conf", syslinux_conf);
}

static void swap_boot_conf_type(bool is_switch_conf)
{
	int conf_type = env_get_ulong("boot_syslinux_conf_type", 10, 0);
	int new_conf_type = conf_type;

	/* for non-upgrade cases, just boot existing boot_syslinux */
	if (!is_switch_conf) {
		upgrade_switch_boot_part(new_conf_type);
		return;
	}

	switch (conf_type) {
	case SYSLINUX_CONF_A:
		new_conf_type = SYSLINUX_CONF_B;
		break;
	case SYSLINUX_CONF_B:
		new_conf_type = SYSLINUX_CONF_A;
		break;
	case SYSLINUX_CONF_DEFAULT:
		new_conf_type = SYSLINUX_CONF_ROLLBACK;
		break;
	default:
		new_conf_type = SYSLINUX_CONF_DEFAULT;
		printf("WARNING: Unknown boot_conf_type %d\n", conf_type);
	}

	upgrade_switch_boot_part(new_conf_type);
	env_set_ulong("boot_syslinux_conf_type", new_conf_type);
	env_save();
}

static void upgrade_bootcount_variable(void)
{
	int upgrade = env_get_ulong("upgrade_available", 10, 0);
	unsigned long bootcount = bootcount_load();
	unsigned long bootlimit = env_get_ulong("bootlimit", 10, 0);
	bool is_switch_conf = false;

	switch (upgrade) {
	case UPGRADE_DONE:
		bootcount_store(0);
		env_set_ulong("upgrade_available", UPGRADE_NO_NEED);
		env_save();
		is_switch_conf = true;
		break;
	case UPGRADE_NO_NEED:
		if (bootlimit && bootcount == bootlimit)
			is_switch_conf = true;
		else if (bootlimit && bootcount > bootlimit)
			bootcount_store(0);
		debug("%s: No upgrade action needed!\n", __func__);
	}

	swap_boot_conf_type(is_switch_conf);
}
#endif

int board_late_init(void)
{
	switch ((imx6_src_get_boot_mode() & IMX6_BMODE_MASK) >>
			IMX6_BMODE_SHIFT) {
	case IMX6_BMODE_SD:
	case IMX6_BMODE_ESD:
	case IMX6_BMODE_MMC:
	case IMX6_BMODE_EMMC:
#ifdef CONFIG_ENV_IS_IN_MMC
		mmc_late_init();
#endif
		env_set("modeboot", "mmcboot");
		break;
	case IMX6_BMODE_NAND_MIN ... IMX6_BMODE_NAND_MAX:
		env_set("modeboot", "nandboot");
		break;
	default:
		env_set("modeboot", "");
		break;
	}

	if (is_mx6ul())
		env_set("console", "ttymxc0");
	else
		env_set("console", "ttymxc3");

	setenv_fdt_file();

#ifdef CONFIG_HW_WATCHDOG
	hw_watchdog_init();
#endif

#ifdef CONFIG_BOARD_SPECIFIC_OPTIONS
	upgrade_bootcount_variable();
#endif

	return 0;
}

int board_init(void)
{
	/* Address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_NAND_MXS
	setup_gpmi_nand();
#endif

#ifdef CONFIG_VIDEO_IPUV3
	setup_display();
#endif

	return 0;
}

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();

	return 0;
}
