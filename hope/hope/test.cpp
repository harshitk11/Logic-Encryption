#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char circuit_name[100];		// Store the name of the circuit. Used for opening the folder containg the files.

int     main( int argc, char *argv[] )
{	char addr1[500],addr2[500];	//addr 1 stores

		strcpy(circuit_name, argv[1]);
		
		strcpy(addr1, "../../Logic_locking_paper_prep/code/");
		strcat(addr1, circuit_name);
		strcat(addr1, "/");
		
		strcpy(addr2,addr1);
		strcat(addr2, "G17");
		
		FILE *read_hope_output;
		
				// Opens the file hope_output.txt for parsing.
		read_hope_output = fopen (addr2,"r");
		printf("\n\n\n\n\naddr2 :%s\n\n\n\n", addr2);
		
		if( read_hope_output == NULL )  
        	{	
      		perror ("Error opening file: ");
      		return(-1);
   		}
   		
   		printf("\n\nSUCCESS\n\n");
  }
