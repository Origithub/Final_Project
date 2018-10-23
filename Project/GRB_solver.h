/*
 * GRB_solver.h	
 *	 
 * GRB_solver Summary:
 * This module uses the gurobi ILP solver to solve the board
 *
 * GRB_solver supports the following functions:
 * solve_for_hint - tries to solve the game, adapting the solving process to the needs of "hint"
 * solve_for_validate - tries to solve the game, adapting the solving process to the needs of "validate"
 * solve_for_generate - tries to solve the game, adapting the solving process to the needs of "generate"
 *** Functions that are static in the .c file:
 * general_solver -  the function that actually runs gurobi ILP solver
*/

#include "The_Board.h"
#ifndef GRB_SOLVER_H_
#define GRB_SOLVER_H_

int solve_for_hint(Game* game,int x, int y);
int solve_for_validate(Game* game);
void solve_for_generate(Game* game,int x, int y, int* amount_of_attempts);

#endif /* GRB_SOLVER_H_ */
