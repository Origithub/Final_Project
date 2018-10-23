CC=gcc
OBJS=main.o Parser.o The_Board.o Utilities_for_Commands.o Commands.o \
 Utilities_for_Parser.o Utilities_for_generate_and_num_solutions.o GRB_solver.o Utilities_for_GRB_solver.o
EXEC = sudoku-console
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56
COMP_FLAG=-ansi -Wall -Wextra \
-Werror -pedantic-errors

all: $(EXEC)


$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c Parser.h The_Board.h SPBufferset.h Commands.h 
	$(CC) $(COMP_FLAG) -c $*.c
Parser.o: Parser.c Parser.h
	$(CC) $(COMP_FLAG) -c $*.c
The_Board.o: The_Board.c The_Board.h
	$(CC) $(COMP_FLAG) -c $*.c
Commands.o: Commands.c Commands.h Utilities_for_Commands.h 
	$(CC) $(GUROBI_COMP) $(COMP_FLAG) -c $*.c
Utilities_for_Commands.o: Utilities_for_Commands.c Utilities_for_Commands.h
	$(CC) $(COMP_FLAG) -c $*.c
Utilities_for_generate_and_num_solutions.o: Utilities_for_generate_and_num_solutions.c Utilities_for_generate_and_num_solutions.h
	$(CC) $(COMP_FLAG) -c $*.c
Utilities_for_Parser.o: Utilities_for_Parser.c Utilities_for_Parser.h
	$(CC) $(COMP_FLAG) -c $*.c
GRB_solver.o: GRB_solver.c GRB_solver.h
	$(CC) $(GUROBI_COMP) $(COMP_FLAG) -c $*.c
Utilities_for_GRB_solver.o: Utilities_for_GRB_solver.c Utilities_for_GRB_solver.h
	$(CC) $(GUROBI_COMP) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC) 
