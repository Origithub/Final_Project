/*
 * Utilities_for_Parser.h
 *
 * Utilities_for_Parser summary:
 * This module consist the necessary functions to be used in the Parser module
 * Utilities_for_Parser supports the following functions:
 * retrieve_args_for_edit - retrieve the arguments given by the user following edit command
 * retrieve_args_for_1_args_exclude_edit - retrieve the arguments given by the user following all the commands that have 1 argument (without edit)
 * retrieve_args_for_generate_hint -retrieve the arguments given by the user following hint command
 * retrieve_args_for_set - retrieve the arguments given by the user following set command
 * is_valid_arguments_for_mark_errors - checks if the arguments are valid for mark_errors
 * is_valid_arguments_for_set - checks if the arguments are valid for set
 * is_valid_arguments_for_generate - checks if the arguments are valid for generate
 * is_valid_arguments_for_hint - checks if the arguments are valid for hint
 * is_valid_arguments_for_solve_save - checks if the arguments are valid for solve or save
 * free_all - frees the content of a list
 *** Functions that are static in the .c file:
 * calc_amount_of_free_cells - calculates the amount of empty cells in a given board
 * is_integer - checks if an argument can be converted to int
 * is_valid_integer_for_set - checks if the integer is valid for the set function (this function is used inside is_valid_arguments_for_set)
 * is_valid_integer_for_generate - checks if the integer is valid for the generate function (this function is used inside is_valid_arguments_for_generate)
 * is_valid_integer_for_hint - checks if the integer is valid for the hint function (this function is used inside is_valid_arguments_for_hint)
 */

#ifndef UTILITIES_FOR_PARSER_H_
#define UTILITIES_FOR_PARSER_H_

void retrieve_args_for_edit(char** , char, int*, int*,int* );
void retrieve_args_for_1_args_exclude_edit(char**,char,int*,int*,int* );
void retrieve_args_for_generate_hint(char**,char,int*,int*,int*);
void retrieve_args_for_set(char**,char,int*,int*,int*);

int is_valid_arguments_for_mark_errors(char **the_information,int  *num_of_args);
int is_valid_arguments_for_set(Game* the_game,char** the_information, int* num_of_args);
int is_valid_arguments_for_generate(Game* the_game,char** the_information,int* num_of_args);
int is_valid_arguments_for_hint(Game* the_game,char** the_information,int* num_of_args);
int is_valid_arguments_for_solve_save(int* num_of_args);

void free_all(char **,int);

#endif /* UTILITIES_FOR_PARSER_H_ */
