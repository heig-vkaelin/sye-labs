/*
 * Copyright (C) 2014-2017 Daniel Rossier <daniel.rossier@heig-vd.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <syscall.h>

/**
 * Transforme le nombre de bytes en une chaîne de caratère facilement lisible
 *
 * Source du code initial: https://gist.github.com/dgoguerra/7194777
 */
static const char* humanSize(unsigned long bytes) {
	char *suffix[] = { "", "K", "M", "G", "T" };
	char length = sizeof(suffix) / sizeof(suffix[0]);

	int i = 0;
	double dblBytes = bytes;

	if (bytes > 1024) {
		for (i = 0; (bytes / 1024) > 0 && i < length - 1; i++, bytes /= 1024)
			dblBytes = bytes / 1024.0;
	}

	static char output[200];
	sprintf(output, "%g%s", dblBytes, suffix[i]);
	return output;
}

/*
 * Main function of ls application.
 * The ls application is very very short and does not support any options like -l -a etc.
 * It is only possible to give a subdir name to list the directory entries of this subdir.
 */
int main(int argc, char **argv) {
	DIR *stream;
	struct dirent *p_entry;
	char *dir = ".";
	bool lFlag = false;

	while ((++argv)[0]) {
		// Parsing des arguments
		if (argv[0][0] == '-') {
			switch (argv[0][1]) {
			case 'l':
				lFlag = true;
				break;
			case 'a':
				// Si l'on voulait ajouter une autre option
				break;
			default:
				printf("Unknown option -%c\n", argv[0][1]);
				return EXIT_FAILURE;
				break;
			}
		}
		// Si ne commence pas par '-', on stocke le path du sous-dossier
		else {
			dir = argv[0];
		}
	}

	stream = opendir(dir);

	if (stream == NULL)
		return EXIT_FAILURE;

	char name[512]; // marge en plus du d_name pour le potientiel lien symbolique

	while ((p_entry = readdir(stream)) != NULL) {
		// Nom
		sprintf(name, p_entry->d_name);
		if (p_entry->d_type == DT_DIR)
			strcat(name, "/");

		// Dans le cas d'une liste détaillée
		if (lFlag) {
			struct stat stats;
			struct tm *local;
			char dateStr[18];

			if (stat(p_entry->d_name, &stats) < 0) {
				printf("Error! In stat() function\n");
				return EXIT_FAILURE;
			}

			// Si c'est un lien symbolique
			bool isSymboLink = stats.st_mode & AM_SYM;
			if (isSymboLink) {
				int fd = open(name, O_PATH | O_NOFOLLOW);

				if (fd < 0) {
					printf("Error! In Symbolink open\n");
					return EXIT_FAILURE;
				}

				char targetFileName[FILENAME_SIZE];
				int n = read(fd, targetFileName, FILENAME_SIZE - 1);
				if (n < 0) {
					printf("Error! In Symbolink read\n");
					if (close(fd) < 0)
						printf("Error! In Close\n");
					return EXIT_FAILURE;
				}
				targetFileName[n] = 0;

				strcat(name, " -> ");
				strcat(name, targetFileName);
				if (close(fd) < 0) {
					printf("Error! In Close\n");
					return EXIT_FAILURE;
				}

			}

			// Attributs
			char attr[] = "----";
			if (p_entry->d_type == DT_DIR)
				attr[0] = 'd';
			else if (isSymboLink)
				attr[0] = 'l';
			if (stats.st_mode & AM_RD)
				attr[1] = 'r';
			if (stats.st_mode & AM_WR)
				attr[2] = 'w';
			if (stats.st_mode & AM_EX)
				attr[3] = 'x';

			// Date et heure
			local = localtime((const time_t*) &stats.st_mtim);
			sprintf(dateStr, "%d-%d-%d %d:%d", local->tm_year + 1900,
					local->tm_mon + 1, local->tm_mday, local->tm_hour,
					local->tm_min);

			printf("%s | %s | %s | %s\n", attr, dateStr,
					humanSize(stats.st_size), name);
		} else {
			printf("%s\n", name);
		}
	}

	if (closedir(stream) < 0) {
		printf("Error! When closing directory\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
