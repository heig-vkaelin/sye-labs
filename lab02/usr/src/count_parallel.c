#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PROCESS 15

int main(int argc, char **argv) {
	int n;
	char mode;
	pid_t pid_list[MAX_PROCESS];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <N> <p/s>\n", argv[0]);
		return EXIT_FAILURE;
	}

	n = atoi(argv[1]);
	mode = *argv[2];

	if (n > MAX_PROCESS) {
		fprintf(stderr, "N can't be bigger than %d\n", MAX_PROCESS);
		return EXIT_FAILURE;
	} else if (mode != 's' && mode != 'p') {
		fprintf(stderr, "Usage: %s <N> <p/s>\n", argv[0]);
		return EXIT_FAILURE;
	}

	for (int id = 0; id < n; ++id) {
		char execName[16];
		char argv1[8];
		sprintf(execName, "count-%d", id);
		sprintf(argv1, "%d", id);
		pid_t child_pid = sys_fork2();
		if (child_pid == 0) {
			execl("./count.elf", execName, argv1, NULL);
		}
		// On sauvegarde le pid du processus si on doit l'attendre plus tard
		// (mode parallèle)
		pid_list[id] = child_pid;
		// On attend la fin du processus uniquement en mode séquentiel
		if (mode == 's') {
			waitpid(child_pid, NULL, 0);
		}
	}

	// Si on est en mode parallèle, on attend tous les processus à la fin
	if (mode == 'p') {
		for (int id = 0; id < n; ++id) {
			waitpid(pid_list[id], NULL, 0);
		}
	}

    return EXIT_SUCCESS;
}
