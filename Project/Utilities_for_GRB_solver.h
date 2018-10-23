/*
* Utilities_for_GRB_solver.h
*
* Utilities_for_GRB_solver summary:
* In this module we include all of the functions that our general GRB solver uses.
*
* Utilities_for_GRB_solver supports the following functions:
* check_for_GRB_allocation_failure - checks for allocation failures
* create_model - creates a new model for the GRB solver
* add_constr_single_value_each_cell - adds a constraint regarding a single value for each cell for our ILP solution
* add_constr_for_row - adds a constraint regarding a each row for our ILP solution
* add_constr_for_column - adds a constraint regarding a each column for our ILP solution
* add_constr_for_block - adds a constraint regarding a each block for our ILP solution
* free_grb - frees all memory allocated in the solving process  
* adapt_to_caller - checks which function initially called for the ILP solver, and proceeds accordingly
*/

#include <stdio.h>
#include "The_Board.h"
#include "gurobi_c.h"

#ifndef UTILITIES_FOR_GRB_SOLVER_
#define UTILITIES_FOR_GRB_SOLVER_

void check_for_GRB_allocation_failure(int* ind,double* val, double* curr_board,char* vtype, double* sol);
void create_model(Game* game,double* curr_board, char* vtype, int N);
int add_constr_single_value_each_cell(GRBmodel *model,int N,int* ind,double* val);
int add_constr_for_row(GRBmodel *model,int N,int* ind,double* val);
int add_constr_for_column(GRBmodel *model,int N,int* ind,double* val);
int add_constr_for_block(Game* game,GRBmodel *model,int N,int* ind,double* val);
void free_grb(double* sol,int* ind,double* val,double* curr_board,char* vtype,GRBmodel* model,GRBenv* env);
void adapt_to_caller(Game* game,char caller[9], int x, int y, double* sol);

#endif /* UTILITIES_FOR_GRB_SOLVER_ */
