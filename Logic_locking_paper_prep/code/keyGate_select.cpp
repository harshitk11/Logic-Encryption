#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })

#define NO_OF_OUTPUT	72   		// Has to be changed for every file. Contains the number of selected outputs for the particular bench file.
#define NO_KEY_NODE	128		// Number of shortlisted key nodes. Is usually 200.

struct node{
	char output_name [30];		// Stores the name of the output
	int dependency_no;		// Stores the dependency number of the output
	};
	
struct key{
	char node_name[30];		// Stores the <NODE_NAME>
	int min_dep;			// Stores the minimum dependency number from the list of outputs in inNum_<NODE_NAME> file
	};

//=========================================================================//
//****************************FUNCTION PROTOTYPE **************************//
void read_all();
struct node* read_inNum(char *str, long fsize, int index);
void read_hope_keynode();
void swap_min_dep(int i, int j);
void swap_node_name(int i, int j);
void bubble_sort();
void output_array_sort();				
//=========================================================================//

char circuit_name[100];		// Store the name of the circuit. Used for opening the folder containg the files.
char hope_key_node[200][50];	// Stores the name of the 200 key nodes we get from hope.

struct key *key_array = (struct key *)malloc(NO_KEY_NODE * sizeof(key));	
struct node **node_array = (struct node **)malloc(NO_KEY_NODE * sizeof(node *));

//for(int i = 0; i< NO_KEY_NODE; i++)
//	node_array[i]= (struct node *)malloc(NO_OF_OUTPUT * sizeof(node));
	
int     main( int argc, char *argv[] )
{
	
	char input_file[100],output_file[100];                              // input_file stores the name of the file inNum, output_file stores the file name of the sorted inNum
        strcpy(circuit_name, argv[1]);
        
        read_all();
        
        for(int i = 0; i<NO_KEY_NODE; i++)
	{
        	free(node_array[i]);
        }
        free(node_array);
}


// Reads all contents of the directory 'dependency' and stores it in 'node_array'. node_array[i][j] contains the j th output in the i th node (inNum_<NODE_NAME>)
void read_all()
{	
	read_hope_keynode(); 			// Read all the <NODE_NAME> for gaining access to the inNum_<NODE_NAME> file.
	char foo1[50];				// Stores the name of the file inNum_<NODE_NAME> inside the 'dependency' directory.
	FILE *fp;
	int temp;
	
	for(int i = 0; i<NO_KEY_NODE; i++)
	{
		strcpy(foo1, circuit_name);
        	strcat(foo1, "/dependency/");
        	strcat(foo1, "inNum_" );
        	strcat(foo1, hope_key_node[i]);
		//printf("%s\n", foo1);
		fp = fopen(foo1,"r");		// Opening the file inNum_<NODE_NAME> 
		
		if( fp == NULL )  
        	{
      			perror ("Error opening file: inNum_<NODE_NAME>");
      			exit(0);
   		}
        
        	fseek(fp, 0, SEEK_END);		// Shifting the cursor to the end of the file
        	long fsize = ftell(fp);		// Stores the size of the entire file
        	fseek(fp, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
        	char *string = (char*)malloc(fsize + 1);
        	fread(string, fsize, 1, fp);
        	fclose(fp);
        
        	string[fsize] = '\0';
        	// 'string' contains the contents of the file inNum_<NODE_NAME> as a properly 0-terminated C-string
        	
		node_array[i] = read_inNum(string, fsize, i);
		
		// Printing the data read
		
		/*for(int j = 0; j<NO_OF_OUTPUT; j++)
		{
			printf("%s\t%d\n", node_array[i][j].output_name, node_array[i][j].dependency_no);
		}*/
		
	}
	
	
	for(int i = 0; i< NO_KEY_NODE; i++)		//Finding the minimum dependency number from the list of outputs in inNum_<NODE_NAME> file
	{	
		temp = node_array[i][0].dependency_no;
		for(int j = 0; j<NO_OF_OUTPUT; j++)
		{
			temp = min(temp,node_array[i][j].dependency_no);
		}
		
		strcpy(key_array[i].node_name, hope_key_node[i]);
		key_array[i].min_dep = temp;
	
	}
	
	// Printing the node and the minimum dependency
	/*for(int i = 0; i< NO_KEY_NODE; i++)
	{	
		printf("%s\t%d\n",key_array[i].node_name,key_array[i].min_dep);
	
	}*/
	
	// Now we need to sort the key_array in the descending order of 'min_dep'
	output_array_sort();
	
	for(int i = 0; i< NO_KEY_NODE; i++)
	{	
		printf("%s\t%d\n",key_array[i].node_name,key_array[i].min_dep);
	
	}
	
	FILE *fk;
	char input_file[100];                              // input_file stores the name of the file hope_key_node_final.txt
        
        strcpy(input_file, circuit_name);
        
        strcat(input_file, "/hope_key_node_final.txt");
        fk = fopen(input_file, "w");
	
	for(int i = 0; i< 128; i++)		// Selecting the top 128 key gates
	{	
		fprintf(fk,"%s\n",key_array[i].node_name);
	
	}
	fclose(fk);
	
	
}					 


struct node* read_inNum(char *str, long fsize, int index)		// This function parses the data from the inNum_<NODE_NAME> file into the outputname and the number of nodes in that output.
{	

	char temp1[30], temp2[10];		// Temporary variables for storing the name and size of the output node respectively.
	memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
	memset(temp2, '\0', sizeof(temp2));	// Clearing the value of temp2
	int j = 0;
	int k,l;
	
	node_array[index]= (struct node *)malloc(NO_OF_OUTPUT * sizeof(node));
	
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
			strcpy(node_array[index][j].output_name,temp1);
			memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
			i++;					// Skipping the '\t' character
			while(str[i] != '\n')
			{
				temp2[l++] = str[i];
				i++;
			}
			
			node_array[index][j++].dependency_no = atoi(temp2);	// Since there was one output also in the list of nodes. Eliminating that.
			memset(temp2, '\0', sizeof(temp2));	// Clearing the value of temp2
			i++;					// Skipping the '\n' character
			
			if(i >= fsize)
				break;					
		}
			
	}
	return(node_array[index]);
			
}

void read_hope_keynode()					// Function for reading all the hope_key_node
{

	char input_file[100];                              // input_file stores the name of the file inNum.
        
        strcpy(input_file, circuit_name);
        
        strcat(input_file, "/hope_key_node_final.txt");
        //printf("%s\n", input_file);
        FILE    *fp;
        
        // Reading the file hope_key_node.txt to get the list of nodes 
        
        fp = fopen(input_file,"r");
        
        if( fp == NULL )  
        {
      		perror ("Error opening file: hope_key_node: ");
      		exit(0);
   	}
        
        fseek(fp, 0, SEEK_END);		// Shifting the cursor to the end of the file
        long fsize = ftell(fp);		// Stores the size of the entire file
        fseek(fp, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
        char *string = (char*)malloc(fsize + 1);
        fread(string, fsize, 1, fp);
        fclose(fp);
        
        string[fsize] = '\0';
        // 'string' contains the contents of the file hope_key_node.txt as a properly 0-terminated C-string
        
        //================================================================================================================================//
        char temp1[30];				// Temporary variable for storing the name of the node.
	memset(temp1, '\0', sizeof(temp1));	
	
	int k,j = 0;
	for(int i = 0; i <= fsize;)
	{	
	
		k=0;	
		while(string[i] != '\n' && string[i] != '\0')
		{
			temp1[k++] = string[i++];
		}
		
		strcpy(hope_key_node[j],temp1);
		j++;
		memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
		
		if(string[i] == '\0')
			break;
			
		i++;
		
							
	}
}

void swap_min_dep(int i, int j)		// For swapping the index of output_array. Used for bubble sort in output_array_sort()
{
    int temp = key_array[i].min_dep;
    key_array[i].min_dep = key_array[j].min_dep;
    key_array[j].min_dep = temp;
}

void swap_node_name(int i, int j)		// For swapping the name of output array. Used for bubble sort in output_array_sort()
{
	char temp[30];
	memset(temp, '\0', sizeof(temp));
	strcpy(temp,key_array[i].node_name);
	memset(key_array[i].node_name, '\0', sizeof(key_array[i].node_name));
	strcpy(key_array[i].node_name, key_array[j].node_name);
	memset(key_array[j].node_name, '\0', sizeof(key_array[j].node_name));
	strcpy(key_array[j].node_name, temp);
}
// A function to implement bubble sort
void bubble_sort()
{
   int i, j;
   int n = NO_KEY_NODE;
   for (i = 0; i < n-1; i++)      
   { 
       // Last i elements are already in place   
       for (j = 0; j < n-i-1; j++)
       { 
           if (key_array[j].min_dep < key_array[j+1].min_dep)
           {
              swap_node_name(j,j+1);
              swap_min_dep(j,j+1);
                            
           }
       }
   }
}

void output_array_sort()		// Sorts the output array in the descending order of the number of nodes and takes the top n% of the outputs.
{
	bubble_sort();
	

} 
