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
#include <dt-bindings/clock/sun8i-r40-ccu.h>
#include <dt-bindings/reset/sun8i-r40-ccu.h>

static struct ccu_clk_map r40_clks[] = {
	[CLK_BUS_OTG]		= { 0x060, BIT(25), NULL },
	[CLK_BUS_EHCI0]		= { 0x060, BIT(26), NULL },
	[CLK_BUS_EHCI1]		= { 0x060, BIT(27), NULL },
	[CLK_BUS_EHCI2]		= { 0x060, BIT(28), NULL },
	[CLK_BUS_OHCI0]		= { 0x060, BIT(29), NULL },
	[CLK_BUS_OHCI1]		= { 0x060, BIT(30), NULL },
	[CLK_BUS_OHCI2]		= { 0x060, BIT(31), NULL },


	[CLK_USB_PHY0]		= { 0x0cc, BIT(8), NULL },
	[CLK_USB_PHY1]		= { 0x0cc, BIT(9), NULL },
	[CLK_USB_PHY2]		= { 0x0cc, BIT(10), NULL },
	[CLK_USB_OHCI0]		= { 0x0cc, BIT(16), NULL },
	[CLK_USB_OHCI1]		= { 0x0cc, BIT(17), NULL },
	[CLK_USB_OHCI2]		= { 0x0cc, BIT(18), NULL },
};

static struct ccu_reset_map r40_resets[] = {
	[RST_USB_PHY0]		= { 0x0cc, BIT(0) },
	[RST_USB_PHY1]		= { 0x0cc, BIT(1) },
	[RST_USB_PHY2]		= { 0x0cc, BIT(2) },

	[RST_BUS_OTG]		= { 0x2c0, BIT(25) },
	[RST_BUS_EHCI0]		= { 0x2c0, BIT(26) },
	[RST_BUS_EHCI1]		= { 0x2c0, BIT(27) },
	[RST_BUS_EHCI2]		= { 0x2c0, BIT(28) },
	[RST_BUS_OHCI0]		= { 0x2c0, BIT(29) },
	[RST_BUS_OHCI1]		= { 0x2c0, BIT(30) },
	[RST_BUS_OHCI2]		= { 0x2c0, BIT(31) },
};

static const struct ccu_desc sun8i_r40_ccu_desc = {
	.clks = r40_clks,
	.num_clks = ARRAY_SIZE(r40_clks),

	.resets = r40_resets,
	.num_resets =  ARRAY_SIZE(r40_resets),
};

static int r40_clk_probe(struct udevice *dev)
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

static int r40_clk_bind(struct udevice *dev)
{
	return sunxi_reset_bind(dev, 80);
}

static const struct udevice_id r40_clk_ids[] = {
	{ .compatible = "allwinner,sun8i-r40-ccu",
	  .data = (ulong)&sun8i_r40_ccu_desc },
	{ }
};

U_BOOT_DRIVER(clk_sun8i_r40) = {
	.name		= "sun8i_r40_ccu",
	.id		= UCLASS_CLK,
	.of_match	= r40_clk_ids,
	.priv_auto_alloc_size	= sizeof(struct sunxi_clk_priv),
	.ops		= &sunxi_clk_ops,
	.probe		= r40_clk_probe,
	.bind		= r40_clk_bind,
};
