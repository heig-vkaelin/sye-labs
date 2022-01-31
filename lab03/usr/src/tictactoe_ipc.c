#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tictactoe_priv.h"

/* Private struct */
sye_ipc_t ipc;

/**
 * @brief Lit la valeur dans le pipe choisi via le paramètre filedes
 * @return -1 en cas d'erreur, la valeur de la commande autrement
 */
int read_cmd(int filedes) {
	char buffer[CMD_SIZE];
	if (read(filedes, buffer, CMD_SIZE) == -1) {
		return -1;
	}
	return atoi(buffer);
}

/**
 * @brief Ecrit dans le pipe choisi via le paramètre filedes
 * @return -1 en cas d'erreur, 0 autrement
 */
int write_cmd(int cmd, int filedes) {
	char buffer[CMD_SIZE];
	sprintf(buffer, "%d", cmd);

	if (write(filedes, buffer, CMD_SIZE) == -1) {
		return -1;
	}
	return 0;
}

/**
 * @brief Create the pipes and save the file descriptors in the ipc struct
 * @return 0;
 */
int ipc_init(void) {
	if (pipe(ipc.pipe1) < 0 || pipe(ipc.pipe2) < 0) {
		exit(-1);
	}

	ipc.gm_recv = ipc.pipe1[READ_SIDE];
	ipc.player_send = ipc.pipe1[WRITE_SIDE];
	ipc.player_recv = ipc.pipe2[READ_SIDE];
	ipc.gm_send = ipc.pipe2[WRITE_SIDE];

	return 0;
}

/**
 * @brief Initializes the player file descriptors
 * @param argv The 2 file descriptors for the child process
 */
int ipc_init_child(char **argv) {
	ipc.player_recv = atoi(argv[1]);
	ipc.player_send = atoi(argv[2]);

	return 0;
}

/**
 * @brief Closes all open pipes
 */
void ipc_close(void) {
	if (close(ipc.gm_recv) != 0 || close(ipc.gm_send) != 0
			|| close(ipc.player_recv) != 0 || close(ipc.player_send) != 0) {
		exit(-1);
	}
}

/**
 * @brief Reads the pipe used to communicate from the player to the game manager
 * @return -1 if error, command value otherwise
 */
int gm_get_cmd(void) {
	return read_cmd(ipc.gm_recv);
}

/**
 * @brief Writes the pipe used to communicate from the the game manager to the player
 * @return -1 if error, 0 otherwise
 */
int gm_send_cmd(int cmd) {
	return write_cmd(cmd, ipc.gm_send);
}

/**
 * @brief Reads the pipe used to communicate from the game manager to the player
 * @return -1 if error, command value otherwise
 */
int player_get_cmd(void) {
	return read_cmd(ipc.player_recv);
}

/**
 * @brief Writes the pipe used to communicate from the the player to the game manager
 * @return -1 if error, 0 otherwise
 */
int player_send_cmd(int cmd) {
	return write_cmd(cmd, ipc.player_send);
}

/**
 * @brief Get player receiving pipe file descriptor
 * @return Pipe file descriptor
 */
int ipc_player_argv1(void) {
	return ipc.player_recv;
}

/**
 * @brief Get player writing pipe file descriptor
 * @return Pipe file descriptor
 */
int ipc_player_argv2(void) {
	return ipc.player_send;
}
