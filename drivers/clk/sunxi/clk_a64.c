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
#include <dt-bindings/clock/sun50i-a64-ccu.h>

static struct ccu_clk_map a64_clks[] = {
	[CLK_BUS_OTG]		= { 0x060, BIT(23), NULL },
	[CLK_BUS_EHCI0]		= { 0x060, BIT(24), NULL },
	[CLK_BUS_EHCI1]		= { 0x060, BIT(25), NULL },
	[CLK_BUS_OHCI0]		= { 0x060, BIT(28), NULL },
	[CLK_BUS_OHCI1]		= { 0x060, BIT(29), NULL },

	[CLK_USB_PHY0]		= { 0x0cc, BIT(8), NULL },
	[CLK_USB_PHY1]		= { 0x0cc, BIT(9), NULL },
	[CLK_USB_HSIC]		= { 0x0cc, BIT(10), NULL },
	[CLK_USB_HSIC_12M]	= { 0x0cc, BIT(11), NULL },
	[CLK_USB_OHCI0]		= { 0x0cc, BIT(16), NULL },
	[CLK_USB_OHCI1]		= { 0x0cc, BIT(17), NULL },
};

static const struct ccu_desc sun50i_a64_ccu_desc = {
	.clks = a64_clks,
	.num_clks = ARRAY_SIZE(a64_clks),
};

static int a64_clk_probe(struct udevice *dev)
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

static const struct udevice_id a64_clk_ids[] = {
	{ .compatible = "allwinner,sun50i-a64-ccu",
	  .data = (ulong)&sun50i_a64_ccu_desc },
	{ }
};

U_BOOT_DRIVER(clk_sun50i_a64) = {
	.name		= "sun50i_a64_ccu",
	.id		= UCLASS_CLK,
	.of_match	= a64_clk_ids,
	.priv_auto_alloc_size	= sizeof(struct sunxi_clk_priv),
	.ops		= &sunxi_clk_ops,
	.probe		= a64_clk_probe,
};
