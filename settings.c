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
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/wait.h>
#include <unistd.h>

#include "settings.h"
#include "logger.h"

struct Settings settings;

typedef void (*handler_t)(char *section, char *key, char *value);

int invalid(char c) {
	return (c == ' ' || c == '\'' || c == '"' || c == '\t' || c == '\r' || c == '\n');
}

char *strip(char *str) {
	char *tmp = strdup(str);
	char *ptr = tmp;
	while (invalid(ptr[0])) {
		ptr++;
	}
	int i = strlen(ptr) - 1;
	while (i >= 0 && invalid(ptr[i])) {
		ptr[i] = 0;
		i--;
	}

	strcpy(str, ptr);
	free(tmp);
	return str;
}

void read_config(char *path, handler_t handler) {
	writelog(LOG_INFO, "Opening configuration file \"%s\".", path);
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		writelog(LOG_ERR, "Couldn't open configuration file \"%s\".", path);
		perror(path);
		exit(EXIT_FAILURE);
	}

	char line[MAX_SETTING_LEN];
	char section[MAX_SETTING_LEN];
	while (fgets(line, MAX_SETTING_LEN, fp) != NULL) {
		if (line[0] == '[') {
			// Section
			char *end = strchr(line, ']');
			if (end != NULL) {
				end[1] = 0;
				strcpy(section, line);
			}
		} else if (line[0] != '#') {
			// Property
			char *key = line;
			char *value = strchr(line, '=');
			if (value != NULL) {
				value[0] = 0;
				value++;
				strip(key);
				strip(value);
				handler(section, key, value);
			}
		}
	}
	fclose(fp);

}

void handle_setting(char *section, char *key, char *value) {
	writelog(LOG_DEBUG, "%s %s=%s", section, key, value);
	if (strcmp(section, "[DMR]") == 0) {
		if (strcmp(key, "Id") == 0) {
			settings.repeater_id = (unsigned int) atoi(value);
		}
	} else if (strcmp(section, "[DMR Network]") == 0) {
		if (strcmp(key, "Password") == 0) {
			strcpy(settings.bm_hs_security, value);
		}
	} else if (strcmp(section, "[Brandmeister DC]") == 0) {
		if (strcmp(key, "VerbosityLevel") == 0) {
			settings.verbosity = (unsigned int) atoi(value);
		} else if (strcmp(key, "DisconnectGroup") == 0) {
			settings.bm_dc_id = (unsigned int) atoi(value);
		} else if (strcmp(key, "ScanInterval") == 0) {
			settings.scan_interval = (unsigned int) atoi(value);
		} else if (strcmp(key, "MMDVMLog") == 0) {
			strcpy(settings.mmdvm_log, value);
		}
	} else if (strcmp(section, "[key]") == 0) {
		if (strcmp(key, "apikey") == 0) {
			strcpy(settings.bm_api_key, value);
		}
	}
}

void read_settings(char *path) {
	// Read main configuration.
	read_config(path, handle_setting);
}
