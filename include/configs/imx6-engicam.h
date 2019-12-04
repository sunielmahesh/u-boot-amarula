/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2016 Amarula Solutions B.V.
 * Copyright (C) 2016 Engicam S.r.l.
 *
 * Configuration settings for the Engicam i.MX6 SOM Starter Kits.
 */

#ifndef __IMX6_ENGICAM_CONFIG_H
#define __IMX6_ENGICAM_CONFIG_H

#include <linux/sizes.h>
#include "mx6_common.h"

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(16 * SZ_1M)

/* Total Size of Environment Sector */
#define CONFIG_ENV_SIZE			SZ_128K

/* Allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/* Environment */
#ifndef CONFIG_ENV_IS_NOWHERE
/* Environment in MMC */
# if defined(CONFIG_ENV_IS_IN_MMC)
#  define CONFIG_ENV_OFFSET		0x100000
/* Environment in NAND */
# elif defined(CONFIG_ENV_IS_IN_NAND)
#  define CONFIG_ENV_OFFSET		0x400000
#  define CONFIG_ENV_SECT_SIZE		CONFIG_ENV_SIZE
# endif
#endif

/* Default environment */
#ifndef CONFIG_SPL_BUILD
#define NAND_BOOTCMD \
	"nandboot=echo Booting from nand ...; " \
	"if mtdparts; then " \
		"echo Starting nand boot ...; " \
	"else " \
		"mtdparts default; " \
	"fi; " \
	"run ubiargs; " \
	"nand read ${loadaddr} kernel 0x800000; " \
	"nand read ${fdt_addr} dtb 0x100000; " \
	"bootm ${loadaddr} - ${fdt_addr}\0"

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 2)
#include <config_distro_bootcmd.h>

#define MEM_LAYOUT_ENV_SETTINGS \
	"bootm_size=0x20000000\0" \
	"fdt_addr_r=0x12100000\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"kernel_addr_r=0x11000000\0" \
	"pxefile_addr_r=0x17100000\0" \
	"ramdisk_addr_r=0x12200000\0" \
	"scriptaddr=0x17000000\0"

/* Default environment */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"boot_syslinux_conf_type=" __stringify(CONFIG_SYSLINUX_CONF_TYPE) "\0" \
	"upgrade_available=0\0" \
	MEM_LAYOUT_ENV_SETTINGS \
	BOOTENV \
	"script=boot.scr\0" \
	"splashpos=m,m\0" \
	"splashimage=" __stringify(CONFIG_LOADADDR) "\0" \
	"image=uImage\0" \
	"fit_image=fit.itb\0" \
	"fdt_high=0xffffffff\0" \
	"fdt_addr=" FDT_ADDR "\0" \
	"boot_fdt=try\0" \
	"nandroot=ubi0:rootfs rootfstype=ubifs\0" \
	"ubiargs=setenv bootargs console=${console},${baudrate} " \
		"ubi.mtd=5 root=${nandroot} ${mtdparts}\0" \
	NAND_BOOTCMD \
	"altbootcmd=run distro_bootcmd\0"
#endif

/* Miscellaneous configurable options */
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x8000000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000

#ifdef CONFIG_MX6UL
# define DRAM_OFFSET(x)			0x87##x
# define FDT_ADDR			__stringify(DRAM_OFFSET(800000))
#else 
# define DRAM_OFFSET(x)			0x1##x
# define FDT_ADDR			__stringify(DRAM_OFFSET(8000000))
#endif

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - \
					GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_INIT_RAM_ADDR + \
					CONFIG_SYS_INIT_SP_OFFSET)

/* UART */
#ifdef CONFIG_MXC_UART
# ifdef CONFIG_MX6UL
#  define CONFIG_MXC_UART_BASE		UART1_BASE
# else
#  define CONFIG_MXC_UART_BASE		UART4_BASE
# endif
#endif

/* MMC */
#ifdef CONFIG_FSL_USDHC
# define CONFIG_SYS_MMC_ENV_DEV		0
#endif

/* NAND */
#ifdef CONFIG_NAND_MXS
# define CONFIG_SYS_MAX_NAND_DEVICE	1
# define CONFIG_SYS_NAND_BASE		0x40000000
# define CONFIG_SYS_NAND_5_ADDR_CYCLE
# define CONFIG_SYS_NAND_ONFI_DETECTION
# define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_TEXT_BASE
# define CONFIG_SYS_NAND_U_BOOT_OFFS	0x200000

/* MTD device */
#endif

/* Ethernet */
#ifdef CONFIG_FEC_MXC
# ifdef CONFIG_TARGET_MX6Q_ICORE_RQS
#  define CONFIG_FEC_MXC_PHYADDR	3
#  define CONFIG_FEC_XCV_TYPE		RGMII
# else
#  define CONFIG_FEC_MXC_PHYADDR	0
#  define CONFIG_FEC_XCV_TYPE		RMII
# endif
#endif

/* USB */
#ifdef CONFIG_USB
# define CONFIG_EHCI_HCD_INIT_AFTER_RESET
# define CONFIG_MXC_USB_PORTSC			(PORT_PTS_UTMI | PORT_PTS_PTW)
# define CONFIG_MXC_USB_FLAGS			0
# define CONFIG_USB_MAX_CONTROLLER_COUNT	2
#endif

/* Falcon Mode */
#ifdef CONFIG_SPL_OS_BOOT
# define CONFIG_SPL_FS_LOAD_ARGS_NAME	"args"
# define CONFIG_SPL_FS_LOAD_KERNEL_NAME	"uImage"
# define CONFIG_CMD_SPL
# define CONFIG_SYS_SPL_ARGS_ADDR	0x18000000
# define CONFIG_CMD_SPL_WRITE_SIZE	(128 * SZ_1K)

/* MMC support: args@1MB kernel@2MB */
# define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR		0x800   /* 1MB */
# define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS		(CONFIG_CMD_SPL_WRITE_SIZE / 512)
# define CONFIG_SYS_MMCSD_RAW_MODE_KERNEL_SECTOR	0x1000  /* 2MB */
#endif

/* Framebuffer */
#ifdef CONFIG_VIDEO_IPUV3
# define CONFIG_IMX_VIDEO_SKIP

# define CONFIG_SPLASH_SCREEN
# define CONFIG_SPLASH_SCREEN_ALIGN
# define CONFIG_BMP_16BPP
# define CONFIG_VIDEO_BMP_RLE8
# define CONFIG_VIDEO_LOGO
# define CONFIG_VIDEO_BMP_LOGO
#endif

/* SPL */
#ifdef CONFIG_SPL
# ifdef CONFIG_ENV_IS_IN_NAND
#  define CONFIG_SPL_NAND_SUPPORT
# else
#  define CONFIG_SPL_MMC_SUPPORT
# endif

# include "imx6_spl.h"
#endif

#endif /* __IMX6_ENGICAM_CONFIG_H */
