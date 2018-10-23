#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPBufferset.h"
#include "Parser.h"
#include "Commands.h"

int main(void){
	Game *game; int exit_prog=0; int num_of_args=0; int no_prev=1;int first_game=1;
	int valid=0; char **information=(char**)malloc(1024);
	if (!information){
		printf("Error: malloc has failed\n");
		exit(0);
	}
	SP_BUFF_SET();
	game = createGame();
	printf("Sudoku\n------\n");
	while (1){
		printf("Enter your command:\n");
		valid=read_input(game, information, &exit_prog, &num_of_args);
		if (valid==1){
			execute_cmd(game, information, &exit_prog, &num_of_args, &no_prev, &first_game);
		}
		if (exit_prog==1) {/*means we reached EOF or got exit command from the user and we exit the program*/
			if (strcmp(game->game_mode,"Init")!=0)free_board(game->board);
			free_list(game->undo_redo);
			free(game);
			free(information);
			printf("Exiting...\n");
			return 0;
		}
	}
	return 100;
}
