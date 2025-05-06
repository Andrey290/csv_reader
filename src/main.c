//---COMMENTARY---//
/*

   =^0w0^=
   1) [Ok] This is the version where files are separated and additional CMakeLists.txt
   2) !No sorting. 
   3) !Searching is brutforce.
   4) !Code may crush on errors.
   5) !Crushes are not heap-safety.
   6) Some decisions are bad
   7) [Ok] Output is nice =}

*/

//---STANDART-LIBRARIES---///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

//---MY-HEDERS---//

#include "../include/config.h"
#include "../include/parsing.h"
#include "../include/computations.h"

int main(int argc, char *argv[]) {

	// OPENING //
	
	if (argc != 2) {
		printf("[FAULT] main : You probably forgot to designate your .csv file or designatet more than one.\n");
		return 1;
	}
	
	const char* csvname = argv[1];
	FILE *csv_file = fopen(csvname, "r");
	if (!csv_file) {
		printf("[FAULT] main : Unable to open %s.\n", argv[1]);
		return 2;
	}

	// GET COLS //

	ColumnsKeeper columns = count_columns(csv_file);	
	if (columns.names == NULL) {
		printf("[FAULT] main : Columns are NULL. Deallocating memory.\n");
		for (int i = 0; i < columns.count; i++) {
			free(columns.names[i]);
	        }
		free(columns.names);
        
		fclose(csv_file);
		return 3;
	}
	//printf("[INFO] main : Finally %d cols\n", columns.count);

	// GET ROWS //

	RowsKeeper rows = count_rows(csv_file);
	    // !!! Add zero rows check
	//printf("[INFO] main : Finally %d rows\n", rows.count);

	// PARSE DATA //
	
	    // Making Cell massive
	Cell **grid = (Cell**)malloc(rows.count * sizeof(Cell*));
	for(int i = 0; i < rows.count; i++) {
		grid[i] = (Cell*)malloc(columns.count * sizeof(Cell));
	}
       
	    // Filling Cell massive with data
	if(parse_csv(grid, &columns, &rows, csv_file)) {
		printf("[FAULT] main : Unsuccessfull parsing.\n");
		return 4;
	} else {
		//printf("[INFO] main : Successfull parsing!\n");
	}
	    /*------------------------------------------//
	    | Here should be FAST SORTING of ROW and COL |
	    //------------------------------------------*/

	    // Print grid (Printing is also calculating formulas, so programm can stack right here if there are many layers)
	if(print_grid(grid, &columns, &rows)) {
        	printf("[FAULT] main : Printing error.\n");
		return 5;
	} else {
		//printf("[INFO] main : Printing succeded! So all John Goodenough in this code!.\n");
	}

	// ENDING //

	fclose(csv_file);

	for (int i = 0; i < rows.count; i++) {
		for (int j = 0; j < columns.count; j++) {
			free(grid[i][j].formula);
		}
		free(grid[i]);
	}
	free(grid);

	for (int i = 0; i < columns.count; i++) {
		free(columns.names[i]);
	}
	free(columns.names);
        
	free(rows.ids);

	return 0;
}
