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

#include "bm_api.h"
#include "logger.h"
#include "settings.h"

#define MAX_CMD_SIZE 2048
#define BM_API "https://api.brandmeister.network/v1.0/repeater"
#define DROP_QSO_CMD "curl -s --user '%s':'%s' '%s/setRepeaterDbus.php?action=dropCallRoute&slot=%d&q=%u' > /dev/null"
#define DROP_GROUPS_CMD "curl -s --user '%s':'%s' '%s/setRepeaterTarantool.php?action=dropDynamicGroups&slot=%d&q=%u' > /dev/null"

char **environ;

void run_cmd(char *cmd) {
	pid_t pid;
	char *argv[] = {"sh", "-c", cmd, NULL};
	int status;
	status = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
	if (status == 0) {
		writelog(LOG_DEBUG, "Child pid: %i", pid);
		do {
			if (waitpid(pid, &status, 0) != -1) {
				writelog(LOG_DEBUG, "Child status %d", WEXITSTATUS(status));
			} else
				perror("waitpid");
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	} else {
		writelog(LOG_ERR, "run_cmd: %s", strerror(status));
	}
}

void drop_qso(int ts) {

	char cmd[MAX_CMD_SIZE];
	snprintf(cmd, MAX_CMD_SIZE, DROP_QSO_CMD, settings.bm_api_key, settings.bm_hs_security, BM_API,
					ts, settings.repeater_id);
	run_cmd(cmd);
}

void drop_groups(int ts) {
	char cmd[MAX_CMD_SIZE];
	snprintf(cmd, MAX_CMD_SIZE, DROP_GROUPS_CMD, settings.bm_api_key, settings.bm_hs_security, BM_API,
					ts, settings.repeater_id);
	run_cmd(cmd);
}
