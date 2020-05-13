// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2020 Amarula Solutions(India)
 * Copyright (c) 2020 Jagan Teki <jagan@amarulasolutons.com>
 * Copyright (C) 2015 Thomas Chou <thomas@wytron.com.tw>
 */

#include <common.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <errno.h>
#include <mtd.h>
#include <linux/log2.h>

int mtd_dread(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
	      u_char *buf)
{
	struct udevice *dev = mtd->dev;
	const struct mtd_ops *ops = mtd_get_ops(dev);

	if (!ops->read)
		return -EOPNOTSUPP;

	*retlen = 0;
	if (from < 0 || from > mtd->size || len > mtd->size - from)
		return -EINVAL;
	if (!len)
		return 0;

	return ops->read(dev, from, len, retlen, buf);
}

int mtd_derase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct udevice *dev = mtd->dev;
	const struct mtd_ops *ops = mtd_get_ops(dev);

	if (!ops->erase)
		return -EOPNOTSUPP;

	if (instr->addr > mtd->size || instr->len > mtd->size - instr->addr)
		return -EINVAL;
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	instr->fail_addr = MTD_FAIL_ADDR_UNKNOWN;
	if (!instr->len) {
		instr->state = MTD_ERASE_DONE;
		return 0;
	}

	return ops->erase(dev, instr);
}

int mtd_dwrite(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
	       const u_char *buf)
{
	struct udevice *dev = mtd->dev;
	const struct mtd_ops *ops = mtd_get_ops(dev);

	if (!ops->write)
		return -EOPNOTSUPP;

	*retlen = 0;
	if (to < 0 || to > mtd->size || len > mtd->size - to)
		return -EINVAL;
	if (!ops->write || !(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	if (!len)
		return 0;

	return ops->write(dev, to, len, retlen, buf);
}

/**
 * mtd_probe - Probe the device @dev if not already done
 *
 * @dev: U-Boot device to probe
 *
 * @return 0 on success, an error otherwise.
 */
int mtd_probe(struct udevice *dev)
{
	if (device_active(dev))
		return 0;

	return device_probe(dev);
}

static int mtd_post_bind(struct udevice *dev)
{
#if defined(CONFIG_NEEDS_MANUAL_RELOC)
	const struct mtd_ops *ops = mtd_get_ops(dev);
	static int reloc_done;

	if (!reloc_done) {
		if (ops->read)
			ops->read += gd->reloc_off;
		if (ops->write)
			ops->write += gd->reloc_off;
		if (ops->erase)
			ops->erase += gd->reloc_off;

		reloc_done++;
	}
#endif
	return 0;
}

/*
 * Implement a MTD uclass which should include most flash drivers.
 * The uclass private is pointed to mtd_info.
 */

UCLASS_DRIVER(mtd) = {
	.id		= UCLASS_MTD,
	.name		= "mtd",
	.per_device_auto_alloc_size = sizeof(struct mtd_info),
	.post_bind	= mtd_post_bind,
};
