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
#include <dt-bindings/clock/sun4i-a10-ccu.h>
#include <dt-bindings/reset/sun4i-a10-ccu.h>

static struct ccu_clk_map a10_clks[] = {
	[CLK_AHB_OTG]		= { 0x060, BIT(0), NULL },
	[CLK_AHB_EHCI0]		= { 0x060, BIT(1), NULL },
	[CLK_AHB_OHCI0]		= { 0x060, BIT(2), NULL },
	[CLK_AHB_EHCI1]		= { 0x060, BIT(3), NULL },
	[CLK_AHB_OHCI1]		= { 0x060, BIT(4), NULL },
	[CLK_AHB_MMC0]		= { 0x060, BIT(8), NULL },
	[CLK_AHB_MMC1]		= { 0x060, BIT(9), NULL },
	[CLK_AHB_MMC2]		= { 0x060, BIT(10), NULL },
	[CLK_AHB_MMC3]		= { 0x060, BIT(11), NULL },

	[CLK_USB_OHCI0]		= { 0x0cc, BIT(6), NULL },
	[CLK_USB_OHCI1]		= { 0x0cc, BIT(7), NULL },
	[CLK_USB_PHY]		= { 0x0cc, BIT(8), NULL },
};

static struct ccu_reset_map a10_resets[] = {
	[RST_USB_PHY0]		= { 0x0cc, BIT(0) },
	[RST_USB_PHY1]		= { 0x0cc, BIT(1) },
	[RST_USB_PHY2]		= { 0x0cc, BIT(2) },
};

static const struct ccu_desc sun4i_a10_ccu_desc = {
	.clks = a10_clks,
	.num_clks = ARRAY_SIZE(a10_clks),

	.resets = a10_resets,
	.num_resets =  ARRAY_SIZE(a10_resets),
};

static int a10_clk_probe(struct udevice *dev)
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

static int a10_clk_bind(struct udevice *dev)
{
	return sunxi_reset_bind(dev, 22);
}

static const struct udevice_id a10_clk_ids[] = {
	{ .compatible = "allwinner,sun4i-a10-ccu",
          .data = (ulong)&sun4i_a10_ccu_desc },
	{ .compatible = "allwinner,sun7i-a20-ccu",
          .data = (ulong)&sun4i_a10_ccu_desc },
	{ }
};

U_BOOT_DRIVER(clk_sun4i_a10) = {
	.name		= "sun4i_a10_ccu",
	.id		= UCLASS_CLK,
	.of_match	= a10_clk_ids,
	.priv_auto_alloc_size	= sizeof(struct sunxi_clk_priv),
	.ops		= &sunxi_clk_ops,
	.probe		= a10_clk_probe,
	.bind		= a10_clk_bind,
};
