/**
 * flash
 * Copyright (c) 2012 King Pearl
 * MIT Licensed
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "commander/commander.h"

#define VERSION "0.0.1"

/**
 * log file
 */
static FILE *log = NULL;

/**
 * concat
 */
char *concat(int len, char **strs) {
	int bytes = len; // ' ' + null
	for (int i = 0; i < len; ++i) bytes += strlen(strs[i]);
	char *str = malloc(bytes);
	if (!str) return NULL;
	for (int i = 0; i < len; ++i) {
		if (i) strcat(str, " ");
		
		strcat(str, strs[i]);
	}
	str[bytes] = 0;
	return str;
}

/**
 * Output error `msg`.
 */
void error(char *msg) {
	fprintf(stderr, "Error: %s\n", msg);
	exit(1);
}

/**
 * log_file <file>
 */

void log_file(command_t *self) {
	log = fopen(self->arg, "a");
	if (!log) error(strerror(errno));
	setlinebuf(log);
}

/**
 * set_time
 */
void set_time(time_t *t, int offset, char **strs) {
	*t = time(0) + offset;
	struct tm *ts = localtime(t);
	ts->tm_hour = atoi(strs[0]);
	ts->tm_min = atoi(strs[1]);
	ts->tm_sec = atoi(strs[2]);
	*t = mktime(ts);
}

/**
 * main [options] <cmd>
 */
int main(int argc, char **argv) {
	command_t program;
	command_init(&program, "flash", VERSION);
	program.usage = "[options] <hour> <min> <sec> <cmd ...>";
	command_option(&program, "-l", "--log <file>", "log to the given file", log_file);
	command_parse(&program, argc, argv);

	// required args
	if (!program.argc || program.argc < 3) error("<hour> <min> <sec> required");
	if (program.argc < 4) error("<cmd> required");

	// cmd
	char *cmd = concat(program.argc - 3, program.argv + 3);

	// time
	time_t t;
	set_time(&t, 0, program.argv);
	if (!t) error("<hour> <min> <sec> invalid");

	// exec
	while (1) {
		time_t ct = time(0);
		int secs = difftime(t, ct);

		if (secs <= 0) {
			set_time(&t, 24 * 60 * 60, program.argv);
			secs = difftime(t, ct);
		}

		if (log) fprintf(log, "sleep %d\n", secs);
		sleep(secs);
		if (log) fprintf(log, "exec `%s`\n", cmd);
		int ret = system(cmd);
		if (log) fprintf(log, "=> %d\n\n", ret);
	}

	command_free(&program);
	return 0;
}