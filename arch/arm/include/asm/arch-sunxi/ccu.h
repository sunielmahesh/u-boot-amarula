// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (C) 2018 Amarula Solutions.
 * Author: Jagan Teki <jagan@amarulasolutions.com>
 */

#ifndef _ASM_ARCH_CCU_H
#define _ASM_ARCH_CCU_H

/**
 * ccu_clk_map - common clock unit clock map
 *
 * @off:		ccu clock offset
 * @bit:		ccu clock bit value
 * @ccu_clk_set_rate:	ccu clock set rate func
 */
struct ccu_clk_map {
	u16 off;
	u32 bit;
	int (*ccu_clk_set_rate)(void *base, u32 bit, ulong rate);
};

/**
 * ccu_reset_map - common clock unit reset map
 *
 * @off:	ccu reset offset
 * @bit:	ccu reset bit value
 */
struct ccu_reset_map {
	u16 off;
	u32 bit;
};

/**
 * struct ccu_desc - common clock unit descriptor
 *
 * @clks:		mapping clocks descriptor
 * @num_clks:		number of mapped clocks
 * @resets:		mapping resets descriptor
 * @num_resets:		number of mapped resets
 */
struct ccu_desc {
	struct ccu_clk_map *clks;
	unsigned long num_clks;

	struct ccu_reset_map *resets;
	unsigned long num_resets;
};

/**
 * struct sunxi_clk_priv - sunxi clock private structure
 *
 * @base:	base address
 * @desc:	ccu descriptor
 */
struct sunxi_clk_priv {
	void *base;
	const struct ccu_desc *desc;
};

extern struct clk_ops sunxi_clk_ops;

/**
 * sunxi_reset_bind() - reset binding
 *
 * @dev:	reset device
 * @count:	reset count
 * @return 0 success, or error value
 */
int sunxi_reset_bind(struct udevice *dev, ulong count);

#endif /* _ASM_ARCH_CCU_H */
