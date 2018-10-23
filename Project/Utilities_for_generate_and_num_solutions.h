/*
* Utilities_for_generate_and_num_solutions.h
*
* Utilities_for_generate_and_num_solutions summary:
* In this module we include all the utilities related to the num_solutions and generate commands. 
*
* Utilities_for_generate_and_num_solutions supports the following functions:
* createStackNode - creates a new StackNode
* createStack -  creates a new Stack
* pop - pop's a StackNode from the stack
* push - push's a StackNode to the stack
* free_stack - frees the memory of all nodes that are in the stack
* check_single_fit - check's if a given cell has only one fitting value
* find_all_solutions - finds all possible solutions to a given game board
* find_last_empty_cell - finds the location of the last empty cell in the board
* case_beginning_of_row - a backtracking function, used when backtracking from the beginning of a certain row
* case_middle_of_row - a backtracking function, used when backtracking from the middle of a certain row
* generate_stage_one - randomly chooses and fills up a given amount of cells in the board
* generate_stage_two - randomly chooses a given amount of cells in the board and sets their value to zero
* generate_recursive - generates a board using a recursive algorithm
* make_generate_node - creates a special node for the undo_redo list, representing the generate command
*/

#ifndef UTILITIES_FOR_GENERATE_AND_NUM_SOLUTIONS_H_
#define UTILITIES_FOR_GENERATE_AND_NUM_SOLUTIONS_H_

#include "The_Board.h"

typedef struct StackNode{
	int y_location;
	int x_location;
	struct StackNode *next;
} StackNode;

typedef struct Stack{
	struct StackNode *head;
	int size;
} Stack;

struct StackNode* createStackNode(int x, int y);
Stack* createStack();
void pop(Stack* stack);
void push(Board* board, Stack* stack, int x_loc, int y_loc, int* new_loc_x, int* new_loc_y);
void free_stack(Stack* stack);

int check_single_fit(Board* board, Cell* cell);
int find_all_solutions(Board* board, Stack* stack);
void find_last_empty_cell(Board* board, int* last_x, int* last_y);

void case_beginning_of_row(int x, int y, Board* board, int* new_loc_x, int* new_loc_y, Stack* stack);
void case_middle_of_row(int x, int y, Board* board, int* new_loc_x, int* new_loc_y, Stack* stack);
void generate_stage_one(Game* game ,int x, int y, int* amount_of_attempts);
void generate_stage_two(Game* game  , int y);
void generate_recursive(Game* game ,int x, int y, int* amount_of_attempts);
void make_generate_node(Game* game,int * no_prev);
#endif /* UTILITIES_FOR_GENERATE_AND_NUM_SOLUTIONS_H_ */
