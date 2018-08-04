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
#include <dt-bindings/clock/sun8i-h3-ccu.h>
#include <dt-bindings/reset/sun8i-h3-ccu.h>

static struct ccu_clk_map h3_clks[] = {
	[CLK_BUS_MMC0]		= { 0x060, BIT(8), NULL },
	[CLK_BUS_MMC1]		= { 0x060, BIT(9), NULL },
	[CLK_BUS_MMC2]		= { 0x060, BIT(10), NULL },
	[CLK_BUS_SPI0]		= { 0x060, BIT(20), NULL },
	[CLK_BUS_SPI1]		= { 0x060, BIT(21), NULL },
	[CLK_BUS_OTG]		= { 0x060, BIT(23), NULL },
	[CLK_BUS_EHCI0]		= { 0x060, BIT(24), NULL },
	[CLK_BUS_EHCI1]		= { 0x060, BIT(25), NULL },
	[CLK_BUS_EHCI2]		= { 0x060, BIT(26), NULL },
	[CLK_BUS_EHCI3]		= { 0x060, BIT(27), NULL },
	[CLK_BUS_OHCI0]		= { 0x060, BIT(28), NULL },
	[CLK_BUS_OHCI1]		= { 0x060, BIT(29), NULL },
	[CLK_BUS_OHCI2]		= { 0x060, BIT(30), NULL },
	[CLK_BUS_OHCI3]		= { 0x060, BIT(31), NULL },

	[CLK_MMC0]		= { 0x088, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC1]		= { 0x08c, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC2]		= { 0x090, BIT(31), &mmc_clk_set_rate },

	[CLK_SPI0]		= { 0x0a0, BIT(31), NULL },
	[CLK_SPI1]		= { 0x0a4, BIT(31), NULL },

	[CLK_USB_PHY0]		= { 0x0cc, BIT(8), NULL },
	[CLK_USB_PHY1]		= { 0x0cc, BIT(9), NULL },
	[CLK_USB_PHY2]		= { 0x0cc, BIT(10), NULL },
	[CLK_USB_PHY3]		= { 0x0cc, BIT(11), NULL },
	[CLK_USB_OHCI0]		= { 0x0cc, BIT(16), NULL },
	[CLK_USB_OHCI1]		= { 0x0cc, BIT(17), NULL },
	[CLK_USB_OHCI2]		= { 0x0cc, BIT(18), NULL },
	[CLK_USB_OHCI3]		= { 0x0cc, BIT(19), NULL },
};

static struct ccu_reset_map h3_resets[] = {
	[RST_USB_PHY0]		=  { 0x0cc, BIT(0) },
	[RST_USB_PHY1]		=  { 0x0cc, BIT(1) },
	[RST_USB_PHY2]		=  { 0x0cc, BIT(2) },
	[RST_USB_PHY3]		=  { 0x0cc, BIT(3) },

	[RST_BUS_MMC0]		=  { 0x2c0, BIT(8) },
	[RST_BUS_MMC1]		=  { 0x2c0, BIT(9) },
	[RST_BUS_MMC2]		=  { 0x2c0, BIT(10) },
	[RST_BUS_SPI0]		=  { 0x2c0, BIT(20) },
	[RST_BUS_SPI1]		=  { 0x2c0, BIT(21) },
	[RST_BUS_OTG]		=  { 0x2c0, BIT(23) },
	[RST_BUS_EHCI0]		=  { 0x2c0, BIT(24) },
	[RST_BUS_EHCI1]		=  { 0x2c0, BIT(25) },
	[RST_BUS_EHCI2]		=  { 0x2c0, BIT(26) },
	[RST_BUS_EHCI3]		=  { 0x2c0, BIT(27) },
	[RST_BUS_OHCI0]		=  { 0x2c0, BIT(28) },
	[RST_BUS_OHCI1]		=  { 0x2c0, BIT(29) },
	[RST_BUS_OHCI2]		=  { 0x2c0, BIT(30) },
	[RST_BUS_OHCI3]		=  { 0x2c0, BIT(31) },
};

static const struct ccu_desc sun8i_h3_ccu_desc = {
	.clks = h3_clks,
	.num_clks = ARRAY_SIZE(h3_clks),

	.resets = h3_resets,
	.num_resets =  ARRAY_SIZE(h3_resets),
};

static int h3_clk_probe(struct udevice *dev)
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

static int h3_clk_bind(struct udevice *dev)
{
	return sunxi_reset_bind(dev, 53);
}

static const struct udevice_id h3_clk_ids[] = {
	{ .compatible = "allwinner,sun8i-h3-ccu",
	  .data = (ulong)&sun8i_h3_ccu_desc },
	{ .compatible = "allwinner,sun50i-h5-ccu",
	  .data = (ulong)&sun8i_h3_ccu_desc },
	{ }
};

U_BOOT_DRIVER(clk_sun8i_h3) = {
	.name		= "sun8i_h3_ccu",
	.id		= UCLASS_CLK,
	.of_match	= h3_clk_ids,
	.priv_auto_alloc_size	= sizeof(struct sunxi_clk_priv),
	.ops		= &sunxi_clk_ops,
	.probe		= h3_clk_probe,
	.bind		= h3_clk_bind,
};
