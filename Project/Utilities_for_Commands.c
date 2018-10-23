/*
 * Utilities_for_Commands.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "The_Board.h"
#include "Utilities_for_Commands.h"

static int get_dimension(FILE* ipf);
static int get_cell_value(FILE* ipf);
static int remove_all_after_current(List* list);
static int mark_erroneous_vals(Game* game, int loc_x, int loc_y, int z, const char* func_name);
static void deletion_of_value(Game* game,int loc_x,int loc_y);
/**
 * print_row - prints a row
 * @param game - a pointer to our played game
 * return void
 */
void print_row(Game* game){ /* 4N+m+1 dashes */
	int i;
	for (i=0 ; i < game->board->length_of_board ; i++) printf("----");
	for (i=0 ; i < game->board->block_x ; i++) printf("-");
	printf("-\n");
}

/**
* check_game_ended - checks if the game has ended (the last cell was correctly filled and th egame board has no mistakes) 
* @param game - a pointer to our played game
* @return void 
*/
void check_game_ended(Game *game){
	if(board_full_test(game->board)){ /*in autofill filled up our board, lets check if we are done*/
		if (contains_erroneous_val(game)){ /*if the board is full, but not correctly solved*/
			printf("Puzzle solution erroneous\n");
		}
		else{ /*if we get here the board is correctly solved!*/
			printf("Puzzle solved successfully\n");
			free_board(game->board);
			game->game_mode = "Init";
		}
	}
}

/**
* get_dimension
* @param ipf - a pointer to the file we are reading from
* @return one of the boards dimension
*/
static int get_dimension(FILE* ipf){
	int i=0; char c; char *temp, *p; int num;
	c=getc(ipf);
	while (c==' ' || c=='\t' || c=='\n' || c=='\r') c=getc(ipf);
	temp=(char*)malloc(sizeof(char));
	check_for_allocation_failure(temp);
	while (c!=' ' && c!='\t' && c!='\n' && c!='\r'){
		temp[i]=c;
		i++;
		p=realloc(temp,(i+1)*sizeof(*p));
		check_for_allocation_failure(p);
		temp=p;
		c=getc(ipf);
	}
	if (c=='\r') c=getc(ipf);
	temp[i]='\0';
	num=atoi(temp);
	free(temp);
	return num;
}

/**
* get_cell_value - gets the value of a cell from a given file
* @param ipf - a pointer to the file we are reading from
* @return the value of the cell, or if the cell is fixed it returns the value as a negative one (and later is switched to positive)
*/
static int get_cell_value(FILE* ipf){
	int i=0; char c; char *temp, *p; int num, indicator=0;
	c=getc(ipf);
	while (c==' ' || c=='\t' || c=='\n' || c=='\r') c=getc(ipf);
	temp=(char*)malloc(sizeof(char));
	check_for_allocation_failure(temp);
	while (c!=' ' && c!='\t' && c!='\n' && c!='\r' && c!='.' && c!=EOF){
		temp[i]=c;
		p=(char*)realloc(temp, (i+2)*sizeof(*p));
		check_for_allocation_failure(p);
		temp=p;
		i++;
		c=getc(ipf);
		if (c=='.') indicator=1;
	}
	if (c=='\r') c=getc(ipf);
	temp[i]='\0';
	num=atoi(temp);
	if (indicator) num*= -1;
	free(temp);
	return num;
}

/**
* load - load's a game board from a file in the given file path
* @param game - a pointer to the game to which we will load the new board
* @param path - a string representing the file path to the file from which we will load our board
* @return 1 in case we successfully loaded the board, else 0 
*/
int load(Game* game,char* path){
	FILE *ipf; int block_x,block_y, i=0,j=0; int value;
	ipf=fopen(path, "r");
	if (ipf==NULL) return 0;
	if (strcmp(game->game_mode,"Init")!=0) free_board(game->board);/*  free previous board if there exists one */
	block_x=get_dimension(ipf); block_y=get_dimension(ipf);
	game->board=createBoard(block_x,block_y);
	while (i< game->board->length_of_board){
		while (j< game->board->length_of_board){
			value=get_cell_value(ipf);
			if (value<0){
				game->board->cells[j][i]->fixed = 1;
				game->board->cells[j][i]->val=value*(-1);
			}
			else {
				game->board->cells[j][i]->val=value;
			}
			j++;
		}
		j=0;
		i++;
	}
	fclose(ipf);
	return 1;
}

/**
 * mark_loaded_cells - marks the non empty cells of the board that were loaded from a file
 * @param board - a pointer to the board that was loaded
 * return void
 */
void mark_loaded_cells(Board* board){
	int i,j;
	for(i=0; i<board->length_of_board; i++){
		for(j=0; j<board->length_of_board; j++){
			if(board->cells[j][i]->val != 0){
				board->cells[j][i]->was_loaded = 1;
			}
		}
	}
}

/**
* contains_erroneous_val - check's if a given board contains erroneous values
* @param game - a pointer to the played game
* @return 1 if the board contains erroneous values, else 0 
*/
int contains_erroneous_val(Game* game){
	int i,j;
	for (i=0 ; i < game->board->length_of_board ; i++){
		for (j=0 ; j < game->board->length_of_board ; j++){
			if (game->board->cells[i][j]->erroneous==1) return 1;
		}
	}
	return 0;
}

/**
* remove_all_after_current - removes all nodes that are located after the current node in a given list
* @param list - a pointer to the list from which we are removing
* @return 0 if the current node is NULL, else 1
*/
static int remove_all_after_current(List* list){
	if (list->current==NULL) return 0;
	else {
		if (list->current->next==NULL) return 1;
		else {
			Node* temp;
			temp=list->current->next->next;
			while (temp!=NULL){
				if (temp->inner_list!=NULL){
					free_list(temp->inner_list);
				}
				free(list->current->next);
				list->current->next=temp;
				temp=temp->next;
			}
			free(list->current->next);
			list->current->next=NULL;
			return 1;
		}
	}
}

 /**
 * add_to_undo_redo_list_for_set - add's a set operation to the undo_redo connected list
 * @param game - a pointer to the game we are playing
 * @param loc_x - the x-axis location of the cell to which a value was set
 * @param loc_y - the y-axis location of the cell to which a value was set
 * @param new_val - the value that was set to the cell
 * @param *no_prev - 1 if the undo-redo list has no undo move (is 1 right after an undo call on the left-most node, else 0)
 * @return void
 */
void add_to_undo_redo_list_for_set(Game* game, int loc_x, int loc_y, int new_val, int *no_prev){
	if (*no_prev==1){
		remove_all_after_current(game->undo_redo);
		free(game->undo_redo->current);
		game->undo_redo->current=createNode(-1,NULL,NULL,1,0,0,NULL,loc_x,loc_y,game->board->cells[loc_x-1][loc_y-1]->val,new_val);
		game->undo_redo->head_of_list = game->undo_redo->current;
	}
	else{
		if (remove_all_after_current(game->undo_redo)!=0){
			game->undo_redo->current->next=createNode(-1,NULL,game->undo_redo->current,0,0,0,NULL,loc_x,loc_y,game->board->cells[loc_x-1][loc_y-1]->val,new_val);
			game->undo_redo->current=game->undo_redo->current->next;
		}
		else {
			game->undo_redo->current=createNode(-1,NULL,NULL,1,0,0,NULL,loc_x,loc_y, game->board->cells[loc_x-1][loc_y-1]->val, new_val);
			game->undo_redo->head_of_list = game->undo_redo->current;
		}
	}
	*no_prev=0;
}

 /**
 * add_to_undo_redo_list_for_autofill_generate - add's an autofill or a generate node to the undo_redo list
 * @param game - a pointer to the game we are playing
 * @param loc_x - the x-axis location of the cell to which a value was set
 * @param loc_y - the y-axis location of the cell to which a value was set
 * @param new_val - the value that was set to the cell
 * @param *no_prev - 1 if the undo-redo list has no undo move, else 0
 * @param new_node - 1 for the creation of the first sub-node only, else 0
 * @param is_autofill - 1 if we are adding an autofill, 0 a generate node
 * @return void
 */
void add_to_undo_redo_list_for_autofill_generate(Game* game, int loc_x, int loc_y, int new_val, int *no_prev, int new_node, int is_autofill){
	if (new_node){ /* first iteration, creation of the sub list */
		if (*no_prev==1){
			remove_all_after_current(game->undo_redo);
			free(game->undo_redo->current);
			game->undo_redo->current=createNode(-1,NULL,NULL,1,0,0,NULL,-1,-1,-1,-1);
			game->undo_redo->head_of_list = game->undo_redo->current;
			if (is_autofill)game->undo_redo->head_of_list->is_autofill=1;
			else game->undo_redo->head_of_list->is_generate=1;
			game->undo_redo->current->inner_list=createList(); /* creation of the first insert inside autofill */
			game->undo_redo->current->inner_list->current=createNode(-1, NULL,NULL,1,0,0,NULL,loc_x,loc_y,0,new_val);
			game->undo_redo->current->inner_list->head_of_list=game->undo_redo->current->inner_list->current;
		}
		else {
			if (remove_all_after_current(game->undo_redo)!=0){
				game->undo_redo->current->next=createNode(-1,NULL,game->undo_redo->current,0,0,0,NULL,-1,-1,-1,-1);
				if (is_autofill)game->undo_redo->current->next->is_autofill=1;
				else game->undo_redo->current->next->is_generate=1;
				game->undo_redo->current->next->inner_list=createList();
				game->undo_redo->current->next->inner_list->current=createNode(-1,NULL,NULL,1,0,0,NULL,loc_x,loc_y,0,new_val);
				game->undo_redo->current->next->inner_list->head_of_list=game->undo_redo->current->next->inner_list->current;
				game->undo_redo->current = game->undo_redo->current->next;

			}
			else {
				game->undo_redo->current=createNode(-1,NULL,NULL,1,0,0,NULL,-1,-1,-1,-1);
				game->undo_redo->current->next = game->undo_redo->current->prev = NULL;
				game->undo_redo->head_of_list = game->undo_redo->current;
				if (is_autofill==1)game->undo_redo->current->is_autofill=1;
				else game->undo_redo->current->is_generate=1;
				game->undo_redo->current->inner_list=createList();
				game->undo_redo->current->inner_list->current=createNode(-1,NULL,NULL,1,0,0,NULL,loc_x,loc_y,0,new_val);
				game->undo_redo->current->inner_list->head_of_list = game->undo_redo->current->inner_list->current;
			}
		}
		*no_prev=0;
	}
	else{ /* only adding the changes to inner_list now */
		game->undo_redo->current->inner_list->current->next=createNode(-1, NULL,game->undo_redo->current->inner_list->current,0,0,0,NULL,loc_x,loc_y,0,new_val);
		game->undo_redo->current->inner_list->current = game->undo_redo->current->inner_list->current->next;
	}
}

/**
* apply_inner_nodes - going over the sub-nodes of a generate or autofill node and applying the changes
* @param game - a pointer to the played game 
* @param node - the current sub-node that is being applied from the changes
* @return void
*/
void apply_inner_nodes(Game *game,Node* node){
	while (node!=NULL){
		game->board->cells[node->loc_x-1][node->loc_y-1]->val= node->new_val;
		mark_unmark_erroneous_vals(game,node->loc_x, node->loc_y , 0 , node->new_val, "others");
		node=node->next;
	}
}
/**
 * print_autofill_or_generate_changes - prints the changes per sub-node
 * @param node - current node that is being printed
 * @param undo - 1 if the function has been called from undo, 0 if from redo
 */
void print_autofill_or_generate_changes(Node* node, int undo){
	while (node!=NULL){
		if (undo==1){
			print_undo_redo(node->loc_x,node->loc_y, 0, node->new_val, 1);
		}
		else {
			print_undo_redo(node->loc_x,node->loc_y, 0, node->new_val, 0);
		}
		node=node->next;
	}
}

/**
* mark_erroneous_vals - marks the erroneous sign of cells that are dependent in the given cell that was set
* @param game - a pointer to the played game
* @param loc_x - the x-axis location of the cell we are testing
* @param loc_y - the y-axis location of the cell we are testing
* @param z - the value of the cell we are testing
* @param func_name - determines from where we're calling mark_erroneous_vals function
* @return 1 if erroneous values were marked, else 0
*/
static int mark_erroneous_vals(Game* game, int loc_x, int loc_y, int z, const char* func_name){
	int i,j, block_x,block_y,found_in_column=0,found_in_row=0, found_in_block=0;
	for (i=0 ; i<game->board->length_of_board ; i++){
		if (game->board->cells[loc_x-1][i]->val==z && i!=(loc_y-1)){
			if (game->board->cells[loc_x-1][i]->fixed==0) { /*fixed cells aren't marked as erroneous */
				game->board->cells[loc_x-1][i]->erroneous=1;
			}
			found_in_column=1;
		}
		if (game->board->cells[i][loc_y-1]->val==z && i!=(loc_x-1)){
			if (game->board->cells[i][loc_y-1]->fixed==0) {
				game->board->cells[i][loc_y-1]->erroneous=1;
			}
			found_in_row=1;
		}
	}
	block_x=game->board->block_x; block_y=game->board->block_y;
	for (i= block_x * ((loc_y-1)/block_x) ; i < block_x * ((loc_y-1)/block_x) + block_x; i++){
		for (j= block_y * ((loc_x-1)/block_y) ; j < block_y * ((loc_x-1)/block_y) + block_y ; j++){
			if (game->board->cells[j][i]->val == z && i!=(loc_y-1) && j!=(loc_x-1) ){
				if (game->board->cells[j][i]->fixed==0){
					game->board->cells[j][i]->erroneous=1;
				}
				found_in_block=1;
			}
		}
	}
	if (found_in_row || found_in_column || found_in_block){
		game->board->cells[loc_x-1][loc_y-1]->erroneous=1;
		if ((strcmp(func_name,"set")==0 && strcmp(game->game_mode,"Edit")==0 )
			|| (strcmp(func_name,"set")==0 &&  strcmp(game->game_mode,"Solve")==0 && game->mark_errors==1))/* prints that we just entered erroneous value*/
			printf("Entered erroneous value!\n");
		return 1;
	}
	else {
		game->board->cells[loc_x-1][loc_y-1]->erroneous=0;
		return 0;
	}
}

/**
* deletion_of_value - sets a cell to 0, and removes the erroneous mark from the ones that aren't erroneous now
* @param game - a pointer to the played game
* @param loc_x - the x-axis location of the cell we set to zero
* @param loc_y - the y-axis location of the cell we set to zero
* @return void
*/
static void deletion_of_value(Game* game,int loc_x,int loc_y){ /* changing non-zero value to zero */
	int i,j,block_x,block_y, z=game->board->cells[loc_x-1][loc_y-1]->val;
	game->board->cells[loc_x-1][loc_y-1]->val=0;
	game->board->cells[loc_x-1][loc_y-1]->erroneous=0;
	for (i=0 ; i<game->board->length_of_board ; i++){
		if (game->board->cells[loc_x-1][i]->val==z && i!=(loc_y-1)){
			mark_erroneous_vals(game,loc_x,i+1,z,"others");
		}
		if (game->board->cells[i][loc_y-1]->val==z && i!=(loc_x-1)){
			mark_erroneous_vals(game,i+1,loc_y,z,"others");
		}
	}
	block_x=game->board->block_x; block_y=game->board->block_y;
	for (i= block_x * ((loc_y-1)/block_x) ; i < block_x * ((loc_y-1)/block_x) + block_x; i++){
		for (j= block_y * ((loc_x-1)/block_y) ; j < block_y * ((loc_x-1)/block_y) + block_y ; j++){
			if (game->board->cells[j][i]->val == z && i!=(loc_y-1) && j!=(loc_x-1) ){
				mark_erroneous_vals(game,j+1,i+1,z,"others");
			}
		}
	}
}

/**
* mark_unmark_erroneous_vals - updates erroneous values according to the specific change in the board
* @param game - a pointer to the played game
* @param loc_x - the x-axis location of the cell that was set
* @param loc_y - the y-axis location of the cell that was set
* @param prev - the old value of the cell that was set
* @param new - the new value of the cell that was set
* @return void
*/
void mark_unmark_erroneous_vals(Game* game, int loc_x, int loc_y, int prev, int new, const char* func_name){
	if (prev==0){
		if (new==0){}
		else{
			game->board->cells[loc_x-1][loc_y-1]->val=new;
			mark_erroneous_vals(game, loc_x,loc_y,new, func_name);
		}
	}
	else {
		if (new==0){ /* deletion */
			deletion_of_value(game,loc_x,loc_y);
		}
		else{ /* new!= 0  */
			deletion_of_value(game,loc_x,loc_y);
			game->board->cells[loc_x-1][loc_y-1]->val=new;
			mark_erroneous_vals(game, loc_x,loc_y,new, func_name);
		}
	}
}

/**
* mark_erroneous_after_load - marks all erroneous values in the board after a load 
* @param game - a pointer to the played game
* @return void
*/
void mark_erroneous_after_load(Game* game){
	int i,j,value;
	for (i=0 ; i < game->board->length_of_board; i ++){
		for (j=0 ; j < game->board->length_of_board; j ++){
			value=game->board->cells[j][i]->val;
			if (game->board->cells[j][i]->erroneous==1
					|| value==0){
				continue;
			}
			mark_erroneous_vals(game,j+1,i+1,value,"others");
		}
	}
}

/**
* print_undo_redo - prints the change (undo or redo)
* @param x - the x-axis location of the cell which value was changed
* @param y - the y-axis location of the cell which value was changed
* @param z1 - the value before the undo or redo
* @param z2 - the value after the undo or redo
* @param undo - if we are handling an undo operation, else 0
* @return void
*/
void print_undo_redo(int x, int y,int z1, int z2, int undo){
	if (undo==1){
		if (z1==0 && z2==0){
			printf("Undo %d,%d: from _ to _\n", x,y);
		}
		else if (z1==0){
			printf("Undo %d,%d: from _ to %d\n", x,y, z2);
		}
		else if (z2==0){
			printf("Undo %d,%d: from %d to _\n", x,y,z1);
		}
		else {
			printf("Undo %d,%d: from %d to %d\n", x,y, z1, z2);
		}
	}
	else {
		if (z1==0 && z2==0){
			printf("Redo %d,%d: from _ to _\n", x,y);
		}
		else if (z1==0){
			printf("Redo %d,%d: from _ to %d\n", x,y, z2);
		}
		else if (z2==0){
			printf("Redo %d,%d: from %d to _\n", x,y,z1);
		}
		else {
			printf("Redo %d,%d: from %d to %d\n", x,y, z1, z2);
		}
	}
}

/**
* remove_fixed_from_cells - sets all cells of a given board to be not fixed
* @param game - a pointer to the played game
* @return void
*/
void remove_fixed_from_cells(Game* game){
	int i,j;
	for (i=0 ; i < game->board->length_of_board ; i ++){
		for (j=0 ; j < game->board->length_of_board ; j++){
			game->board->cells[i][j]->fixed=0;
		}
	}
}

/**
 * board_full_test - checks if board is full
 * @param board - a pointer to the board we are checking
 * @return 1 if board is full, 0 if not
 */
int board_full_test(Board* board){
	int i,j;
	for(i=0; i<board->length_of_board; i++){
		for(j=0; j<board->length_of_board; j++){
			if(board->cells[j][i]->val == 0){
				return 0;
			}
		}
	}
	return 1;
}

/**
 * board_empty_test - checks if board is empty
 * @param board - the board we are checking
 * @return 1 if board is empty, 0 if not
 */
int board_empty_test(Board* board){
	int i,j;
	for(i=0; i<board->length_of_board; i++){
		for(j=0; j<board->length_of_board; j++){
			if(board->cells[j][i]->val != 0){
				return 0;
			}
		}
	}
	return 1;
}

/**
 * clear_board - sets the value of all the board's cell's to 0
 * @param board - the board we are clearing
 * @return void
 */
void clear_board(Board* board){
	int i,j;
	for(i=0; i<board->length_of_board; i++){
		for(j=0; j<board->length_of_board; j++){
			board->cells[j][i]->val = 0;
			board->cells[j][i]->was_loaded = 0;
		}
	}
}
