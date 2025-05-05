//---COMMENTARY---//
/*


*/

//---STANDART-LIBRARIES---///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//---CONSTANTS---//

const int MAX_LINE_SIZE = 1024;

//---STRUCTURES---//

typedef struct {
	char *formula;
	double value;
	int is_resolved;
} Cell;

typedef struct {
	char **names;
	int count;
} ColumnsKeeper;

typedef struct {
	int *ids;
	int **sorted_ids_pointers;
	int count;
} RowsKeeper;

//---FUNCTIONS---//

ColumnsKeeper count_columns(FILE *csv_file);
RowsKeeper    count_rows(FILE *csv_file);
int           parse_csv(Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows, FILE *file);
int           print_grid(Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows);



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
	printf("[INFO] main : Finally %d cols\n", columns.count);

	// GET ROWS //

	RowsKeeper rows = count_rows(csv_file);
	    // !!! Add zero rows check
	printf("[INFO] main : Finally %d rows\n", rows.count);

	// PARSE DATA //
	
	    // Making Cell massive
	Cell **grid = (Cell**)malloc(rows.count * sizeof(Cell));
	for(int i = 0; i < rows.count; i++) {
		grid[i] = (Cell*)malloc(columns.count * sizeof(Cell));
	}
       
	    // Filling Cell massive with data
	if(parse_csv(grid, &columns, &rows, csv_file)) {
		printf("[FAULT] main : Unsuccessfull parsing.\n");
		return 4;
	} else {
		printf("[INFO] main : Successfull parsing!\n");
	}
	    // Print grid (Printing is also calculating formulas, so programm can stack right here)
	if(print_grid(grid, &columns, &rows)) {
        	printf("[FAULT] main : Printing error.\n");
		return 5;
	} else {
		printf("[INFO] main : Printing succeded! So all John Goodenough in this code!.\n");
	}

	// DO NOT NEED CSV_FILE ANYMORE //

	fclose(csv_file);
	
	// ENDING //

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

ColumnsKeeper count_columns(FILE *csv_file) {
        // I should keep in mind that first col is filled by indexes
	ColumnsKeeper columns = {0};
	char header_line[MAX_LINE_SIZE];
	if (!fgets(header_line, sizeof(header_line), csv_file)) {
		printf("[FAULT] count columns : Header line is empty.\n");
		return columns;
	}

	// Counting
	int cols = 0;
	int name_index = 0;
	int pass_row_names_col = 0;
	for (int i = 0; ; i++) {
		if (!pass_row_names_col && (header_line[i] == ',')) {
			pass_row_names_col++;
		} else if (header_line[i] == ',' || header_line[i] == '\n' || header_line[i] == '\0') {
		       	cols++;
		}
		if (header_line[i] == '\n' || header_line[i] == '\0') break;
	}
	columns.count = cols;
	printf("[INFO] count_columns : Finally %d cols\n", columns.count);
	
	// Filling structure
	columns.names = (char**)malloc(columns.count * sizeof(char*));
	
	int current_col = 0;
	int name_start = 0;
	pass_row_names_col = 0;
	for (int i = 0; ; i++) {
		if (!pass_row_names_col && (header_line[i] == ',')) {
			pass_row_names_col++;
			name_start = i + 1;
		} else if (header_line[i] == ',' || header_line[i] == '\n' || header_line[i] == '\0') {
			int name_len = i - name_start;
			columns.names[current_col] = (char*)malloc(name_len + 1); // +1 is for terminator (for not holding all names lengths)
			strncpy(columns.names[current_col],
				&header_line[name_start],
				name_len);
			columns.names[current_col][name_len] = '\0';

			name_start = i + 1;
			current_col++;
		}
		if (header_line[i] == '\n' || header_line[i] == '\0') break;
	}
	
	for (int i = 0; i < columns.count; i++) {
		printf("[INFO] count_columns : Columns number %d name is %s\n", i, columns.names[i]);
	}
	return columns;
}

RowsKeeper count_rows(FILE *csv_file) {	
	RowsKeeper rows = {0};
	char line[MAX_LINE_SIZE];

	rewind(csv_file);
	fgets(line, sizeof(line), csv_file);
	// Counting
	int c_rows = 0;
	while (fgets(line, sizeof(line), csv_file)) {
		c_rows++;	
	}
        rewind(csv_file); // return pointer
        printf("[INFO] count_rows : There are %d rows in file (exluded header line). \n", c_rows);
	rows.count = c_rows;

	// Filling
	
	rows.ids = (int*)malloc(rows.count * sizeof(int));
	rows.sorted_ids_pointers = (int**)malloc(rows.count * sizeof(int*));

	c_rows = 0;
	while (fgets(line, sizeof(line), csv_file)) {
		if (c_rows >= 1) {
			char* id_ascii = (char*)malloc(32);
			memset(id_ascii, 0, 32);

			for (int i = 0; ; i++) {
				if (line[i] == ',' || line[i] == '\n' || line[i] == '\0') break;
				id_ascii[i] = line[i];
			}
                	int id = atoi(id_ascii);
			free(id_ascii);
			rows.ids[c_rows - 1] = id; 
			rows.sorted_ids_pointers[c_rows - 1] = &rows.ids[c_rows - 1];

			printf("[INFO] count_rows : Row number %d has id %d. \n", c_rows, rows.ids[c_rows - 1]);
			c_rows++;	
		}
	}
	rewind(csv_file); // return pointer
	return rows;
}

int parse_csv(Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows, FILE *file) {
	char line[MAX_LINE_SIZE];
	fgets(line, sizeof(line), file);

	for (int row = 0; row < rows->count; row++) {
		if (!fgets(line, sizeof(line), file)) break;
		
		int current_col = 0;
		int name_start = 0;
		int pass_row_names_col = 0;
		for (int i = 0; ; i++) {
			if (current_col >= cols->count) break;
			if (!pass_row_names_col && (line[i] == ',' || line[i] == '\n' || line[i] == '\0')) {
				pass_row_names_col++;
				name_start = i + 1;
			} else if (line[i] == ',' || line[i] == '\n' || line[i] == '\0') {
				int name_len = i - name_start;
				grid[row][current_col].formula = (char*)malloc(name_len + 1); // +1 is for terminator (for not holding all names lengths)
				strncpy(grid[row][current_col].formula,
					&line[name_start],
					name_len);
				grid[row][current_col].formula[name_len] = '\0';
				printf("[INFO] parse_csv : Cell[%d][%d] formula is : %s\n", row, current_col, grid[row][current_col].formula);	
				name_start = i + 1;
				current_col++;
			}
		}
	}
	return 0;
}

int print_grid(Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows) {
	printf("[INFO] print_grid : Grid is:\n");
	for(int i = 0; i < rows->count; i++) {
		for (int j = 0; j < cols->count; j++) {
			if (grid[i][j].formula[0] != '=') {
				grid[i][j].value = (double)atoi(grid[i][j].formula);
				grid[i][j].is_resolved = 1;
				printf("%.2f,", grid[i][j].value);
			} else {
				printf("_ne_,");
			}
		}
		printf("\n");
	}	
	return 0;
}
