//---COMMENTARY---//
/*

   =^0w0^=
   1) This is the first version in which everything works in accordance with technical requirement.
   2) [INFO] outputs are still not deleted.
   3) !No sorting. 
   4) !Searching is brutforce.
   5) !Code may crush on errors.
   6) !Crushes are not heap-safety.

*/

//---STANDART-LIBRARIES---///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

//---CONSTANTS---//

const int  MAX_LINE_SIZE = 1024;
const char *ARAB_NUMBERS = "0123456789";

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
int           calculate(Cell *processed_cell, Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows);
int           geting_row_and_col_separately(int *just_a_number_flag, char *argument_letters, char *argument_numbers, char *argument);
Cell* find_cell(char* argument_letters, char* argument_numbers, ColumnsKeeper *cols, RowsKeeper *rows, Cell **grid);


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
	Cell **grid = (Cell**)malloc(rows.count * sizeof(Cell*));
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
	    /*------------------------------------------//
	    | Here should be FAST SORTING of ROW and COL |
	    //------------------------------------------*/

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
		} else {
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
				grid[i][j].value = atof(grid[i][j].formula);
				grid[i][j].is_resolved = 1;
				printf("%.2f,", grid[i][j].value);
			} else {
				printf("[INFO] print_grid : if(!calculate(&grid[%d][%d], grid, cols, rows)).\n", i, j);
				if(!calculate(&grid[i][j], grid, cols, rows)) {
					printf("%.2f,", grid[i][j].value);
				} else {
					grid[i][j].value = NAN; // Not A Number is mean
					grid[i][j].is_resolved = 1;
					printf("NAN,");
				}
			}
		}
		printf("\n");
	}	
	return 0;
}


int calculate(Cell *processed_cell, Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows) {
	printf("[INFO] calculate : Entered into caluculate with %s.\n", processed_cell->formula);	
	
	if (isdigit(processed_cell->formula[0])) {
		processed_cell->value = atof(processed_cell->formula);
		processed_cell->is_resolved = 1;
		return 0;
	}

	
	double value_to_return = 0;
	
	char   argument_one[MAX_LINE_SIZE  +  32];
	char   argument_one_letters[  MAX_LINE_SIZE ];
	char   argument_one_numbers[32];
	double argument_one_value = 0;

	char   argument_two[MAX_LINE_SIZE  +  32];
	char   argument_two_letters[  MAX_LINE_SIZE ];
	char   argument_two_numbers[32];
	double argument_two_value = 0;
	
	char operator;

	// Parsing formula
	
	//// Getting arguments and operator itself

	int operator_flag = 0;
	int writing_index = 0;
	for (int i = 1; ; i++) {
		if (processed_cell->formula[i] == '+' || processed_cell->formula[i] == '-' || processed_cell->formula[i] == '*' || processed_cell->formula[i] == '/') {
			operator = processed_cell->formula[i];
			operator_flag++;
			argument_one[writing_index] = '\0';
			writing_index = 0;
		} else if (processed_cell->formula[i] == '\0' || processed_cell->formula[i] == '\n') {
			if (!operator_flag) {
				argument_one[writing_index] = '\0';
			} else {
				argument_two[writing_index] = '\0';
			}
			break;
		} else {
			if (!operator_flag) {
				argument_one[writing_index] = processed_cell->formula[i];
			} else {
				argument_two[writing_index] = processed_cell->formula[i];
			}
			writing_index++;
		}
	}

	printf("[INFO] calculate : Argument one is: %s.\n", argument_one);	
	printf("[INFO] calculate : Argument two is: %s.\n", argument_two);	
	printf("[INFO] calculate : Operator is    : %c.\n", operator);	


	//// Geting arg col and arg row separately
	//// And then finding
	//// And further computationg cells value
	
	////// ARG1
	
	int just_a_number_flag = 0;

	printf("[INFO] calculate : arg one letters before %s\n", argument_one_letters);
	printf("[INFO] calculate : arg one numbers before %s\n", argument_one_numbers);
	geting_row_and_col_separately(&just_a_number_flag, argument_one_letters, argument_one_numbers, argument_one);	
	printf("[INFO] calculate : arg one letters after%s\n", argument_one_letters);
	printf("[INFO] calculate : arg one numbers after%s\n", argument_one_numbers);

	if (just_a_number_flag) {
		printf("IT IS JUST A NUMBER!!!\n");
		argument_one_value = (double)atoi(argument_one_numbers);
	} else {
		Cell *target_cell = find_cell(argument_one_letters, argument_one_numbers, cols, rows, grid);
		printf("[INFO] calculate/founded_cell : target_cell (%s%s) formula is : %s.\n", argument_one_letters, argument_one_numbers, target_cell->formula);	
		if (target_cell->is_resolved) {
			argument_one_value = target_cell->value;
		} else {
			calculate(target_cell, grid, cols, rows);
                        argument_one_value = target_cell->value;
		}	
	}

	////// ARG2
	
	just_a_number_flag = 0;
	
	geting_row_and_col_separately(&just_a_number_flag, argument_two_letters, argument_two_numbers, argument_two);
	
	if (just_a_number_flag) {
		argument_two_value = (double)atoi(argument_two_numbers);
	} else {
		Cell *target_cell = find_cell(argument_two_letters, argument_two_numbers, cols, rows, grid);
		printf("[INFO] calculate/founded_cell : target_cell (%s%s) formula is : %s.\n",argument_two_letters, argument_two_numbers, target_cell->formula);	
		if (target_cell->is_resolved) {
			argument_two_value = target_cell->value;
		} else {
                        calculate(target_cell, grid, cols, rows);
                        argument_two_value = target_cell->value;

		}	
	}

	// Now computation of the value
	
	printf("\n||||||||\n");
	printf("[INFO] calculate : argument_one_value %f.\n", argument_one_value);
	printf("[INFO] calculate : argument_two_value %f.\n", argument_two_value);
	switch (operator) {
		case '+': processed_cell->value = argument_one_value + argument_two_value; printf("+\n"); break;
		case '-': processed_cell->value = argument_one_value - argument_two_value; printf("-\n"); break;
		case '*': processed_cell->value = argument_one_value * argument_two_value; printf("*\n"); break;
		case '/': processed_cell->value = argument_one_value / argument_two_value; printf("/\n"); break;
		default: printf("[FAULT] calculate : Operator is bad.\n");
	
	}
	processed_cell->is_resolved = 1;
	printf("[INFO] calculate : processed_cell value %f.\n", processed_cell->value);
	printf("\n||||||||\n");
	printf("\n[INFO] calculate : %s %s %s.\n", &argument_one, &operator, &argument_two);
	
	return 0;
}

int geting_row_and_col_separately(int *just_a_number_flag, char *argument_letters, char *argument_numbers, char *argument) {	
	printf("[INFO] geting_row_and_col_separately : Entered into GR&CS with a_l:%s.\n", argument_letters);	
	printf("[INFO] geting_row_and_col_separately : Entered into GR&CS with a_n:%s.\n", argument_numbers);	
	printf("[INFO] geting_row_and_col_separately : Entered into GR&CS with a  :%s.\n", argument);	
	int windex_let = 0;
	int windex_num = 0;
	for (int i = 0; argument[i] != '\0'; i++) {
	        printf("[INFO] geting_row_and_col_separately/cycle : Step %d.\n", i);	
		// In this realisation numbers are not allowed in columns names	
	        printf("[INFO] geting_row_and_col_separately/cycle : Literal on the Step %c.\n", argument[i]);	
		if (isdigit(argument[i])) {
			if (i == 0) {
			        printf("1\n");
				printf("%d\n", *just_a_number_flag);
				*just_a_number_flag++;
				printf("%d\n", *just_a_number_flag);
				argument_numbers[windex_num] = argument[i];
				windex_num++;
			} else {
			        printf("2\n");
				argument_numbers[windex_num] = argument[i];
				windex_num++;
			}
		} else {
			if (windex_num == 0) {
			        printf("3\n");
				argument_letters[windex_let] = argument[i];
				windex_let++;
			} else {
			        printf("4\n");
				break;
			}
		}
	}

	argument_letters[windex_let] = '\0';
	argument_numbers[windex_num] = '\0';

	printf("[INFO] geting_row_and_col_separately : argument_letters %s.\n", argument_letters);	
	printf("[INFO] geting_row_and_col_separately : argument_numbers %s.\n", argument_numbers);		
	
	return 0;
}

Cell* find_cell(char* argument_letters, char* argument_numbers, ColumnsKeeper *cols, RowsKeeper *rows, Cell **grid) {
		int integer_argument_numbers = atoi(argument_numbers);
	        // FIND ROW (Later there should be binary searching)
		int target_row = 0;
		for (int i = 0; i < rows->count; i++) {
			if (rows->ids[i] == integer_argument_numbers) {
				target_row = i;
			}
		}
	
		// FIND COL
		int target_col = 0;
		for (int i = 0; i < cols->count; i++) {
			if (strcmp(cols->names[i], argument_letters) == 0) {
				target_col = i;
			}
		}
	return &grid[target_row][target_col];
}
