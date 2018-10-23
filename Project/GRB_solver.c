
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gurobi_c.h"
#include "The_Board.h"
#include "Utilities_for_Commands.h"
#include "Utilities_for_generate_and_num_solutions.h"
#include "Utilities_for_GRB_solver.h"
#include "GRB_solver.h"

static int general_solver(Game* game,char caller[9], int x, int y);

/**
* general_solver - attempts to solve a given board
* @param game - a pointer to the game we wish to solve
* @param caller - an array containing the name of the function who called the solver
* @param x - only in use when the function is called from "hint" - x marks the x-axis location of the only cell to be filled
* @param y - only in use when the function is called from "hint" - y marks the y-axis location of the only cell to be filled
* @return 1 if board was solved, 0 if not solvable
*/
static int general_solver(Game* game,char caller[9], int x, int y){
	GRBenv *env = NULL; GRBmodel *model = NULL;int optimstatus,error=0;
	int N=game->board->length_of_board; int* ind=(int*)malloc(sizeof(int)*N); double objval;
	double* val=(double*)malloc(sizeof(double)*N); double* curr_board=(double*)malloc(sizeof(double)*N*N*N);
	char* vtype=(char*)malloc(sizeof(char)*N*N*N); double* sol=(double*)malloc(sizeof(double)*N*N*N);
	check_for_GRB_allocation_failure(ind,val,curr_board,vtype,sol);
	create_model(game,curr_board,vtype,N); /* Create an empty model */
	error = GRBloadenv(&env, NULL); if (error) goto Exit; /* Create environment */
	error = GRBsetintparam(env, "OutputFlag",0); if (error) goto Exit;
	error = GRBnewmodel(env, &model, "sudoku", N*N*N, NULL, curr_board, NULL, vtype, NULL); if (error) goto Exit; /* Create new model */
	/*add constraints*/
	error=add_constr_single_value_each_cell(model,N,ind,val);if (error) goto Exit;
	error=add_constr_for_row(model,N,ind,val); if (error) goto Exit;
	error=add_constr_for_column(model,N,ind,val);if (error) goto Exit;
	error=add_constr_for_block(game,model,N,ind,val);if (error) goto Exit;
	/* Capture solution information */
	error = GRBoptimize(model); if (error) goto Exit; /* Optimize model */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus); if (error) goto Exit; 
	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval); if (error) goto Exit;
   	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N*N*N, sol); if (error) goto Exit;
	adapt_to_caller(game, caller,x,y,sol);
Exit:
    free_grb(sol,ind,val,curr_board,vtype,model,env);
    if (error) return 0;
    else{
    	return 1;
    }
}

/**
* solve_for_hint - tryies to solve the game, addapting the solving process to the needs of "hint"
* @param game - a pointer to the game we are trying to solve
* @param x - the x-axis location of the cell a hint was called for
* @param y - the y-axis location of the cell a hint was called for
* @return 1 if board was solved, 0 if not solvable
*/
int solve_for_hint(Game* game,int x, int y){
	int decide;
	char hint[9] = "hint";
	decide = general_solver(game, hint, x, y);
	return decide;
}

/**
* solve_for_generate - tryies to solve the game, addapting the solving process to the needs of "generate"
* @param game - a pointer to the game we are trying to solve
* @param x - amount of random cells we will fill up to begin with
* @param y - amount of random cells we will leave non empty at the end of the process
* @param amount_of_attempts - a pointer to an integer that tells us how many times we already tried and failed to generate the board.
* @return void
*/
void solve_for_generate(Game* game,int x, int y, int* amount_of_attempts){
	int decide;
	char generate[9] = "generate";
	decide = general_solver(game, generate, 0, 0);
	if (decide == 0){
		/*getting the board back to being empty:*/
		clear_board(game->board);
		(*amount_of_attempts)++;
		if(*amount_of_attempts <= 999){
			generate_recursive(game, x, y, amount_of_attempts); /*if we get here then ILP did not work but we are still under 1000 tries*/
		}
	}
}

/**
* solve_for_validate - tryies to solve the game, addapting the solving process to the needs of "validate"
* @param game - a pointer to the game we are trying to solve
* @return 1 if board was solved, 0 if not solvable
*/
int solve_for_validate(Game* game){
	int decide;
	char validate[9] = "validate";
	decide = general_solver(game, validate, 0, 0);
	return decide;
}
