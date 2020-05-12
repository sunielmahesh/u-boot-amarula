/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2015 Thomas Chou <thomas@wytron.com.tw>
 */

#ifndef _MTD_H_
#define _MTD_H_

#include <linux/mtd/mtd.h>

struct mtd_ops {
	int (*erase)(struct udevice *dev, struct erase_info *instr);
	int (*read)(struct udevice *dev, loff_t from, size_t len,
		    size_t *retlen, u_char *buf);
	int (*write)(struct udevice *dev, loff_t to, size_t len,
		     size_t *retlen, const u_char *buf);
};

#define mtd_get_ops(dev) ((struct mtd_ops *)(dev)->driver->ops)

/**
 * mtd_dread() - Read data from MTD device
 *
 * @mtd:	MTD device
 * @from:	Offset into device in bytes to read from
 * @len:	Length of bytes to read
 * @retlen:	Length of return bytes read to
 * @buf:	Buffer to put the data that is read
 * @return 0 if OK, -ve on error
 */
int mtd_dread(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
	      u_char *buf);

/**
 * mtd_dwrite() - Write data to MTD device
 *
 * @mtd:	MTD device
 * @to:		Offset into device in bytes to write to
 * @len:	Length of bytes to write
 * @retlen:	Length of return bytes to write to
 * @buf:	Buffer containing bytes to write
 * @return 0 if OK, -ve on error
 */
int mtd_dwrite(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
	       const u_char *buf);

/**
 * mtd_derase() - Erase blocks of the MTD device
 *
 * @mtd:	MTD device
 * @instr:	Erase info details of MTD device
 * @return 0 if OK, -ve on error
 */
int mtd_derase(struct mtd_info *mtd, struct erase_info *instr);

int mtd_probe(struct udevice *dev);
int mtd_probe_devices(void);

#endif	/* _MTD_H_ */
