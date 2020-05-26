// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <dm.h>
#include <asm/arch-rockchip/periph.h>
#include <power/regulator.h>
#include <spl_gpio.h>
#include <asm/io.h>
#include <asm/arch-rockchip/gpio.h>

#ifndef CONFIG_SPL_BUILD
int board_early_init_f(void)
{
	struct udevice *regulator;
	int ret;

	ret = regulator_get_by_platname("vcc5v0_host", &regulator);
	if (ret) {
		debug("%s vcc5v0_host init fail! ret %d\n", __func__, ret);
		goto out;
	}

	ret = regulator_set_enable(regulator, true);
	if (ret)
		debug("%s vcc5v0-host-en set fail! ret %d\n", __func__, ret);
out:
	return 0;
}
#endif

#if defined(CONFIG_SPL_BUILD)

#include <i2c.h>
#include <asm/arch-rockchip/cru.h>
#include <asm/arch-rockchip/grf_rk3399.h>

#define BUS_NUM				2
#define ROC_RK3399_MEZZ_BAT_ADDR	0x62
#define PMUGRF_BASE			0xff320000
#define GPIO0_BASE			0xff720000

enum roc_rk3399_pc_board_type {
       ROC_RK3399_PC,                  /* roc-rk3399-pc base board */
       ROC_RK3399_MEZZ_M2_POE,         /* roc-rk3399-Mezz M.2 PoE */
};

void board_early_led_setup(void)
{
        struct rockchip_gpio_regs * const gpio0 = (void *)GPIO0_BASE;
        struct rk3399_pmugrf_regs * const pmugrf = (void *)PMUGRF_BASE;

        /* 1. Glow yellow LED, termed as low power */
        spl_gpio_output(gpio0, GPIO(BANK_A, 2), 1);

        /* 2. Poll for on board power key press */
        spl_gpio_set_pull(&pmugrf->gpio0_p, GPIO(BANK_A, 5), GPIO_PULL_NORMAL);
        while (readl(&gpio0->ext_port) & 0x20);

        /* 3. Once 2 done, turn off yellow */
        spl_gpio_output(gpio0, GPIO(BANK_A, 2), 0);
}

int board_early_init_f(void)
{
	struct udevice *bus, *dev;
	int ret;
	struct rockchip_gpio_regs * const gpio0 = (void *)GPIO0_BASE;

	/* default board type */
	gd->board_type = ROC_RK3399_PC;

	/* Set the low power leds, power key only during POR */
        if (!strcmp(get_reset_cause(), "POR"))
                board_early_led_setup();
	/* 4. Turn on red LED, indicating full power mode */
        spl_gpio_output(gpio0, GPIO(BANK_B, 5), 1);

	ret = uclass_get_device_by_seq(UCLASS_I2C, BUS_NUM, &bus);
	if (ret) {
		debug("failed to get i2c bus 2\n");
		return ret;
	}

	ret = dm_i2c_probe(bus, ROC_RK3399_MEZZ_BAT_ADDR, 0, &dev);
	if (ret) {
		debug("failed to probe i2c2 battery controller IC\n");
		return ret;
	}

	gd->board_type = ROC_RK3399_MEZZ_M2_POE;

	return 0;
}

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	if (gd->board_type == ROC_RK3399_PC) {
		if (!strcmp(name, "rk3399-roc-pc.dtb"))
			return 0;
	}

	if (gd->board_type == ROC_RK3399_MEZZ_M2_POE) {
		if (!strcmp(name, "rk3399-roc-pc-mezzanine.dtb"))
			return 0;
	}

	return -EINVAL;
}
#endif

#endif /* CONFIG_SPL_BUILD */
