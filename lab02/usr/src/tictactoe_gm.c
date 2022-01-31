/**
 * @file    tictactoe_gm.c
 * @author  REDS Institute / HEIG-VD
 * @date    2021-10-04
 *
 * @brief   SYE 2021 - Lab02
 *
 * Tic-tac-toe game manager.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <syscall.h>
#include <unistd.h>

#include <tictactoe.h>

// Variable that represents a tictactoe game.
game_t game;

/**
 * @brief   Min-max algorithm.
 *
 * @param   grid    Pointer to game's grid
 * @param   player  Player number
 * @param   depth   Max depth to explore
 * @return  int     The player score for the grid
 */
static int minmax(int player, unsigned int depth) {
    int i, move, next_score, score;

    if (!depth) {
        return 0;
    }

    move  = -1;
    score = -2;   // Losing moves are preferred to no move

    // For all moves,
    for (i = 0; i < GRID_SIZE; i++) {
        // If legal
        if (game.grid[i] == EMPTY_ID) {
            // Try the move
            game.grid[i] = player;

            // Get move score
            next_score = -minmax(-player, depth - 1);

            // Pick the one that's worst for the opponent
            if (next_score > score) {
                score = next_score;
                move  = i;
            }

            // Revert move
            game.grid[i] = 0;

            if (score > 0) {
                return score;
            }
        }
    }
    if (move == -1) {
        return 0;
    }

    return score;
}

/**
 * @brief   Play computer's turn.
 *
 * @return  int     Grid index of the computer's turn
 */
static int computers_turn(void) {
    int move  = -1;
    int score = -2;
    int i;
    int depth = MAX_DEPTH;
    int tempScore;

    for (i = 0; i < GRID_SIZE; i++) {
        if (game.grid[i] == EMPTY_ID) {
            // Do move
            game.grid[i] = AI_ID;

            // Check move score
            tempScore = -minmax(AI_ID, depth - 1);

            // Store better score
            if (tempScore > score) {
                score = tempScore;
                move  = i;
            }

            // Revert move
            game.grid[i] = 0;
        }
    }

    // Return best move
    return move + 1;
}

/**
 * @brief   Draw the game's grid.
 */
static void draw_grid(void) {
    int  i, j;
    char symbol;

    for (i = 0; i < GRID_HEIGHT; i++) {
        for (j = 0; j < GRID_WIDTH; j++) {
            switch (game.grid[i * GRID_WIDTH + j]) {
                case PLAYER_ID:
                    symbol = PLAYER_SYMBOL;
                    break;

                case AI_ID:
                    symbol = AI_SYMBOL;
                    break;

                default:
                    symbol = EMPTY_SYMBOL;
                    break;
            }
            if (j > 0)
                printf("|");
            printf(" %c ", symbol);
        }
        printf(" \n");
        if (i < GRID_HEIGHT - 1) {
            for (j = 0; j < GRID_WIDTH; j++) {
                if (j > 0)
                    printf("+");
                printf("---");
            }
            printf(" \n");
        }
    }
}

/**
 * @brief   Check if the game is won by anybody, either the player or the computer. The participant who has aligned
 *          three symbols wins the game.
 *
 * @returns STATE_WIN if one of the two players has won the game, STATE_DRAW if the grid is full and there is no winner,
 *          STATE_PROGRESS otherwise.
 */
static int win(void) {
	// L'algorithme de détection de victoire fonctionne que pour des grilles carrées.

	// Lignes
	for (int i = 0; i < GRID_HEIGHT; ++i) {
		int rowIndex = i * GRID_WIDTH;
		int rowSum = 0;
		for (int j = 0; j < GRID_WIDTH; ++j) {
			rowSum += game.grid[rowIndex + j];
		}
		if (rowSum == AI_ID * GRID_WIDTH || rowSum == PLAYER_ID * GRID_WIDTH) {
			return STATE_WIN;
		}
	}

	// Colonnes
	for (int i = 0; i < GRID_WIDTH; ++i) {
		int colSum = 0;
		for (int j = 0; j < GRID_HEIGHT; ++j) {
			colSum += game.grid[GRID_WIDTH * j + i];
		}
		if (colSum == AI_ID * GRID_HEIGHT || colSum == PLAYER_ID * GRID_HEIGHT) {
			return STATE_WIN;
		}
	}

	// Diagonales
	int diag1Sum = 0;
	int diag2Sum = 0;
	for (int i = 0; i < GRID_WIDTH; ++i) {
		diag1Sum += game.grid[(GRID_WIDTH + 1)  * i];
		diag2Sum += game.grid[(GRID_WIDTH - 1) * (i + 1)];
	}
	if (diag1Sum == AI_ID * GRID_WIDTH || diag1Sum == PLAYER_ID * GRID_WIDTH) {
		return STATE_WIN;
	}
	if (diag2Sum == AI_ID * GRID_WIDTH || diag2Sum == PLAYER_ID * GRID_WIDTH) {
		return STATE_WIN;
	}

	// Partie pas encore terminée
	for (int i = 0; i < GRID_SIZE; ++i) {
		if (game.grid[i] == EMPTY_ID) {
			return STATE_PROGRESS;
		}
	}

	// Autrement: égalité
    return STATE_DRAW;
}

/**
 * @brief   Initialize game data.
 */
static void init_game(void) {
    // TO COMPLETE
	printf("Initial board\n");
	draw_grid();
	printf("The valid moves are 1-%d.\n", GRID_SIZE);
}

/**
 * @brief   Function to display start of player's turn.
 */
static void displayBeginPlayersTurn(void) {
    printf("Begin playersTurn\n");
}

/**
 * @brief   Function to display start of computer's turn.
 */
static void displayBeginComputersTurn(void) {
    printf("Begin computersTurn\n");
}

/**
 * @brief   Function to display end of player's turn.
 */
static void displayEndPlayersTurn(void) {
    printf("End playersTurn\n");
}

/**
 * @brief   Function to display end of computer's turn.
 */
static void displayEndComputersTurn(void) {
    printf("End computersTurn\n");
}

/**
 * @brief   Ask player for a move until we get a valid one
 */
static int get_player_move(void) {
    int move, valid = 0;

    // Ask player-side for player move
    gm_send_cmd(YOU_CAN_PLAY);

    while (!valid) {
        move = gm_get_cmd();

        // If move is false, ask again. Else, return it
        if ((move < 0) || (move > GRID_SIZE-1) || (game.grid[move] != 0)) {
            gm_send_cmd(MOVE_REJECTED);
        }
        else {
            valid = 1;
        }
    }

    return move;
}

/**
 * @brief   Check the state of the game.
 */
static void check_game_state(int currentID) {
    int state;

    switch (state = win()) {
        case STATE_PROGRESS:
            break;

        case STATE_WIN:
            if (currentID == PLAYER_ID) {
                gm_send_cmd(YOU_WIN);
            }
            else {
                gm_send_cmd(YOU_LOSE);
            }
            game.over = 1;

            break;

        case STATE_DRAW:
            gm_send_cmd(NO_WINNER);
            game.over = 1;

            break;
    }
}

/**
 * @brief   Manages the game behaviour.
 */
static void game_manager(void) {
    int move;
    int player = PLAYER_ID;   // Start with the player

    // Signal player-side that the game has started
    gm_send_cmd(GAME_START);

    while (!game.over) {
        if (player == PLAYER_ID) {
            displayBeginPlayersTurn();

            // Ask for player's move
            move = get_player_move();

            // Apply move on grid
            game.grid[move] = PLAYER_ID;

            displayEndPlayersTurn();
            draw_grid();
        }
        else {
            displayBeginComputersTurn();

            // The computer plays its turn
            move            = computers_turn() - 1;
            game.grid[move] = AI_ID;

            draw_grid();
            displayEndComputersTurn();
        }

        // Check for a winner or a draw
        check_game_state(player);

        player = ((player == PLAYER_ID) ? AI_ID : PLAYER_ID);
    }

    // Signal player-side that the game has ended
    gm_send_cmd(GAME_END);

    return;
}

/**
 * @brief   Main function
 */
int main(int argc, char **argv) {
	// La détection du gagnant ne fonctionne que sur les grilles carrées (NxN)
	if (GRID_WIDTH != GRID_HEIGHT) {
		printf("Erreur: La grille doit être carrée.\n");
		return -1;
	}
	init_game();

    // Initialize Inter-Process Communication library
    ipc_init();   // DON'T REMOVE

    pid_t child_pid = sys_fork2();
    // Process parent : gm
	if (child_pid != 0) {
		game_manager();
		waitpid(child_pid, NULL, 0);
	}
	// Process enfant: player
	else {
		char argv1[16];
		char argv2[16];
		sprintf (argv1, "%d", ipc_player_argv1());
		sprintf (argv2, "%d", ipc_player_argv2());

		execl("./tictactoe_player.elf", "tictactoe_player", argv1, argv2, NULL);
	}

    ipc_close();    // DON'T REMOVE

    return 0;
}
