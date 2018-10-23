#include <stdio.h>
#include <stdlib.h>
#include "Utilities_for_generate_and_num_solutions.h"
#include "Utilities_for_Commands.h"
#include "GRB_solver.h"

/**
 * createStackNode - creates a new node for a stack
 * @param x - location on x axis
 * @param y - location on y axis
 * return - a pointer to a new struct StackNode
 */
struct StackNode* createStackNode(int x, int y){
	struct StackNode* node = (struct StackNode*) malloc(sizeof(struct StackNode));
	check_for_allocation_failure(node);
	node->x_location = x;
	node->y_location = y;
	node->next = NULL;
	return node;
}

/**
 * createStack - creates a new stack
 * return - a pointer to a new Stack
 */
Stack* createStack(){
	Stack* stack = (Stack*) malloc(sizeof(Stack));
	check_for_allocation_failure(stack);
	stack->head = NULL;
	stack->size = 0;
	return stack;
}

/**
 * free_stack - frees the stack
 * @param stack - a pointer the stack we wish to free
 * @return - void
 */
void free_stack(Stack* stack){
	struct StackNode* temp; struct StackNode* to_free;
	temp = stack->head;
	while(temp != NULL){
		to_free = temp;
		temp = temp->next;
		free(to_free);
	}
	/*free(stack) is done outside in case we will never backtrack*/
}

/**
 * pop - pops the top item of the stack (a LIFO stack)
 * @param stack - a pointer to the stack from which we will pop
 * @return void
 */
void pop(Stack* stack){
	struct StackNode* temp;
	temp = stack->head;
	stack->head = temp->next;
	temp->next = NULL;
	free(temp);
	stack->size--;
}

/**
 * push - pushes a StackNode to the top of the stack (a LIFO stack)
 * @param board - a pointer to the board on which we are backtracking
 * @param stack - a pointer to the stack to which we push
 * @param x_loc - Location of the pushed node on the x axis
 * @param y_loc - Location of the pushed node on the y axis
 * @param new_loc_x - A pointer that will be changed to the new location on the x axis from which we will continue filling our board
 * @param new_loc_y - A pointer that will be changed to the new location on the y axis from which we will continue filling our board
 * @return void
 */
void push(Board* board, Stack* stack, int x_loc, int y_loc, int* new_loc_x, int* new_loc_y){
	struct StackNode* node;
	node = createStackNode(x_loc, y_loc);
	if (stack->head == NULL){
		stack->head = node;
	}
	else{
		node->next = stack->head;
		stack->head = node;
	}
	stack->size++; /*the size grows regardless*/
	if((stack->head->x_location == 0) && (stack->head->y_location == 0)){
		/*if we got back to the beginning of the board - meaning our top node corresponds to the first cell:
		 * we shall stop solving and free our stack.*/
		*new_loc_x = (board->length_of_board+1);
		*new_loc_y = (board->length_of_board+1);
		free_stack(stack);
		/*getting here means we have no more solutions - so we set special values to break the loops in: find_all_solutions*/
	}
	else if(stack->head->x_location == 0){ /*we need to go a row up in the board*/
		case_beginning_of_row(x_loc, y_loc, board, new_loc_x, new_loc_y, stack);
	}
	else{ /*we are at the middle of the board, or we are at the first row(y==0) - and only need to walk left on x */
		case_middle_of_row(x_loc, y_loc, board, new_loc_x, new_loc_y, stack);
	}
}


/**
* check_single_fit - checks if a cell has only one fitting value
* @param board - a pointer to the board we are playing
* @param cell - a pointer to the cell we wish to check
* @return void
*/
int check_single_fit(Board* board, Cell* cell){
	int is_valid; int i=1;
	for(;i<(cell->length_of_mother_board+1);i++){
		is_valid = is_valid_value_for_cell(board, cell, i);
		if (is_valid == 1){
			insert_node(cell->possible_vals, cell->possible_vals->head_of_list, i);
		}
	}
	if(cell->possible_vals->size > 1){ /*meaning more than one possible value*/
		free_content_of_list(cell->possible_vals); /*we will free the content of our list and exit with failure 01)*/
		return 0;
	}
	/*if we get here it means we have exactly one or no possible values*/
	else if (cell->possible_vals->size == 1){ /*if there is a single suitable value*/
		cell->single_fit = cell->possible_vals->head_of_list->val; /*we shall set our single fit*/
		free_content_of_list(cell->possible_vals); /*we will free the content of our list and exit with success (1)*/
		return 1;
	}
	else{ /*if we get here then our list is empty anyways. it also means there is no solution to the board, but not of the kind we need to detect*/
		return 0;
	}
}

/*
 * find_last_empty_cell - finds the location of the last empty cell in the board
 * @param board - a pointer to the board which we will solve
 * @param last_x - a pointer to where we will save the x-axis location of the last empty cell in the board  
 * @param last_y - a pointer to where we will save the y-axis location of the last empty cell in the board  
 * @return void
 */
void find_last_empty_cell(Board* board, int* last_x, int* last_y){
	int i=0, j=0;
	for(j=board->length_of_board-1 ;j>=0;j--){ /*first we will find the location of the last free cell:*/
		for(i=board->length_of_board-1 ;i>=0;i--){
			if((board->cells[i][j]->fixed == 0) && (board->cells[i][j]->was_set == 0) && (board->cells[i][j]->was_loaded == 0)){
				(*last_x)= i; (*last_y) = j;
				i = j = -1; /*this way we ensure we break from both loops*/
				break;
			}
		}
	}
}

/**
 * find_all_solutions:
 * Receives a board and finds all of his solutions - returns the amount of solutions found
 * @param board - a pointer to the board which we will solve
 * @param stack - a pointer to the stack of our backtracking calls
 * @return the amount of solutions found
 */
int find_all_solutions(Board* board, Stack* stack){
	int i=0, j=0, k ,new_loc_x, new_loc_y, num_of_solutions=0;
	int last_x = 0; int last_y = 0;
	find_last_empty_cell(board, &last_x, &last_y);
	while(j<board->length_of_board){
		i=0;
		while(i<board->length_of_board){
			if((board->cells[i][j]->fixed == 0) && (board->cells[i][j]->was_set == 0)&& (board->cells[i][j]->was_loaded == 0)){
				/*first we check if the current cell was backtracked from before:*/
				if((board->cells[i][j]->backtracked_to == 0)&&(board->cells[i][j]->forbiden_vals->size > 0)){
					free_content_of_list(board->cells[i][j]->forbiden_vals);
					board->cells[i][j]->forbiden_vals->head_of_list = NULL; board->cells[i][j]->forbiden_vals->size = 0;
				}
				/*secondly, we look for a new value for the cell in a deterministic way*/
				for(k=1 ;k<board->length_of_board+1;k++){
					if((is_valid_value_for_cell(board, board->cells[i][j], k) == 1)&&(is_node_present(board->cells[i][j]->forbiden_vals, k)==0)){
						board->cells[i][j]->val = k;
						if (stack->head != NULL) pop(stack);
						i++;
						break; /*if we found our new value, we shall exit the loop*/
					}
				}
				if (k == board->length_of_board+1){/*there is no fitting value*/
					board->cells[i][j]->backtracked_to = 0; /*because we go back*/
					if((stack->head != NULL)&&(stack->head->x_location == i)&&(stack->head->y_location == j)){
						pop(stack);
					}
					push(board, stack, i, j, &new_loc_x, &new_loc_y);
					i=new_loc_x; j=new_loc_y; /*updating our indexes after backtracking*/
				}
				else if(((i-1) == last_x) && (j == last_y)){ /*lets check we didn't set the last value:*/
					/*if we enter here, it means we found a solution, so we shall mark it, and continue searching for more:*/
					num_of_solutions++; /*if we get here it means we solved the board*/
					insert_node(board->cells[i-1][j]->forbiden_vals, board->cells[i-1][j]->forbiden_vals->head_of_list, k);
					board->cells[i-1][j]->val = 0;
					push(board, stack, i-1, j, &new_loc_x, &new_loc_y);
					i=new_loc_x; j=new_loc_y; /*updating our indexes after backtracking*/
				}
			}
			else{
				/*we push set/loaded/fixed cells - so we will pop them too*/
				if (stack->head != NULL){
					pop(stack);
				}
				i++;
			}
		}
		j++;
	}
	return num_of_solutions;
}

/**
 * case_beginning_of_row:
 * In use when backtracking from beginning of row
 * @param x - Location of the cell on the x axis
 * @param y - Location of the cell on the y axis
 * @param board - a pointer to the board on which we are backtracking
 * @param new_loc_x - A pointer that will be changed to the new location on the x axis from which we will continue filling our board
 * @param new_loc_y - A pointer that will be changed to the new location on the y axis from which we will continue filling our board
 * @param stack - a pointer to the stack of our backtracking calls
 * @return void
 */
void case_beginning_of_row(int x, int y, Board* board, int* new_loc_x, int* new_loc_y,Stack* stack){
	int k;
	board->cells[x][y]->backtracked_to = 0; /*we went back from here*/
	if((board->cells[board->length_of_board-1][y-1]->was_set == 1)||(board->cells[board->length_of_board-1][y-1]->fixed == 1)|| (board->cells[board->length_of_board-1][y-1]->was_loaded == 1)){
	/*previous cell is fixed or was set before the current run:*/
		push(board, stack, board->length_of_board-1, y-1, new_loc_x, new_loc_y);
	}
	else if(board->cells[board->length_of_board-1][y-1]->val == board->length_of_board){
		board->cells[board->length_of_board-1][y-1]->backtracked_to = 1;
		insert_node(board->cells[board->length_of_board-1][y-1]->forbiden_vals, board->cells[board->length_of_board-1][y-1]->forbiden_vals->head_of_list, board->cells[board->length_of_board-1][y-1]->val);
		board->cells[board->length_of_board-1][y-1]->val = 0;
		push(board, stack, board->length_of_board-1, y-1, new_loc_x, new_loc_y);
	}
	else{
		board->cells[board->length_of_board-1][y-1]->backtracked_to = 1;
		insert_node(board->cells[board->length_of_board-1][y-1]->forbiden_vals, board->cells[board->length_of_board-1][y-1]->forbiden_vals->head_of_list, board->cells[board->length_of_board-1][y-1]->val);
		k = (board->cells[board->length_of_board-1][y-1]->val + 1);
		for(;k<board->length_of_board+1;k++){
			if((is_valid_value_for_cell(board, board->cells[board->length_of_board-1][y-1], k) == 1)&&(is_node_present(board->cells[board->length_of_board-1][y-1]->forbiden_vals, k)==0)){
				board->cells[board->length_of_board-1][y-1]->val = k;
				*new_loc_x = x;
				*new_loc_y = y;
				return;
			}
		}
		/*if we get here, it means we couldn't set a value - and need to keep backtracking*/
		board->cells[board->length_of_board-1][y-1]->val = 0;
		push(board, stack, board->length_of_board-1, y-1, new_loc_x, new_loc_y);
	}
}

/**
 * case_middle_of_row:
 * In use when backtracking from middle of row
 * @param x - Location of the cell on the x axis
 * @param y - Location of the cell on the y axis
 * @param board - a pointer to the board on which we are backtracking
 * @param new_loc_x - A pointer that will be changed to the new location on the x axis from which we will continue filling our board
 * @param new_loc_y - A pointer that will be changed to the new location on the y axis from which we will continue filling our board
 * @param stack - a pointer to the stack of our backtracking calls
 * @return void
 */
void case_middle_of_row(int x, int y, Board* board, int* new_loc_x, int* new_loc_y, Stack* stack){
	int k;
	board->cells[x][y]->backtracked_to = 0;
	if((board->cells[x-1][y]->was_set == 1)||(board->cells[x-1][y]->fixed == 1) || (board->cells[x-1][y]->was_loaded == 1)){
	/*previous cell is fixed or was set before the current run:*/
		push(board, stack, x-1, y, new_loc_x, new_loc_y);
	}
	else if(board->cells[x-1][y]->val == board->length_of_board){
	/*his value can't be incremented*/
		board->cells[x-1][y]->backtracked_to = 1;
		insert_node(board->cells[x-1][y]->forbiden_vals, board->cells[x-1][y]->forbiden_vals->head_of_list, board->cells[x-1][y]->val);
		board->cells[x-1][y]->val = 0;
		push(board, stack, x-1, y, new_loc_x, new_loc_y);
	}
	else{
		board->cells[x-1][y]->backtracked_to = 1;
		insert_node(board->cells[x-1][y]->forbiden_vals, board->cells[x-1][y]->forbiden_vals->head_of_list, board->cells[x-1][y]->val);
		k = (board->cells[x-1][y]->val + 1);
		for(;k<board->length_of_board+1;k++){
			if((is_valid_value_for_cell(board, board->cells[x-1][y], k) == 1)&&(is_node_present(board->cells[x-1][y]->forbiden_vals, k)==0)){
				board->cells[x-1][y]->val = k;
				*new_loc_x = x;
				*new_loc_y = y;
				return;
			}
		}
		/*if we get here, it means we couldn't set a value - and need to keep backtracking*/
		board->cells[x-1][y]->val = 0;
		push(board, stack, x-1, y, new_loc_x, new_loc_y);
	}
}

/**
 * generate_stage_one - fills random x cells with random fitting values
 * @param game - a pointer to the game board we are trying to fill
 * @param x - the amount of cells we wish to fill
 * @param y - the amount of cells we wish to keep
 * @param amount_of_attempts - a pointer to an integer that tells us how many times we already tried and
 * failed to generate the board
 * @return void
 */
void generate_stage_one(Game* game ,int x,int y, int* amount_of_attempts){
	int x_loc, y_loc, node_loc, i=0;
	while (i<x){ /*first lets randomly fill up x cells*/
		x_loc = rand()%game->board->length_of_board;
		y_loc = rand()%game->board->length_of_board;
		if(game->board->cells[x_loc][y_loc]->val == 0){ /*if we haven't set this cell a value yet*/
			rebuild_full_list(game->board, game->board->cells[x_loc][y_loc]); /*rebuilds a cells list of compatible values*/
			if (game->board->cells[x_loc][y_loc]->possible_vals->size == 0){/*if there is no fitting value, we back track, and dont set a value yet*/
				(*amount_of_attempts)++;
				clear_board(game->board);
				if(*amount_of_attempts <= 999){
					generate_stage_one(game , x, y, amount_of_attempts);
				}
			}
			else if (game->board->cells[x_loc][y_loc]->possible_vals->size == 1){/*if there is only one value - no need for randomization*/
				game->board->cells[x_loc][y_loc]->val = game->board->cells[x_loc][y_loc]->possible_vals->head_of_list->val; /*setting up our only good value*/
				game->board->cells[x_loc][y_loc]->was_loaded = 1; /*we are marking all cell's to the event we will run num_solutions on the created board*/
				free_content_of_list(game->board->cells[x_loc][y_loc]->possible_vals); /*lets not forget to free memory*/
				i++;
			}
			else{
				node_loc = rand() % (game->board->cells[x_loc][y_loc]->possible_vals->size); /*need to look at the cells list of possible values*/
				game->board->cells[x_loc][y_loc]->val = find_node_in_loc(game->board->cells[x_loc][y_loc]->possible_vals->head_of_list, node_loc);
				game->board->cells[x_loc][y_loc]->was_loaded = 1; /*we are marking all cell's to the event we will run num_solutions on the created board*/
				free_content_of_list(game->board->cells[x_loc][y_loc]->possible_vals); /*lets not forget to free memory*/
				i++;
			}
		}
	}
}

/**
 * generate_stage_two - leaves y random cells
 * @param game - a pointer to the game board we are trying to fill (being received full!)
 * @param y - the amount of cells we wish to keep
 * @return void
 */
void generate_stage_two(Game* game , int y){
	int x_loc, y_loc, j=0;
	while (j<(game->board->size_of_board - y)){
		x_loc = rand()%game->board->length_of_board;
		y_loc = rand()%game->board->length_of_board;
		if(game->board->cells[x_loc][y_loc]->val != 0){ /*if we haven't already cleared the cell*/
			game->board->cells[x_loc][y_loc]->val = 0;
			game->board->cells[x_loc][y_loc]->was_loaded = 0; /*we are unmarking all cell's we don't use to the event we will run num_solutions on the created board*/
			j++;
		}
	}
}

/**
* make_generate_node - makes a special node representing a generate node  
* @param game - a pointer to the game we are playing
* @param *no_prev - 1 if the undo-redo list has no undo move, else 0
* @return void
*/
void make_generate_node(Game* game, int* no_prev){
	int i,j, new_node=1;
	for (i=0 ;i < game->board->length_of_board ; i ++){
		for (j=0 ; j < game->board->length_of_board ; j++){
			if (game->board->cells[j][i]->val!=0){
				add_to_undo_redo_list_for_autofill_generate(game,j+1,i+1, game->board->cells[j][i]->val, no_prev, new_node, 0);
				new_node=0;
			}
		}
	}
}

/**
 * generate_recursive - uses a recursive algorithm to generate a board
 * @param game - a pointer to the game board we wish to generate
 * @param x - amount of random cells we will fill up to begin with
 * @param y - amount of random cells we will leave non empty at the end of the process
 * @param amount_of_attempts - a pointer to an integer that tells us how many times we already tried and
 * failed to generate the board.
 * @return void
 */
void generate_recursive(Game* game ,int x, int y, int* amount_of_attempts){
	srand(time(NULL));
	generate_stage_one(game, x, y, amount_of_attempts); /*let's fill up x cells*/
	if (*amount_of_attempts <= 999){ /* we did not fail in stage 1*/
		solve_for_generate(game,x,y,amount_of_attempts);
	}
}
