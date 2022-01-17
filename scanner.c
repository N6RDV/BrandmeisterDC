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
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "scanner.h"
#include "logger.h"
#include "settings.h"

#define PATH_LEN  256
#define VALUE_LEN 256

struct Listener {
	listener_t *listener;
	regex_t regex;
	size_t ngroups;
	regmatch_t *groups;
};

void utc_time(struct tm *utc) {
	time_t now = time(&now);
	*utc = *gmtime(&now);
}

FILE *open_log(struct tm* utc) {
	char path[PATH_LEN];
	strftime(path, PATH_LEN, settings.mmdvm_log, utc);
	writelog(LOG_INFO, "Opening MMDVM log file \"%s\".", path);
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		writelog(LOG_ERR, "Couldn't open MMDVM log file \"%s\".", path);
		perror(path);
		exit(EXIT_FAILURE);
	}
	fseek(fp, 0, SEEK_END);
	return fp;
}

char *process_group(char *line, regmatch_t *group) {
	size_t len = group->rm_eo - group->rm_so;
	char *value = malloc(len + 1);
	strncpy(value, line + group->rm_so, len);
	value[len] = '\0';
	return value;
}

void process_match(char *line, struct Listener *listener) {
	char *values[listener->ngroups];
	for (size_t j = 0; j < listener->ngroups; j++) {
		values[j] = process_group(line, &listener->groups[j]);
	}
	(*listener->listener)(values, listener->ngroups);
	for (size_t j = 0; j < listener->ngroups; j++) {
		free(values[j]);
	}
}

void process_line(char *line, struct Listener *listeners, int size) {
	for (int i = 0; i < size; i++) {
		struct Listener *listener = &listeners[i];
		if (regexec(&listener->regex, line, listener->ngroups, listener->groups, 0) == 0) {
			process_match(line, listener);
		}
	}
}

void build_listeners(listener_t *callbacks, char **filters, struct Listener *listeners, size_t size) {
	for (int i = 0; i < size; i++) {
		int r = regcomp(&listeners[i].regex, filters[i], REG_EXTENDED);
		if (r) {

			writelog(LOG_ERR, "Filter expression is malformed \"%s\"; Error code %d.", filters[i], r);
			exit(EXIT_FAILURE);
		}
		size_t ngroups = listeners[i].regex.re_nsub + 1;
		listeners[i].ngroups = ngroups;
		listeners[i].groups = malloc(ngroups * sizeof (regmatch_t));
		listeners[i].listener = &callbacks[i];
	}
}

void free_listeners(struct Listener *listeners, size_t size) {
	for (int i = 0; i < size; i++) {
		regfree(&listeners[i].regex);
		free(listeners[i].groups);
	}
}

void listen(listener_t *callbacks, char **filters, size_t size) {
	struct Listener listeners[size];
	build_listeners(callbacks, filters, listeners, size);

	struct tm utc;
	struct tm cur_utc;
	utc_time(&cur_utc);

	FILE* fp = NULL;
	size_t len;
	char *line = NULL;
	while (1) {
		utc = cur_utc;
		fp = open_log(&utc);
		do {
			if (getline(&line, &len, fp) == -1) {
				sleep(settings.scan_interval / 1000);
				clearerr(fp);
			} else
				process_line(line, listeners, size);
			utc_time(&cur_utc);
		} while (cur_utc.tm_yday == utc.tm_yday);
		fclose(fp);
		if (line)
			free(line);
		writelog(LOG_NOTICE, "Log is closed due to expiration.");
	}
	free_listeners(listeners, size);
}
