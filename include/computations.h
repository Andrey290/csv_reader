#ifndef COMPUTATIONS_H
#define COMPUTATIONS_H

/*------------------------------------------------//
| THIS FILE CONTAINS FUNCTIONS FOR COMPUTE VALUES |
//------------------------------------------------*/

//---LIBRARIES-AND-STRUCTURES---//

#include "config.h"

//---FUNCTIONS---//

int           print_grid(Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows);
int           calculate(Cell *processed_cell, Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows);
int           geting_row_and_col_separately(int *just_a_number_flag, char *argument_letters, char *argument_numbers, char *argument);
Cell*         find_cell(int *find_error, char* argument_letters, char* argument_numbers, ColumnsKeeper *cols, RowsKeeper *rows, Cell **grid);

#endif
