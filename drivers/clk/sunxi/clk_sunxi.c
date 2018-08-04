// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (C) 2018 Amarula Solutions.
 * Author: Jagan Teki <jagan@amarulasolutions.com>
 */

#include <common.h>
#include <clk-uclass.h>
#include <dm.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/ccu.h>
#include <linux/log2.h>

static ulong sunxi_clk_set_rate(struct clk *clk, ulong rate)
{
	struct sunxi_clk_priv *priv = dev_get_priv(clk->dev);
	struct ccu_clk_map *map = &priv->desc->clks[clk->id];
	u32 *base;

	if (!map->ccu_clk_set_rate) {
		debug("%s (CLK#%ld) unhandled\n", __func__, clk->id);
		return 0;
	}

	debug("%s(#%ld) off#0x%x, BIT(%d)\n", __func__,
	      clk->id, map->off, ilog2(map->bit));

	base = priv->base + map->off;
	return map->ccu_clk_set_rate(base, map->bit, rate);
}

static int sunxi_clk_enable(struct clk *clk)
{
	struct sunxi_clk_priv *priv = dev_get_priv(clk->dev);
	struct ccu_clk_map *map = &priv->desc->clks[clk->id];
	u32 reg;

	if (!map->off || !map->bit) {
		debug("%s (CLK#%ld) unhandled\n", __func__, clk->id);
		return 0;
	}

	debug("%s(#%ld) off#0x%x, BIT(%d)\n", __func__,
	      clk->id, map->off, ilog2(map->bit));

	reg = readl(priv->base + map->off);
	writel(reg | map->bit, priv->base + map->off);

	return 0;
}

static int sunxi_clk_disable(struct clk *clk)
{
	struct sunxi_clk_priv *priv = dev_get_priv(clk->dev);
	struct ccu_clk_map *map = &priv->desc->clks[clk->id];
	u32 reg;

	if (!map->off || !map->bit) {
		debug("%s (CLK#%ld) unhandled\n", __func__, clk->id);
		return 0;
	}

	debug("%s(#%ld) off#0x%x, BIT(%d)\n", __func__,
	      clk->id, map->off, ilog2(map->bit));

	reg = readl(priv->base + map->off);
	writel(reg & ~map->bit, priv->base + map->off);

	return 0;
}

struct clk_ops sunxi_clk_ops = {
	.enable = sunxi_clk_enable,
	.disable = sunxi_clk_disable,
	.set_rate = sunxi_clk_set_rate,
};
