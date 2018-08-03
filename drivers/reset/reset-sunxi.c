// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (C) 2018 Amarula Solutions.
 * Author: Jagan Teki <jagan@amarulasolutions.com>
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <reset-uclass.h>
#include <asm/io.h>
#include <dm/lists.h>
#include <linux/log2.h>
#include <asm/arch/ccu.h>

struct sunxi_reset_priv {
	void *base;
	ulong count;
	const struct ccu_desc *desc;
};

static int sunxi_reset_request(struct reset_ctl *reset_ctl)
{
	struct sunxi_reset_priv *priv = dev_get_priv(reset_ctl->dev);

	debug("%s (RST#%ld)\n", __func__, reset_ctl->id);

	/* check dt-bindings/reset/sun8i-h3-ccu.h for max id */
	if (reset_ctl->id >= priv->count)
		return -EINVAL;

	return 0;
}

static int sunxi_reset_free(struct reset_ctl *reset_ctl)
{
	debug("%s (RST#%ld)\n", __func__, reset_ctl->id);

	return 0;
}

static int sunxi_reset_assert(struct reset_ctl *reset_ctl)
{
	struct sunxi_reset_priv *priv = dev_get_priv(reset_ctl->dev);
	struct ccu_reset_map *map = &priv->desc->resets[reset_ctl->id];
	u32 reg;

	if (!map->off || !map->bit) {
		debug("%s (RST#%ld) unhandled\n", __func__, reset_ctl->id);
		return 0;
	}

	debug("%s(#%ld) off#0x%x, BIT(%d)\n", __func__,
	      reset_ctl->id, map->off, ilog2(map->bit));

	reg = readl(priv->base + map->off);
	writel(reg & ~map->bit, priv->base + map->off);

	return 0;
}

static int sunxi_reset_deassert(struct reset_ctl *reset_ctl)
{
	struct sunxi_reset_priv *priv = dev_get_priv(reset_ctl->dev);
	struct ccu_reset_map *map = &priv->desc->resets[reset_ctl->id];
	u32 reg;

	if (!map->off || !map->bit) {
		debug("%s (RST#%ld) unhandled\n", __func__, reset_ctl->id);
		return 0;
	}

	debug("%s(#%ld) off#0x%x, BIT(%d)\n", __func__,
	      reset_ctl->id, map->off, ilog2(map->bit));

	reg = readl(priv->base + map->off);
	writel(reg | map->bit, priv->base + map->off);

	return 0;
}

struct reset_ops sunxi_reset_ops = {
	.request = sunxi_reset_request,
	.free = sunxi_reset_free,
	.rst_assert = sunxi_reset_assert,
	.rst_deassert = sunxi_reset_deassert,
};

static int sunxi_reset_probe(struct udevice *dev)
{
	struct sunxi_reset_priv *priv = dev_get_priv(dev);

	priv->base = dev_read_addr_ptr(dev);

	return 0;
}

int sunxi_reset_bind(struct udevice *dev, ulong count)
{
	struct udevice *rst_dev;
	struct sunxi_reset_priv *priv;
	int ret;

	ret = device_bind_driver_to_node(dev, "sunxi_reset", "reset",
					 dev_ofnode(dev), &rst_dev);
	if (ret) {
		debug("Warning: failed to bind sunxi_reset driver: ret=%d\n", ret);
		return ret;
	}
	priv = malloc(sizeof(struct sunxi_reset_priv));
	priv->count = count;
	priv->desc = (const struct ccu_desc *)dev_get_driver_data(dev);
	rst_dev->priv = priv;

	return 0;
}

U_BOOT_DRIVER(reset_sun8i_h3) = {
	.name		= "sunxi_reset",
	.id		= UCLASS_RESET,
	.ops		= &sunxi_reset_ops,
	.probe		= sunxi_reset_probe,
	.priv_auto_alloc_size = sizeof(struct sunxi_reset_priv),
};
