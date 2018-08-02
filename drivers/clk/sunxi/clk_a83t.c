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
#include <dt-bindings/clock/sun8i-a83t-ccu.h>
#include <dt-bindings/reset/sun8i-a83t-ccu.h>

static struct ccu_clk_map a83t_clks[] = {
	[CLK_BUS_OTG]		= { 0x060, BIT(24), NULL },
	[CLK_BUS_EHCI0]		= { 0x060, BIT(26), NULL },
	[CLK_BUS_EHCI1]		= { 0x060, BIT(27), NULL },
	[CLK_BUS_OHCI0]		= { 0x060, BIT(29), NULL },

	[CLK_USB_PHY0]		= { 0x0cc, BIT(8), NULL },
	[CLK_USB_PHY1]		= { 0x0cc, BIT(9), NULL },
	[CLK_USB_HSIC]		= { 0x0cc, BIT(10), NULL },
	[CLK_USB_HSIC_12M]	= { 0x0cc, BIT(11), NULL },
	[CLK_USB_OHCI0]		= { 0x0cc, BIT(16), NULL },
};

static struct ccu_reset_map a83t_resets[] = {
	[RST_USB_PHY0]		= { 0x0cc, BIT(0) },
	[RST_USB_PHY1]		= { 0x0cc, BIT(1) },
	[RST_USB_HSIC]		= { 0x0cc, BIT(2) },

	[RST_BUS_OTG]		= { 0x2c0, BIT(24) },
	[RST_BUS_EHCI0]		= { 0x2c0, BIT(26) },
	[RST_BUS_EHCI1]		= { 0x2c0, BIT(27) },
	[RST_BUS_OHCI0]		= { 0x2c0, BIT(29) },
};

static const struct ccu_desc sun8i_a83t_ccu_desc = {
	.clks = a83t_clks,
	.num_clks = ARRAY_SIZE(a83t_clks),

	.resets = a83t_resets,
	.num_resets =  ARRAY_SIZE(a83t_resets),
};

static int a83t_clk_probe(struct udevice *dev)
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

static int a83t_clk_bind(struct udevice *dev)
{
	return sunxi_reset_bind(dev, 44);
}

static const struct udevice_id a83t_clk_ids[] = {
	{ .compatible = "allwinner,sun8i-a83t-ccu",
	  .data = (ulong)&sun8i_a83t_ccu_desc },
	{ }
};

U_BOOT_DRIVER(clk_sun8i_a83t) = {
	.name		= "sun8i_a83t_ccu",
	.id		= UCLASS_CLK,
	.of_match	= a83t_clk_ids,
	.priv_auto_alloc_size	= sizeof(struct sunxi_clk_priv),
	.ops		= &sunxi_clk_ops,
	.probe		= a83t_clk_probe,
	.bind		= a83t_clk_bind,
};
