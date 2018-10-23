/*
 * The_Board.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "The_Board.h"

/**
* check_for_allocation_failure - checks for an allocation failure
* @param ptr - a pointer to the object we wish to check was allocated well
* @return void
*/
void check_for_allocation_failure(void* ptr){
	if (ptr==NULL){ /* allocation failed */
		printf("Error: malloc has failed\n");
		exit(0);
	}
}

/**
 * createNode:
 * @param val - Creates a node with the value val
 * @return A pointer to the structed node
 */
struct Node* createNode(int val, Node* next, Node* prev, int is_head, int is_autofill, int is_generate, List *inner_list, int loc_x, int loc_y, int prev_val, int new_val){
	struct Node* node = (struct Node*) malloc(sizeof(struct Node));
	check_for_allocation_failure(node);
	node->val = val; node->next = next;
	node->prev = prev; node->is_head = is_head;
	node->is_autofill=is_autofill; node->is_generate=is_generate;
	node->inner_list=inner_list;
	node->loc_x=loc_x; node->loc_y=loc_y;
	node->new_val=new_val; node->prev_val=prev_val;
	return node;
}

/**
 * insert_node:
 * We receive a struct Node which is the head of our list, and the "father" list (thats the way our implementation is built)
 * We then find the location of insertion - keeping the list sorted at all times
 * @param list - The list which the node is inserted to
 * @param head - The head of the list
 * @param val_of_node - The value of the node that is being inserted
 * @return void
 */
void insert_node(List* list, struct Node* head, int val_of_node){
	struct Node* node;
	node = head;
	if (head == NULL){/*meaning list is empty*/
		list->head_of_list = createNode(val_of_node,NULL,NULL,1,0,0,NULL,-1,-1,-1,-1);
	}
	else{
		while ((node->val < val_of_node)&&(node->next != NULL)){
			node = node->next;
		}
		if (node->val > val_of_node){/*meaning our place for insertion is one node back*/
			if (node->prev == NULL){ /*meaning we are inserting at the beginning*/
				list->head_of_list = createNode(val_of_node,head,NULL,1,0,0,NULL,-1,-1,-1,-1);
				head->prev = list->head_of_list;
				head->is_head = 0;
			}
			else{/*inserting in the middle of the list*/
				(node->prev)->next = createNode(val_of_node,node,node->prev,0,0,0,NULL,-1,-1,-1,-1);
				node->prev = (node->prev)->next;
			}
		}
		else{ /*we need to insert at the end of the list*/
			node->next = createNode(val_of_node,NULL,node,0,0,0,NULL,-1,-1,-1,-1);
			(node->next)->prev = node;
		}
	}
	list->size++;
}

/**
 * rebuild_full_list:
 * When a cell is optionally opened to new options (while randomly filling up the board),
 * we will find them, and rebuild our list of "possible_vals"
 * @param board - A pointer to the game board
 * @param cell - A pointer to the specific cell that its list is rebuilt
 * @return void
 */
void rebuild_full_list(Board* board, Cell* cell){
	int i=1; int is_present, is_valid, is_forbiden;
 	for(;i<(cell->length_of_mother_board+1);i++){
 		is_present = is_node_present(cell->possible_vals,i);
 		is_forbiden = is_node_present(cell->forbiden_vals,i);
 		is_valid = is_valid_value_for_cell(board, cell, i);
		if((is_present==0)&&(is_valid ==1)&&(is_forbiden==0)&&(i != cell->val)){
			insert_node(cell->possible_vals, cell->possible_vals->head_of_list, i);
		}
		else if(((is_present==1)&&(is_valid==0)) || ((is_present==1)&&(is_forbiden==1))){
			sub_from_List(cell->possible_vals, i);
		}
	}
}

/**
 * find_node_in_loc:
 * @param node - the head of the list in which the function performs its search
 * @param loc - the location in the list to which the function will go
 * @return  The value of the node in position loc in the linked list that starts with node
 */
int find_node_in_loc(struct Node* node, int loc){
	struct Node* temp; int i=0;
	temp = node;
	for(;i<loc;i++){
		temp = temp->next;
	}
	return temp->val;
}

/**
 * sub_from_List:
 * While backtracking, if we cross a certain value, we need to make sure we wont choose it again
 * so we therefore - erase it from the cell's list - using only the pointers.
 * @param list - The list in which we want to erase a certain node
 * @param value_to_delete - The value of the node we wish to delete from the list
 * @return void
 */
void sub_from_List(List* list, int value_to_delete){
	struct Node* temp; struct Node* to_free;
	if(list->size == 1){
		free_node(list->head_of_list);
		list->head_of_list = NULL;
	}
	else{
		temp = list->head_of_list;
		if (temp->val == value_to_delete){ /*the head is he one to be deleted*/
			temp = temp->next;
			to_free = temp->prev;
			temp->prev = NULL;
			temp->is_head = 1;
			list->head_of_list = temp;
			free_node(to_free);
		}
		else{
			while (temp->val != value_to_delete){ /*this one loop is enough, because we will only call the function, if value_to_delete is 100% in the list */
				temp = temp->next;
			}
			if (temp->next == NULL){ /*we are at the end of the list*/
				to_free = temp;
				(temp->prev)->next = NULL;
				free_node(to_free);
			}
			else{
				to_free = temp;
				(temp->next)->prev = temp->prev;
				(temp->prev)->next = temp->next;
				free_node(to_free);
			}
		}
	}
	list->size --; /*the lists size got smaller by 1*/
}

/**
 * is_node_present:
 * Iterates over the given list until he finds a node with the given value,
 * or until reaching the end of the list
 * @param list - The list on which we wish to check if a node is present
 * @param val_of_node - The value of the node that we want to check if present
 * @return 1 if the list contains a node in which its value==val_of_node, else 0
 */
int is_node_present(List* list, int val_of_node){
	struct Node* temp;
	if(list->head_of_list == NULL){
		return 0;
	}
	else{
		temp = list->head_of_list;
		while (temp->next != NULL){
			if (temp->val == val_of_node){
				return 1;
			}
			else{
				temp = temp->next;
			}
		}
		if (temp->val == val_of_node){
			return 1;
		}
	}
	return 0;
}

/**
 * createCell:
 * Creating a new cell
 * @param loc_x - Location of the cell on the x axis (columns)
 * @param loc_y - Location of the cell on the y axis (cells)
 * @param val - The value that the cell will have
 * @param length_of_mother_board - The length of the board inside of which the cell is created
 * @return A pointer to the cell created
 */
Cell* createCell(int loc_x, int loc_y, int val, int length_of_mother_board){
	Cell* cell = (Cell*) malloc(sizeof(Cell));
	check_for_allocation_failure(cell);
	cell -> loc_x = loc_x; cell -> loc_y = loc_y; cell -> val = val; cell -> fixed = 0; cell -> was_set = 0;
	cell -> fixed = 0; cell -> backtracked_to = 0; cell -> length_of_mother_board = length_of_mother_board;
	cell -> possible_vals = createList();
	cell -> forbiden_vals =  createList();
	cell-> erroneous = 0;
	cell-> single_fit = 0;
	cell-> was_loaded = 0;
	return cell;
}

/**
 * createList:
 * @return Pointer to the created list
 */
List* createList(){
	List* list = (List*) malloc(sizeof(List));
	check_for_allocation_failure(list);
	list->current=NULL;
	list->size = 0;
	list->head_of_list = NULL;
	return list;
}

/**
 * createBoard:
 * @param block_x, block_y - The number of columns and rows in each block, respectively
 * @return Pointer to the created board
 */
Board* createBoard(int block_x, int block_y){
	int total_size_of_block = block_x*block_y; int i=0; int j=0;
	Board* our_board = (Board*) malloc(sizeof(Board));
	check_for_allocation_failure(our_board);
	our_board->cells = (Cell***) malloc(total_size_of_block*sizeof(Cell));
	if (!our_board->cells){ /* allocation failed */
		printf("Error: malloc has failed\n");
		exit(0);
	}
	our_board->length_of_board = total_size_of_block;
	our_board->size_of_board=total_size_of_block*total_size_of_block;
	our_board->block_x = block_x; our_board->block_y = block_y;
	our_board->no_solution=0;
	for (;i<total_size_of_block; i++){
		our_board->cells[i] = (Cell**) malloc(total_size_of_block*sizeof(Cell)); /* its pointers, so cells+i means the place cells points to +i */
		if (!our_board->cells[i]){ /* allocation failed */
			printf("Error: malloc has failed\n");
			exit(0);
		}
	}
	i=0;
	for (;i<total_size_of_block; i++){
		for (;j<total_size_of_block; j++){
			our_board->cells[i][j] = createCell(i,j,0,total_size_of_block); /*we initialize all cells of the board */
		}
		j=0;
	}
	return our_board;
}

/**
 * createGame:
 * @return Pointer to the created Game
 */
Game* createGame(){
	Game *game = (Game*)malloc(sizeof(Game));
	check_for_allocation_failure(game);
	game->game_mode = "Init";
	game->mark_errors=0;
	game->undo_redo=createList();
	game->undo_redo->current=NULL;
	return game;
}

/**
 * copyBoard:
 * @param board - The board that should be copied (saving the copy of the board in his field solution_of_board)
 * @return void
 */
void copyBoard(Board* board){
	int i,j;
	board->solution_of_board  = (Board*) malloc(sizeof(struct Board));
	check_for_allocation_failure(board->solution_of_board);
	board->solution_of_board ->cells = (Cell***) malloc(board->length_of_board*sizeof(Cell));
	if (!board->solution_of_board ->cells){ /* allocation failed */
		printf("Error: malloc has failed\n");
		exit(0);
	}
	for (i=0;i<board->length_of_board; i++){
		board->solution_of_board ->cells[i] = (Cell**) malloc(board->length_of_board*sizeof(Cell));
		if (!board->solution_of_board ->cells[i]){ /* allocation failed */
			printf("Error: malloc has failed\n");
			exit(0);
		}
	}
	for (i=0;i<board->length_of_board; i++){
		for (j=0;j<board->length_of_board; j++){
			board->solution_of_board ->cells[i][j] = createCell(i,j,board->cells[i][j]->val,board->length_of_board);
			board->solution_of_board ->cells[i][j]->fixed = board->cells[i][j]->fixed;
			board->solution_of_board ->cells[i][j]->was_set = board->cells[i][j]->was_set;
			board->solution_of_board ->cells[i][j]->was_loaded = board->cells[i][j]->was_loaded;
			board->solution_of_board ->cells[i][j]->erroneous = board->cells[i][j]->erroneous;
		}
	}
	board->solution_of_board ->length_of_board = board->length_of_board; board->solution_of_board ->size_of_board = board->size_of_board;
	board->solution_of_board ->no_solution = board->no_solution;
	board->solution_of_board ->block_x = board->block_x; board->solution_of_board ->block_y = board->block_y;
	board->solution_of_board ->solution_of_board=NULL; /*we only keep one copy to a solution*/
}

/**
 * is_valid_value_for_cell_row:
 * @param board - The game's board
 * @param cell - A pointer to the current cell
 * @param value_for_cell - value to be put in the cell if valid
 * @return 0 if invalid value, else 1
 */
int is_valid_value_for_cell_row(Board* board, Cell* cell, int value_for_cell){
	int j;
	for (j=0;j<board->length_of_board;j++ ){
		if ((board->cells[j][cell->loc_y]->val == value_for_cell) && (cell->loc_x != j) ){
			return 0;
		}
	}
	return 1;
}

/**
 * is_valid_value_for_cell_column:
 * @param board - The game's board
 * @param cell - A pointer to the current cell
 * @param value_for_cell - value to be put in the cell if valid
 * @return 0 if invalid value, else 1
 */
int is_valid_value_for_cell_column(Board* board, Cell* cell, int value_for_cell){
	int i;
	for (i=0 ; i<board->length_of_board ; i++ ){ /*first go over the column*/
		if ((board->cells[cell->loc_x][i]->val == value_for_cell)  && (cell->loc_y != i) ){
			return 0;
		}
	}
	return 1;
}

/**
 * is_valid_value_for_cell_block:
 * @param board - The game's board
 * @param cell - A pointer to the current cell
 * @param value_for_cell - value to be put in the cell if valid
 * @return 0 if invalid value, else 1
 */
int is_valid_value_for_cell_block(Board* board, Cell* cell, int value_for_cell){
	int i,j;
	int x_relative, y_relative, x_start, y_start;
	x_relative = cell->loc_x / board->block_y; /*tells us in which block we are on the x-axis*/
	y_relative = cell->loc_y / board->block_x; /*tells us in which block we are on the y-axis*/
	x_start = board->block_y*x_relative; /*beginning of our block on x*/
	y_start = board->block_x*y_relative; /*beginning of our block on y*/
	for(i=x_start; i<(x_start + board->block_y); i++){
		for(j=y_start; j<(board->block_x + y_start); j++){
			if ((board->cells[i][j]->val == value_for_cell) && (i!=(cell->loc_x)) && (j!=(cell->loc_y))){
				return 0;
			}
		}
	}
	return 1;
}

/**
 * is_valid_value_for_cell:
 * @param board - The game's board
 * @param cell - A pointer to the current cell
 * @param value_for_cell - value to be put in the cell if valid
 * @return 0 if invalid value, else 1
 */
int is_valid_value_for_cell(Board* board, Cell* cell, int value_for_cell){
	if(value_for_cell==0){ /*we can always empty a non fixed cell*/
		return 1;
	}
	else{
		if (is_valid_value_for_cell_column(board, cell, value_for_cell) == 0){ /*first go over the column*/
			return 0;
		}
		else if(is_valid_value_for_cell_row(board, cell, value_for_cell) == 0){ /*secondly go over the row*/
			return 0;
		}
		else if(is_valid_value_for_cell_block(board, cell, value_for_cell) == 0){ /*thirdly go over the block*/
			return 0;
		}
	}
	return 1;
}

/**
 * free_node:
 * @param node - A pointer to the node that is going to be freed
 * @return void
 */
void free_node(struct Node* node){
	free(node);
	node = NULL;
}

/**
 * free_content_of_list:
 * @param list - A pointer to the list that its nodes are going to be freed
 * @return void
 */
void free_content_of_list(List* list){
	struct Node* temp; struct Node* to_free;
	temp = list->head_of_list;
	list->head_of_list = NULL;
	list->size = 0;
	while(temp != NULL){
		to_free = temp;
		temp = temp->next;
		free_node(to_free);
	}
}

/**
 * free_list:
 * @param list - A pointer to the list that is going to be freed, including its content
 * @return void
 */
void free_list(List* list){
	struct Node* temp; struct Node* to_free;
	if (list==NULL) return;
	else if(list->head_of_list == NULL){
		/*nothing to free*/
	}
	else{
		temp = list->head_of_list;
		list->head_of_list = NULL;
		list->size = 0;
		while(temp != NULL){
			if (temp->is_autofill==1 || temp->is_generate==1){
				free_list(temp->inner_list);
			}
			to_free = temp;
			temp = temp->next;
			free_node(to_free);
		}
	}
	free(list);
	list = NULL;
}

/**
 * free_cell:
 * @param cell - The cell that is going to be freed
 * @return void
 */
void free_cell(Cell* cell){
	free_list(cell -> possible_vals);
	free_list(cell -> forbiden_vals);
	free(cell);
}

/**
 * free_board:
 * @param board - Pointer to the board that is going to be freed
 * @return void
 */
void free_board(Board* board){
	int i ,j, k;
	i=board->length_of_board-1, j=board->length_of_board-1;
	while(j>=0){
		i=board->length_of_board-1;
		while(i>=0){
			free_cell(board->cells[i][j]);
			i--;
		}
		j--;
	}
	for(k=board->length_of_board-1;k>=0;k--){
		free(board->cells[k]);
	}
	free(board->cells);
	free(board);
}
