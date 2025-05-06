#ifndef CONFIG_H
#define CONFIG_H

/*-------------------------------------------//
| THIS FILE CONTAINS STRUCTURES AND CONSTANTS |
//-------------------------------------------*/

//---LIBRARIES---//

#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>  
#include <ctype.h>   
#include <math.h>    

//---CONSTANTS---//

#define MAX_LINE_SIZE 1024
#define DIV_ZERO_EPS 1e-9

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

#endif
