// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (C) 2018 Amarula Solutions B.V.
 * Author: Jagan Teki <jagan@amarulasolutions.com>
 */

#include <common.h>
#include <clk-uclass.h>
#include <dm.h>
#include <errno.h>
#include <asm/arch/ccu.h>
#include <dt-bindings/clock/sun6i-a31-ccu.h>
#include <dt-bindings/reset/sun6i-a31-ccu.h>

static struct ccu_clk_map a31_clks[] = {
	[CLK_AHB1_MMC0]		= { 0x060, BIT(8), NULL },
	[CLK_AHB1_MMC1]		= { 0x060, BIT(9), NULL },
	[CLK_AHB1_MMC2]		= { 0x060, BIT(10), NULL },
	[CLK_AHB1_MMC3]		= { 0x060, BIT(12), NULL },
	[CLK_AHB1_SPI0]		= { 0x060, BIT(20), NULL },
	[CLK_AHB1_SPI1]		= { 0x060, BIT(21), NULL },
	[CLK_AHB1_SPI2]		= { 0x060, BIT(22), NULL },
	[CLK_AHB1_SPI3]		= { 0x060, BIT(23), NULL },
	[CLK_AHB1_OTG]		= { 0x060, BIT(24), NULL },
	[CLK_AHB1_EHCI0]	= { 0x060, BIT(26), NULL },
	[CLK_AHB1_EHCI1]	= { 0x060, BIT(27), NULL },
	[CLK_AHB1_OHCI0]	= { 0x060, BIT(29), NULL },
	[CLK_AHB1_OHCI1]	= { 0x060, BIT(30), NULL },
	[CLK_AHB1_OHCI2]	= { 0x060, BIT(31), NULL },

	[CLK_MMC0]		= { 0x088, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC1]		= { 0x08c, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC2]		= { 0x090, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC3]		= { 0x094, BIT(31), &mmc_clk_set_rate },

	[CLK_SPI0]		= { 0x0a0, BIT(31), NULL },
	[CLK_SPI1]		= { 0x0a4, BIT(31), NULL },
	[CLK_SPI2]		= { 0x0a8, BIT(31), NULL },
	[CLK_SPI3]		= { 0x0ac, BIT(31), NULL },

	[CLK_USB_PHY0]		= { 0x0cc, BIT(8), NULL },
	[CLK_USB_PHY1]		= { 0x0cc, BIT(9), NULL },
	[CLK_USB_PHY2]		= { 0x0cc, BIT(10), NULL },
	[CLK_USB_OHCI0]		= { 0x0cc, BIT(16), NULL },
	[CLK_USB_OHCI1]		= { 0x0cc, BIT(17), NULL },
	[CLK_USB_OHCI2]		= { 0x0cc, BIT(18), NULL },
};

static struct ccu_reset_map a31_resets[] = {
	[RST_USB_PHY0]		= { 0x0cc, BIT(0) },
	[RST_USB_PHY1]		= { 0x0cc, BIT(1) },
	[RST_USB_PHY2]		= { 0x0cc, BIT(2) },

	[RST_AHB1_MMC0]		= { 0x2c0, BIT(8) },
	[RST_AHB1_MMC1]		= { 0x2c0, BIT(9) },
	[RST_AHB1_MMC2]		= { 0x2c0, BIT(10) },
	[RST_AHB1_MMC3]		= { 0x2c0, BIT(11) },
	[RST_AHB1_OTG]		= { 0x2c0, BIT(24) },
	[RST_AHB1_EHCI0]	= { 0x2c0, BIT(26) },
	[RST_AHB1_EHCI1]	= { 0x2c0, BIT(27) },
	[RST_AHB1_OHCI0]	= { 0x2c0, BIT(29) },
	[RST_AHB1_OHCI1]	= { 0x2c0, BIT(30) },
	[RST_AHB1_OHCI2]	= { 0x2c0, BIT(31) },
};

static const struct ccu_desc sun6i_a31_ccu_desc = {
	.clks = a31_clks,
	.num_clks = ARRAY_SIZE(a31_clks),

	.resets = a31_resets,
	.num_resets =  ARRAY_SIZE(a31_resets),
};

static int a31_clk_probe(struct udevice *dev)
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

static int a31_clk_bind(struct udevice *dev)
{
	return sunxi_reset_bind(dev, 56);
}

static const struct udevice_id a31_clk_ids[] = {
	{ .compatible = "allwinner,sun6i-a31-ccu",
	  .data = (ulong)&sun6i_a31_ccu_desc },
	{ }
};

U_BOOT_DRIVER(clk_sun6i_a31) = {
	.name		= "sun6i_a31_ccu",
	.id		= UCLASS_CLK,
	.of_match	= a31_clk_ids,
	.priv_auto_alloc_size	= sizeof(struct sunxi_clk_priv),
	.ops		= &sunxi_clk_ops,
	.probe		= a31_clk_probe,
	.bind		= a31_clk_bind,
};
