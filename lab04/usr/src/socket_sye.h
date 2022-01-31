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
 
#ifndef __SOCKET_SYE_H_INCLUDED__
#define __SOCKET_SYE_H_INCLUDED__

/* Chaines de charactères à envoyer */
#define BONJOUR     "Bonjour"
#define AUREVOIR    "Aurevoir"
#define COMPTEUR    "Compteur"
#define QUITTER     "Quitter"

/* Index de routage pouvant être utilisés dans le client */
typedef enum MSG_IDX { GREETING=1, COUNTING, QUITING} msg_idx_t;

// Taille des buffers de lecture et d'écriture côté client et serveur
#define BUFFER_LEN 80

/*
 * Fonction facilitant la gestion et l'affichage des erreurs
 * Source: https://man7.org/linux/man-pages/man2/bind.2.html#EXAMPLES
 */
#define handle_error(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)

#endif /* __SOCKET_SYE_H_INCLUDED__ */
