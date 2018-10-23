#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "The_Board.h"
#include "Commands.h"
#include "Utilities_for_Commands.h"
#include "GRB_solver.h"
#include "Utilities_for_generate_and_num_solutions.h"
#include "Utilities_for_Parser.h"

static void edit(Game* game, char* file_name, int num_of_args);
static void solve(Game* game, char* file_name,int *first_game);
static void mark_errors(Game* game, int x);
static void set(Game* game ,int x, int y, int z, int *no_prev);
static int validate(Game* game);
static void generate(Game* game, int x, int y, int* no_prev);
static void undo(Game* game, int* no_prev, int reset);
static void redo(Game* game, int *no_prev);
static void save(Game* game, char* file_path);
static void hint(Game* game, int x, int y);
static void num_solutions(Game* game);
static void autofill(Game* game, int *no_prev);
static void reset(Game* game, int* no_prev);
static void exit_now(Game* game, int *exit_prog);


/**
 * execute_cmd:
 * only valid commands reach this part.
 * @param game - a pointer to our played game
 * @param cmd - an array of strings representing the command we need to follow and its arguments
 * @param *exit_prog - turned to 1 when we call exit and marks that the game was ended
 * @param *num_of_args - states the amount of arguments in the command
 * @param *no_prev - 1 if the undo-redo list has no undo move, else 0
 * @param *first_game - equals 1 in the first and only time solve is called, used to set the mark_errors field
 * @return void
 * */
void execute_cmd(Game* game, char** cmd, int *exit_prog, int* num_of_args, int* no_prev, int *first_game){
	int cell_one, cell_two, cell_three;
	if (strcmp(cmd[0],"solve")==0){
		solve(game,cmd[1], first_game);
	}
	else if(strcmp(cmd[0],"edit")==0){
		if (num_of_args==0){
			edit(game,NULL,*num_of_args);
		}
		else{
			edit(game,cmd[1],*num_of_args);
		}
	}
	else if(strcmp(cmd[0],"mark_errors")==0){
		cell_one = atoi(cmd[1]);
		mark_errors(game, cell_one);
	}
	else if(strcmp(cmd[0],"print_board")==0){
		print_board(game);
	}
	else if(strcmp(cmd[0],"set")==0){
		cell_one = atoi(cmd[1]);
		cell_two = atoi(cmd[2]);
		cell_three = atoi(cmd[3]);
		set(game, cell_one, cell_two, cell_three, no_prev);
	}
	else if(strcmp(cmd[0],"validate")==0){
		validate(game);
	}
	else if(strcmp(cmd[0],"generate")==0){
		cell_one = atoi(cmd[1]);
		cell_two = atoi(cmd[2]);
		generate(game, cell_one, cell_two, no_prev);
	}
	else if(strcmp(cmd[0],"undo")==0){
		undo(game, no_prev,0);
	}
	else if(strcmp(cmd[0],"redo")==0){
		redo(game, no_prev);
	}
	else if(strcmp(cmd[0],"save")==0){
		save(game, cmd[1]);
	}
	else if(strcmp(cmd[0],"hint")==0){
		cell_one = atoi(cmd[1]);
		cell_two = atoi(cmd[2]);
		hint(game, cell_one, cell_two);
	}
	else if(strcmp(cmd[0],"num_solutions")==0){
		num_solutions(game);
	}
	else if(strcmp(cmd[0],"autofill")==0){
		autofill(game, no_prev);
	}
	else if(strcmp(cmd[0],"reset")==0){
		reset(game, no_prev);
	}
	else if(strcmp(cmd[0],"exit")==0){
		exit_now(game, exit_prog);
	}
	free_all(cmd, *num_of_args);
}

/**
* print_board - prints the game board
* @param game - a pointer to the game board we wish to print
* @return void
*/
void print_board(Game* game){
	int i,j;
	for (i=0;i< game->board->length_of_board ; i++){
		if ((i%game->board->block_x)==0){
			print_row(game);
		}
		for (j=0; j<game->board->length_of_board ; j++){
			if ((j)%(game->board->block_y)==0){
				printf("|");
			}
			printf(" "); /*stage a. */
			if (game->board->cells[j][i]->val==0){ /*stage b. */
				printf("  ");
			}
			else {
				printf("%2d",game->board->cells[j][i]->val); /*stage b. */
			}
			if (game->board->cells[j][i]->fixed==1){/*stage c. */
				printf(".");
			}
			else if ((game->board->cells[j][i]->erroneous==1 && strcmp(game->game_mode,"Edit")==0) || /*stage c. */
					(game->board->cells[j][i]->erroneous==1 && game->mark_errors==1)){
				printf("*");
			}
			else {  /*stage c. */
				printf(" ");
			}
		}
		printf("|\n");
	}
	print_row(game);
}


/**
 * solve:
 * @param game - a pointer to our played game
 * @param *file_name - the file path from which our board is loaded
 * @param *first_game - equals 1 in the first and only time solve is called, used to set the mark_errors field
 * @return void
 **/
static void solve(Game* game, char* file_name, int *first_game){
	int temp=0;int prev_mark_errors;
	prev_mark_errors=game->mark_errors;
	temp=load(game, file_name);
	if (temp==0){
		printf("Error: File doesn't exist or cannot be opened\n");
		return;
	}
	mark_erroneous_after_load(game);
	mark_loaded_cells(game->board); /*we will mark the non empty loaded cells*/
	free_list(game->undo_redo);
	game->undo_redo=createList(); game->undo_redo->current=NULL;
	if (*first_game){
		*first_game=0;
		game->mark_errors=1;
	}
	else {
		game->mark_errors=prev_mark_errors;
	}
	game->game_mode="Solve";
	print_board(game);
}

/**
 * edit:
 * @param game - a pointer to our played game
 * @param *file_name - the file path from which our board is loaded
 * @param num_of_args - amount of arguments that were sent in the command
 * @return void
 * */
static void edit(Game* game, char* file_name, int num_of_args){
	int temp=0;
	if (num_of_args==1){
		temp=load(game, file_name);
		if (temp==0){
			printf("Error: File cannot be opened\n");
			return;
		}
		remove_fixed_from_cells(game);
		mark_erroneous_after_load(game);
		mark_loaded_cells(game->board); /*we will mark the non empty loaded cells*/
	}
	else {
		if (strcmp(game->game_mode,"Init")!=0){ /* free the previous board, if not in Init mode, I assume there is a board */
			free_board(game->board);
		}
		game->board=createBoard(3,3);
	}
	free_list(game->undo_redo);
	game->undo_redo= createList(); game->undo_redo->current=NULL;
	game->game_mode="Edit";
	print_board(game);
}

/**
 * mark_errors:
 * @param game - a pointer to our played game
 * @param x - the value we shall set to mark errors
 * @return void
 */
static void mark_errors(Game* game, int x){
	game->mark_errors = x;
}

/**
 * set - sets a value to a given cell
 * @param game - a pointer to the game we are playing
 * @param x - x-axis location of the cell which we want to set a value to 
 * @param y - y-axis location of the cell which we want to set a value to
 * @param z - the value we wish to set to the given cell
 * @param *no_prev - 1 if the undo-redo list has no undo move, else 0
 * @return void
 */
static void set(Game* game ,int x, int y, int z, int *no_prev){
	if (z==game->board->cells[x-1][y-1]->val) {
		print_board(game);
		return; /* not changing anything */
	}
	add_to_undo_redo_list_for_set(game, x,y,z, no_prev);
	if(z==0){
		game->board->cells[x-1][y-1]->was_loaded = 0;
		game->board->cells[x-1][y-1]->was_set = 0;
	}
	else{/*if z!=0*/
		game->board->cells[x-1][y-1]->was_set = 1;
	}
	mark_unmark_erroneous_vals(game,x,y,game->board->cells[x-1][y-1]->val, z,"set");
	print_board(game);
	if (strcmp(game->game_mode,"Solve")==0){
		check_game_ended(game);
	}

}

/**
 * validate - validates if a game board is solvable
 * @param game - a pointer to the game we wish to validate
 * @return 1 if game board is solvable, else 0
 */
static int validate(Game* game){
	int solvable = solve_for_validate(game);
	if(contains_erroneous_val(game)){
		printf("Error: board contains erroneous values\n");
	}
	else if (solvable){
		printf("Validation passed: board is solvable\n");
	}
	else {
		printf("Validation failed: board is unsolvable\n");
	}
	return solvable;
}


/**
 * generate - generates a board by first filling x random cells, then running ILP and then leaving only
 * y random cells filled up
 * @param game - a pointer to the game on which we work
 * @param x - amount of random cells we will fill up to begin with
 * @param y - amount of random cells we will leave non empty at the end of the process
 * @param *no_prev - 1 if the undo-redo list has no undo move, else 0
 * @return void
 */
static void generate(Game* game, int x, int y, int* no_prev){
	int *amount_of_attempts = (int*)malloc(sizeof(int));
	*amount_of_attempts = 0;
	if (board_empty_test(game->board) == 1){
		generate_recursive(game, x, y, amount_of_attempts);
		if ((*amount_of_attempts) <= 999){ /* we did not fail in stage one and in ILP*/
			generate_stage_two(game, y); /*let's keep only y cells*/
			make_generate_node(game, no_prev);
			print_board(game);/*finally let's print the board*/
		}
		else{
			clear_board(game->board);
			printf("Error: puzzle generator failed\n");
		}
	}
	else{
		printf("Error: board is not empty\n");
	}
	free(amount_of_attempts);
}

/**
* undo - un does a move
* @param game - a pointer to the game which we wish to undo a move in
* @param *no_prev - 1 if the undo-redo list has no undo move, else 0
* @param reset - 1 if the call for the command came from the reset command, else 0
* @return void 
*/
static void undo(Game* game, int* no_prev, int reset){
	int loc_x,loc_y,new,prev; Node* temp;
	if (game->undo_redo->current==NULL || *no_prev==1){
		if (reset==0)printf("Error: no moves to undo\n");
		return;
	}
	if (game->undo_redo->current->is_autofill==0 && game->undo_redo->current->is_generate==0){
		loc_x=game->undo_redo->current->loc_x  ; loc_y = game->undo_redo->current->loc_y;
		new=game->undo_redo->current->prev_val;  prev=game->board->cells[loc_x-1][loc_y-1]->val;
		if (reset==0){
			mark_unmark_erroneous_vals(game,loc_x,loc_y,prev,new,"others");
			game->board->cells[loc_x-1][loc_y-1]->val=game->undo_redo->current->prev_val;
			print_board(game);
			print_undo_redo(loc_x,loc_y,game->undo_redo->current->new_val,game->undo_redo->current->prev_val,1);
			if (game->undo_redo->current->prev==NULL){
				*no_prev=1;
			}
			else {
				game->undo_redo->current=game->undo_redo->current->prev;
			}
		}
		else {/* command came from reset */
			mark_unmark_erroneous_vals(game,loc_x,loc_y,prev,new,"others");
			game->board->cells[loc_x-1][loc_y-1]->val=game->undo_redo->current->prev_val;
			game->undo_redo->current=game->undo_redo->current->prev;
		}
	}
	else {
		if (game->undo_redo->current==NULL || *no_prev==1){
			if (reset==1)printf("Error: no moves to undo\n");
			return;
		}
		temp=game->undo_redo->current->inner_list->head_of_list;
		while (temp!=NULL){
			mark_unmark_erroneous_vals(game, temp->loc_x, temp->loc_y, temp->new_val, 0,"others");
			game->board->cells[temp->loc_x-1][temp->loc_y-1]->val=0;
			temp=temp->next;
		}
		if (reset==0){
			temp=game->undo_redo->current->inner_list->head_of_list;
			print_board(game); print_autofill_or_generate_changes(temp,1);
		}
		if (game->undo_redo->current->prev==NULL){
			*no_prev=1;
		}
		else {
			game->undo_redo->current=game->undo_redo->current->prev;
		}
	}
}

/**
* redo - redoes a move
* @param game - a pointer to the game which we wish to redo a move in
* @param *no_prev - 1 if the undo-redo list has no undo move, else 0
* @return void 
*/
static void redo(Game* game, int *no_prev){
	int loc_x, loc_y;
	if (game->undo_redo->current==NULL || (*no_prev==0 && game->undo_redo->current->next==NULL)) {
		printf("Error: no moves to redo\n");
		return;
	}
	if (*no_prev==1){
		if (game->undo_redo->current==NULL){
			printf("Error: no moves to redo\n");
			return;
		}
		else if (game->undo_redo->current->is_generate==0 && game->undo_redo->current->is_autofill==0){
			loc_x= game->undo_redo->current->loc_x;loc_y= game->undo_redo->current->loc_y;
			*no_prev=0;
			mark_unmark_erroneous_vals(game, loc_x,loc_y, game->undo_redo->current->prev_val,game->undo_redo->current->new_val,"others");
			game->board->cells[loc_x-1][loc_y-1]->val=game->undo_redo->current->new_val;
			print_board(game);
			print_undo_redo(loc_x,loc_y,game->undo_redo->current->prev_val,game->undo_redo->current->new_val, 0 );
		}
		else{
			Node* temp = game->undo_redo->current->inner_list->head_of_list;
			*no_prev=0;
			apply_inner_nodes(game,temp);
			print_board(game); print_autofill_or_generate_changes(temp,0);
		}
	}
	else {
		if (game->undo_redo->current->next==NULL){
			printf("Error: no moves to redo\n");
			return;
		}
		else if (game->undo_redo->current->next->is_autofill==0 && game->undo_redo->current->next->is_generate==0){
			loc_x= game->undo_redo->current->next->loc_x;loc_y= game->undo_redo->current->next->loc_y;
			mark_unmark_erroneous_vals(game, loc_x,loc_y,game->undo_redo->current->next->prev_val , game->undo_redo->current->next->new_val,"others");
			print_board(game); print_undo_redo(loc_x,loc_y,game->undo_redo->current->next->prev_val, game->undo_redo->current->next->new_val, 0);
			game->undo_redo->current = game->undo_redo->current->next;
		}
		else{
			Node* temp = game->undo_redo->current->next->inner_list->head_of_list;
			apply_inner_nodes(game,temp);
			print_board(game); print_autofill_or_generate_changes(temp,0);
			game->undo_redo->current = game->undo_redo->current->next;
		}
	}
}

/**
* save - saves a game board to a given file
* @param game - a pointer to the game board we wish to save
* @param *file_path - the file path to the file in which we want to save our game board
* @return void
*/
static void save(Game* game, char* file_path){
	FILE* opf; int i,j;
	if (strcmp(game->game_mode,"Edit")==0){
		if (contains_erroneous_val(game)){
			printf("Error: board contains erroneous values\n");
			return;
		}
		else if (solve_for_validate(game)==0){
		 	 printf("Error: board validation failed\n");
		 	 return;
		}
	}
	opf=fopen(file_path,"w");
	if (opf==NULL){
		printf("Error: File cannot be created or modified\n");
		return;
	}
	fprintf(opf,"%d %d\n",game->board->block_x,game->board->block_y);
	if (strcmp(game->game_mode,"Edit")==0){
		for (i=0 ; i < game->board->length_of_board ; i++){
			for (j=0 ; j < game->board->length_of_board ; j++){
				if (game->board->cells[j][i]->val==0){
					fprintf(opf, "%d",game->board->cells[j][i]->val);
				}
				else {
					fprintf(opf, "%d.",game->board->cells[j][i]->val);
				}
				if (j!=(game->board->length_of_board-1)){
					fprintf(opf, " ");
				}
			}
			fprintf(opf,"\n");
		}
	}
	else {
		for (i=0 ; i < game->board->length_of_board ; i++){
			for (j=0 ; j < game->board->length_of_board ; j++){
				if (game->board->cells[j][i]->fixed==1){
					fprintf(opf, "%d.",game->board->cells[j][i]->val);
				}
				else {
					fprintf(opf, "%d",game->board->cells[j][i]->val);
				}
				if (j!=(game->board->length_of_board-1)){
					fprintf(opf, " ");
				}
			}
			fprintf(opf,"\n");
		}
	}
	printf("Saved to: %s\n",file_path);
	fclose(opf);
}

/**
* hint - gives the user a hint in form of a possible legal value to a given cell
* @param game - a pointer to the game board we are working on
* @param x - x-axis location of the cell which we want to get a hint about 
* @param y - y-axis location of the cell which we want to get a hint about
* @return void
*/
static void hint(Game* game, int x, int y){
	if (solve_for_hint(game, x-1, y-1)){ /*prints the hint if successfully finds one*/
	}
	else{ /*we failed in ILP*/
		printf("Error: board is unsolvable\n");
	}
}

/**
* num_solutions - calculates the number of solutions the board has and prints an appropriate message
* @param game - a pointer to the game board who's amount of solutions we wish to find
* @return void
*/
static void num_solutions(Game* game){
	int num_of_solutions;
	Stack* stack;
	if (contains_erroneous_val(game)){ /*need to check if there are erroneous values*/
		printf("Error: board contains erroneous values\n");
	}
	else{ /*we shall execute*/
		if (board_full_test(game->board)){ /*if a board is full and not erroneous - then he is correctly solved*/
			printf("Number of solutions: 1\n");
			printf("This is a good board!\n");
		}
		else{
			copyBoard(game->board); /*we will work on a copy of our board*/
			stack = createStack();
			num_of_solutions = find_all_solutions(game->board->solution_of_board, stack);
			printf("Number of solutions: %d\n", num_of_solutions);
			if(num_of_solutions == 1){
				printf("This is a good board!\n");
			}
			else if(num_of_solutions > 1){
				printf("The puzzle has more than 1 solution, try to edit it further\n");
			}
			free_board(game->board->solution_of_board); /*in any case we will free the memory we used*/
			free(stack);
		}
	}
}


/**
 * autofill - fills up the board cells which only have one option to be filled up by
 * @param game - a pointer the game board we wish to autofill
 * @param *no_prev - 1 if the undo-redo list has no undo move, else 0
 * @return void
 */
static void autofill(Game* game, int *no_prev){
	int new_node=1;
	int i, j, is_single = 0;
	if (contains_erroneous_val(game)){ /*we check if there are erroneous values*/
		printf("Error: board contains erroneous values\n");
	}
	else{ /*we shall execute*/
		for (i=0; i < game->board->length_of_board; i++){
			for (j=0; j < game->board->length_of_board; j++){
				if (game->board->cells[j][i]->val == 0){
					/*if we find an empty cell that has a single possible value - we would like to know about it in order to not iterate in vein*/
					is_single += check_single_fit(game->board, game->board->cells[j][i]);
				}
			}
		}
		if (is_single > 0){
			for (i=0; i < game->board->length_of_board; i++){
				for (j=0; j < game->board->length_of_board; j++){
					if (game->board->cells[j][i]->single_fit > 0){ /*meaning we found a single fitting value*/
						game->board->cells[j][i]->val = game->board->cells[j][i]->single_fit; /*we first set the new value*/
						game->board->cells[j][i]->single_fit = 0; /*we then return the value to 0, in case the function will be called again*/
						game->board->cells[j][i]->was_set = 1;
						add_to_undo_redo_list_for_autofill_generate(game,j+1, i+1,game->board->cells[j][i]->val , no_prev,new_node,1);
						new_node=0;
						printf("Cell <%d,%d> set to %d\n",j+1,i+1,game->board->cells[j][i]->val);
					}
				}
			}
		}
		mark_erroneous_after_load(game); /*after we are done there may be new erroneous vals*/
		print_board(game); /*finally lets print as requested*/
		check_game_ended(game);
	}
}

/**
 * reset - resets the game board back to its original state
 * @param game - a pointer the game board we wish to reset
 * @param *no_prev - 1 if the undo-redo list has no undo move, else 0
 * @return void
 */
static void reset(Game* game, int* no_prev){
	while (game->undo_redo->current!=NULL ){
		if (*no_prev==1){
			break;
		}
		undo(game,no_prev,1);
	}
	free_list(game->undo_redo);
	game->undo_redo=createList();
	printf("Board reset\n");
}

/**
 * exit_now - exit's the game and frees all memmory
 * @param game - a pointer the game we wish to exit
 * @param *exit_prog - 1 if the exit_now command was called, else 0
 * @return void
 */
static void exit_now(Game* game, int *exit_prog){
	(void)game;
	*exit_prog=1;
}
