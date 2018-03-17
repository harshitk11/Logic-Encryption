#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define number_of_test_pattern 100
#define MAX_NODE 50000
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })

char stuck_at_zero[50000][100];
char stuck_at_one[50000][100];
int stuck_at_zero_count[50000];
int stuck_at_one_count[50000];
char circuit_name[100];		// Store the name of the circuit. Used for opening the folder containg the files.

struct node{
	char name [100];		// Stores the name of the node
	int index;			// Stores the index of the nodes. Nodes will be reffered with the help of index. 
	int stuck_at_zero_count;	// Stores the number of times s-a-0 fault has occured at the node
	int stuck_at_one_count;		// Stores the number of times s-a-1 fault has occured at the node
	int stuck_at_total;		// Total stuck at faults 
	};
	
struct  node *node_array = (struct node*) malloc (MAX_NODE * sizeof(struct node));
struct  node *node_final = (struct node*) malloc (200 * sizeof(struct node));		// There will be 200 final selected nodes which are intersection of hope and keyNodes_sort

char keyNodes_sort[MAX_NODE][100];	// Stores the list of nodes which we got from running inNum.cpp
// Our goal is to take the top 200 nodes of node_array which are also in the list keyNodes_sort.
int NO_OF_NODE;   			// Stores the number of nodes as read from the file keyNodes_sort

//===================FUNCTION PROTOTYPE======================//
void initialize_node();
void node_array_sort(int n);	
void bubble_sort(int n);
void swap_name(int i, int j);
void swap_index(int i, int j);
void swap_stuck_at_one_count(int i, int j);
void swap_stuck_at_zero_count(int i, int j);
void swap_stuck_at_total(int i, int j);
int read_keyNodes(char *str, long fsize);
void node_final_select(int index);
//===========================================================//


int     main( int argc, char *argv[] )
{
		
		srand(time(NULL));   // should only be called once
		initialize_node(); 		// Initializing the values of node_array
		char addr1[500],addr2[500],taddr[500];	// addr 1 stores the location of the bench file and addr 2 stores the location of the target output file created by hope.
		char addr3[500];		// For storing the address of the output file which contains the list of nodes with the number of faults in them.
		char command[500];		// Fed as a command to the system() function
		char keyNodes[500];		// Stores the filename of the file keyNodes_sort
		strcpy(circuit_name, argv[1]);
		
		
		strcpy(addr1, "../../Logic_locking_paper_prep/code/");
		strcat(addr1, circuit_name);
		strcat(addr1, "/");
		
		strcpy(addr2,addr1);
		strcpy(addr3,addr1);
		strcpy(keyNodes,addr1);
		strcat(addr1, circuit_name);
		strcat(addr1, ".bench");
		strcat(addr2, "hope_output.txt");
		
		
		
		strcpy(command, "./hope -s 5 -r 100 -N -D ");
		strcat(command, addr1);
		strcat(command, " -l ");
		strcat(command, addr2);
		
		system(command);
		
		FILE *read_hope_output, *hope_key_node, *key;
		
		strcat(keyNodes,"keyNodes_sort");
		strcat(addr3,"hope_key_node.txt");			// File containg the list of nodes and their number of faults
		hope_key_node = fopen(addr3,"w");
		
		read_hope_output = fopen (addr2, "r");			// Opens the file hope_output.txt for parsing.
		//===================================================================================================//
		//Reading the data of the file keyNodes_sort
		key = fopen(keyNodes, "r");
		if( key == NULL )  
        	{	
      		perror ("Error opening keyNodes_sort: ");
      		return(-1);
   		}
   		
   		fseek(key, 0, SEEK_END);		// Shifting the cursor to the end of the file
        	long fsize = ftell(key);		// Stores the size of the entire file
        	fseek(key, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
        	char *string = (char*)malloc(fsize + 1);
        	fread(string, fsize, 1, key);
        	fclose(key);
        
        	string[fsize] = '\0';
        	// 'string' contains the contents of the file inNum as a properly 0-terminated C-string
        	// Now we need to parse the data of 'string'
        	
		NO_OF_NODE = read_keyNodes(string, fsize);
		
		// Printing all the nodes in the file keyNodes_sort
		for(int i=0; i<NO_OF_NODE; i++)
		printf("%s\n",keyNodes_sort[i]);
		//===================================================================================================//
		
		
		if( read_hope_output == NULL )  
        	{	
      		perror ("Error opening hope_output: ");
      		return(-1);
   		}
		
		int no_of_faults_detected;
		char temp[10000],buffer[10];
		char temp1[10000], temp2[10000], temp3[10000],temp4[10000];
		int index = 0;
		
	for(int z = 0; z<100; z++)	
	{	
		int r = rand();      // returns a pseudo-random integer between 0 and RAND_MAX
		strcpy(taddr,"./hope -s ");
		sprintf(buffer, "%d", r);
		strcat(taddr,buffer);
		strcat(taddr," -r 100 -N -D ");
		strcpy(command,taddr);
		strcat(command, addr1);
		strcat(command, " -l ");
		strcat(command, addr2);
			
		system(command);
		read_hope_output = fopen (addr2, "r");			// Opens the file hope_output.txt for parsing.
		
		
		for(int ii = 0; ii < number_of_test_pattern; ii++)
		{	

			no_of_faults_detected = 0;
			while(strcmp (temp, "faults") != 0)
			{
				strcpy(temp1, temp);				// temp1 stores the previous value of temp here
				fscanf(read_hope_output,"%s", temp);
				
			}
			no_of_faults_detected = atoi(temp1);			// Loop breaks when temp = faults. At that point temp1 stores the number of faults
			fscanf(read_hope_output,"%s\n", temp);
			//printf("no_of_faults_detected %d\n", no_of_faults_detected);
			for(int jj = 0; jj < no_of_faults_detected; jj++)	// For every test pattern we are iterating over the number of faults detected
			{
				fscanf(read_hope_output,"%s %s\n", temp, temp1);		// temp stores the name of the node and temp1 stores the type of fault
				
				
				// Now we take care of the special cases where we get the value of temp like node1->node2
				int foo = 0;
				memset(temp4, '\0', sizeof(temp4));
				while(foo < strlen(temp))
				{
					if(temp[foo] == '-' && temp[foo+1] == '>')
						break;
					temp4[foo] = temp[foo];
					foo++;	
				}
				
				memset(temp, '\0', sizeof(temp));
				strcpy(temp,temp4);
				
				//printf("\n%s\t%s", temp, temp1);
				
				if(strcmp(temp1, "/0") == 0)
				{
				
					int zero_flag = 0; 
					for(int kk = 0; kk < index; kk++)			// Checking whether the s-a-0 fault has occured before at any other node
					{
						if(strcmp(node_array[kk].name, temp) == 0)
						{
							node_array[kk].stuck_at_zero_count++;
							zero_flag = 1;
							break;
						}		
					}
					if(zero_flag == 0)						// s-a-0 fault has not ccured at this node and the list needs to be updated
					{
					
						strcpy(node_array[index].name, temp);
						node_array[index].stuck_at_zero_count++;
						index++;
					
					}
				
				}
				if(strcmp(temp1, "/1") == 0)						// Same procedure as the s-a-0 fault
				{
					int one_flag = 0; 
					for(int kk = 0; kk < index; kk++)
					{
						if(strcmp(node_array[kk].name, temp) == 0)
						{
							node_array[kk].stuck_at_one_count++;
							one_flag = 1;
							break;
						}		
					}	
					if(one_flag == 0)
					{
						strcpy(node_array[index].name, temp);
						node_array[index].stuck_at_one_count++;
						index++;
					
					}	
				
				}
			
			}
		}
		fclose(read_hope_output);
		
		for(int i = 0; i<index; i++)
		{	
			node_array[i].stuck_at_total = node_array[i].stuck_at_one_count + node_array[i].stuck_at_zero_count;
			
		}
		
		
		//printf("******************************************SORTED***************************\n");
		for(int i = 0; i<index; i++)
		{	
			printf("%s\t%d\t%d\t%d\n",node_array[i].name,node_array[i].stuck_at_zero_count,node_array[i].stuck_at_one_count,node_array[i].stuck_at_total);
		}
	}
	
	node_array_sort(index);
	node_final_select(index);
	for(int i = 0; i<min(200,index); i++)			// Printing the nodes in the file hope_key_node.txt. Change the value of index as per your need.
		{	
			//fprintf(hope_key_node,"Orig: %s\t%d\n",node_array[i].name,node_array[i].stuck_at_total);
			//fprintf(hope_key_node,"New : %s\t%d\n",node_final[i].name,node_final[i].stuck_at_total);
			fprintf(hope_key_node,"%s\n",node_final[i].name);
		}
		
	
	fclose(hope_key_node);
	
	
}


void initialize_node()
{
	for(int i = 0; i< MAX_NODE; i++)
	{
		node_array[i].index = i;
		node_array[i].stuck_at_zero_count = 0;
		node_array[i].stuck_at_one_count = 0;
	}

}

void swap_stuck_at_total(int i, int j)		// For swapping the size of node_array. Used for bubble sort in node_array_sort()
{
    int temp = node_array[i].stuck_at_total;
    node_array[i].stuck_at_total = node_array[j].stuck_at_total;
    node_array[j].stuck_at_total = temp;
}

void swap_stuck_at_zero_count(int i, int j)		// For swapping the size of node_array. Used for bubble sort in node_array_sort()
{
    int temp = node_array[i].stuck_at_zero_count;
    node_array[i].stuck_at_zero_count = node_array[j].stuck_at_zero_count;
    node_array[j].stuck_at_zero_count = temp;
}

void swap_stuck_at_one_count(int i, int j)		// For swapping the size of node_array. Used for bubble sort in node_array_sort()
{
    int temp = node_array[i].stuck_at_one_count;
    node_array[i].stuck_at_one_count = node_array[j].stuck_at_one_count;
    node_array[j].stuck_at_one_count = temp;
}

void swap_index(int i, int j)		// For swapping the index of node_array. Used for bubble sort in node_array_sort()
{
    int temp = node_array[i].index;
    node_array[i].index = node_array[j].index;
    node_array[j].index = temp;
}

void swap_name(int i, int j)		// For swapping the name of output array. Used for bubble sort in node_array_sort()
{
	char temp[30];
	memset(temp, '\0', sizeof(temp));
	strcpy(temp,node_array[i].name);
	memset(node_array[i].name, '\0', sizeof(node_array[i].name));
	strcpy(node_array[i].name, node_array[j].name);
	memset(node_array[j].name, '\0', sizeof(node_array[j].name));
	strcpy(node_array[j].name, temp);
}
// A function to implement bubble sort
void bubble_sort(int n)
{
   int i, j;
   
   for (i = 0; i < n-1; i++)      
   { 
       // Last i elements are already in place   
       for (j = 0; j < n-i-1; j++)
       { 
           if (node_array[j].stuck_at_total < node_array[j+1].stuck_at_total)
           {
              swap_stuck_at_total(j,j+1);
              swap_stuck_at_zero_count(j,j+1);
              swap_stuck_at_one_count(j,j+1);
              swap_name(j,j+1);
              swap_index(j,j+1);
              
           }
       }
   }
}

void node_array_sort(int n)		// Sorts the output array in the descending order of the number of nodes and takes the top n% of the outputs.
{
	bubble_sort(n);
	

} 

int read_keyNodes(char *str, long fsize)
{	

	char temp1[30];				// Temporary variable for storing the name of the node.
	memset(temp1, '\0', sizeof(temp1));	
	
	int k,j = 0;
	for(int i = 0; i <= fsize;)
	{	
	
		k=0;	
		while(str[i] != '\n' && str[i] != '\0')
		{
			temp1[k++] = str[i++];
		}
		
		strcpy(keyNodes_sort[j],temp1);
		j++;
		memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
		
		if(str[i] == '\0')
			break;
			
		i++;
		
							
	}
	return(j);
			
}
	
void node_final_select(int index)
{
	int flag = 0;
	for(int i=0; i< index; i++)	// Iterating over all the node_array
	{	
		
		for(int j = 0; j< NO_OF_NODE; j++)	// Iterating over all the keyNodes_sort
		{
			if(!strcmp(node_array[i].name,keyNodes_sort[j]))
			{
				node_final[flag].stuck_at_total = node_array[i].stuck_at_total;
				strcpy(node_final[flag++].name,node_array[i].name);
				break;
			}
		}
		//printf("flag = %d\n",flag);
		if(flag == min(200,index))		// We need only 200 final nodes or if total nodes are less than 200 then we take all of them.
			break;
	}
}
			





















