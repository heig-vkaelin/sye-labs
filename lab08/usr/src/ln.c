#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Error! Usage: %s <target> <symbolic_link>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *target = argv[1];
	char *symboLink = argv[2];

	int fd = open(target, O_RDONLY);
	if (fd < 0) {
		printf("Error! Invalid target file\n");
		return EXIT_FAILURE;
	}

	if (sys_symlink(fd, symboLink) < 0) {
		printf("Error in symlink system call\n");
		if (close(fd) < 0)
			printf("Error! In Close\n");
		return EXIT_FAILURE;
	}

	if (close(fd) < 0)
		printf("Error! In Close\n");

	return EXIT_SUCCESS;
}
