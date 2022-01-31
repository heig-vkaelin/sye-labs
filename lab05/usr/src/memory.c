#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

// 1024 * 4 (uint32_t) = 4096 = 4 KiB
#define MEMORY_SIZE 1024

int main(int argc, char **argv) {
	uint32_t *range = (uint32_t*) malloc(MEMORY_SIZE * sizeof(uint32_t));
	if (range == NULL) {
		printf("Error in memory allocation!\n");
		return EXIT_FAILURE;
	}

	printf("Adresse virtuelle: %p - Adresse physique: %p \n", range,
			sys_translate((uint32_t) range));

	free(range);

	return EXIT_SUCCESS;
}
