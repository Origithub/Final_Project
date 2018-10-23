/*
 * Utilities_for_Commands.h
 *
 * Utilities_for_Commands summary:
 * This module consists the functions that help process the users input.
 *
 * Utilities_for_Commands supports the following functions:
 * mark_loaded_cells - marks the non empty cells of the board that were loaded from a file
 * load_puzzle_from_file - loads a puzzle from a file in the given file path
 * print_row - prints a row of the board 
 * load - load's a board to the game from a given file path
 * contains_erroneous_val - check's if a given board contains erroneous values
 * check_game_ended - checks if the game has ended (the last cell was correctly filled and the board isn't erroneous)
 * add_to_undo_redo_list_for_set - adds a set operation to the undo_redo list
 * print_autofill_or_generate_changes - prints the changes per sub-node of the generate or autofill node
 * apply_inner_nodes - applying to the board the changes from the inner list
 * add_to_undo_redo_list_for_autofill_generate - add's an autofill or a generate node to the undo_redo list
 * mark_unmark_erroneous_vals - add to remove erroneous signs to cells
 * mark_erroneous_after_load - marks all erroneous values in the board after a load 
 * print_undo_redo - prints an action done in a redo or undo move
 * remove_fixed_from_cells - sets all cells of a given board to be not fixed
 * board_full_test - checks if board is full
 * board_empty_test - checks if board is empty
 * clear_board - sets the value of all the board's cell's to 0
 *** Functions that are static in the .c file:
 * get_dimension - retrieves one of the board dimensions from the file
 * get_cell_value - retrieves a cell value from the file
 * remove_all_after_current - removes all nodes that are located after the current node in a given list 
 * mark_erroneous_vals - marks erroneous values that were effected by the insertion of a value to a cell
 * deletion_of_value - unmarks erroneous values that are effected by the deletion of a value from a cell
 */

#ifndef UTILITIES_FOR_COMMANDS_H_
#define UTILITIES_FOR_COMMANDS_H_

void mark_loaded_cells(Board* board);
int load_puzzle_from_file (char* file_path);

void print_row(Game* game);
int load(Game*, char*);
int contains_erroneous_val(Game*);
void check_game_ended(Game*);
void add_to_undo_redo_list_for_set(Game*,int, int,int, int*);
void print_autofill_or_generate_changes(Node* node,int undo);
void apply_inner_nodes(Game*, Node*);
void add_to_undo_redo_list_for_autofill_generate(Game*,int, int,int, int*, int, int);
void mark_unmark_erroneous_vals(Game*, int,int,int, int, const char*);
void mark_erroneous_after_load(Game*);
void print_undo_redo(int x, int y,int z1, int z2, int undo);
void remove_fixed_from_cells(Game*);

int board_full_test(Board* board);
int board_empty_test(Board* board);
void clear_board(Board* board);
#endif /* UTILITIES_FOR_COMMANDS_H_ */
