#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#define CURL_PATH "https://github.com/2cat1vim/exposx/releases/latest"
#define CONF_PATH ".exposx"
	#define VERSION_STRING "version:"
	#define VERSION_SIZE 4

#define READ_LIMIT 256

#define NO_OPT (1u << 0)
#define VERBOSE_OPT (1u << 1)
#define STDIN_OPT (1u << 2)

typedef unsigned int f_uint;
typedef unsigned long long ull;

f_uint flag = 0;
char version[VERSION_SIZE + 1];

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
	CURL *curl_version;
	CURL *curl;
	CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
	if (result != CURLE_OK) {
		fprintf(stderr, "[v]: curl not ok\n");
		return ;
	}
	curl_version = curl_easy_init();
	if (curl_version) {
		curl_easy_setopt(curl_version, CURLOPT_URL, CURL_PATH);
		curl_easy_setopt(curl_version, CURLOPT_NOBODY, 1L);
		if (curl_easy_perform(curl_version) != CURLE_OK) {	
			fprintf(stderr, "[v]: curl request failed\n");
		}
		char *info = NULL;
		curl_easy_getinfo(curl_version, CURLINFO_REDIRECT_URL, &info);
		if (info) {
			char *git_version = strrchr(info, '/') + 1;
			if (memcmp(git_version, version, strlen(git_version)) == 0) {
				fprintf(stdout, "You already at the latest version `%s`!\nAbort..\n", version);
				curl_easy_cleanup(curl_version);
				return ;
			}
			else {
				fprintf(stdout, "You are outdated, latest is `%s`\nUpdating..", git_version);
			}
		}
		curl_easy_cleanup(curl_version);
	}	
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, CURL_PATH);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		FILE *f = fopen("update_exposx.tar.gz", "w");
		if (!f) {
			perror("fopen");
			exit(EXIT_FAILURE);
		}
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "exposx");
		if (curl_easy_perform(curl) != CURLE_OK) {
			fprintf(stderr, "[v]: curl request failed\n");
		}
		curl_easy_cleanup(curl);
		fclose(f);
	}
	curl_global_cleanup();
}

void set_version() {
	FILE *f = fopen((const char*)CONF_PATH, "r");
	if (!f) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	while (1) {
		char buf;
		char storage[(ull)READ_LIMIT];
		ull pos = 0;
		ull r = 0;
		while ((r = read(fileno(f), &buf, 1)) > 0) {
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
		if (memcmp(VERSION_STRING, storage, strlen(VERSION_STRING)) == 0) {
			strncpy(version, storage + strlen(VERSION_STRING), VERSION_SIZE);
			if (flag & (f_uint)VERBOSE_OPT) {
				printf("[v]: found version `%s`\n", version);
			}
			break;
		}
		if (r == 0) {
			fprintf(stderr, "[v]: .exposx is missing 'version:'\n");
			break;
		}
	}
	fclose(f);
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
	set_version();
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

