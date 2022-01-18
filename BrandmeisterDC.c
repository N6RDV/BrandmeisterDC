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
#include <mcheck.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include "BrandmeisterDC.h"
#include "bm_api.h"
#include "logger.h"
#include "scanner.h"
#include "settings.h"

#define DMR_REGEX "DMR Slot ([12]{1}), received RF voice header from ([A-Z0-9]+) to (TG|)[ ]*([0-9]+)"

const char *signame[] = {"INVALID", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT",
	"SIGBUS", "SIGFPE", "SIGKILL", "SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM",
	"SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG",
	"SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGPOLL", "SIGPWR", "SIGSYS", NULL};

char config_path[MAX_SETTING_LEN] = "/etc/mmdvmhost";
char bmapikey_path[MAX_SETTING_LEN] = "/etc/bmapi.key";
unsigned int ignore_process = 0;
unsigned int trace_memory = 0;

void terminate(int sig) {
	writelog(LOG_ERR, "BrandmeisterDC v%s terminated on signal %s (%s)", BrandmeisterDC_VERSION,
					signame[sig], strsignal(sig));
	exit(EXIT_SUCCESS);
}

void disconnect(char **values, size_t size) {
	int ts = atoi(values[1]);
	char *callsign = values[2];
	char *tg = values[3];
	int id = atoi(values[4]);
	writelog(LOG_INFO, "DMR Slot %d, received RF voice header from %s to %s %d", ts, callsign, tg, id);
	if (id == settings.bm_dc_id) {
		writelog(LOG_WARNING, "Disconnecting from dynamic groups on TS %d.", ts);
		drop_groups(ts);
		drop_qso(ts);
	}
}

void parse_args(int argc, char *argv[]) {
	int opt;
	while ((opt = getopt(argc, argv, "dvhimc:k:")) != -1) {
		switch (opt) {
			case 'v':
				printf("\nBrandmeisterDC v%s\n", BrandmeisterDC_VERSION);
				printf("Copyright (c) 2022 Dennis Riabchenko N6RDV.\n\n");
				exit(EXIT_SUCCESS);
				break;
			case 'c':
				memset(config_path, 0, sizeof (config_path));
				strncpy(config_path, optarg, sizeof (config_path) - 2);
				break;
			case 'k':
				memset(bmapikey_path, 0, sizeof (bmapikey_path));
				strncpy(bmapikey_path, optarg, sizeof (bmapikey_path) - 2);
				break;
			case 'd':
				settings.debug_mode = 1;
				break;
			case 'm':
				trace_memory = 1;
				break;
			case 'i':
				ignore_process = 1;
				break;
			case 'h':
			case ':':
				printf("\nBrandmeisterDC v%s\n", BrandmeisterDC_VERSION);
				printf("Copyright (c) 2022 Dennis Riabchenko N6RDV.\n\n");
				printf("Usage: %s -c <configuration file> [-k <bmapikey fiel>] [-i] [-d] [-m] [-h]\n\n", argv[0]);
				printf("  -c\tspecify the path to MMDVM configuration file\n");
				printf("  -k\tspecify the path to BM API key file\n");
				printf("  -i\tignore other instances and run anyway\n");
				printf("  -d\tstart in debug mode\n");
				printf("  -m\ttrace memory allocation (MALLOC_TRACE environment variable should be set; see mtrace manpage)\n");
				printf("  -h\tthis help.\n\n");
				exit(EXIT_SUCCESS);

				break;
		}
	}
}

pid_t proc_find(const char* name) {
	// TODO implement

	return -1;
}

void detach() {
	pid_t pid = 0;
	pid_t sid = 0;

	pid = fork();
	if (pid < 0) {
		writelog(LOG_ERR, "Fork failed");
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
		exit(EXIT_SUCCESS);
	umask(0);
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);
	chdir("/tmp");
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int main(int argc, char *argv[]) {
	settings.verbosity = LOG_NOTICE;

	parse_args(argc, argv);

	if (trace_memory) {
		writelog(LOG_NOTICE, "Starting with memory allocation  tracing.");
		mtrace();
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, terminate);
	signal(SIGQUIT, terminate);
	signal(SIGHUP, terminate);
	signal(SIGTERM, terminate);

	// Default settings
	settings.bm_dc_id = 4000;
	settings.scan_interval = 1000;
	strcpy(settings.mmdvm_log, "/var/log/pi-star/MMDVM-%F.log");

	if (settings.debug_mode)
		writelog(LOG_NOTICE, "Starting in console mode.");
	else {
		writelog(LOG_NOTICE, "Starting in daemon mode.");
		detach();
	}
	writelog(LOG_NOTICE, "BrandmeisterDC v%s", BrandmeisterDC_VERSION);
	writelog(LOG_NOTICE, "Copyright (c) 2022 Dennis Riabchenko N6RDV.");

	read_settings(config_path);
	read_settings(bmapikey_path);

	int ok = proc_find("BrandmeisterDC");
	if (ok > 0) {
		if (ignore_process) {
			writelog(LOG_WARNING, "Warning: BrandmeisterDC with PID %d already running", ok);
		} else {
			writelog(LOG_ERR, "Error: BrandmeisterDC with PID %d already running!", ok);
			exit(EXIT_FAILURE);
		}
	}

	writelog(LOG_DEBUG, "config_path=%s\nsettings.bm_dc_id=%d\nsettings.scan_interval=%d\n"
					"settings.verbosity=%d\nsettings.repeater_id=%u\nsettings.bm_api_key=%s\n"
					"settings.bm_hs_security=%s\nsettings.mmdvm_log=%s\n",
					config_path, settings.bm_dc_id, settings.scan_interval, settings.verbosity,
					settings.repeater_id, settings.bm_api_key, settings.bm_hs_security, settings.mmdvm_log);

	// TODO: Validate main configuration

	listener_t listeners[1] = {&disconnect};
	char *filters[1] = {DMR_REGEX};
	writelog(LOG_INFO, "Starting scanner.");
	listen(listeners, filters, 1);
	writelog(LOG_INFO, "Scanner stopped.");
}
