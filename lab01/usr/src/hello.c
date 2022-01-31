

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char msg[] = "Hello world !";
	printf("%s\n", msg);
	printf("%p\n", msg);

	// Adresse en dur récupérée grâce à l'exécution du code au-dessus
	const unsigned ADR = 0xbffcef24;
	const unsigned NB_CHAR = sizeof(msg) / sizeof(char);

	for (unsigned i = 0; i < NB_CHAR; ++i) {
		(*(char*) (ADR + i))++;
	}
	printf("%s\n", msg);

	return 0;
}
