#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

// INPUT in the command line is ./out <circuit_name> <no_of_key_gates>
// Make sure there is a file hope_key_node_final.txt in the folder <circuit_name>/<no_of_key_gates>/  which contains the list of key gates
// Make sure there is a file <circuit_name>_1xor.bench in the folder <circuit_name>/<no_of_key_gates>/  which contains the unencrypted bench file
int main(int argc, char * argv[])
{
	FILE *read_dff, *read_file, *write_file;
	
	char circuit_name[100];
	char arr[100], arr1[10];
	strcpy(circuit_name,argv[1]);
	int no_of_key = atoi(argv[2]);
	
	strcpy(arr,circuit_name);
	strcat(arr,"/");
	sprintf(arr1,"%d", no_of_key);
	strcat(arr, arr1);
	strcat(arr,"/hope_key_node_final.txt");
	
	//printf("*******%s\n", arr);
	
	read_dff = fopen(arr, "r");
	
	char encrypt_dff_list[no_of_key][20];
	int dff_count = 0;
	while(!feof(read_dff))
	{
		fscanf(read_dff, "%s\t",encrypt_dff_list[dff_count]);
		dff_count++;
	}
	fclose(read_dff);

	//for(int i=0;i<dff_count;i++)
	//	printf("%d\t%s\n",i,encrypt_dff_list[i]);
	
	//exit(0);
	int key_size = 1;
	int key_index = 0;
	int line_index = 1;
	while(key_size <= no_of_key)
	{
		char arr[100], arr1[10], arr2[100];
		strcpy(arr,circuit_name);
		strcat(arr,"/");
		sprintf(arr1,"%d", no_of_key);
		strcat(arr, arr1);
		strcat(arr,"/");
		strcat(arr,circuit_name);
		strcat(arr,"_");
		strcpy(arr2,arr);
		sprintf(arr1, "%d", key_size);
		strcat(arr, arr1);
		strcat(arr, "xor.bench");
		sprintf(arr1, "%d", key_size+1);
		strcat(arr2, arr1);
		strcat(arr2, "xor.bench");
		
		//printf("--------%s\n",arr );
		//printf("^^^^^^^^%s\n",arr2 );
		read_file = fopen(arr, "r");
		//read_file1 = fopen(arr, "r");
		//read_file2 = fopen(arr, "r");
		write_file = fopen (arr2, "w");
		int flag = 0;
		while(!feof(read_file))
		{
			fscanf(read_file,"%[^\n]\n", arr);
			int ii = 0;
			char temp[100];
			char temp1[100];
			char temp2[10][100];
			
			if(arr[ii] == '#')
			{
				fprintf(write_file,"%s\n",arr);
			}
			
			else if(arr[ii] == 'I' && arr[ii+1] == 'N' && arr[ii+2] == 'P' && arr[ii+3] == 'U' && arr[ii+4] == 'T')
			{
				fprintf(write_file,"%s\n",arr);
			}
			else if(arr[ii] == 'O' && arr[ii+1] == 'U' && arr[ii+2] == 'T' && arr[ii+3] == 'P' && arr[ii+4] == 'U' && arr[ii+5] == 'T')
			{
				if(flag == 0)
				{
					fprintf(write_file, "INPUT(key_%d)\n", key_index+1);
					flag = 1;
				}
				fprintf(write_file,"%s\n",arr);
			}
			else
			{
				
				int jj = 0;
				while(arr[ii]!= '=')
				{
					
					temp[jj] = arr[ii];
					ii++;
					jj++;
				}
				jj--;
				ii++;
				temp[jj] = '\0';
				//printf("\n%s\n", temp);
				if(strcmp(temp, encrypt_dff_list[key_index]) == 0)
				{
					
					fprintf(write_file,"%s\n",arr);
					fprintf(write_file, "key_out_%d = XOR(%s, key_%d)\n",key_index+1, temp, key_index+1);
					
					/*fprintf(write_file, "line%d = NOT(key_%d)\n", line_index, key_index+1);
					line_index++;
					fprintf(write_file, "line%d = NOT(%s)\n", line_index, temp);
					line_index++;
					fprintf(write_file, "line%d = AND(line%d, %s)\n", line_index,line_index-2, temp);
					line_index++;
					fprintf(write_file, "line%d = AND(key_%d, line%d)\n", line_index, key_index+1, line_index-2);
					line_index++;
					fprintf(write_file, "mux_out_%d = OR(line%d, line%d)\n", key_index+1, line_index-1, line_index-2);*/
				}

				
				else
				{
					fprintf(write_file, "%s =", temp);
					int kk = 0;
					while(arr[ii] != '(')
					{
						fprintf(write_file, "%c", arr[ii]);
						temp1[kk] = arr[ii];
						ii++;
						kk++;
					}
					temp1[kk] = '\0';
					fprintf(write_file, "%c", arr[ii]);
					ii++;
					int gate_input = 0;
					int ll = 0;
					while(arr[ii] != ')')
					{
						temp2[gate_input][ll] = arr[ii];
						ii++;
						ll++;
						if(arr[ii] == ',')
						{
							temp2[gate_input][ll] = '\0';
							if(strcmp (temp2[gate_input], encrypt_dff_list[key_index]) == 0)
							{
								fprintf(write_file, "key_out_%d, ", key_index+1);
							}
							else
							{
								fprintf(write_file,"%s, ", temp2[gate_input]);
							}
							gate_input++;
							ll = 0;
							ii = ii+2;
						}
					}
					temp2[gate_input][ll] = '\0';
					if(strcmp (temp2[gate_input], encrypt_dff_list[key_index]) == 0)
					{
						fprintf(write_file, "key_out_%d)\n", key_index+1);
					}
					else
					{
						fprintf(write_file,"%s)\n", temp2[gate_input]);
					}
				
				}
				
			}
		}
		fclose(read_file);
		fclose(write_file);
		key_size++;
		key_index++;
	}	
}
