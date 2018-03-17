#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_INPUT	1000
#define MAX_OUTPUT	3500
#define MAX_DFF		2000
#define MAX_NODE	60000
#define LARGE_NUMBER	5000000
#define NO_HOPE_KEYNODE 200
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })
     
     
struct  node{
        int     color;
        int     start_time;
        int     finish_time;
        char    type[30];               	// Type of node. Eg INPUT, OUTPUT
        char    name[100];              	// Name of the node (given in the benchmark file)
        int     no_of_input;            	// Stores the number of inputs to a particular node
        char    name_input[30][100];    	// Stores the name of all the nodes which act as inputs to the current node
        char    name_output[300][100];  	// Stores the name of all the nodes to which the output of the current node is connected
	int     index_of_input[30];           	// Stores the index (address) of all the nodes whose output are connected to the input of the current node.
	int     index_of_output[300];         	// Stores the index (address) of all the nodes to which the output of the current node is connected.
        //unsigned int     input_val_new[30];
	//unsigned int     input_val_old[30];
        unsigned int     output_val_new;
        unsigned int     output_val_prev;
        struct  node *ptr[1000];      		// Array of pointers 'ptr'. Each element in this array points to the struct node. Used for interconnections later. 
        int     no_of_output;
        int     order_evaluation;
        int     node_no;
};

//struct  node    node_array[60000];
struct  node *node_array = (struct node*) malloc (MAX_NODE * sizeof(struct node));  // Creating an array of nodes. Each node can be a DFF (or any gate [check])

int     count=0;
int     time1=0;
int     *ar = (int*) malloc(MAX_NODE * sizeof(int));
int     out=0;
char    *arr = (char*) malloc(LARGE_NUMBER * sizeof(char));
char	FILE_NM1[ 100 ] , FILE_NM2[ 100 ] , FILE_NM3[ 100 ] ;
char    output_name[MAX_OUTPUT][100];             	// Stores the name of the OUTPUT nodes as defined in the benchmark file.
int     output_node[MAX_OUTPUT];
int	NO_OF_INPUT , NO_OF_OUTPUT ;                 	// Stores the total number of inputs and outputs as defined in the benchmark file.
//char	DFF_list[MAX_DFF][100];
int	dff_count = 0;
char circuit_name[100]; 				// Stores the name of the circuit
//char     cone_of_dependency[MAX_DFF][MAX_OUTPUT];
struct tms buf1, buf2;

char hope_key_node[200][50];				// Stores the name of the key nodes read from the file hope_key_node.txt
//int NO_HOPE_KEYNODE;					// Stores the number of hope_keynode. Maximum 200.
FILE *input_num;

void    initilize_node();
void    make_graph(int start);
void    make_interconnect(char  str1[100],int    c);
void    topologicalsort();
void    dfs_visit(int   i);
void find_path(char* hope_key_node);
void read_hope_keynode();
int read_shortlisted_output();
int check_node(char *node, int size);
//void	dependency();

int     main( int argc, char *argv[] )
{
        	
        	int     start,i=0,j;
        	char    c;
       	 	struct  node    *ptr=NULL;
       	 	FILE    *fp;
        	times( &buf1);	
        	
        	char input_file[100];                              // input_file and FILE_NM1 store the name of the input file
        	strcpy(circuit_name, argv[1]);
        	strcpy(input_file, circuit_name);
        	strcat(input_file, "/");
        	strcat(input_file, circuit_name);
        	strcat(input_file, ".bench");
        	//printf("\n%s\n", input_file);
		//FILE *output_file;
		//output_file = fopen("graph_s9234.txt","w");
		
		read_hope_keynode();			// For reading the contents of the file hope_key_node.txt and storing it in the array hope_key_node[]
	
	strcpy( FILE_NM1 ,input_file);

	i = 0 ;

        initilize_node();
        fp=fopen(FILE_NM1,"r");      // Reading the contents of the input file
        if(!feof(fp))
        {
                do{
                   fscanf(fp,"%c",&arr[i]);  	 // Reading the contents of the input file into a single array arr
                   i++;
                   
                }while(!feof(fp));
               	arr[i]='\n';
               	i++;
                arr[i]='\0';               	// Appending NULL character since no null character is appended when using %c
                fclose(fp);
        }
        else
                perror("FF");
                
        i=0;
      
        while(arr[i]=='#'){             	// Skipping the first few lines which starts with '#' and contains information about FF, input, output etc.
                while(arr[i]!='\n')	
                        i++;
                i++;
        }
        while(arr[i++]=='\n');
        i--;
        start=i;
        make_graph(start);                	// 'start' store the startung index in array for creating the graph 
        
        // Since, we have made the graph, we now need to update the list of outputs from the file 'shortlisted_output'
       	NO_OF_OUTPUT = read_shortlisted_output();
       	
        free(arr);
        free(ar);
        
        for(int z = 0; z < NO_HOPE_KEYNODE; z++)			// Iterating over all the hope_key_node to find the dependency of each node in the list of hope_key_node 
	{
        	find_path(hope_key_node[z]);
        }
	//dependency();
	free(node_array);
}

void    initilize_node()   // Resetting the value of all nodes
{
        int     i,j;
        
        for(i=0;i<MAX_NODE;i++){
                node_array[i].color=0;              // Color is used for determining whether you've visited the node during graph traversal
                node_array[i].no_of_input=0;
                node_array[i].no_of_output=0;
                node_array[i].output_val_new=0;
                
        }
}

void    make_graph(int start)
{
        int     i,j,k,l,max_fan_in=0,max_fan_out=0;
        char    str1[100],str2[100];
        struct  node    *ptr=NULL;
        FILE    *fp;
  
  	// arr stores the contents of the input file (benchmark file)

	k = 0 ;     // 'k' stores the total number of inputs
        
        // Searching for the character 'I' in the array, which signifies the beginning of input node
        
	for(i=start;arr[i]=='I';)                         // Going through all the INPUTS and storing their 'TYPE' and 'NAME'
        {
                while(arr[i]!='\n')
                {
                        j=0;
                        while(arr[i]!='(')
                                str1[j++]=arr[i++];       // str1 stores the 'type' of the node
                        
                        i++;
                        str1[j]='\0';
                        j=0;
                        
                        while(arr[i]!=')')
                                str2[j++]=arr[i++];       // str2 stores the 'name' of the node
                        
                        i++;
                        str2[j]='\0';
                        strcpy(node_array[count].type,str1);                   
                        strcpy(node_array[count].name,str2);
                        node_array[count].node_no=count;
                        count++;
			                  
                        k++ ;
               }
               i++;
       }
               NO_OF_INPUT = k ; 
               // The first k [= NO_OF_INPUT] values in the node_array consists of INPUT nodes.      
	       //printf("\nCount =%d\n",count);

	l = 0 ;                                                // 'l' stores the total number of outputs
       	while(arr[i]=='O'||arr[i]=='\n')
       	{
                // Searching for the character 'O' in the array, which signifies the beginning of output node
                if(arr[i]=='O') 
                {
			l ++ ;

                        j=0;
                        while(arr[i++]!='(');

                        while(arr[i]!=')')
                                output_name[out][j++]=arr[i++];     // out = 0 initially. See line 42.
                        
                        output_name[out][j]='\0';                   // output_name is a  2D array (array of strings) which stores the name of each output node in a fresh row.
                        out++;
                        
                        while(arr[i]!='\n')
                                i++;
                }

                else
                        i++;
       	}
  	// Here output is not stored as a node in the node_array as the same output will repeat again in the future and
  	// this will lead to repetition.     	
	NO_OF_OUTPUT = l ;
	int bad_flag = 0;
       while(arr[i]!='\0')
       {
                while(1)
                {
                        if(bad_flag == 1)
                        {
                        	i--;
                        }	
                        j=0;
                        
                        while(arr[i]!='=')
                        {
                                //str1[j++]=arr[i++]; 
                                //printf("%c",arr[i]);
                                if(bad_flag == 0)
                                {
                                	
                                	str1[j++]=arr[i++];  
                                	bad_flag = 1;
                                }	
                                else
                                {	
                                	//i--;
                                	str1[j++]=arr[i++]; 
                                }	              // Here str1 is storing the name of the node as given in the benchmark file.
                        }
                        //printf("\n\n\n");
                        i=i+2;
                        j--;
                        str1[j]='\0';                               

                        j=0;
                        while(arr[i]!='(')
                                str2[j++]=arr[i++];                 // Here str2 is storing the type of node. eg DFF, NOT, AND etc
                        i++;
                        str2[j]='\0';

                        strcpy(node_array[count].type,str2);
                        strcpy(node_array[count].name,str1);
                        
                        node_array[count].node_no=count;
                        
                        node_array[count].no_of_input=1;          // No. of input to a node is 1 by default.
                        node_array[count].output_val_new=0;
                        j=k=0;
                        while(arr[i]!=')')
                        {
                                if(arr[i]==','){                        
                                        i=i+2;
                                        str1[j]='\0';              // str1 stores the name of the input to the node whose index = count
                                        j=0;
                                        strcpy(node_array[count].name_input[k++],str1);     //REQUIRES CHECKING (name_input is a 2D array)  
                                        node_array[count].no_of_input++;           	    // Incrementing the number of input by 1 for every ','. Refer the benchmark file.
                                }

                                else
                                        str1[j++]=arr[i++];        // storing the name of the input to a node in str (character wise) and then finally copying it to name_input in line 236 
                        }
                        str1[j]='\0';   // If there is a single input or for storing the name of the last input (after the',')
                        strcpy(node_array[count].name_input[k],str1);
                        
                        //printf("\nCount+++ =%d\t%s\n",count,node_array[count].name);
                        
                        
                        count++;
                       //printf("%d\n", count);
                       // printf("\ni before  is %d\n", i);
                        while(arr[i++]!='\n');
                       // printf("\ni next  is %d\n", j);
                        break;
                }
                i++;
       }
      /* for(i=0;i<count;i++){
       	printf("%s\n", node_array[i].name);
       }*/
       for(i=0;i<count;i++)
       {
                for(l=0;l<NO_OF_OUTPUT;l++)
                {         // Checking whether the output stored in output_name is there in node_array. If so, then its address is stored in output_node
                         if(!(strcmp(output_name[l],node_array[i].name)))       
                         {
                                output_node[l]=i;
                                //printf("%s\t%s\t%d\n", output_name[l],node_array[i].name,output_node[l]);
                                        break;
                        }
                }
       }
       
      /* for(i=0;i<count;i++)
       {
       		if(!(strcmp(node_array[i].type, "DFF")))
       		{
       			strcpy(DFF_list[dff_count],node_array[i].name);
       			dff_count++;
       		}
       }*/
       
      /* for(int ii = 0; ii< dff_count; ii++)
       {
       		for(int jj = 0; jj< NO_OF_OUTPUT; jj++)
       		{
       			cone_of_dependency[ii][jj] = '0';
       		}
       }*/
       //printf("Input no %d\t%s\n", NO_OF_INPUT,node_array[4].name);
       for(i=NO_OF_INPUT;i<count;i++)
                for(j=0;j<node_array[i].no_of_input;j++)  // j iterates over all the input to the i th node
                        make_interconnect(node_array[i].name_input[j],i); // Argument of the function is the name of the j th input to the i th node and the node number 'i'    
      
      
      topologicalsort();
      
      
        
      for(i=0;i<count;i++)
      {
                if(node_array[ar[i]].no_of_input>6)
                {
                        printf("large fanin");
                        exit(0);
                } 
               
      }
      
      
      
}

void    make_interconnect(char  str1[100], int c)  // Here str1 is the name of the input to the c th node.
{
        int     i,k;
        for(i=0;i<count;i++)
        {         
                 // Checking that the input 'str1' to node c is the output of which node and making the required connection 
                 if(!(strcmp(str1,node_array[i].name))) 
                 {              
                                // If we enter this loop, this means that str1 is the same as node i. 
                                // If the output of the i th node is the input to the c th node 
                                // no_of_output is initially set to 0 in the function initilize_node() and is incremented in line 320
                                node_array[i].ptr[node_array[i].no_of_output]=&node_array[c];  // Setting the pointer to point to node c
                                strcpy(node_array[i].name_output[node_array[i].no_of_output],node_array[c].name);  // Adding the name of node c to the list of outputs of node i
                                // Updating the address of the node c in the list containing the address of the nodes to which the output of the current node is connected to.
                                node_array[i].index_of_output[node_array[i].no_of_output]=c;    
                                node_array[i].no_of_output++;
                                // printf("%d\n",node_array[i].no_of_output );

                                // Storing the index (address) of all the nodes whose output are connected to the input of the current node (node c).
				                        for(k=0;k<node_array[c].no_of_input;k++)
	     				                      if(!(strcmp(str1,node_array[c].name_input[k]))){   // This line is basically finding the value of j in line 293
		  				                          node_array[c].index_of_input[k] = i;          
		  				                          // str1 is the k th input to the c th node. Here we are updating the index of the k th input (of node c) to 'i' i.e. the k th input to the c th node is coming from the i th node.
		  				                          //printf("%s-------->%d\n",node_array[c].name_input[k],node_array[c].index_of_input[k]);
		  				                          }
                                break;
                 }
	      }
}

void    topologicalsort()     // Doing topological sort of the nodes to get the dependency graph
{
        int i,j,temp1,temp2;
        for(i=0;i<count;i++){
               ar[i]=i;
               if(node_array[i].color==0)
                        dfs_visit(i);
        }
        for(i=count-1;i>=0;i--){
                for(j=1;j<=i;j++){
                        if(node_array[j-1].finish_time<node_array[j].finish_time){
                                temp1=node_array[j-1].finish_time;
                                temp2=ar[j-1];
                                node_array[j-1].finish_time=node_array[j].finish_time;
                                ar[j-1]=ar[j];                   
                                node_array[j].finish_time=temp1;
                                ar[j]=temp2;
                        }
                }
        }
       
}

void    dfs_visit(int   i)
{
        int     j;
        
        node_array[i].color=1;
        time1=time1+1;
        node_array[i].start_time=time1;
        for(j=0;j<node_array[i].no_of_output;j++)
                if(node_array[i].ptr[j]->color==0)       
                        dfs_visit(node_array[i].ptr[j]->node_no);
        time1=time1+1;
        node_array[i].finish_time=time1;
        node_array[i].color=2;
}



void find_path(char* hope_key_node1)
{
	// Topological sort has been done, so we have a dependency graph.
  	// (path_history) stores all the primary inputs in the cone of dependency of output.
  	// (dff_history) stores all the DFF in the cone of dependency of an output. 

	//FILE *path_history, *dff_history, *output_history, *input_num;
	//FILE *output_history, *input_num;
	
	char input_list[100];
	char affected_dff_list[100];
	char output_dependency[100];			// file name for storing the output cone of dependency
	char input_no[100];				// file name for storing the number of inputs in each output cone of dependency
	
	strcpy(input_list, circuit_name);
	strcat(input_list, "/");
	strcpy(affected_dff_list, input_list);

	//printf("\n I am here\n");
	//strcat(input_list, "inputs_affecting_each_output.txt");
	//strcat(affected_dff_list, "dffs_affecting_each_output.txt");
	
	
	//path_history = fopen(input_list,"w");
	//dff_history = fopen(affected_dff_list,"w");
	
	
	/*int **dff_dependency_matrix = (int **)malloc(dff_count * sizeof(int *));
	
	for(int i = 0; i < dff_count; i++)
	{
		dff_dependency_matrix[i] = (int *)malloc(dff_count * sizeof(int));
		
	}
	
	
	//int dff_dependency_matrix[dff_count][dff_count];
	for(int ii = 0; ii < dff_count; ii++)
	{
		for(int jj = 0; jj < dff_count; jj++)
		{
			dff_dependency_matrix[ii][jj] = 0;
		}
	}*/
	
	
	int inNum; // Stores the number of nodes in easch output cone of dependency
	memset(output_dependency, '\0', sizeof(output_dependency));
	memset(input_no, '\0', sizeof(input_no));
	strcpy(output_dependency, circuit_name);		// This creates a new directory inside <circuit_name>/dependency/
	strcat(output_dependency, "/dependency/");
	//strcat(output_dependency, hope_key_node);
	
	//struct stat st = {0};
	//if (stat(output_dependency, &st) == -1) 
	//{
	//	mkdir(output_dependency, 0700);
	//}
	
	strcpy(input_no, output_dependency);
	strcat(input_no, "inNum_" );
	strcat(input_no, hope_key_node1);
	printf("%s\n",input_no);
	input_num = fopen(input_no,"w");			// For storing the number of nodes in each output cone of dependency
	
	for(int ii = 0; ii < NO_OF_OUTPUT; ii++)  // This loop iterates over the array of strings 'output_name[]' which contains the list of output nodes.
	{	
		/*strcpy(output_dependency, circuit_name);		// This creates a new file (of the name of output) inside <circuit_name>/dependency/<hope_key_node>/
		strcat(output_dependency, "/dependency/");
		strcat(output_dependency, hope_key_node );
		
		strcat(output_dependency, "/" );		
		strcat(output_dependency, output_name[ii]);
		
		output_history = fopen(output_dependency,"w");
		printf("\n%s\n",output_dependency);
		if( output_history == NULL )  
        	{	
      		perror ("Error opening file: output_dependency: ");
      		exit(0);
   		}*/
				
		
		inNum = 0;		// Resetting the value of inNum for each cone
		
    		for(int i=0;i<count;i++)                // Resetting the color values of all the nodes so that all the nodes are unvisited.
		{
      			node_array[i].color=0;
    		}	
		
		/*for(int i=0;i<count;i++)                
		{
      			printf("%s\t%d\n",node_array[i].name,node_array[i].color);
    		}	
		*/
		
		char **track_path = (char **)malloc(MAX_NODE * sizeof(char *));   // Array of strings which contains the track path diverging from a specific output (ii th output)

		for(int i = 0; i < MAX_NODE; i++)
		{
			track_path[i] = (char *)malloc((100) * sizeof(char));
		}
		
		//char track_path[120000][100];
		int path_counter = 0;               // Stores the index which is used in the track_path
		for(int jj = 0; jj< count; jj++)
		{
			
			if(!(strcmp(output_name[ii],node_array[jj].name)))
			{
				strcpy(track_path[path_counter],node_array[jj].name);
				//fprintf(path_history,"%s\t====>", node_array[jj].name);
				//fprintf(dff_history,"%s\t====>", node_array[jj].name);
				//fprintf(output_history,"%s\t====>", node_array[jj].name);
				//printf("%s**\n",node_array[jj].name);
				path_counter++;
				break;
			}
			
		}
		
		// Shit storm. Please decode this at your own risk. It may lead to temporary loos of sanity.
		
		while(path_counter>0)
		{
			char temp[200];
			strcpy(temp,track_path[0]); 
			for(int mm = 0; mm <= path_counter-2; mm++)
			{
				strcpy(track_path[mm],track_path[mm+1]);
			}
			path_counter--;
			
			for(int jj = 0; jj < count; jj++)
			{	
				//if(check_node(node_array[jj].name, NO_HOPE_KEYNODE) != (-1))
				//{	//printf("Here\n");
				
				if(!(strcmp(temp,node_array[jj].name)) && strcmp(temp,hope_key_node1))
				{
					//printf("Here1**************\n");
					//fprintf(output_history,"%s\t", node_array[jj].name);     // Printing the output code of dependency
					//fprintf(output_history,"%s\n", node_array[jj].name);
					if(check_node(node_array[jj].name, NO_HOPE_KEYNODE) != (-1))
						inNum++;
					
					/*if(!(strcmp(node_array[jj].type, "DFF")))
					{
						fprintf(dff_history,"%s\t", node_array[jj].name);
						/*for(int aa = 0; aa < dff_count; aa++)
						{
							if(!(strcmp(DFF_list[aa],node_array[jj].name)))
							{
								dff_dependency_matrix[ii][aa] = 1;
								break;
							}
						}*/
							
					//printf("******************************************************************************/n");
					//printf("-----%s\t%d\n",node_array[jj].name,node_array[jj].no_of_input);
					if(node_array[jj].no_of_input > 0)
					{
						for(int kk = 0; kk < node_array[jj].no_of_input; kk++)
						{
							{
								//printf("\n++++%s\t\n", node_array[jj].name_input[kk]);
								//printf("\n****%s\t%d\n",node_array[node_array[jj].index_of_input[kk]].name,node_array[node_array[jj].index_of_input[kk]].color);
								
								if(node_array[node_array[jj].index_of_input[kk]].color == 0)
								{	
									
									if(path_counter>0)
									{
										for(int mm = path_counter-1; mm >= 0; mm--)
										{
											strcpy(track_path[mm+1],track_path[mm]);
										}		
									}
									strcpy(track_path[0], node_array[jj].name_input[kk]);
									//printf("@@@@%s\t", node_array[jj].name_input[kk]);
									
									path_counter++;
									node_array[node_array[jj].index_of_input[kk]].color = 1;
								}	
							}
						}
						
					
					}
					
					/*printf("\ntrack path \n");
					for (int z = 0; z<path_counter;z++)
						printf("%s\t",track_path[z]);
					printf("\n\n");
					*/
					
					else
					{
						//fprintf(path_history,"%s\t", node_array[jj].name);
						/*for(int xx = 0; xx< NO_OF_OUTPUT; xx++)
						{
							if(!(strcmp(node_array[jj].name, output_name[xx])))
							{
								cone_of_dependency[ii][xx] = '1';
							}
						}	*/
					}	
				}
				//}
			}

		}
		
		fprintf(input_num,"%s\t%d\n",output_name[ii], inNum);
		//fprintf(path_history,"\n\n\n");
		//fprintf(dff_history,"\n\n\n");
		//fprintf(output_history,"\n\n\n");		//Do not change this. Other codes will throw segmentation error.

		for(int i = 0; i < MAX_NODE; i++)
    		{
        		free(track_path[i]);
    		}
    		free(track_path);
		//fclose(output_history);
		
	}
	//fclose(path_history);
	//fclose(dff_history);
	fclose(input_num);
	//free(hope_key_node);
	//free(input_num);
	/*FILE *cone;
	cone = fopen("cone_of_dependency_s9234.txt","w");
	for(int ii = 0; ii< dff_count; ii++)
       	{
       		cone_of_dependency[ii][NO_OF_OUTPUT] = '\n';
       	}
	int duplicate[dff_count];
	int same_count[dff_count];
	for(int ii = 0; ii< dff_count; ii++)
       	{
       		duplicate[ii]= 0;
       		same_count[ii] = 0;
       	}
       	for(int ii = 0; ii< dff_count-1; ii++)
       	{
       		if(duplicate[ii]== 0)
       		{
	       		for(int jj = ii+1; jj< dff_count; jj++)
	       		{
	       			if(!(strcmp(cone_of_dependency[ii], cone_of_dependency[jj])))
	       			{
	       				duplicate[jj]= 1;
	       				same_count[ii]++;
	       			}
	       		}
	       	}	
       	}
       	int max_count = 0;
       	int max_count_index;
       	int dff_marker[dff_count];
       	for(int ii = 0; ii< dff_count; ii++)
       	{
       		dff_marker[ii] = 0;
       	}
       	for(int ii = 0; ii< dff_count; ii++)
       	{
       		if(same_count[ii] > max_count)
       		{
       			max_count = same_count[ii];
       			max_count_index = ii;
       		}
       	}
       	
       	
       	for(int ii = 0; ii< dff_count; ii++)
       	{
       		if(!(strcmp(cone_of_dependency[ii], cone_of_dependency[max_count_index])))
       		{
       			fprintf(cone,"%s\t", DFF_list[ii]);
       			dff_marker[ii] = 1;
       		}
       	}
       	fclose(cone);*/
       	
}

void read_hope_keynode()
{

	char input_file[100];                              // input_file stores the name of the file inNum.
        
        strcpy(input_file, circuit_name);
        
        strcat(input_file, "/hope_key_node.txt");
        printf("%s\n", input_file);
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

int read_shortlisted_output()
{

	char input_file[100];                              // input_file stores the name of the file inNum.
        
        strcpy(input_file, circuit_name);
        
        strcat(input_file, "/shortlisted_output");
        printf("%s\n", input_file);
        FILE    *fp;
        
        // Reading the file hope_key_node.txt to get the list of nodes 
        
        fp = fopen(input_file,"r");
        
        if( fp == NULL )  
        {
      		perror ("Error opening file: shortlisted_output ");
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
		
		strcpy(output_name[j],temp1);
		j++;
		memset(temp1, '\0', sizeof(temp1));	// Clearing the value of temp1
		
		if(string[i] == '\0')
			break;
			
		i++;
		
							
	}
	
	
	NO_OF_OUTPUT = j-1;
	for(int i = 0; i<NO_OF_OUTPUT; i++)			// Printing the nodes in the file hope_key_node.txt. Change the value of index as per your need.
		{	
			printf("%s\n",output_name[i]);
		}
	printf("output no = %d\n",NO_OF_OUTPUT);
	return NO_OF_OUTPUT;
}

int check_node(char *node, int size)		// Checks the presence of 'node' in the list of hope_key_node[] whose length is given by 'size' 
{
	// Function returns the index of the element in the array where the match is found. -1 is returned if no match is found.
	for(int i = 0; i<size ; i++)
	{
		if(!strcmp(node,hope_key_node[i]))
			{
				//printf("%d *** \n",i);
				return i;
			}
	}
	//printf("-1 *** \n");
	return -1;
}























