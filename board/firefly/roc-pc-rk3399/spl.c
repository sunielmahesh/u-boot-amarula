// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2020 Amarula Solutions(India)
 */

#include <common.h>
#include <spl.h>

void spl_board_init(void)
{
	preloader_console_init();
}
