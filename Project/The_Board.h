/*
 * The_Board.h
 *
 * The_Board Summary:
 * In this module we define the data structures that our board uses:
 * the board consists of cells, each cell has it's personal information, that information also includes
 * two doubly linked lists, which are made of nodes.
 * We also define the board it self.

 * The_Board supports the following functions:
 * check_for_allocation_failure - checks for an allocation failure
 * createNode - creates a new node
 * createList - create new list
 * createCell - creates new cell
 * createBoard - creates new board
 * createGame - creates new game
 * find_node_in_loc - finds the node in the requested position of the given list (receives the head of the list) and returns its value
 * insert_node - inserts a node to a list
 * is_node_present - returns 1 if a certain node is present in a given list, 0 otherwise
 * rebuild_full_list - rebuilds a list to match up to date values
 * sub_from_List - deletes a node from the list
 * copyBoard - creates a cop to the board
 * is_valid_value_for_cell - checks if a value is compatible to a non fixed cell, calls:
 * is_valid_value_for_cell_row - checks if a value appears in its row
 * is_valid_value_for_cell_column - checks if a value appears in its column
 * is_valid_value_for_cell_block - checks if a value appears in its block
 * free_node - frees all memory resources associated with a node
 * free_list - frees all memory resources associated with a list
 * free_content_of_list - frees all memory resources associated with a list, except the pointer to the list
 * free_cell - frees all memory resources associated with a cell
 * free_board - frees all memory resources associated with a board
 */


#include <time.h>
#ifndef THE_BOARD_H_
#define THE_BOARD_H_


/**************************************************************************************/

typedef struct Node{
	int val;
	int is_head;
	int loc_x;
	int loc_y;
	int prev_val;
	int new_val;
	struct Node *next;
	struct Node *prev;
	int is_autofill;
	int is_generate;
	struct List *inner_list;
}Node;

/**************************************************************************************/

typedef struct List{
	int size;
	struct Node* head_of_list;
	struct Node* current;

} List;

/**************************************************************************************/

typedef struct cell{
	int loc_x;
	int loc_y;
	int erroneous;
	int single_fit;
	int val; /* 0 means empty */
	int length_of_mother_board;
	int fixed; /* 0 if not fixed */
	int was_set; /*if a cell have gotten a value before we called the board to be filled up - we are not allowed to touch it in the process*/
	int backtracked_to;
	int was_loaded;
	List* possible_vals;
	List* forbiden_vals;
} Cell;

/**************************************************************************************/

typedef struct Board{
	int size_of_board;
	int block_x;
	int block_y;
	int length_of_board;
	int no_solution;
	Cell*** cells;  /*this is a pointer, to an array of pointers. each pointer will be assigned to a *cell */
	struct Board *solution_of_board;
} Board;

typedef struct Game{
	struct Board *board;
	int mark_errors;
	List* undo_redo;
	char* game_mode;
}Game;

/**************************************************************************************/
void check_for_allocation_failure(void* ptr);
void free_node(struct Node* node);
struct Node* createNode(int val, Node* next, Node* prev, int is_head, int is_autofill, int is_generate, List *inner_list, int loc_x, int loc_y, int prev_val, int new_val);
int find_node_in_loc(struct Node* node, int loc);
List* createList();
void sub_from_List(List* list, int value_to_delete);
int is_node_present(List* list, int val_of_node);
void insert_node(List* list, struct Node* head, int val_of_node);
void rebuild_full_list(Board* board, Cell* cell);
void free_list(List* list);
void free_content_of_list(List* list);
Cell* createCell(int loc_x, int loc_y, int val, int length_of_mother_board);
void free_cell(Cell* cell);

Board* createBoard(int block_x, int block_y);
void copyBoard(Board* board);
void free_board(Board* board);
Game* createGame();


int is_valid_value_for_cell(Board* board, Cell* cell, int value_for_cell);
int is_valid_value_for_cell_row(Board* board, Cell* cell, int value_for_cell);
int is_valid_value_for_cell_column(Board* board, Cell* cell, int value_for_cell);
int is_valid_value_for_cell_block(Board* board, Cell* cell, int value_for_cell);
#endif /* THE_BOARD_H_ */
