#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#include <pthread.h>
#include <string.h>

#define COUNT 500
#define MAX_THREADS 15

/*
 * Variable globale permettant de savoir si le programme s'est bien déroulé.
 * Permet de savoir quelle valeur va retourner la fonction principale.
 * 1: aucune erreur / 0: une erreur s'est produite
 */
int all_good = 1;

/**
 * @brief Function that execute the count
 * @param args The name of the thread
 * @return NULL
 */
void* count(void *args) {
	int id = atoi((char*) args);

	for (size_t i = 0; i < COUNT; ++i) {
		fprintf(stdout, "Thread %d: %zu\n", id, i);
	}

	return NULL;
}

/**
 * @brief Launches N parallel threads
 * @param num_of_threads Number of threads to be launched
 */
void parallel_threads_launch(int num_of_threads) {
	pthread_t thread_list[MAX_THREADS];
	// Tableau qui va contenir l'id de chaque thread sous forme de tableau de char
	char args[MAX_THREADS][8];

	for (int id = 0; id < num_of_threads; ++id) {
		int code;
		sprintf(args[id], "%d", id);

		code = pthread_create(&thread_list[id], NULL, count, (void*) args[id]);
		// Si la création du thread ne fonctionne pas, on continue l'exécution du
		// programme avec les threads précédemment créés.
		if (code) {
			fprintf(stderr, "ERROR; return code from pthread_create() is %d\n",
					code);
			num_of_threads = id;
			all_good = 0;
			break;
		}
	}

	for (int id = 0; id < num_of_threads; ++id) {
		pthread_join(thread_list[id], NULL);
	}
}

/**
 * @brief Launches N sequential threads
 * @param num_of_threads Number of threads to be launched
 */
void sequential_threads_launch(int num_of_threads) {
	for (int id = 0; id < num_of_threads; ++id) {
		pthread_t thread;
		char arg[8];
		int code;
		sprintf(arg, "%d", id);

		code = pthread_create(&thread, NULL, count, (void*) arg);
		if (code) {
			fprintf(stderr, "ERROR; return code from pthread_create() is %d\n",
					code);
			all_good = 0;
			return;
		}
		pthread_join(thread, NULL);
	}
}

/**
 * @brief Main function
 */
int main(int argc, char **argv) {
	int n;
	char mode;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <N> <p/s>\n", argv[0]);
		return EXIT_FAILURE;
	}

	n = atoi(argv[1]);
	mode = *argv[2];

	if (n > MAX_THREADS) {
		fprintf(stderr, "N can't be bigger than %d\n", MAX_THREADS);
		return EXIT_FAILURE;
	}

	switch (mode) {
	case 'p':
		parallel_threads_launch(n);
		break;
	case 's':
		sequential_threads_launch(n);
		break;
	default:
		fprintf(stderr, "Usage: %s <N> <p/s>\n", argv[0]);
		return EXIT_FAILURE;
	}

	return all_good ? EXIT_SUCCESS : EXIT_FAILURE;
}
