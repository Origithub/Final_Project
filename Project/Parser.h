/*
 * Parser.h
 *
 * Parser summary:
 * In this module we proccess the user input and check its correctness
 *
 * Parser supports the following functions:
 * read_input - executes a given command 
 *** Functions that are static in the .c file:
 * is_valid_func_name - checks if the function name is a valid one
 * is_valid_arguments - checks if the arguments that were given are valid
 * retrieve_arguments - processing the users arguments
 */
#include "The_Board.h"
#ifndef PARSER_H_
#define PARSER_H_
int read_input(Game*, char**, int*, int*);
#endif /* PARSER_H_ */
