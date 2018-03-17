#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_OUTPUT	1000    // Maximum number of outputs in a circuit ( May have to change depending on the circuit)
#define MAX_NODE 	50000	// May have to change for very large circuits. (Maximum number of nodes which are connected to all the outputs)

struct output{
	char name [30];		// Stores the name of the output node
	int size;		// Stores the number of nodes in the output node
	int indexOfOutput;	// Stores the index of output so that name can be avoided
	};

struct node{
	char name [30];		// Stores the name of the internal node
	int visit;		// Stores the number of times the node has been visited. The number of outputs this node provides output to. Default value 1.
	};

struct  output *output_array = (struct output*) malloc (MAX_OUTPUT * sizeof(struct output));
char key_node[MAX_NODE][30];	// Stores the name of all the nodes which are connected to all the outputs.
//---------------------------------------//
//-------FUNCTION PROTOTYPE--------------//

int read_inNum(char *str, long fsize);
struct node *read_output(char *output_name, int output_size);
int check_node(char *node, struct node* outlist, int size);
void read_all_output();
void swap_size(int i, int j);
void swap_index(int i, int j);
void swap_name(int i, int j);
void bubble_sort();
void output_array_sort();

//---------------------------------------//


char circuit_name[100];		// Store the name of the circuit. Used for opening the folder containg the files.
int NO_OF_OUTPUT;		// Stores the total number of outputs

int     main( int argc, char *argv[] )
{
	double x; 						// For selecting the number of outputs we want. x = 1 if we want all the outputs. 0<x<1
	char input_file[100],output_file[100];                              // input_file stores the name of the file inNum, output_file stores the file name of the sorted inNum
        strcpy(circuit_name, argv[1]);
        strcpy(input_file, circuit_name);
        
        //strcat(input_file, "/inNum");
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
        //---------------------------------------------------------------------------------------------//
        // Writing the sorted output file in the file inNum_sort
        
        
        strcpy(output_file, circuit_name);
        strcat(output_file, "/inNum_sort");
        
        fk = fopen(output_file,"w");		// Clearing the prevoius contents of the file.
	fclose(fk);
	
        fk = fopen(output_file,"a");
        for(int i = 0; i< NO_OF_OUTPUT ; i++)
        	fprintf(fk,"%s\t%d\n",output_array[i].name, output_array[i].size);
        
        fclose(fk);
        //---------------------------------------------------------------------------------------------//
        // If we want only the top x% outputs then we will reduce the valu of NO_OF_OUTPUT. In that case, other outputs will be there but we will ignore them.
        x= 1;				// 0<x<=1
        NO_OF_OUTPUT = (int)ceil(x*NO_OF_OUTPUT);
        NO_OF_OUTPUT = 19;
        //======================================================================================================//
        // We need to write the shortlisted outputs(with maximum overlap) into a new file 'shortlisted_outputs'
        strcpy(output_file, circuit_name);
        strcat(output_file, "/shortlisted_output");
        
        fk = fopen(output_file,"w");		// Clearing the prevoius contents of the file.
	fclose(fk);
	
        fk = fopen(output_file,"a");
        for(int i = 0; i< NO_OF_OUTPUT ; i++)
        	fprintf(fk,"%s\n",output_array[i].name);
        
        fclose(fk);
        //==================================================================================================//
        
        // Printing all the outputs and the number of nodes in them
        for(int i = 0; i< NO_OF_OUTPUT ; i++)
        	printf("%d\t%s\t%d\n",output_array[i].indexOfOutput,output_array[i].name,output_array[i].size);
        	
        printf("No of output = %d\n",NO_OF_OUTPUT);
        printf("-------------------------------------------------------------\n");
        
        
        //printf("%s",string);
        //printf("Testing\n\n%c\t%c\t%c\n",string[fsize-2],string[fsize-1],string[fsize]);
        
       	read_all_output();
        /*for(int i = 0; i< output_array[0].size ; i++)
        	printf("%d\t%s\t%d\n",i,out1[i].name,out1[i].visit);
        */
        
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
			output_array[j++].size = atoi(temp2)-1;	// Since there was one output also in the list of nodes. Eliminating that.
			memset(temp2, '\0', sizeof(temp2));	// Clearing the value of temp2
			i++;					// Skipping the '\n' character
			
			if(i >= fsize)
				break;					
		}
			
	}
	return(j);
			
}

struct node *read_output(char *output_name, int output_size)	// This function takes as input the 'name of the output' and the number of nodes under that output (output_size) and parses the contents of the output file into an array.
{	
	
	
	char output_file[30];
	int initFlag; 	// For eliminating the first redundant output nodes in the output file.
	char temp[30];
	
	FILE *outcone;
	// Creating a loop for iterating over all the output files
	
		
		// Opening the file containing the cone of dependency of each output
		
		memset(output_file, '\0', sizeof(output_file));		// Clearing out the contents of output file.
		strcpy(output_file, circuit_name);
		strcat(output_file,"/");
		strcat(output_file,output_name);
		
		outcone = fopen(output_file, "r");
		if( outcone == NULL )  
        	{
      			perror ("Error opening file");
      			exit(0);
   		}
   		
   				 
				
		// Reading the contents of the output file
		fseek(outcone, 0, SEEK_END);		// Shifting the cursor to the end of the file
        	long fsize = ftell(outcone);		// Stores the size of the entire file
        	fseek(outcone, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
        	char *string = (char*)malloc(fsize + 1);
        	fread(string, fsize, 1, outcone);
        	fclose(outcone);
        	string[fsize] = '\0';
        	// 'string' contains the contents of the output file as a properly 0-terminated C-string
        	//------------------------------------------------------------------------------------------//
        	
        	// Parsing the contents of the string into the array node_array. Name of the output will be stored in node_array[_].name
        	struct  node *node_array = (struct node*) malloc (output_size * sizeof(struct node));
		
		for (int i = 0; i< output_size; i++)
		{
			node_array[i].visit = 1;	// Default value. i.e. one node provides output to atleast one output. 
		}
        	
        	initFlag = 1;
        	memset(temp, '\0', sizeof(temp));	// Clearing the value of temp
        	int j,k = 0;				// For indexing the value of the number of nodes
        	
        	for(int i = 0; i< fsize;)
        	{
        		if(string[i]=='\n' && string[i+1]=='\n' && string[i+2]=='\n')
        			break;			// You've reached the end of the output file.
        		
        		while(string[i]!='>' && initFlag == 1)
        			i++;			// Skipping the part before the arrow; Has to run once.
        			
        		// The next node after the arrow is the same output node so we need to skip that too.; Has to run once.
        		while(string[i]!='\t' && initFlag ==1)
        			i++;
        		
        		if(initFlag == 1)
        		{
        			i++; // This skips the '\t' and the input node starts after this.
        			initFlag = 0;
        		}
        		
        		j = 0;
        		while(string[i] != '\t')
        		{	
        			temp[j++] = string[i];
        			i++;
        		}
        		i++; 					// Skipping the '\t'
        		strcpy(node_array[k].name, temp);
        		k++; 
        		memset(temp, '\0', sizeof(temp));	// Clearing the value of temp
        	}	
        	
		//printf("Testing\n"); 
		//printf("%s",string); 	  
		return node_array;
}

int check_node(char *node, struct node* outlist, int size)		// Checks the presence of 'node' in an array of nodes 'outlist' whose length is given by 'size' 
{
	// Function returns the index of the element in the array where the match is found. -1 is returned if no match is found.
	for(int i = 0; i<size ; i++)
	{
		if(!strcmp(node,outlist[i].name))
			return i;
		
	}
	return -1;
}



void read_all_output() 					// This will read the nodes in all the output files 
{
	// Allocating memory for storing the cone of dpendency of outputs into an array.
	struct node **out = (struct node **)malloc(NO_OF_OUTPUT * sizeof(struct node *));
	int index;
	struct node *temp;
	
	
	
	for(int i = 0; i< NO_OF_OUTPUT; i++)								// Iterating through all the outputs
		out[i] = (struct node *)malloc(output_array[i].size * sizeof(struct node));		// out[i][j] means the j th node in the cone of dependency of the i th output
		
	
	for(int i = 0; i< NO_OF_OUTPUT; i++)
	{
		temp = read_output(output_array[i].name, output_array[i].size);
		out[i] = temp;
		//out[output_array[i].indexOfOutput] = read_output(output_array[i].name, output_array[i].size);
	
	}
	
	/*for(int i = 0; i< output_array[0].size ; i++)
        	printf("%d\t%s\t%d\n",i,out[0][i].name,out[0][i].visit);
	*/
	
	// Printing all the outputs and their cone of dependency.
	
	/*printf("\n******************Before**************************\n");
	
	for(int i = 0; i< NO_OF_OUTPUT; i++)
	{
		printf("\n\n%d\t%s\n",output_array[i].indexOfOutput,output_array[i].name);
		for(int j = 0; j< output_array[i].size; j++)
		printf("Node : %s\t visit: %d\n", out[i][j].name,out[i][j].visit);
	
	}
	*/
	
	// Finding input nodes which are connected to all the outputs.
	
	/* For this we will iterate over all the input nodes. Every time a match is found, the visit variable of both of the nodes which have matched will be incremented.
	At the end, those nodes will be selected whose value of visit variable = NO_OF_OUTPUT */
	
	//for(int i = 0; i< NO_OF_OUTPUT-1; i++) // i th output
	//{
		int i = 0;
		// Comparing the nodes of the i th output with the nodes of the j th output.
		for(int j = 0; j< output_array[i].size; j++) // j th node of the i th output
		{
			//For each j th node in the i th output, we are iterating over all the nodes of the remaining outputs and compairing them with the j th node of the i th output		
			for(int k = i+1; k< NO_OF_OUTPUT; k++)	// k th output
			{
				index = check_node(out[i][j].name, out[k], output_array[k].size);
				
				if(index >= 0)			// If a match is found then we'll increment the visit values of the matched pair.
				{
					out[i][j].visit++;
					out[k][index].visit++;
				}
			}
		}
	//}
	
	/*printf("\n******************After**************************\n");
	
	for(int i = 0; i< NO_OF_OUTPUT; i++)
	{
		printf("\n\n%d\t%s\n",output_array[i].indexOfOutput,output_array[i].name);
		for(int j = 0; j< output_array[i].size; j++)
		printf("Node : %s\t visit: %d\n", out[i][j].name,out[i][j].visit);
	
	}*/
	
	/* Now we want to get the nodes whose visit value = NO_OF_OUTPUT i.e. these nodes are connected to all the outputs. Since these nodes are connected to all the outputs so we 
	just need to search the cone of dependency of any single output and check the visit values of its nodes. If the visit value == NO_OF_OUTPUT that node is connected to all the
	outputs.*/
	
	// Checking the cone of dependency of the first output.
	int size_keynode = 0;		// Stores the number of key_nodes
	for(int i = 0; i< output_array[0].size; i++)
	{
		if(out[0][i].visit == NO_OF_OUTPUT)
		{
			strcpy(key_node[size_keynode], out[0][i].name);
			size_keynode ++;
		}	
	}
	
	char key_file[100];                              // key_file stores the name of the file containing key nodes
	
        strcpy(key_file, circuit_name);
        
        //strcat(key_file, "/keyNodes");
	strcat(key_file, "/keyNodes_sort");
	
	FILE *fk;
	
	fk = fopen(key_file,"w");		// Clearing the prevoius contents of the file.
	fclose(fk);
	
	fk = fopen(key_file,"a");
	fseek(fk, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
	// Printing the key nodes and storing them in a file keyNodes
	printf("\n*******Printing the keynodes********\n");
	printf("\nNumber of key nodes = %d\n",size_keynode);
	for(int i = 0; i< size_keynode; i++)
	{
		printf("%s\n",key_node[i]);
		fprintf(fk,"%s\n", key_node[i]);
	}
	
	fclose(fk);


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
















