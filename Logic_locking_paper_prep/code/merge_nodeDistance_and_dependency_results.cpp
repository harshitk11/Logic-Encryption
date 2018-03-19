#include <stdio.h>
#include <string.h>
#include <stdlib.h>




#define MAX_NODE 200    // Maximum number of selected key nodes in a circuit ( usually 200)
char circuit_name[100];		// Store the name of the circuit. Used for opening the folder containg the files.
int NO_OF_OUTPUT;		// Stores the total number of outputs

struct output{
	char name [30];		// Stores the name of the output node
	int size;		// Stores the number of nodes in the output node
	};

struct  output *output_array1 = (struct output*) malloc (MAX_NODE * sizeof(struct output));			//for dependency values
struct  output *output_array2 = (struct output*) malloc (MAX_NODE * sizeof(struct output));			//for node_distance values

int read_file1(char *str, long fsize);
int read_file2(char *str, long fsize);

int     main( int argc, char *argv[] )
{
		
		char input_file[100],input_file1[100], output_file[100];                              // input_file stores the name of the file inNum, output_file stores the file name of nodeDistance_and_dependency
        strcpy(circuit_name, argv[1]);
        strcpy(input_file, circuit_name);
        strcpy(input_file1, circuit_name);
        strcpy(output_file, circuit_name);
        //strcat(input_file, "/inNum");
        strcat(input_file, "/hope_key_node_final.txt");
        strcat(input_file1, "/node_distance_from_primary_input.txt");
        strcat(output_file, "/merged_nodeDistance_and_dependency_results.txt");
        FILE    *fp,*fk;
        
        // Reading the file inNum to get the list of outputs and the number of nodes in the cone of dependency of each output
        // Make sure the inNum file does not have an extral line at the end of the file. The file should terminate at the end of the last row.
        fp = fopen(input_file,"r");
        
        if( fp == NULL )  
        {
      		perror ("Error opening file hope_key_node_final.txt");
      		return(-1);
	   	}
        
        fseek(fp, 0, SEEK_END);		// Shifting the cursor to the end of the file
        long fsize = ftell(fp);		// Stores the size of the entire file
        fseek(fp, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
        char *string = (char*)malloc(fsize + 1);
        fread(string, fsize, 1, fp);
        fclose(fp);
        
        string[fsize] = '\0';
        // 'string' contains the contents of the file hope_key_node_final.txt as a properly 0-terminated C-string
        int NO_OF_NODES1 = read_file1(string, fsize);			//data stored in output_array1[]

        
        fp = fopen(input_file1,"r");
        
        if( fp == NULL )  
        {
      		perror ("Error opening file node_distance_from_primary_input.txt");
      		return(-1);
	   	}
        
        fseek(fp, 0, SEEK_END);		// Shifting the cursor to the end of the file
        fsize = ftell(fp);		// Stores the size of the entire file
        fseek(fp, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
        char *string1 = (char*)malloc(fsize + 1);
        fread(string1, fsize, 1, fp);
        fclose(fp);
        
        string1[fsize] = '\0';
        // 'string1' contains the contents of the file node_distance_from_primary_input.txt as a properly 0-terminated C-string

        
        int NO_OF_NODES2 = read_file2(string1, fsize);			//data stored in output_array2

        fk = fopen(output_file,"w");

        for(int i=0; i<	MAX_NODE; i++ )
        {	
        	
        	for(int j=0; j< MAX_NODE; j++)
        	{
        		if(!strcmp(output_array1[i].name,output_array2[j].name))
        		{
        			fprintf(fk, "%s\t%d\t%d\n",output_array1[i].name, output_array1[i].size, output_array2[j].size);
        		}
        	}
        }
        fclose(fk);
        return 0;
}



int read_file1(char *str, long fsize)		// This function parses the data from the inNum file into the outputname and the number of nodes in that output.
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
			strcpy(output_array1[j].name,temp1);
			memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
			i++;					// Skipping the '\t' character
			while(str[i] != '\n')
			{
				temp2[l++] = str[i];
				i++;
			}
			
			
			output_array1[j++].size = atoi(temp2);	// Since there was one output also in the list of nodes. Eliminating that.
			memset(temp2, '\0', sizeof(temp2));	// Clearing the value of temp2
			i++;					// Skipping the '\n' character
			
			if(i >= fsize)
				break;					
		}
			
	}
	return(j);
			
}

int read_file2(char *str, long fsize)		// This function parses the data from the inNum file into the outputname and the number of nodes in that output.
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
			strcpy(output_array2[j].name,temp1);
			memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
			i++;					// Skipping the '\t' character
			while(str[i] != '\n')
			{
				temp2[l++] = str[i];
				i++;
			}
			
			
			output_array2[j++].size = atoi(temp2);	// Since there was one output also in the list of nodes. Eliminating that.
			memset(temp2, '\0', sizeof(temp2));	// Clearing the value of temp2
			i++;					// Skipping the '\n' character
			
			if(i >= fsize)
				break;					
		}
			
	}
	return(j);
			
}