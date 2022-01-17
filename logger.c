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
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

#include "logger.h"
#include "settings.h"

const char *level_name[] = {"EMERGENCY", "ALERT", "CRITICAL", "ERROR", "WARNING", "NOTICE", "INFO",
	"DEBUG"};

void writelog(unsigned int level, char *format, ...) {
	va_list args;
	char str[1024];

	if (settings.debug_mode || level <= settings.verbosity) {
		va_start(args, format);

		if (settings.debug_mode) {
			vsnprintf(str, 1024, format, args);
			printf("%s: %s\n", level_name[level], str);
		} else {
			vsnprintf(str, 100, format, args);
			str[98] = str[99] = str[100] = '.';
			str[101] = 0;
			syslog(level, "%s", str);
		}
		va_end(args);
	}
}
