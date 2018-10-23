#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "The_Board.h"
#include "Utilities_for_Commands.h"

static int calc_amount_of_free_cells(Game* the_game);
static int is_integer(char* word);
static int is_valid_integer_for_set(Game* the_game,char* word, int allowed_to_be_zero);
static int is_valid_integer_for_generate(Game* the_game, char** the_information);
static int is_valid_integer_for_hint(Game* the_game, char** the_information);

/**
 * calc_amount_of_free_cells - calculates amount of empty cells in a given board
 * @param *the_game - a pointer to the game we are playing
 * @return int - the amount of free cells (0 if it is a full board)
 */
static int calc_amount_of_free_cells(Game* the_game){
	int amount=0,i,j;
	for (i=0; i < the_game->board->length_of_board ; i++){
		for (j=0 ; j < the_game->board->length_of_board ; j++){
			if (the_game->board->cells[i][j]->val==0) amount++;
		}
	}
	return amount;
}

/**
 * is_integer - checks if an argument can be converted to int
 * @param *word - the current argument
 * @return int - 0 if isn't an int, else the length of the integer
 */
static int is_integer(char* word){
	int i=0; int temp;
	while (word[i]!='\0'){
		temp=isdigit(word[i]);
		if (temp==0) return 0;
		i++;
	}
	return i; /* the length of the integer */
}

/**
 * is_valid_integer_for_set - checks if the integer is valid for the set function
 * @param *the_game - a pointer to the game we are playing
 * @param *word - the current argument
 * @param allowed_to_be_zero - an indicator that when is 1 means users argument 0 is valid (cell value 0 is valid, location 0 is invalid)
 * @return int - 0 if isn't an int, else the length of the integer
 */
static int is_valid_integer_for_set(Game* the_game,char* word, int allowed_to_be_zero){
	int temp, length;
	length=is_integer(word); /* returns 0 if not an int, else the length of the int */
	if (length==0) return 0;
	temp=atoi(word);
	if (length>1 && temp==0) return 0; /*means atoi failed to convert the int (too big etc)*/
	if (allowed_to_be_zero==0){
		if (temp<=0 || temp > the_game->board->length_of_board) return 0;
	}
	else {
		if (temp< 0 || temp > the_game->board->length_of_board) return 0;
	}
	return 1;
}

/**
 * is_valid_integer_for_generate - checks if the arguments are valid for generate
 * @param *the_game - a pointer to the game we are playing
 * @param **the_information - array of strings which contains the function name and users arguments
 * @return int - 1 if its a valid integer for generate, 0 if invalid
 */
static int is_valid_integer_for_generate(Game* the_game, char** the_information){
	int i,temp,length, num_of_free_cells;
	num_of_free_cells=calc_amount_of_free_cells(the_game);
	for (i=1; i < 3 ; i++){
		length=is_integer(the_information[i]);
		if (length==0){
			printf("Error: value not in range 0-%d\n",num_of_free_cells);
			return 0;
		}
		/* means it is an int */
		temp=atoi(the_information[i]);
		if (length>1 && temp==0) {
			printf("Error: value not in range 0-%d\n",num_of_free_cells);
			return 0; /*means atoi failed to convert the int (too big etc)*/
		}
		if (temp>num_of_free_cells || temp < 0){
			printf("Error: value not in range 0-%d\n",num_of_free_cells);
			return 0;
		}
	}
	return 1;
}

/**
 * is_valid_integer_for_hint - checks if the arguments are valid for hint
 * @param *the_game - a pointer to the game we are playing
 * @param **the_information - array of strings which contains the function name and users arguments
 * @return int - 1 if its a valid integer for hint, 0 if invalid
 */
static int is_valid_integer_for_hint(Game* the_game, char** the_information){
	int i,temp,length;
	for (i=1; i < 3 ; i++){
		length=is_integer(the_information[i]);
		if (length==0){
			printf("Error: value not in range 1-%d\n",the_game->board->length_of_board);
			return 0;
		}
		/* means it is an int */
		temp=atoi(the_information[i]);
		if (length>1 && temp==0){
			printf("Error: value not in range 1-%d\n",the_game->board->length_of_board);
			return 0;
		}
		if (temp>the_game->board->length_of_board || temp < 1){
			printf("Error: value not in range 1-%d\n",the_game->board->length_of_board);
			return 0;
		}
	}
	if (contains_erroneous_val(the_game)==1){
		printf("Error: board contains erroneous values\n");
		return 0;
	}
	else if (the_game->board->cells[atoi(the_information[1])-1][atoi(the_information[2])-1]->fixed==1){
		printf("Error: cell is fixed\n");
		return 0;
	}
	else if (the_game->board->cells[atoi(the_information[1])-1][atoi(the_information[2])-1]->val!=0){
		printf("Error: cell already contains a value\n");
		return 0;
	}
	return 1;
}

/**
 * free_all - frees the content of a list
 * @param **list - a pointer to the list which is getting freed
 * @param num - amount of nodes
 * @return void
 */
void free_all(char **list, int num){
	int i;
	free(list[0]);
	for (i=1 ; i < num+1 ; i++){
		free(list[i]);
	}
}

/**
 * retrieve_args_for_edit - retrieve the arguments given by the user following edit command
 * @param **the_information - array of strings which contains the function name and users arguments
 * @param c - the character that is being read
 * @param *exit_prog - an indicator that is 1 when we should exit the program (when encountering EOF or after exit command from user)
 * @param *num_of_args - number of arguments given by the user
 * @param *length - the length of the users command
 * @return void
 */
void retrieve_args_for_edit(char** the_information, char c, int* exit_prog, int* num_of_args, int* length){
	int i=0; char *p;
	if (c==EOF || c=='\n' || c=='\r') {
		if (c==EOF) *exit_prog=1;
		if (c=='\r') c=getc(stdin);
		*num_of_args=0;
		return;
	}
	while (c==' ' || c=='\t') {
		c=getc(stdin); (*length)++;
		if (c=='\n' || c=='\r' || c==EOF){
			if (c==EOF) *exit_prog=1;
			else if (c=='\r') c=getc(stdin);
			*num_of_args=0;
			return;
		}
	}
	the_information[1]=(char*)malloc(sizeof(char));
	check_for_allocation_failure(the_information[1]);
	while (c!='\n' && c!='\r' && c!=EOF){
		the_information[1][i]=c;
		i++;
		p=realloc(the_information[1], (i+1)*sizeof(*p));
		check_for_allocation_failure(p);
		the_information[1]=p;
		c=getc(stdin); (*length)++;
		if (c==' ' || c=='\t') break;
	}
	the_information[1][i]='\0';
	if (c=='\r') c=getc(stdin);
	while (c!='\n' && c!='\r' && c!=EOF){
		c=getc(stdin); (*length)++; /* finish the whole line */
	}
	if (c==EOF) *exit_prog=1;
	else if (c=='\r') c=getc(stdin);
	*num_of_args=1;
}

/**
 * retrieve_args_for_1_args_exclude_edit - retrieve the arguments given by the user following all the commands that have 1 argument (without edit)
 * @param **the_information - array of strings which contains the function name and users arguments
 * @param c - the character that is being read
 * @param *exit_prog - an indicator that is 1 when we should exit the program (when encountering EOF or after exit command from user)
 * @param *num_of_args - number of arguments given by the user
 * @param *length - the length of the users command
 * @return void
 */
void retrieve_args_for_1_args_exclude_edit(char** the_information, char c, int* exit_prog, int* num_of_args, int* length){
	int i=0; char *p;
	while (c==' ' || c=='\t'){
		c=getc(stdin); (*length)++;	
	}
	if (c==EOF || c=='\n' || c=='\r'){ /* means no argument given */
		if (c==EOF) *exit_prog=1;
		if (c=='\r') c=getc(stdin);
		*num_of_args=0;
		return;
	}
	the_information[1]=(char*)malloc(sizeof(char));
	check_for_allocation_failure(the_information[1]);
	while (c!='\n' && c!='\r' && c!=EOF){
		the_information[1][i]=c;
		i++;
		p=(char*)realloc(the_information[1],(i+1)*sizeof(*p));
		check_for_allocation_failure(p);
		the_information[1]=p;
		c=getc(stdin); (*length)++;
		if (c==' ' || c=='\t') break;
	}
	the_information[1][i]='\0';
	if (c=='\r') c=getc(stdin);
	while (c!='\n' && c!='\r' && c!=EOF){ 
		c=getc(stdin); (*length)++;/* finish the whole line */
	}
	if (c=='\r') c=getc(stdin);
	if (c==EOF) *exit_prog=1;
	*num_of_args=1;
}

/**
 * retrieve_args_for_generate_hint -retrieve the arguments given by the user following hint command
 * @param **the_information - array of strings which contains the function name and users arguments
 * @param c - the character that is being read
 * @param *exit_prog - an indicator that is 1 when we should exit the program (when encountering EOF or after exit command from user)
 * @param *num_of_args - number of arguments given by the user
 * @param *length - the length of the users command
 * @return void
 */
void retrieve_args_for_generate_hint(char** the_information,char c,int* exit_prog,int* num_of_args, int* length){
	int i=1, j=0 ; char *p;
	if (c=='\n' || c=='\r' || c==EOF){
		if (c==EOF) *exit_prog=1;
		if (c=='\r') c=getc(stdin);
		*num_of_args=0;
		return;
	}
	c=getc(stdin); (*length)++;;
	while (i!=3){
		while (c==' ' || c=='\t'){
			c=getc(stdin);(*length)++;;
		}
		the_information[i]=(char*)malloc(sizeof(char));
		check_for_allocation_failure(the_information[i]);
		while (c!=' ' && c!='\t' && c!= '\n' && c!='\r' && c!=EOF){
			the_information[i][j]=c;
			p=(char*)realloc(the_information[i],(j+2)*sizeof(*p));
			check_for_allocation_failure(p);
			the_information[i]=p;
			j++;
			c=getc(stdin);(*length)++;
		}
		if (c=='\r') c=getc(stdin);
		the_information[i][j]='\0';
		while (c==' ' || c=='\t') {
			c=getc(stdin);(*length)++;
		}
		if ((c==EOF || c=='\n' || c=='\r') && i==1){ /* means the user didnt provide the right amount of args */
			if (c==EOF) *exit_prog=1;
			if (c=='\r') c=getc(stdin);
			*num_of_args=1;
			return;
		}
		i++; j=0;
	}
	while (c!='\n' && c!='\r' && c!=EOF){
		c=getc(stdin); (*length)++;
	}
	if (c=='\r') c=getc(stdin);
	if (c==EOF) *exit_prog=1;
	*num_of_args=2;
}

/**
 * retrieve_args_for_set - retrieve the arguments given by the user following set command
 * @param **the_information - array of strings which contains the function name and users arguments
 * @param c - the character that is being read
 * @param *exit_prog - an indicator that is 1 when we should exit the program (when encountering EOF or after exit command from user)
 * @param *num_of_args - number of arguments given by the user
 * @param *length - the length of the users command
 * @return void
 */
void retrieve_args_for_set(char** the_information,char c,int* exit_prog,int* num_of_args, int* length){
	int i=1, j=0; char *p;
	if (c=='\n' || c=='\r' || c==EOF){
		if (c==EOF) *exit_prog=1;
		if (c=='\r') c=getc(stdin);
		*num_of_args=0;
		return;
	}
	c=getc(stdin);(*length)++;
	while (i!=4){
		while (c==' ' || c=='\t'){
			c=getc(stdin);(*length)++;
		}
		the_information[i]=(char*)malloc(sizeof(char));
		check_for_allocation_failure(the_information[i]);
		while (c!=' ' && c!='\t' && c!= '\n' && c!='\r' && c!=EOF){
			the_information[i][j]=c;
			/*p=(char*)realloc(the_information[i], sizeof(the_information[i])+sizeof(char));*/
			p=(char*)realloc(the_information[i],(j+2)*sizeof(*p));
			check_for_allocation_failure(p);
			the_information[i]=p;
			j++;
			c=getc(stdin);(*length)++;
		}
		if (c=='\r') c=getc(stdin);
		the_information[i][j]='\0';
		while (c==' ' || c=='\t'){
			c=getc(stdin);(*length)++;
		}
		if ((c==EOF || c=='\n' || c=='\r') && (i==1 || i==2)){ /* means the user didn't provide the right amount of args */
			if (c==EOF) *exit_prog=1;
			if (c=='\r') c=getc(stdin);
			*num_of_args=i;
			return;
		}
		i++; j=0;
	}
	while (c!= '\n' && c!='\r' && c!=EOF){
		c=getc(stdin);(*length)++;
	}
	if (c=='\r') c=getc(stdin);
	if (c==EOF) *exit_prog=1;
	*num_of_args=3;
}

/**
 * is_valid_arguments_for_mark_errors - checks if the arguments are valid for mark_errors
 * @param **the_information - array of strings which contains the function name and users arguments
 * @param *num_of_args - number of arguments given by the user
 * @return int - 1 if is valid, 0 else
 */
int is_valid_arguments_for_mark_errors(char **the_information,int  *num_of_args){
	if (*num_of_args==0) {
		printf("ERROR: invalid command\n");
		return 0;
	}
	if (strcmp(the_information[1],"0")!=0 && strcmp(the_information[1],"1")!=0){
		printf("Error: the value should be 0 or 1\n");
		return 0;
	}
	return 1;
}

/**
 * is_valid_arguments_for_set - checks if the arguments are valid for set
 * @param *the_game - a pointer to the game we are playing
 * @param **the_information - array of strings which contains the function name and users arguments
 * @param *num_of_args - number of arguments given by the user
 * @return int - 1 if is valid, 0 else
 */
int is_valid_arguments_for_set(Game* the_game,char** the_information, int* num_of_args){
	int i=0, allowed_to_be_zero=0,is_valid;
	if (*num_of_args!=3) {
		printf("ERROR: invalid command\n");
		return 0;
	}
	for (i=1 ; i < 4 ; i++){
		if (i==3) allowed_to_be_zero=1;
		is_valid=is_valid_integer_for_set(the_game, the_information[i],allowed_to_be_zero);
		if (is_valid==0){
			printf("Error: value not in range 0-%d\n",the_game->board->length_of_board);
			return 0;
		}
	}
	if (the_game->board->cells[atoi(the_information[1])-1][atoi(the_information[2])-1]->fixed==1){
		printf("Error: cell is fixed\n");
		return 0;
	}
	return 1;
}

/**
 * is_valid_arguments_for_generate - checks if the arguments are valid for generate
 * @param *the_game - a pointer to the game we are playing
 * @param **the_information - array of strings which contains the function name and users arguments
 * @param *num_of_args - number of arguments given by the user
 * @return int - 1 if is valid, 0 else
 */
int is_valid_arguments_for_generate(Game* the_game,char** the_information,int* num_of_args){
	int is_valid;
	if (*num_of_args!=2) {
		printf("ERROR: invalid command\n");
		return 0;
	}
	is_valid=is_valid_integer_for_generate(the_game, the_information);
	if (is_valid==0){
		return 0;
	}
	return 1;
}

/**
 * is_valid_arguments_for_hint - checks if the arguments are valid for hint
 * @param *the_game - a pointer to the game we are playing
 * @param **the_information - array of strings which contains the function name and users arguments
 * @param *num_of_args - number of arguments given by the user
 * @return int - 1 if is valid, 0 else
 */
int is_valid_arguments_for_hint(Game* the_game,char** the_information,int* num_of_args){
	int is_valid;
	if (*num_of_args!=2) {
		printf("ERROR: invalid command\n");
		return 0;
	}
	is_valid=is_valid_integer_for_hint(the_game, the_information);
	if (is_valid==0){
		return 0;
	}
	return 1;
}

/**
 * is_valid_arguments_for_solve_save - checks if the arguments are valid for solve or save
 * @param *num_of_args - number of arguments given by the user
 * @return int - 1 if is valid, 0 else
 */
int is_valid_arguments_for_solve_save(int *num_of_args){
	if (*num_of_args!=1) {
		printf("ERROR: invalid command\n");
		return 0;
	}
	else{
		return 1;
	}
}
