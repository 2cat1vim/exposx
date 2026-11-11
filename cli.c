#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define READ_LIMIT 256

#define NO_OPT (1u << 0)
#define VERBOSE_OPT (1u << 1)
#define STDIN_OPT (1u << 2)

typedef unsigned int f_uint;
typedef unsigned long long ull;

f_uint flag = 0;

void header(void) {
	printf("\n░█▀▀░█░█░█▀█░█▀█░█▀▀░█░█\n"
					 "░█▀▀░▄▀▄░█▀▀░█░█░▀▀█░▄▀▄\n"
					 "░▀▀▀░▀░▀░▀░░░▀▀▀░▀▀▀░▀░▀\n\n");
}

void cmd_exit(void) {
	if (flag & (f_uint)VERBOSE_OPT) {
		fprintf(stdout, "[v]: exiting..\n");
	}
	exit(EXIT_FAILURE);
}

void cmd_search(void) {
	return ;
}

void cmd_update(void) {
	return ;
}

int main(int ac, char *const *av) {
	opterr = 0;
	optopt = '?';
	int r;
	header();
	while ((r = getopt(ac, av, (const char *)"hvs")) != (int)-1) {
			switch ((unsigned char)r) {
			case 'h':
				fprintf(stdout, "\nUsage:\n -v	verbose\n -s	stdin\n -h	help\n");
				cmd_exit();
				break;
			case 'v':
				flag |= (f_uint)VERBOSE_OPT;
				break;
			case 's':
				flag |= (f_uint)STDIN_OPT;
				break;
			case '?':
				fprintf(stderr, (const char*)"unknown option `%c`\n", (char)optopt);
				return 1;
			}
	}
	if (flag & (f_uint)VERBOSE_OPT) {
		fprintf(stdout, "[v]: exposx - version 1.00\n");
	}
	while (1) {
		fprintf(stdout, "exposx$ ");
		fflush(stdout);
		char storage[(ull)READ_LIMIT];
		char buf;
		ull pos = 0;
		while ((r = read(STDIN_FILENO, &buf, 1)) > 0) {
			if (pos == (READ_LIMIT - 1)) {
				if (flag & (f_uint)VERBOSE_OPT) {
					printf("[v]: max READ_LIMIT, increase with -d\n");
				}
				break;
			}
			if (buf == '\n') {
				break;
			}
			storage[pos++] = buf;
		}
		storage[pos] = '\0';
		/* CTRL+D NL */
		if (r == 0) {
			printf("\n");
			continue;
		}
		if (strlen(storage) == (READ_LIMIT - 1)) {
			cmd_exit();
		}
		char *cmds[] = {"search", "update", "exit", NULL};
		void (*exec_cmds[])(void) = {&cmd_search, &cmd_update, &cmd_exit, NULL};
		for (ull i = 0; cmds[i]; i++) {
			if (memcmp(cmds[i], storage, strlen(cmds[i])) == 0) {
				if (flag & (f_uint)VERBOSE_OPT) {
					printf("[v]: command match: %s\n", cmds[i]);
				}
				exec_cmds[i]();
				break;
			}
			else {
				if (*storage == '\0')
					continue;
				if (!cmds[i + 1]) {
					if (flag & (f_uint)VERBOSE_OPT) {
						printf("[v]: command unknown: %s\n", storage);
					}
					break;
				}
			}
		}
	}
}

