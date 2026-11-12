#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <archive.h>
#include <ctype.h>

#define CURL_PATH "https://github.com/2cat1vim/exposx/releases/latest"
#define CURL_DOWNLOAD_START_PATH "https://github.com/2cat1vim/exposx/archive/refs/tags/"
#define CURL_DOWNLOAD_END_PATH ".tar.gz"
#define UPDATETAR_NAME "update_exposx.tar.gz"
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

void build_update(void) {
	ssize_t r;
	char buf[16384];
	size_t bufsize = sizeof(buf);
	struct archive *a = archive_read_new();
	struct archive_entry *entry;
	archive_read_support_filter_all(a);
	archive_read_support_format_raw(a);
	if (archive_read_open_filename(a, (const char*)UPDATETAR_NAME, 16384) != ARCHIVE_OK) {
		fprintf(stderr, "[v]: archive not ok\n");
		return ;
	}
	if (archive_read_next_header(a, &entry) != ARCHIVE_OK) {
		fprintf(stderr, "[v]: archive not ok\n");
		return ;
	}
	while (1) {
		r = archive_read_data(a, buf, bufsize);
		if (r < 0) {
			fprintf(stderr, "[v]: archive not ok\n");
			return ;
		}
		if (r == 0) {
			break;
		}
		write(1, buf, r);
	}
	archive_read_free(a);	
}

void cmd_update(void) {
	CURL *curl;
	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
		fprintf(stderr, "[v]: curl not ok\n");
		return ;
	}
	curl = curl_easy_init();
	if (!curl) {
		if (flag & (f_uint)VERBOSE_OPT) {
			fprintf(stderr, "[v]: curl_easy_init is NULL");
		}
		curl_global_cleanup();
		exit(EXIT_FAILURE);
	}
	curl_easy_setopt(curl, CURLOPT_URL, CURL_PATH);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
	if (curl_easy_perform(curl) != CURLE_OK) {	
		fprintf(stderr, "[v]: curl request failed\n");
	}
	char *info = NULL;
	curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &info);
	if (!info) {
		if (flag & (f_uint)VERBOSE_OPT) {
			fprintf(stderr, "[v]: curl_easy_getinfo is NULL");
		}
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		exit(EXIT_FAILURE);
	}
	char *git_version = strrchr(info, '/') + 1;
	if (memcmp(git_version, version, strlen(git_version)) == 0) {
		fprintf(stdout, "You already at the latest version `%s`!\nAbort..\n", version);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return ;
	}
	fprintf(stdout, "You are outdated, latest is `%s`\nUpdating? [y/n]\n", git_version);
	char reply;
	read(STDIN_FILENO, &reply, 1);
	getchar();
	reply = tolower(reply);
	if (reply != 'y') {
		fprintf(stdout, "Abort..\n");
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return ;
	}
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	FILE *f = fopen((const char*)UPDATETAR_NAME, "w");
	if (!f) {
		perror("fopen");
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		exit(EXIT_FAILURE);
	}
	ull len = strlen(CURL_DOWNLOAD_START_PATH) + strlen(git_version) + strlen(CURL_DOWNLOAD_END_PATH) + 1;
	char format_path[len];
	snprintf(format_path, len, "%s%s%s", CURL_DOWNLOAD_START_PATH, git_version, CURL_DOWNLOAD_END_PATH);
	curl_easy_setopt(curl, CURLOPT_URL, format_path);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "exposx");
	if (curl_easy_perform(curl) != CURLE_OK) {
		fprintf(stderr, "[v]: curl request failed\n");
	}
	fclose(f);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	build_update();
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

