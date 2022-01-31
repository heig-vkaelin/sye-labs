#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


#define RESET 2
#define DELTA 2
#define EXIT_MEMRPLC  (-2)


#define PAGE_NB_MASK    0x00F0
#define REF_BIT_MASK    0x0004
#define SWAP_BIT_MASK   0x0002
#define VALID_BIT_MASK  0x0001


#define NB_PAGES 16


/* [7..4] = Page number on 4 bits */
/* [3] = Unused 1 bit */
/* [2] = Reference on 1 bit */
/* [1] = SWAP on 1 bit */
/* [0] = Valid on 1 bit */
uint8_t page_table[NB_PAGES] = {0};

// Partie 2 - LRU
// Compteurs de réinitialisation du bit référence pour chaque page
uint8_t counters[NB_PAGES] = {0};
uint8_t globalCounter = 0;

// Partie 3 - WSClock
uint16_t tvc = 0;
uint16_t tdu[NB_PAGES] = {0};


void replaceLRU(void) {
	int toReplace = -1;
	for (size_t i = 0; i < NB_PAGES; ++i) {
		if (!(page_table[i] & VALID_BIT_MASK))
			continue;

		if (toReplace == -1 || counters[i] < counters[toReplace])
			toReplace = i;
	}

	if (toReplace == -1) {
		printf("Error: valid page not found\n");
		exit(-1);
	}
	page_table[toReplace] &= ~VALID_BIT_MASK;
	page_table[toReplace] |= SWAP_BIT_MASK;
}


void incCompteur(int page_no) {
	if (page_no >= 0 && page_no < NB_PAGES)
		counters[page_no] = ++globalCounter;
}


void replaceWSC(void) {
	int toReplace = -1;
	for (size_t i = 0; i < NB_PAGES; ++i) {
		if (!(page_table[i] & VALID_BIT_MASK))
			continue;

		// On stocke la 1ère en RAM si aucune page ne page ne remplit les conditions
		if (toReplace == -1)
			toReplace = i;

		if (page_table[i] & REF_BIT_MASK) {
			page_table[i] &= ~REF_BIT_MASK;
		} else if (tvc - tdu[i] > DELTA) {
			toReplace = i;
			break;
		}
	}

	if (toReplace == -1) {
		printf("Error: valid page not found\n");
		exit(-1);
	}
	page_table[toReplace] &= ~VALID_BIT_MASK;
	page_table[toReplace] |= SWAP_BIT_MASK;
}


void updateTDU(void) {
	for (size_t i = 0; i < NB_PAGES; ++i) {
		if (page_table[i] & REF_BIT_MASK & VALID_BIT_MASK)
			tdu[i] = tvc;
	}
}



void init_page_table(void) {
	int i;

	for(i = 0; i < 16; i++)
		page_table[i] |= (i & 0xF) << 4;

	page_table[0] |= VALID_BIT_MASK;
	page_table[1] |= VALID_BIT_MASK;
	page_table[2] |= VALID_BIT_MASK;
	page_table[3] |= SWAP_BIT_MASK;
}

void print_memory(void) {
	int i;

	printf("RAM : ");

	for(i = 0; i < 16; i++)
		if(page_table[i] & 1)
			printf("[%d] ", i);

	printf("\nSWAP : ");
	for(i = 0; i < 16; i++)
			if(!(page_table[i] & VALID_BIT_MASK) &&  (page_table[i] & SWAP_BIT_MASK))
				printf("[%d] ", i);

	printf("\n");
}

int ask_user_page_no() {
	char input[4];

	printf("Enter the page to be access: ");
	fflush(stdout);
	fgets(input, sizeof(input), stdin);

	if(input[0] == 'E' || input[0] == 'e')
		return EXIT_MEMRPLC;

	if (!isdigit(input[0]))
		return -1;

	return atoi(input);
}

int main(int argc, char *argv[]) {
	int page_no;
	int is_valid = 1;
	int LRU_run = 0;

	if (argc != 2) {
		is_valid = 0;
	} else if (strcmp("LRU", argv[1]) == 0) {
		LRU_run = 1;
	} else if (strcmp("WSC", argv[1]) == 0) {
		LRU_run = 0;
	} else {
		is_valid = 0;
	}

	if (!is_valid) {
		printf("Usage: memreplace LRU|WSC (press e|E for exit)\n");
		exit(1);
	}

	init_page_table();
	print_memory();

	for(;;)
	{
		page_no = ask_user_page_no();

		if(page_no == EXIT_MEMRPLC)
			return 0;
		else
		{
			if(page_no >= 0 && page_no <= 15)
			{
				if (LRU_run) {
					/* LRU Related */

					/* VALID = 0 ? */
					if (!(page_table[page_no] & VALID_BIT_MASK)) {
						replaceLRU();
						page_table[page_no] &= ~SWAP_BIT_MASK;
						page_table[page_no] |= VALID_BIT_MASK;
					}

					incCompteur(page_no);
				} else {
					/* WSC Related */

                    /* VALID = 0 ? */
					if (!(page_table[page_no] & VALID_BIT_MASK)) {
						replaceWSC();
						page_table[page_no] &= ~SWAP_BIT_MASK;
					}
					page_table[page_no] |= REF_BIT_MASK;
					page_table[page_no] |= VALID_BIT_MASK;

					tvc++;
					updateTDU();
				}

				print_memory();
			}
			else
				printf("Invalid page number\n");
		}
	}

	return 0;
}
