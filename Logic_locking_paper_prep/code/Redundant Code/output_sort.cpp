/* This code modifies the inNum and writes the output nodes in the descending order of the number of nodes in them*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_OUTPUT	1000    // Maximum number of outputs in a circuit ( May have to change depending on the circuit)

//---------------------------------------//
//-------FUNCTION PROTOTYPE--------------//
int     main( int argc, char *argv[] );
int read_inNum(char *str, long fsize);
void swap_size(int i, int j);
void swap_index(int i, int j);
void swap_name(int i, int j);
void bubble_sort();
void output_array_sort();

char circuit_name[100];		// Store the name of the circuit. Used for opening the folder containg the files.
int NO_OF_OUTPUT;		// Stores the total number of outputs

struct output{
	char name [30];		// Stores the name of the output node
	int size;		// Stores the number of nodes in the output node
	int indexOfOutput;	// Stores the index of output so that name can be avoided
	};
	
struct  output *output_array = (struct output*) malloc (MAX_OUTPUT * sizeof(struct output));

int     main( int argc, char *argv[] )
{
	double x; 						// For selecting the number of outputs we want. x = 1 if we want all the outputs. 0<x<1
	char input_file[100],output_file[100];                              // input_file stores the name of the file inNum, output_file stores the file name of the sorted inNum
        strcpy(circuit_name, argv[1]);
        strcpy(input_file, circuit_name);
        strcat(input_file, "/inNum");
        
        FILE    *fp,*fk;
        
        // Reading the file inNum to get the list of outputs and the number of nodes in the cone of dependency of each output
        // Make sure the inNum file does not have an extral line at the end of the file. The file should terminate at the end of the last row.
        fp = fopen(input_file,"r");
        
        if( fp == NULL )  
        {
      		perror ("Error opening file");
      		return(-1);
   	}
        
        fseek(fp, 0, SEEK_END);		// Shifting the cursor to the end of the file
        long fsize = ftell(fp);		// Stores the size of the entire file
        fseek(fp, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
        char *string = (char*)malloc(fsize + 1);
        fread(string, fsize, 1, fp);
        fclose(fp);
        
        string[fsize] = '\0';
        // 'string' contains the contents of the file inNum as a properly 0-terminated C-string
        
        NO_OF_OUTPUT = read_inNum(string, fsize);
        output_array_sort();			// Sorts the array containing the output names in the descending order of the number of nodes in them.
        
        
        // Printing all the outputs and the number of nodes in them
        for(int i = 0; i< NO_OF_OUTPUT ; i++)
        	printf("%d\t%s\t%d\n",output_array[i].indexOfOutput,output_array[i].name,output_array[i].size);
        	
        printf("No of output = %d\n",NO_OF_OUTPUT);
        printf("-------------------------------------------------------------\n");
        
        // Writing the sorted output file in the file inNum_sort
        strcpy(output_file, circuit_name);
        strcat(output_file, "/inNum_sort");
        
        fk = fopen(output_file,"a");
        for(int i = 0; i< NO_OF_OUTPUT ; i++)
        	fprintf(fk,"%s\t%d\n",output_array[i].name, output_array[i].size);
        
        fclose(fk);
        return 0;
}

int read_inNum(char *str, long fsize)		// This function parses the data from the inNum file into the outputname and the number of nodes in that output.
{	

	char temp1[30], temp2[10];		// Temporary variables for storing the name and size of the output node respectively.
	memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
	memset(temp2, '\0', sizeof(temp2));	// Clearing the value of temp2
	int j = 0;
	int k,l;
	for(int i = 0; i < fsize;)
	{
		while(str[i] != '\0')
		{	
			k = 0,l = 0;
			while(str[i] != '\t')
			{
				temp1[k++] = str[i];
				i++;
			}
			strcpy(output_array[j].name,temp1);
			memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
			i++;					// Skipping the '\t' character
			while(str[i] != '\n')
			{
				temp2[l++] = str[i];
				i++;
			}
			
			output_array[j].indexOfOutput = j;
			output_array[j++].size = atoi(temp2);	// Since there was one output also in the list of nodes. Eliminating that.
			memset(temp2, '\0', sizeof(temp2));	// Clearing the value of temp2
			i++;					// Skipping the '\n' character
			
			if(i >= fsize)
				break;					
		}
			
	}
	return(j);
			
}

void swap_size(int i, int j)		// For swapping the size of output_array. Used for bubble sort in output_array_sort()
{
    int temp = output_array[i].size;
    output_array[i].size = output_array[j].size;
    output_array[j].size = temp;
}

void swap_index(int i, int j)		// For swapping the index of output_array. Used for bubble sort in output_array_sort()
{
    int temp = output_array[i].indexOfOutput;
    output_array[i].indexOfOutput = output_array[j].indexOfOutput;
    output_array[j].indexOfOutput = temp;
}

void swap_name(int i, int j)		// For swapping the name of output array. Used for bubble sort in output_array_sort()
{
	char temp[30];
	memset(temp, '\0', sizeof(temp));
	strcpy(temp,output_array[i].name);
	memset(output_array[i].name, '\0', sizeof(output_array[i].name));
	strcpy(output_array[i].name, output_array[j].name);
	memset(output_array[j].name, '\0', sizeof(output_array[j].name));
	strcpy(output_array[j].name, temp);
}
// A function to implement bubble sort
void bubble_sort()
{
   int i, j;
   int n = NO_OF_OUTPUT;
   for (i = 0; i < n-1; i++)      
   { 
       // Last i elements are already in place   
       for (j = 0; j < n-i-1; j++)
       { 
           if (output_array[j].size < output_array[j+1].size)
           {
              swap_size(j,j+1);
              swap_name(j,j+1);
              swap_index(j,j+1);
              
           }
       }
   }
}

void output_array_sort()		// Sorts the output array in the descending order of the number of nodes and takes the top n% of the outputs.
{
	bubble_sort();
	

} 
