#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int     main( int argc, char *argv[] )
{
        	
        	char circuit_name[100];         	
        	char input_file[100];                              // input_file and FILE_NM1 store the name of the input file
        	strcpy(circuit_name, argv[1]);
        	strcpy(input_file, circuit_name);
        	//strcat(input_file, "/");
        	//strcat(input_file, circuit_name);
        	strcat(input_file, ".bench");
        	printf("\n%s\n", input_file);
		//FILE *output_file;
}
