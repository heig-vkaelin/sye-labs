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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket_sye.h"

int main(int argc, char *argv[]) {
	/* Déclarations des variables */
	const int SERVER_PORT = 5000;
	const int NB_CONNECTIONS = 1;

	int counter = 0, n = 0;
	int listenfd = 0, connfd = 0;

	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	char client_ip[INET_ADDRSTRLEN];

	char sendBuff[BUFFER_LEN];
	char receiveBuff[BUFFER_LEN];

	/* Mise en place du socket et connexion avec le client */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
		handle_error("socket");

	memset(&serv_addr, 0, sizeof(serv_addr));
	memset(sendBuff, 0, sizeof(sendBuff));
	memset(receiveBuff, 0, sizeof(receiveBuff));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVER_PORT);

	if (bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		handle_error("bind");

	if (listen(listenfd, NB_CONNECTIONS) < 0)
		handle_error("listen");

	printf("Waiting for clients...\n");

	memset(&client_addr, 0, sizeof(client_addr));
	int client_addr_size = sizeof(client_addr);

	connfd = accept(listenfd, (struct sockaddr*) &client_addr,
			(socklen_t*) &client_addr_size);
	if (connfd < 0) {
		close(listenfd);
		handle_error("accept");
	}

	/*
	 * Solution afin de stocker l'adresse ip du client sous forme de chaîne de caractères:
	 * https://stackoverflow.com/a/23040684/9188650
	 */
	inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

	printf("Client connected!\n");

	/* Boucle de traitement principal. C'est ici que la reception des message
	 et l'envoi des réponses se fait. */
	while (1) {
		/* Reception */
		n = read(connfd, receiveBuff, sizeof(receiveBuff));
		if (n < 0) {
			printf("Error: server read\n");
			break;
		}
		receiveBuff[n] = 0;
		printf("The client said: %s\n", receiveBuff);

		/* Traitement des messages */
		if (strcmp(receiveBuff, BONJOUR) == 0
				|| strcmp(receiveBuff, AUREVOIR) == 0) {
			snprintf(sendBuff, sizeof(sendBuff), "%s client %s", receiveBuff,
					client_ip);
		} else if (strcmp(receiveBuff, COMPTEUR) == 0) {
			snprintf(sendBuff, sizeof(sendBuff), "Valeur compteur %d",
					++counter);
		} else if (strcmp(receiveBuff, QUITTER) == 0) {
			printf("The client asked for a disconnection, now quitting...\n");
			break;
		} else { // Message du client inconnu
			snprintf(sendBuff, sizeof(sendBuff), "Error");
		}

		/* Envoi de la réponse */
		if (write(connfd, sendBuff, sizeof(sendBuff)) < 0) {
			printf("Error: server write\n");
			break;
		}
	}

	/* Nettoyage des ressources*/
	close(listenfd);
	close(connfd);

	return EXIT_SUCCESS;
}
