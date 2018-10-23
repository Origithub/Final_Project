#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utilities_for_GRB_solver.h"

/**
* check_for_GRB_allocation_failure - checks for allocation failure 
* @param ind - array of relevant indexes
* @param val - array that if a value is 1 it indicates the coefficient in the ILP
* @param curr_board - array that represent the current board
* @param vtype - array that represent the type of variable
* @param sol - array that stores the solution to the board
* @return void
*/
void check_for_GRB_allocation_failure(int* ind,double* val, double* curr_board,char* vtype, double* sol){
	if (ind==NULL || val==NULL || curr_board==NULL || vtype==NULL || sol==NULL){
		printf("malloc failure\n");
		exit(0);
	}
}

/**
* create_model - creates a new model 
* @param game - a pointer to the played game
* @param curr_board - array that represent the current board
* @param vtype - array that represent the type of variable
* @param N - the boards length
* @return void
*/
void create_model(Game* game,double* curr_board, char* vtype,int N){
	int i,j,value;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
		  for (value = 0; value < N; value++) {
			  if (game->board->cells[i][j]->val-1 == value)
				  curr_board[i*N*N+j*N+value] = 1;
			  else{
				  curr_board[i*N*N+j*N+value] = 0;
			  }
				  vtype[i*N*N+j*N+value] = GRB_BINARY;
			  }
		 }
	}
}

/**
* add_constr_single_value_each_cell - adds constraints regarding a single value in each_cell 
* @param model - the ILPs model
* @param N - the boards length
* @param ind - array of relevant indexes
* @param val - array that if a value is 1 it indicates the coefficient in the ILP
* @return 1 if an error occurred, else 0
*/
int add_constr_single_value_each_cell(GRBmodel *model,int N,int* ind,double* val){
	int i,j,value,error;
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++) {
			for (value = 0; value < N; value++) {
				ind[value] = i*N*N+j*N+value;
				val[value] = 1.0;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) return 1;
		}
	}
	return 0;
}

/**
* add_constr_for_row - adds constraints for each row 
* @param model - the ILPs model
* @param N - the boards length
* @param ind - array of relevant indexes
* @param val - array that if a value is 1 it indicates the coefficient in the ILP
* @return 1 if an error occurred, else 0
*/
int add_constr_for_row(GRBmodel *model,int N,int* ind,double* val){
	int i,j,value,error;
	for (value = 0; value < N; value++) {
		for (j = 0; j < N; j++) {
			for (i = 0; i < N; i++) {
				ind[i] = i*N*N+j*N+value;
				val[i] = 1.0;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) return 1;
		}
	}
	return 0;
}

/**
* add_constr_for_column - adds constraints for each column 
* @param model - the ILPs model
* @param N - the boards length
* @param ind - array of relevant indexes
* @param val - array that if a value is 1 it indicates the coefficient in the ILP
* @return 1 if an error occurred, else 0
*/
int add_constr_for_column(GRBmodel *model,int N,int* ind,double* val){
	int i,j,value,error;
	for (value = 0; value < N; value++) {
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				ind[j] = i*N*N+j*N+value;
				val[j] = 1.0;
			}
			error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) return 1;
		}
	}
	return 0;	
}

/**
* add_constr_for_block - adds constraints for each block
* @param game - a pointer to the played game
* @param model - the ILPs model
* @param N - the boards length
* @param ind - array of relevant indexes
* @param val - array that if a value is 1 it indicates the coefficient in the ILP
* @return 1 if an error occurred, else 0
*/
int add_constr_for_block(Game* game,GRBmodel *model,int N,int* ind,double* val){
	int i,j,value, block_i,block_j, count,error;
		for (value = 0; value < N; value++) {
			for (block_i = 0; block_i < game->board->block_x; block_i++) {
				for (block_j = 0; block_j < game->board->block_y; block_j++) {
					count = 0;
					for (i = block_i*game->board->block_y; i < (block_i+1)*game->board->block_y; i++) {
						for (j = block_j*game->board->block_x; j < (block_j+1)*game->board->block_x; j++) {
							ind[count] = i*N*N+j*N+value;
							val[count] = 1.0;
							count++;
						}
					}
					error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
					if (error) return 1;;
				}
			}
		}
		return 0;
}

/**
* adapt_to_caller - sets values or prints according to the identity of its caller
* @param game - a pointer to the played game
* @param caller - a string representing the name of the function which made the initial call 
* @param x - only in use when the function is called from "hint" - x marks the x-axis location of the only cell to be filled
* @param y - only in use when the function is called from "hint" - y marks the y-axis location of the only cell to be filled
* @param sol - a pointer to an array containing the solution that was found for the board
* @return void
*/
void adapt_to_caller(Game* game,char caller[9], int x, int y,double* sol){
	int i,j, value;
	int N=game->board->length_of_board;
	if (strcmp(caller,"generate")==0){
	   for (i = 0; i < N; i++) {
		   for (j = 0; j < N; j++) {
			   for (value = 0; value < N; value++) {
				   if (sol[i*N*N+j*N+value]==1){
					   game->board->cells[i][j]->val=value+1;
					   game->board->cells[i][j]->was_loaded = 1;
				   }
	       	   }
	       }
	   }
   	}
  	else if (strcmp(caller,"hint")==0){
   	   for (value = 0; value < N; value++) {
		   if (sol[x*N*N+y*N+value]==1) printf("Hint: set cell to %d\n",value+1 );  
   	   }
  	}
}

/**
* free_grb - frees all memory that regards the GRB solver 
* @param sol - array that stores the solution to the board
* @param ind - array of relevant indexes
* @param val - array that if a value is 1 it indicates the coefficient in the ILP
* @param curr_board - array that represent the current board
* @param vtype - vtype - array that represent the type of variable
* @param model - the ILPs model
* @param env - the ILPs environment
* @return void
*/
void free_grb(double* sol,int* ind,double* val,double* curr_board,char* vtype,GRBmodel* model,GRBenv* env){
	free(sol);
	free(ind);
	free(val);
	free(curr_board);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
}
