#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#define MAX_INPUT	100
#define MAX_OUTPUT	150
#define MAX_DFF		2000
#define MAX_NODE	40000
#define LARGE_NUMBER	5000000
struct  node{
        int     color;
        int     start_time;
        int     finish_time;
        char    type[30];
        char    name[100];
        int     no_of_input;
        char    name_input[30][100];
        char    name_output[300][100];
	int     index_of_input[30];  
	int     index_of_output[300]; 
        //unsigned int     input_val_new[30];
	//unsigned int     input_val_old[30];
        unsigned int     output_val_new;
        unsigned int     output_val_prev;
        struct  node *ptr[1000]; 
        int     no_of_output;
        int     order_evaluation;
        int     node_no;
};

//struct  node    node_array[60000];
struct  node *node_array = (struct node*) malloc (MAX_NODE * sizeof(struct node));

int     count=0;
int     time1=0;
int     *ar = (int*) malloc(MAX_NODE * sizeof(int));
int     out=0;
char    *arr = (char*) malloc(LARGE_NUMBER * sizeof(char));
char	FILE_NM1[ 100 ] , FILE_NM2[ 100 ] , FILE_NM3[ 100 ] ;
char    output_name[MAX_OUTPUT][100];
int     output_node[MAX_OUTPUT];
int	NO_OF_INPUT , NO_OF_OUTPUT ;
//char	DFF_list[MAX_DFF][100];
int	dff_count = 0;
char circuit_name[100]; 
//char     cone_of_dependency[MAX_DFF][MAX_OUTPUT];
struct tms buf1, buf2;


void    initilize_node();
void    make_graph(int start);
void    make_interconnect(char  str1[100],int    c);
void    topologicalsort();
void    dfs_visit(int   i);
void    find_path();
//void	dependency();

int     main( int argc, char *argv[] )
{
        	
        	int     start,i=0,j;
        	char    c;
       	 	struct  node    *ptr=NULL;
       	 	FILE    *fp;
        	times( &buf1);	
        	
        	char input_file[100];
        	strcpy(circuit_name, argv[1]);
        	strcpy(input_file, circuit_name);
        	strcat(input_file, "/");
        	strcat(input_file, circuit_name);
        	strcat(input_file, ".bench");
        	printf("\n%s\n", input_file);
		//FILE *output_file;
		//output_file = fopen("graph_s9234.txt","w");
	
	strcpy( FILE_NM1 ,input_file);

	i = 0 ;

        initilize_node();
        fp=fopen(FILE_NM1,"r");
        if(!feof(fp)){
                do{
                   fscanf(fp,"%c",&arr[i]);
                   i++;
                   
                }while(!feof(fp));
               	arr[i]='\n';
               	i++;
                arr[i]='\0';
                fclose(fp);
        }
        else
                perror("FF");
                
        i=0;
      
        while(arr[i]=='#'){
                while(arr[i]!='\n')
                        i++;
                i++;
        }
        while(arr[i++]=='\n');
        i--;
        start=i;
	
	printf("%s",arr);
	//printf("\n%d",start); 	
	make_graph(start); 
	
       	//printf("\n rajit\n"); 
        free(arr);
        free(ar);
	find_path();
	//dependency();
	free(node_array);
}

void    initilize_node()
{
        int     i,j;
        
        for(i=0;i<MAX_NODE;i++){
                node_array[i].color=0;
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
        
	k = 0 ;
	
        for(i=start;arr[i]=='I';){
                while(arr[i]!='\n'){
                        j=0;
                        while(arr[i]!='(')
                                str1[j++]=arr[i++];
                        i++;
                        str1[j]='\0';
                        j=0;
                        while(arr[i]!=')')
                                str2[j++]=arr[i++];
                        i++;
                        str2[j]='\0';
                        strcpy(node_array[count].type,str1);
                        strcpy(node_array[count].name,str2);
                        node_array[count].node_no=count;
                        count++;
			k ++ ;
               }
               i++;
       }
               NO_OF_INPUT = k ;        

	l = 0 ;
       while(arr[i]=='O'||arr[i]=='\n'){
                if(arr[i]=='O'){
			l ++ ;
                        j=0;
                        while(arr[i++]!='(');
                        while(arr[i]!=')')
                                output_name[out][j++]=arr[i++];
                        output_name[out][j]='\0';
                        out++;
                        while(arr[i]!='\n')
                                i++;
                }
                else
                        i++;
       }
      	
	NO_OF_OUTPUT = l ;
	
       while(arr[i]!='\0'){
                while(1){
                        i--;
                        j=0;
                        while(arr[i]!='=')
                        {
                                //printf("%c",arr[i]);
                                str1[j++]=arr[i++];
                        }
                        //printf("\n\n\n");
                        i=i+2;
                        j--;
                        str1[j]='\0';
                        j=0;
                        while(arr[i]!='(')
                                str2[j++]=arr[i++];
                        i++;
                        str2[j]='\0';
                        strcpy(node_array[count].type,str2);
                        strcpy(node_array[count].name,str1);
                        node_array[count].node_no=count;
                        
                        node_array[count].no_of_input=1;
                        node_array[count].output_val_new=0;
                        j=k=0;
                        while(arr[i]!=')'){
                                if(arr[i]==','){
                                        i=i+2;
                                        str1[j]='\0';
                                        j=0;
                                        strcpy(node_array[count].name_input[k++],str1);
                                        node_array[count].no_of_input++;
                                }
                                else
                                        str1[j++]=arr[i++];
                        }
                        str1[j]='\0';
                        strcpy(node_array[count].name_input[k],str1);
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
       for(i=0;i<count;i++){
                for(l=0;l<NO_OF_OUTPUT;l++)
                         if(!(strcmp(output_name[l],node_array[i].name))){
                                output_node[l]=i;
                                //printf("%s\t%s\t%d\n", output_name[l],node_array[i].name,output_node[l]);
                                        break;
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
       for(i=NO_OF_INPUT;i<count;i++)
                for(j=0;j<node_array[i].no_of_input;j++)
                        make_interconnect(node_array[i].name_input[j],i);     
      
      
      topologicalsort();   
      for(i=0;i<count;i++){
                if(node_array[ar[i]].no_of_input>6){
                        printf("large fanin");
                        exit(0);
                }
               
      }

      
}

void    make_interconnect(char  str1[100],int    c)
{
        int     i,k;
        for(i=0;i<count;i++){
                 if(!(strcmp(str1,node_array[i].name))){
                                node_array[i].ptr[node_array[i].no_of_output]=&node_array[c];
                                strcpy(node_array[i].name_output[node_array[i].no_of_output],node_array[c].name);
                                node_array[i].index_of_output[node_array[i].no_of_output]=c;
                                node_array[i].no_of_output++;
                               // printf("%d\n",node_array[i].no_of_output );
				for(k=0;k<node_array[c].no_of_input;k++)
	     				if(!(strcmp(str1,node_array[c].name_input[k])))
		  				node_array[c].index_of_input[k]=i;
                                break;
                        }
	}
}

void    topologicalsort()
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



void find_path()
{
	//printf("\n I am Rajit \n");
	//exit(0);
	FILE *path_history, *dff_history;
	char input_list[100];
	char affected_dff_list[100];
	strcpy(input_list, circuit_name);
	strcat(input_list, "/");
	strcpy(affected_dff_list, input_list);
	strcat(input_list, "inputs_affecting_each_output.txt");
	strcat(affected_dff_list, "dffs_affecting_each_output.txt");
	path_history = fopen(input_list,"w");
	dff_history = fopen(affected_dff_list,"w");
	
	
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
	
	for(int ii = 0; ii < NO_OF_OUTPUT; ii++)
	{
		for(int i=0;i<count;i++)
		{
                	node_array[i].color=0;
                }	
		
		char **track_path = (char **)malloc(MAX_NODE * sizeof(char *));

		for(int i = 0; i < MAX_NODE; i++)
		{
			track_path[i] = (char *)malloc((100) * sizeof(char));
		}
		
		//char track_path[120000][100];
		int path_counter = 0;
		for(int jj = 0; jj< count; jj++)
		{
			if(!(strcmp(output_name[ii],node_array[jj].name)))
			{
				strcpy(track_path[path_counter],node_array[jj].name);
				fprintf(path_history,"%s\t====>", node_array[jj].name);
				fprintf(dff_history,"%s\t====>", node_array[jj].name);
				//printf("%s**\n",node_array[jj].name);
				path_counter++;
				break;
			}
		}
		
		while(path_counter>0)
		{
			char temp[200];
			strcpy(temp,track_path[0]); 
			for(int mm = 0; mm<= path_counter-2; mm++)
			{
				strcpy(track_path[mm],track_path[mm+1]);
			}
			path_counter--;
			
			for(int jj = 0; jj< count; jj++)
			{
				if(!(strcmp(temp,node_array[jj].name)))
				{
					if(!(strcmp(node_array[jj].type, "DFF")))
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
					}
					if(node_array[jj].no_of_input > 0)
					{
						for(int kk = 0; kk < node_array[jj].no_of_input; kk++)
						{
							{
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
									
									path_counter++;
									node_array[node_array[jj].index_of_input[kk]].color = 1;
								}	
							}
						}
						
					}
					else
					{
						fprintf(path_history,"%s\t", node_array[jj].name);
						/*for(int xx = 0; xx< NO_OF_OUTPUT; xx++)
						{
							if(!(strcmp(node_array[jj].name, output_name[xx])))
							{
								cone_of_dependency[ii][xx] = '1';
							}
						}	*/
					}	
				}
			}

		}
		
		fprintf(path_history,"\n\n\n");
		fprintf(dff_history,"\n\n\n");
		free(track_path);
	}
	fclose(path_history);
	fclose(dff_history);
	
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



























