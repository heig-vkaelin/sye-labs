/*
 * Copyright (C) 2021 David Truan <david.truan@heig-vd.ch>
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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "socket_sye.h"

/* Variable de "routage" */
msg_idx_t message_to_send = 0;

/* Handlers des signaux */
void process_usr_sig(int sig) {
	switch (sig) {
	case SIGUSR1:
		printf("Handler USR1\n");
		message_to_send = GREETING;
		break;
	case SIGUSR2:
		printf("Handler USR2\n");
		message_to_send = COUNTING;
		break;
	default:
		break;
	}
}

void process_int_sig(int sig) {
	printf("Handler INT\n");
	message_to_send = QUITING;
}

int main(int argc, char **argv) {
	/* Variables */
	int running = 1, sockfd = 0, port = 0, n = 0;
	bool greetings = true;
	struct sockaddr_in serv_addr;

	char sendBuff[BUFFER_LEN];
	char receiveBuff[BUFFER_LEN];
	memset(sendBuff, 0, sizeof(sendBuff));
	memset(receiveBuff, 0, sizeof(receiveBuff));

	/* Traitement des arguments */
	if (argc != 3) {
		printf("\n Usage: %s <ip of server> <port>\n", argv[0]);
		return EXIT_FAILURE;
	}
	port = atoi(argv[2]);
	if (port <= 0) {
		printf("Error: Invalid port %d\n", port);
		return EXIT_FAILURE;
	}

	/* Liens entre signaux et handlers */
	signal(SIGUSR1, process_usr_sig);
	signal(SIGUSR2, process_usr_sig);
	signal(SIGINT, process_int_sig);

	/* Création du socket et connection au serveur */
	printf("Connecting to server at %s:%d...", argv[1], port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		handle_error("socket");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
		handle_error("inet_pton");

	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		handle_error("connect");

	// Connexion avec le serveur réussie
	printf("SUCCESS\n");

	while (running) {
		/* Préparation en envoi du message */
		switch (message_to_send) {
		case GREETING:
			snprintf(sendBuff, sizeof(sendBuff), "%s",
					greetings ? BONJOUR : AUREVOIR);
			greetings = !greetings;
			break;
		case COUNTING:
			snprintf(sendBuff, sizeof(sendBuff), "%s", COMPTEUR);
			break;
		case QUITING:
			snprintf(sendBuff, sizeof(sendBuff), "%s", QUITTER);
			break;
		default:
			continue;
		}

		if (write(sockfd, sendBuff, strlen(sendBuff)) < 0) {
			printf("Error: client write\n");
			break;
		}

		/* Réception, si nécessaire de la réponse */
		switch (message_to_send) {
		case GREETING:
		case COUNTING:
			n = read(sockfd, receiveBuff, sizeof(receiveBuff));
			if (n < 0) {
				printf("Error: client read\n");
				running = 0; // stop la boucle car on ne peut pas break dans un switch
				break;
			}
			receiveBuff[n] = 0;
			printf("%s\n", receiveBuff);
			break;
		case QUITING:
			running = 0;
			break;
		}

		message_to_send = 0;
	}

	/* Nettoyage des ressources */
	close(sockfd);

	return EXIT_SUCCESS;
}
