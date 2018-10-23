/*
 * Commands.h
 *
 * Commands summary:
 * In this module we handle the commands sent by the user, and send them on to the suiting function.
 *
 * Commands supports the following functions:
 * execute_cmd - executes a given command 
 * print_board - prints the board we are playing
 *** Functions that are static in the .c file:
 * solve - starts a puzzle in solve mode
 * edit - starts a puzzle in edit mode
 * mark_errors - sets the mark errors setting to the given input
 * set - sets a given value to a given cell
 * validate - validates the current board using ILP
 * generate - generates a puzzle using a unique method
 * undo - undo a previous move done by the user
 * redo - redo a previous move done by the user
 * save - saves the current game board to the specified file
 * hint - gives the user a hint about a certain cell in the board
 * num_solutions - print the number of solutions for the current board
 * autofill  - automatically fill "obvious" values to cells which contain a single legal value
 * reset - ndoes all moves, reverting the board to its original loaded state
 * exit_now - terminates the program
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

void execute_cmd(Game* game, char** cmd, int*, int*, int*, int*);
void print_board(Game* board);


#endif /* COMMANDS_H_ */
