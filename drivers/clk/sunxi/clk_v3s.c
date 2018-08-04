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
#include <dt-bindings/clock/sun8i-v3s-ccu.h>
#include <dt-bindings/reset/sun8i-v3s-ccu.h>

static struct ccu_clk_map v3s_clks[] = {
	[CLK_BUS_MMC0]		= { 0x060, BIT(8), NULL },
	[CLK_BUS_MMC1]		= { 0x060, BIT(9), NULL },
	[CLK_BUS_MMC2]		= { 0x060, BIT(10), NULL },
	[CLK_BUS_OTG]		= { 0x060, BIT(24), NULL },

	[CLK_MMC0]		= { 0x088, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC1]		= { 0x08c, BIT(31), &mmc_clk_set_rate },
	[CLK_MMC2]		= { 0x090, BIT(31), &mmc_clk_set_rate },

	[CLK_USB_PHY0]          = { 0x0cc, BIT(8), NULL },
};

static struct ccu_reset_map v3s_resets[] = {
	[RST_USB_PHY0]		=  { 0x0cc, BIT(0) },

	[RST_BUS_MMC0]		=  { 0x2c0, BIT(8) },
	[RST_BUS_MMC1]		=  { 0x2c0, BIT(9) },
	[RST_BUS_MMC2]		=  { 0x2c0, BIT(10) },
	[RST_BUS_OTG]		=  { 0x2c0, BIT(24) },
};

static const struct ccu_desc sun8i_v3s_ccu_desc = {
	.clks = v3s_clks,
	.num_clks = ARRAY_SIZE(v3s_clks),

	.resets = v3s_resets,
	.num_resets =  ARRAY_SIZE(v3s_resets),
};

static int v3s_clk_probe(struct udevice *dev)
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

static int v3s_clk_bind(struct udevice *dev)
{
	return sunxi_reset_bind(dev, 53);
}

static const struct udevice_id v3s_clk_ids[] = {
	{ .compatible = "allwinner,sun8i-v3s-ccu",
	  .data = (ulong)&sun8i_v3s_ccu_desc },
	{ }
};

U_BOOT_DRIVER(clk_sun8i_v3s) = {
	.name		= "sun8i_v3s_ccu",
	.id		= UCLASS_CLK,
	.of_match	= v3s_clk_ids,
	.priv_auto_alloc_size	= sizeof(struct sunxi_clk_priv),
	.ops		= &sunxi_clk_ops,
	.probe		= v3s_clk_probe,
	.bind		= v3s_clk_bind,
};
