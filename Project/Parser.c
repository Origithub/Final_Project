#include <stdio.h>
#include "Parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "The_Board.h"
#include "Utilities_for_Commands.h"
#include "Utilities_for_Parser.h"
static int is_valid_func_name(Game* the_game,char *the_word, char c, int* exit_prog);
static void retrieve_arguments(char** the_information, char c, int * exit_prog, int *num_of_args,int *length);
static int is_valid_arguments(Game* the_game, char **the_information, int *num_of_args);

/**
 * is_valid_func_name - checks if the input function is a valid one
 * @param *game - a pointer to the game we are playing
 * @param *the_word - the function name 
 * @param c - the previous char that has been read
 * @param *exit_prog - an indicator that is 1 when we should exit the program (when encountering EOF or after exit command from user)
 * @return int - 1 if valid, else 0
 */
static int is_valid_func_name(Game* the_game,char *the_word, char c, int* exit_prog){
	if (strcmp(the_word,"solve")==0 || strcmp(the_word,"edit")==0 || strcmp(the_word,"exit")==0){
		return 1;
	}
	else if (strcmp(the_word,"mark_errors")==0 ||strcmp(the_word,"hint")==0 || strcmp(the_word,"autofill")==0 ){
		if (strcmp(the_game->game_mode,"Solve")==0){
			return 1;
		}
		else {
			while (c!=EOF && c!='\n' && c!='\r') c=getc(stdin); /* need to deplete the line of the invalid function */
			if (c=='\r') c=getc(stdin);
			else if (c==EOF) *exit_prog=1;
			return 0;
		}
	}
	else if (strcmp(the_word,"print_board")==0 || strcmp(the_word,"set")==0 ||
			strcmp(the_word,"validate")==0 || strcmp(the_word,"undo")==0 ||
			strcmp(the_word,"redo")==0 || strcmp(the_word,"save")==0 || 
			strcmp(the_word,"num_solutions")==0 || strcmp(the_word,"reset")==0){
		if (strcmp(the_game->game_mode,"Init")==0){
			while (c!=EOF && c!='\n' && c!='\r') c=getc(stdin); /* need to deplete the line of the invalid function */
			if (c=='\r') c=getc(stdin);
			else if (c==EOF) *exit_prog=1;
			return 0;
		}
		else {
			return 1;
		}
	}
	else if (strcmp(the_word,"generate")==0){
		if (strcmp(the_game->game_mode,"Edit")==0){
			return 1;
		}
		else {
			while (c!=EOF && c!='\n' && c!='\r') c=getc(stdin); /* need to deplete with the line of the invalid function */
			if (c=='\r') c=getc(stdin);
			else if (c==EOF) *exit_prog=1;
			return 0;
		}
	}
	else {
		while (c!=EOF && c!='\n' && c!='\r') c=getc(stdin); /* need to deplete with the line of the invalid function */
		if (c=='\r') c=getc(stdin);
		else if (c==EOF) *exit_prog=1;
		return 0;
	}
}

/**
 * retrieve_arguments - getting the users arguments
 * @param **the_information - an array of strings, containing the function name in first place, and other arguments by order
 * @param c - the previous char that has been read 
 * @param *exit_prog - an indicator that is 1 when we should exit the program (when encountering EOF or exit command from user)
 * @param *num_of_args - the number of arguments given by the user
 * @param *length - the length of the users command
 * @return void
 */
static void retrieve_arguments(char** the_information, char c, int * exit_prog, int *num_of_args,int *length){
	if (strcmp(*the_information,"print_board")==0 || strcmp(*the_information,"validate")==0 ||
			strcmp(*the_information,"undo")==0 || strcmp(*the_information,"redo")==0 ||
			strcmp(*the_information,"num_solutions")==0 || strcmp(*the_information,"autofill")==0 ||
			strcmp(*the_information,"reset")==0 || strcmp(*the_information,"exit")==0){
		*num_of_args=0;
		while (c!=EOF && c!='\n' && c!='\r'){
			c=getc(stdin);
			(*length)++;
		}
		if (c=='\r') c=getc(stdin);
		else if (c==EOF) *exit_prog=1;
		return;
	}
	else if(strcmp(*the_information,"solve")==0 || strcmp(*the_information,"edit")==0 ||
			strcmp(*the_information,"mark_errors")==0 || strcmp(*the_information,"save")==0){
		if (strcmp(*the_information,"edit")==0){ /*because edit can be without arguments */
			retrieve_args_for_edit(the_information, c, exit_prog, num_of_args,length);
		}
		else { /* all the ones with 1 arguments, exclude edit */
			retrieve_args_for_1_args_exclude_edit(the_information, c, exit_prog, num_of_args,length);
		}
	}
	else if (strcmp(*the_information,"generate")==0  || strcmp(*the_information,"hint")==0){
		retrieve_args_for_generate_hint(the_information, c, exit_prog, num_of_args,length);
	}
	else { /*function name is valid so we are sure it is the "set" function */
		retrieve_args_for_set(the_information, c, exit_prog, num_of_args,length);
	}
}

/**
 * is_valid_arguments - checks if the users arguments are valid
 * @param the_game - a pointer to the game we are playing
 * @param **the_information - an array of strings, containing the function name in first place, and other arguments by order
 * @param *num_of_args - the number of arguments given by the user
 * @return int - 1 if valid, else 0
 */
static int is_valid_arguments(Game* the_game, char **the_information, int *num_of_args){
	if (strcmp(*the_information,"mark_errors")==0){
		return is_valid_arguments_for_mark_errors(the_information, num_of_args);
	}
	else if (strcmp(*the_information,"set")==0){
		return is_valid_arguments_for_set(the_game, the_information, num_of_args);
	}
	else if (strcmp(*the_information,"generate")==0){
		return is_valid_arguments_for_generate(the_game, the_information, num_of_args);
	}
	else if (strcmp(*the_information,"hint")==0){
		return is_valid_arguments_for_hint(the_game, the_information, num_of_args);

	}
	else if (strcmp(*the_information,"solve")==0 || strcmp(*the_information,"save")==0){
		return is_valid_arguments_for_solve_save(num_of_args);
	}
	else { /* the rest, without args so always valid */
		return 1;
	}
}

/**
 * read_input - responsible to process each line from the user
 * @param the_game - a pointer to the game we are playing
 * @param **the_information - an array of strings, containing the function name in first place, and other arguments by order
 * @param *exit_prog - an indicator that is 1 when we should exit the program (when encountering EOF or exit command from user)
 * @param *num_of_args - the number of arguments given by the user
 * @return int - 1 if valid, else 0
 */
int read_input(Game* the_game, char** the_information, int *exit_prog, int* num_of_args){
	int length=0,valid,counter=0, i=0; char c;
	*the_information=(char*) malloc(sizeof(char)*20);
	check_for_allocation_failure(*the_information);
	c=getc(stdin);
	if (c=='\n' || c=='\r' ){ /*no input just new line */
		free(*the_information);
		if (c=='\r') c=getc(stdin);/* '\r' comes as \r\n so need to deplete \n */
		return 0;
	}
	while (c==' ' || c=='\t') {
		c=getc(stdin);/* remove white spaces before the function name */
		length++;
	}
	while (c!=' ' && c!='\n' && c!='\r' && c!='\t' && c!=EOF){
		the_information[0][i]=c;
		length++;
		counter++;
		i++;
		c=getc(stdin);
		if (counter==19) {
			free(*the_information);
			printf("ERROR: invalid command\n");
			while (c!=EOF && c!='\n' && c!='\r') c=getc(stdin);
			if (c==EOF) *exit_prog=1;
			return 0;
		}
	}
	if (c=='\r') c=getc(stdin);
	if (c==EOF) *exit_prog=1;
	the_information[0][i]='\0';
	valid=is_valid_func_name(the_game, the_information[0],c,exit_prog);
	if (valid!=1){
		free(*the_information);
		printf("ERROR: invalid command\n");
		return 0;
	}
	retrieve_arguments(the_information,c,exit_prog,num_of_args, &length);
	if (length>256){
		free_all(the_information, *num_of_args);
		printf("ERROR: invalid command\n");
		return 0;
	}
	valid=is_valid_arguments(the_game, the_information, num_of_args);
	if (valid!=1){
		free_all(the_information, *num_of_args);
		return 0;	
	}
	return 1;
}
