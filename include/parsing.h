#ifndef PARSING_H
#define PARSING_H

/*---------------------------------------//
| THIS FILE CONTAINS FUNCTION FOR PARSING |
//---------------------------------------*/

//---LIBRARIES-AND-STRUCTURES---//

#include "config.h"

//---FUNCTIONS---//

ColumnsKeeper count_columns(FILE *csv_file);
RowsKeeper    count_rows(FILE *csv_file);
int           parse_csv(Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows, FILE *file);

#endif
