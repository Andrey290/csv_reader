//---MY-HEADERS---//

#include "../include/computations.h"
#include "../include/parsing.h" 
#include "../include/config.h"

//---LIBRARIES---//



//---FUNCTIONS---//

int print_grid(Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows) {
	//printf("[INFO] print_grid : Grid is:\n");
	printf(",");
	for (int i = 0; i < cols->count; i++) {
		printf("%s,", cols->names[i]);
	}
	printf("\n");
	for (int i = 0; i < rows->count; i++) {
		printf("%d,", rows->ids[i]);
		for (int j = 0; j < cols->count; j++) {
			if (grid[i][j].formula[0] != '=') {
				grid[i][j].value = atof(grid[i][j].formula);
				grid[i][j].is_resolved = 1;
				printf("%.2f,", grid[i][j].value);
			} else {
				//printf("[INFO] print_grid : if(!calculate(&grid[%d][%d], grid, cols, rows)).\n", i, j);
				int calc_error = calculate(&grid[i][j], grid, cols, rows);
				if(!calc_error) {
					printf("%.2f,", grid[i][j].value);
				} else {  // DIVISION BY ZERO
					grid[i][j].value = NAN; // Not A Number is mean
					grid[i][j].is_resolved = 1;
					printf("ERROR,");
				}
			}
		}
		printf("\n");
	}	
	return 0;
}


int calculate(Cell *processed_cell, Cell **grid, ColumnsKeeper *cols, RowsKeeper *rows) {
	//printf("[INFO] calculate : Entered into caluculate with %s.\n", processed_cell->formula);	
	
	// It IS a bad solution. I will refactor it.
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

	//printf("[INFO] calculate : Argument one is: %s.\n", argument_one);	
	//printf("[INFO] calculate : Argument two is: %s.\n", argument_two);	
	//printf("[INFO] calculate : Operator is    : %c.\n", operator);	


	//// Geting arg col and arg row separately
	//// And then finding
	//// And further computationg cells value
	
	////// ARG1
	
	int just_a_number_flag = 0;

	//printf("[INFO] calculate : arg one letters before %s\n", argument_one_letters);
	//printf("[INFO] calculate : arg one numbers before %s\n", argument_one_numbers);
	geting_row_and_col_separately(&just_a_number_flag, argument_one_letters, argument_one_numbers, argument_one);	
	//printf("[INFO] calculate : arg one letters after%s\n", argument_one_letters);
	//printf("[INFO] calculate : arg one numbers after%s\n", argument_one_numbers);

	if (atoi(argument_one)) {
		//printf("IT IS JUST A NUMBER!!!\n");
		argument_one_value = atof(argument_one);
	} else {

		int find_error = 0;
		Cell *target_cell = find_cell(&find_error, argument_one_letters, argument_one_numbers, cols, rows, grid);
		if (find_error) {
			return 1;
		}
		//printf("[INFO] calculate/founded_cell : target_cell (%s%s) formula is : %s.\n", argument_one_letters, argument_one_numbers, target_cell->formula);	
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
	
	if (atoi(argument_two)) {
		argument_two_value = atof(argument_two);
	} else {

		int find_error = 0;
		Cell *target_cell = find_cell(&find_error, argument_two_letters, argument_two_numbers, cols, rows, grid);
		if (find_error) {
			return 2;
		}
		//printf("[INFO] calculate/founded_cell : target_cell (%s%s) formula is : %s.\n",argument_two_letters, argument_two_numbers, target_cell->formula);	
		if (target_cell->is_resolved) {
			argument_two_value = target_cell->value;
		} else {
                        calculate(target_cell, grid, cols, rows);
                        argument_two_value = target_cell->value;

		}	
	}

	// Now computation of the value
	
	//printf("\n||||||||\n");
	//printf("[INFO] calculate : argument_one_value %f.\n", argument_one_value);
	//printf("[INFO] calculate : argument_two_value %f.\n", argument_two_value);
	switch (operator) {
		case '+': processed_cell->value = argument_one_value + argument_two_value; /*printf("+\n");*/ break;
		case '-': processed_cell->value = argument_one_value - argument_two_value; /*printf("-\n");*/ break;
		case '*': processed_cell->value = argument_one_value * argument_two_value; /*printf("*\n");*/ break;
		case '/':
			  //printf("\n %f\n", argument_two_value); 
			  if (fabs(argument_two_value) <= DIV_ZERO_EPS) {
				  //printf("[FAULT] calculate : Operator is bad.\n")
				  return 3;
			  }
		          processed_cell->value = argument_one_value / argument_two_value;
			  /*printf("/\n");*/
			  
			  break;
		default: printf("[FAULT] calculate : Operator is bad.\n");
	
	}
	processed_cell->is_resolved = 1;
	//printf("[INFO] calculate : processed_cell value %f.\n", processed_cell->value);
	//printf("\n||||||||\n");
	//printf("\n[INFO] calculate : %s %s %s.\n", &argument_one, &operator, &argument_two);
	
	return 0;
}

int geting_row_and_col_separately(int *just_a_number_flag, char *argument_letters, char *argument_numbers, char *argument) {	
	//printf("[INFO] geting_row_and_col_separately : Entered into GR&CS with a_l:%s.\n", argument_letters);	
	//printf("[INFO] geting_row_and_col_separately : Entered into GR&CS with a_n:%s.\n", argument_numbers);	
	//printf("[INFO] geting_row_and_col_separately : Entered into GR&CS with a  :%s.\n", argument);	
	int windex_let = 0;
	int windex_num = 0;
	for (int i = 0; argument[i] != '\0'; i++) {
	        //printf("[INFO] geting_row_and_col_separately/cycle : Step %d.\n", i);	
		// In this realisation numbers are not allowed in columns names	
	        //printf("[INFO] geting_row_and_col_separately/cycle : Literal on the Step %c.\n", argument[i]);	
		if (isdigit(argument[i])) {
			if (i == 0) {
			        //printf("1\n");
				//printf("%d\n", *just_a_number_flag);
				*just_a_number_flag++;
				//printf("%d\n", *just_a_number_flag);
				argument_numbers[windex_num] = argument[i];
				windex_num++;
			} else {
			        //printf("2\n");
				argument_numbers[windex_num] = argument[i];
				windex_num++;
			}
		} else {
			if (windex_num == 0) {
			        //printf("3\n");
				argument_letters[windex_let] = argument[i];
				windex_let++;
			} else {
			        //printf("4\n");
				break;
			}
		}
	}

	argument_letters[windex_let] = '\0';
	argument_numbers[windex_num] = '\0';

	//printf("[INFO] geting_row_and_col_separately : argument_letters %s.\n", argument_letters);	
	//printf("[INFO] geting_row_and_col_separately : argument_numbers %s.\n", argument_numbers);		
	
	return 0;
}

Cell*  find_cell(int *find_error, char* argument_letters, char* argument_numbers, ColumnsKeeper *cols, RowsKeeper *rows, Cell **grid) {
	int integer_argument_numbers = atoi(argument_numbers);
	// FIND ROW (Later there should be binary searching)
	int target_row = -1;
	for (int i = 0; i < rows->count; i++) {
		if (rows->ids[i] == integer_argument_numbers) {
			target_row = i;
		}
	}
	if (target_row == -1) {
		*find_error = 1;
	//	return 1;
	}
	
	// FIND COL
	int target_col = -1;
	for (int i = 0; i < cols->count; i++) {
		if (strcmp(cols->names[i], argument_letters) == 0) {
			target_col = i;
		}
	}
	if (target_col == -1) {
		*find_error = 2;
	//	return 2;
	}
	//find_cell = &grid[target_row][target_col];
	//return 0;
	return &grid[target_row][target_col];
}
