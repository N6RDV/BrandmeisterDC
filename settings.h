/*
 * Copyright (c) 2022 Dennis Riabchenko N6RDV.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#if !defined(settings_H)
#define settings_H

#define MAX_SETTING_LEN 256

struct Settings {
	unsigned int repeater_id;
	char bm_api_key[MAX_SETTING_LEN + 1];
	char bm_hs_security[MAX_SETTING_LEN + 1];
	char mmdvm_log[MAX_SETTING_LEN + 1];
	unsigned int bm_dc_id;
	unsigned int scan_interval;
	unsigned int verbosity;
	unsigned int debug_mode;
};

extern struct Settings settings;

void read_settings(char *path);

#endif
