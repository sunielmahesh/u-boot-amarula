// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (C) 2018 Amarula Solutions.
 * Author: Jagan Teki <jagan@amarulasolutions.com>
 */

#include <common.h>
#include <clk-uclass.h>
#include <dm.h>
#include <errno.h>
#include <asm/arch/ccu.h>
#include <dt-bindings/clock/sun5i-ccu.h>
#include <dt-bindings/reset/sun5i-ccu.h>

static struct ccu_clk_map a10s_clks[] = {
	[CLK_AHB_OTG]		= { 0x060, BIT(0), NULL },
	[CLK_AHB_EHCI]		= { 0x060, BIT(1), NULL },
	[CLK_AHB_OHCI]		= { 0x060, BIT(2), NULL },
	[CLK_AHB_MMC0]		= { 0x060, BIT(8), NULL },
	[CLK_AHB_MMC1]		= { 0x060, BIT(9), NULL },
	[CLK_AHB_MMC2]		= { 0x060, BIT(10), NULL },

#ifdef CONFIG_MMC
	[CLK_MMC0]		= { 0x088, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC1]		= { 0x08c, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC2]		= { 0x090, BIT(31), &mmc_clk_set_rate },
#endif

	[CLK_USB_OHCI]		= { 0x0cc, BIT(6), NULL },
	[CLK_USB_PHY0]		= { 0x0cc, BIT(8), NULL },
	[CLK_USB_PHY1]		= { 0x0cc, BIT(9), NULL },
};

static struct ccu_reset_map a10s_resets[] = {
	[RST_USB_PHY0]		=  { 0x0cc, BIT(0) },
	[RST_USB_PHY1]		=  { 0x0cc, BIT(1) },
};

static const struct ccu_desc sun5i_a10s_ccu_desc = {
	.clks = a10s_clks,
	.num_clks = ARRAY_SIZE(a10s_clks),

	.resets = a10s_resets,
	.num_resets =  ARRAY_SIZE(a10s_resets),
};

static int a10s_clk_probe(struct udevice *dev)
{
	struct sunxi_clk_priv *priv = dev_get_priv(dev);

	priv->base = dev_read_addr_ptr(dev);
	if (!priv->base)
		return -ENOMEM;

	priv->desc = (const struct ccu_desc *)dev_get_driver_data(dev);
	if (!priv->desc)
		return -EINVAL;

	return 0;
}

static int a10s_clk_bind(struct udevice *dev)
{
	return sunxi_reset_bind(dev, 10);
}

static const struct udevice_id a10s_clk_ids[] = {
	{ .compatible = "allwinner,sun5i-a10s-ccu",
          .data = (ulong)&sun5i_a10s_ccu_desc },
	{ .compatible = "allwinner,sun5i-a13-ccu",
          .data = (ulong)&sun5i_a10s_ccu_desc },
	{ }
};

U_BOOT_DRIVER(clk_sun5i_a10s) = {
	.name		= "sun5i_a10s_ccu",
	.id		= UCLASS_CLK,
	.of_match	= a10s_clk_ids,
	.priv_auto_alloc_size	= sizeof(struct sunxi_clk_priv),
	.ops		= &sunxi_clk_ops,
	.probe		= a10s_clk_probe,
	.bind		= a10s_clk_bind,
};
