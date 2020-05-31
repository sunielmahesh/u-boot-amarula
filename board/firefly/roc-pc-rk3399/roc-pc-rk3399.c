// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <dm.h>
#include <log.h>
#include <asm/arch-rockchip/periph.h>
#include <power/regulator.h>
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

#ifdef CONFIG_SPL_BOARD_INIT
#include <spl.h>

#ifdef CONFIG_SPL_GPIO_SUPPORT
#include <spl_gpio.h>

#define GPIO0_BASE      0xff720000

static void led_setup(void)
{
	struct rockchip_gpio_regs * const gpio0 = (void *)GPIO0_BASE;

	spl_gpio_output(gpio0, GPIO(BANK_B, 5), 1);
}
#endif /* CONFIG_SPL_GPIO_SUPPORT */

void spl_board_init(void)
{
#ifdef CONFIG_SPL_GPIO_SUPPORT
	led_setup();
#endif

	preloader_console_init();
}

#endif /* CONFIG_SPL_BOARD_INIT */
#endif /* CONFIG_SPL_BUILD */
