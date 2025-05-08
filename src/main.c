//---COMMENTARY---//
/*

   =^0w0^=
   1) [Ok] This is the version where files are separated and additional CMakeLists.txt
   2) !No sorting. 
   3) !Searching is brutforce.
   4) [Ok]Code may crush on errors.
   5) [Ok]Crushes are not heap-safety.
   6) Some decisions are bad
   7) [Ok] Output is nice =}
   8) [Ok] Added RAII (Resource Acquisition Is Initialization) :w

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

//---MY-FUNCTIONS---//

static void free_columns(ColumnsKeeper *cols);
static void free_grid(Cell ***grid_ptr, int row_count, int col_count);
static void free_rows(RowsKeeper *rows);


int main(int argc, char *argv[]) {
	
	// INITIALISATION //
	ColumnsKeeper columns = { .names = NULL, .count = 0};
	RowsKeeper rows       = { .ids = NULL, .sorted_ids_pointers = NULL, .count = 0};
	Cell **grid           = NULL;
        FILE *csv_file        = NULL;
        int exit_code         = 0;

	// MAIN ACTIONS //
	// 1) Arguments verification
	if (argc != 2) {
		printf("[FAULT] main : You probably forgot to designate your .csv file or designatet more than one.\n");
		exit_code = 1;
		goto cleanup;
	}

	// 2) File opening
	csv_file = fopen(argv[1], "r");
	if (!csv_file) {
		printf("[FAULT] main : Unable to open %s.\n", argv[1]);
		exit_code = 2;
		goto cleanup;
	}

	// 3) Reading columns headers
	columns = count_columns(csv_file);	
	if (!columns.names || columns.count <= 0) {
		printf("[FAULT] main : No columns detected.\n");
		exit_code = 3;
		goto cleanup;
	}

	// 4) Reading rows ids
	rows = count_rows(csv_file);
	if (!rows.ids || rows.count <= 0) {
		printf("[FAULT] main : No rows detected.\n");
		exit_code = 4;
		goto cleanup;
	}

	// 5) Grid allocation
	
	grid = malloc(rows.count * sizeof(Cell*));
	if (!grid) {
		printf("[FAULT] main : Memory allocation failed for grid rows.\n");
		exit_code = 5;
		goto cleanup;

	}
	for(int i = 0; i < rows.count; i++) {
		grid[i] = malloc(columns.count * sizeof(Cell));
		if (!grid[i]) {
			printf("[FAULT] main : Memory allocation failed for grid[%d].\n", i);
			exit_code = 5;
			goto cleanup;
		}
	}
       
	// 6) Parsing file into grid
	if(parse_csv(grid, &columns, &rows, csv_file)) {
		printf("[FAULT] main : Unsuccessfull parsing.\n");
		exit_code = 6;
		goto cleanup;
	}

        /*------------------------------------------//
        | Here should be FAST SORTING of ROW and COL |
        //------------------------------------------*/

	// 7) Compute and print grid
	if(print_grid(grid, &columns, &rows)) {
        	printf("[FAULT] main : Printing error.\n");
		exit_code = 7;
		goto cleanup;
	}

	// ENDING //
	cleanup:
		if (csv_file) fclose(csv_file);
		free_grid(&grid, rows.count, columns.count);
		free_columns(&columns);
		free_rows(&rows);
		return exit_code;
}

//

static void free_columns(ColumnsKeeper *cols) {
    if (!cols->names) {
	    return;
    }
    for (int i = 0; i < cols->count; ++i) {
        free(cols->names[i]);
    }
    free(cols->names);
    cols->names = NULL;
    cols->count = 0;
}

static void free_rows(RowsKeeper *rows) {
    if (rows->ids) {
	    free(rows->ids);
    }
    if (rows->sorted_ids_pointers) {
	    free(rows->sorted_ids_pointers);
    }
    rows->ids = NULL;
    rows->sorted_ids_pointers = NULL;
    rows->count = 0;
}

static void free_grid(Cell ***grid_ptr, int row_count, int col_count) {
    if (!*grid_ptr) {
	    return;
    }
    Cell **grid = *grid_ptr;
    for (int i = 0; i < row_count; ++i) {
        for (int j = 0; j < col_count; ++j) {
            free(grid[i][j].formula);
        }
        free(grid[i]);
    }
    free(grid);
    *grid_ptr = NULL;
}
