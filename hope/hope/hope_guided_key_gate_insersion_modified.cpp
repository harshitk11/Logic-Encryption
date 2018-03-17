#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define number_of_test_pattern 1000
#define key_size 128

char stuck_at_zero[50000][100];
char stuck_at_one[50000][100];
int stuck_at_zero_count[50000];
int stuck_at_one_count[50000];

int main()
{
	int current_key_no = 1;
	
	while(current_key_no <= key_size )
	{
	
		system("./hope -r 100 -N -D security.bench -l security.txt");
		FILE *read_hope_output, *write_zero, *write_one, *write_fault, *write_fault_list, *read_bench, *write_bench, *read_bench1, *read_bench2;
	
		read_hope_output = fopen ("security.txt", "r");
		//write_zero = fopen("security_zero_stat.txt","w");
		//write_one = fopen("security_one_stat.txt","w");
		//write_fault = fopen("security_fault_stat.txt","w");
		//write_fault_list = fopen("security_fault_list.txt","w");
		int no_of_faults_detected;
		char temp[10000];
		char temp1[10000], temp2[10000], temp3[10000];
		int stuck_at_zero_index = 0;
		int stuck_at_one_index = 0;
		int max_fault_impact_value = 0;
		char max_fault_impact_gate[100];
		char max_fault_impact_line[100];
		char max_fault_impact_gate_list[200][100];
		char output_list[5000][100];
		for(int ii = 0; ii< 50000; ii++)
		{
			stuck_at_zero_count[ii] = 0;
			stuck_at_one_count[ii] = 0;
		}
		for(int ii = 0; ii < number_of_test_pattern; ii++)
		{
			no_of_faults_detected = 0;
			while(strcmp (temp, "faults") != 0)
			{
				strcpy(temp1, temp);
				fscanf(read_hope_output,"%s", temp);
			}
			no_of_faults_detected = atoi(temp1);
			fscanf(read_hope_output,"%s\n", temp);
			//printf("no_of_faults_detected %d\n", no_of_faults_detected);
			for(int jj = 0; jj < no_of_faults_detected; jj++)
			{
				fscanf(read_hope_output,"%s %s\n", temp, temp1);
		
				if(strcmp(temp1, "/0") == 0)
				{
				
					int zero_flag = 0; 
					for(int kk = 0; kk < stuck_at_zero_index; kk++)
					{
						if(strcmp(stuck_at_zero[kk], temp) == 0)
						{
							stuck_at_zero_count[kk]++;
							zero_flag = 1;
							break;
						}		
					}
					if(zero_flag == 0)
					{
					
						strcpy(stuck_at_zero[stuck_at_zero_index], temp);
						stuck_at_zero_count[stuck_at_zero_index]++;
						stuck_at_zero_index++;
					
					}
				
				}
				if(strcmp(temp1, "/1") == 0)
				{
					int one_flag = 0; 
					for(int kk = 0; kk < stuck_at_one_index; kk++)
					{
						if(strcmp(stuck_at_one[kk], temp) == 0)
						{
							stuck_at_one_count[kk]++;
							one_flag = 1;
							break;
						}		
					}	
					if(one_flag == 0)
					{
						strcpy(stuck_at_one[stuck_at_one_index], temp);
						stuck_at_one_count[stuck_at_one_index]++;
						stuck_at_one_index++;
					
					}	
				
				}
			
			}
		}
		fclose(read_hope_output);
		
		/*for(int ii = 0; ii < stuck_at_zero_index; ii++)
		{
			fprintf(write_zero,"%s\t%d\n", stuck_at_zero[ii], stuck_at_zero_count[ii]);
		}
		for(int ii = 0; ii < stuck_at_one_index; ii++)
		{
			fprintf(write_one,"%s\t%d\n", stuck_at_one[ii], stuck_at_one_count[ii]);
		}
		
		fclose(write_zero);
		fclose(write_one);*/
		
		read_bench = fopen("security.bench","r");
		write_bench = fopen ("security_mod.bench","w");
		read_bench1 = fopen("security.bench","r");
		read_bench2 = fopen("security.bench","r");
		
		while(!feof(read_bench))
		{
			fscanf(read_bench,"%[^\n]\n", temp);
			if(temp[0] == '#')
			{
				fprintf(write_bench,"%s\n", temp);
				fscanf(read_bench2,"%[^\n]\n", temp2);
			}
			int ii;
			for(ii = 0; ii<5; ii++)
			{
				temp1[ii] = temp[ii];
			}
			temp1[ii] = '\0';
			if(strcmp(temp1, "INPUT") == 0)
			{
				fprintf(write_bench,"%s\n", temp);
			}
		
		}
		fclose(read_bench);
		fprintf(write_bench,"INPUT(key_in%d)\n", current_key_no);
		fprintf(write_bench,"\n");
		
		int output_count = 0;
		while(!feof(read_bench1))
		{
			fscanf(read_bench1,"%s", temp);
			fscanf(read_bench1,"%[^\n]\n", temp2);
			//printf("%s\n", temp);
			if(temp[strlen(temp)-1] == ')')
			{
				
				fscanf(read_bench2,"%[^\n]\n", temp2);
				int ii = 0;
				while(temp[ii] != '(')
				{
					temp1[ii] = temp[ii];
					ii++;
				}
				temp1[ii] = '\0';
				ii++;
				int jj =0;
				while(temp[ii] != ')')
				{
					temp3[jj] = temp[ii];
					jj++;
					ii++;
				}
				temp3[jj] = '\0';
				//printf("\n%s\n", temp3);
				if(strcmp(temp1, "INPUT") == 0)
				{
					continue;
				}
				if(strcmp(temp1, "OUTPUT") == 0)
				{
					fprintf(write_bench,"%s\n", temp);
					strcpy(output_list[output_count],temp3); 
					output_count++;
				}
			}	
			

		}
		fclose(read_bench1);
		fprintf(write_bench,"\n");
		
		//fclose(write_bench);
		//write_bench = fopen ("security_mod.bench","a");
		printf("\n %d\t%d\n", stuck_at_zero_index, stuck_at_one_index);
	
		//printf("\n rajit\n");
		
		
		for(int ii = 0; ii<stuck_at_zero_index ; ii++)
		{
			//int jj;
			//printf("\n *******%s\t%ld\n", stuck_at_zero[ii],strlen(stuck_at_zero[ii]));
			for(int jj = 0; jj< (strlen(stuck_at_zero[ii]) -3); jj++)
			{
				if(stuck_at_zero[ii][jj] == 'k' && stuck_at_zero[ii][jj+1] == 'e' && stuck_at_zero[ii][jj+2] == 'y' && stuck_at_zero[ii][jj+3] == '_')
				{
					stuck_at_zero_count[ii] = 0;
				}
				
			}
			
	
			for(int kk = 0; kk< current_key_no; kk++)
			{
				if(strcmp(stuck_at_zero[ii],max_fault_impact_gate_list[kk]) == 0)
				{
					stuck_at_zero_count[ii] = 0;
				}
			}
			
			for(int kk = 0; kk< output_count; kk++)
			{
				if(strcmp(stuck_at_zero[ii],output_list[kk]) == 0)
				{
					stuck_at_zero_count[ii] = 0;
				}
			}
		}
		
		
		for(int ii = 0; ii<stuck_at_one_index ; ii++)
		{
			int jj;
			
			
			for(int jj = 0; jj< (strlen(stuck_at_one[ii]) -3); jj++)
			{
				if(stuck_at_one[ii][jj] == 'k' && stuck_at_one[ii][jj+1] == 'e' && stuck_at_one[ii][jj+2] == 'y' && stuck_at_one[ii][jj+3] == '_')
				{
					stuck_at_one_count[ii] = 0;
				}
				
			}
			
			for(int kk = 0; kk< current_key_no; kk++)
			{
				if(strcmp(stuck_at_one[ii],max_fault_impact_gate_list[kk]) == 0)
				{
					stuck_at_one_count[ii] = 0;
				}
			}
			for(int kk = 0; kk< output_count; kk++)
			{
				if(strcmp(stuck_at_one[ii],output_list[kk]) == 0)
				{
					stuck_at_one_count[ii] = 0;
				}
			}
		}
		
		
		if(stuck_at_zero_index >= stuck_at_one_index)
		{
			for(int ii = 0; ii< stuck_at_zero_index; ii++)
			{
				int duplicate_flag = 0;
				for(int jj = 0; jj< stuck_at_one_index; jj++)
				{
					if(strcmp (stuck_at_zero[ii], stuck_at_one[jj]) == 0)
					{
						duplicate_flag = 1;
						stuck_at_one_count[jj] = stuck_at_one_count[jj] + stuck_at_zero_count[ii];
					}
				}
				if(duplicate_flag == 0)
				{
					if(stuck_at_zero_count[ii] > max_fault_impact_value)
					{
						max_fault_impact_value = stuck_at_zero_count[ii];
						strcpy(max_fault_impact_gate, stuck_at_zero[ii]);
						strcpy(max_fault_impact_gate_list[current_key_no - 1], stuck_at_zero[ii]);	
					}
					//fprintf(write_fault,"%s\t%d\n", stuck_at_zero[ii], stuck_at_zero_count[ii]);
					//fprintf(write_fault_list,"%s\t /0\n", stuck_at_zero[ii]);
				}
			}
			for(int jj = 0; jj< stuck_at_one_index; jj++)
			{
				if(stuck_at_one_count[jj] > max_fault_impact_value)
				{
					max_fault_impact_value = stuck_at_one_count[jj];
					strcpy(max_fault_impact_gate, stuck_at_one[jj]);
					strcpy(max_fault_impact_gate_list[current_key_no - 1], stuck_at_one[jj]);	
				}
				//fprintf(write_fault,"%s\t%d\n", stuck_at_one[jj], stuck_at_one_count[jj]);
				//fprintf(write_fault_list,"%s\t /1\n", stuck_at_one[jj]);
			}
		}
		else
		{
			for(int ii = 0; ii< stuck_at_one_index; ii++)
			{
				int duplicate_flag = 0;
				for(int jj = 0; jj< stuck_at_zero_index; jj++)
				{
					if(strcmp (stuck_at_one[ii], stuck_at_zero[jj]) == 0)
					{
						duplicate_flag = 1;
						stuck_at_zero_count[jj] = stuck_at_zero_count[jj] + stuck_at_one_count[ii];
					}
				}
				if(duplicate_flag == 0)
				{
					if(stuck_at_one_count[ii] > max_fault_impact_value)
					{
						max_fault_impact_value = stuck_at_one_count[ii];
						strcpy(max_fault_impact_gate, stuck_at_one[ii]);
						strcpy(max_fault_impact_gate_list[current_key_no - 1], stuck_at_one[ii]);	
					}
					//fprintf(write_fault,"%s\t%d\n", stuck_at_one[ii], stuck_at_one_count[ii]);
					//fprintf(write_fault_list,"%s\t /1\n", stuck_at_one[ii]);
				}
			}
			for(int jj = 0; jj< stuck_at_zero_index; jj++)
			{
				if(stuck_at_zero_count[jj] > max_fault_impact_value)
				{
					max_fault_impact_value = stuck_at_zero_count[jj];
					strcpy(max_fault_impact_gate, stuck_at_zero[jj]);	
					strcpy(max_fault_impact_gate_list[current_key_no - 1], stuck_at_zero[jj]);
				}
				//fprintf(write_fault,"%s\t%d\n", stuck_at_zero[jj], stuck_at_zero_count[jj]);
				//fprintf(write_fault_list,"%s\t /0\n", stuck_at_zero[jj]);
			}
		}
	
		printf("%s\t%d\n", max_fault_impact_gate, max_fault_impact_value);
	
	
		/*read_bench = fopen("security","r");
		write_bench = fopen ("security_mod.bench","w");
		read_bench1 = fopen("security","r");
		read_bench2 = fopen("security","r");
		while(!feof(read_bench))
		{
			fscanf(read_bench,"%[^\n]\n", temp);
			if(temp[0] == '#')
			{
				fprintf(write_bench,"%s\n", temp);
				fscanf(read_bench2,"%[^\n]\n", temp2);
			}
			int ii;
			for(ii = 0; ii<5; ii++)
			{
				temp1[ii] = temp[ii];
			}
			temp1[ii] = '\0';
			if(strcmp(temp1, "INPUT") == 0)
			{
				fprintf(write_bench,"%s\n", temp);
			}
		
		}
		fprintf(write_bench,"INPUT(key_in%d)\n", current_key_no);
		fprintf(write_bench,"\n");
		while(!feof(read_bench1))
		{
			fscanf(read_bench1,"%s", temp);
			fscanf(read_bench1,"%[^\n]\n", temp2);
			//printf("%s\n", temp);
			if(temp[strlen(temp)-1] == ')')
			{
				
				fscanf(read_bench2,"%[^\n]\n", temp2);
				int ii = 0;
				while(temp[ii] != '(')
				{
					temp1[ii] = temp[ii];
					ii++;
				}
				temp1[ii] = '\0';
				if(strcmp(temp1, "INPUT") == 0)
				{
					continue;
				}
				if(strcmp(temp1, "OUTPUT") == 0)
				{
					fprintf(write_bench,"%s\n", temp);
				}
			}	
			

		}
		fprintf(write_bench,"\n");*/
		
		int special_line_fault_flag = 0;
		int ll = 0;
		int mm = 0;
		for(int ii = 0; ii< (strlen(max_fault_impact_gate)); ii++)
		{
			if(special_line_fault_flag == 0)
			{
				temp1[mm] = max_fault_impact_gate[ii];
				mm++;
			}	
			if(max_fault_impact_gate[ii+1] == '-' && max_fault_impact_gate[ii+2] == '>')
			{
				printf("\n rajit\n");
				special_line_fault_flag = 1;
				ii = ii+3;
			}
			if(special_line_fault_flag == 1)
			{
				temp[ll] = max_fault_impact_gate[ii];
				ll++;
			}
		}
		if(special_line_fault_flag == 1)
		{
			temp[ll] = '\0';
			temp1[mm] = '\0';
			strcpy(max_fault_impact_gate, temp1);
			strcpy(max_fault_impact_gate_list[current_key_no - 1], temp1);
			strcpy(max_fault_impact_line, temp);
			printf("%s\t%s\t%d\n", max_fault_impact_gate, max_fault_impact_line, max_fault_impact_value);
		}
		
		fprintf(write_bench,"key_out%d = XOR(%s,key_in%d)\n", current_key_no,max_fault_impact_gate, current_key_no);
		
	
	
		while(!feof(read_bench2))
		{
			fscanf(read_bench2,"%[^\n]\n", temp2);
			int ii = 0;
		
			
			
			if(special_line_fault_flag == 0)
			{
				while(temp2[ii] != '(')
				{
					fprintf(write_bench,"%c", temp2[ii]);
					ii++;
				}
				ii++;
				fprintf(write_bench,"(");
				int jj = 0;
				while(temp2[ii] != '\n')
				{
			
					temp3[jj] = temp2[ii];
					jj++;
					ii++;
					if(temp2[ii] == ',')
					{	
						temp3[jj] = '\0';
						if(strcmp(temp3,max_fault_impact_gate) == 0)
						{
							fprintf(write_bench,"key_out%d,", current_key_no);
						}
						else
							fprintf(write_bench,"%s,", temp3);
						jj = 0;
						ii++;
				
					}	
					if(temp2[ii] == ')')
					{
						temp3[jj] = '\0';
						if(strcmp(temp3,max_fault_impact_gate) == 0)
						{
							fprintf(write_bench,"key_out%d)\n", current_key_no);
						}
						else
							fprintf(write_bench,"%s)\n", temp3);
						break;
					}
				}
			}	
			else
			{
				int ll = 0;
				while(temp2[ii] != ' ')
				{
					temp[ll] = temp2[ii];
					fprintf(write_bench,"%c", temp2[ii]);
					ii++;
					ll++;
				}
				temp[ll] = '\0';
				while(temp2[ii] != '(')
				{
					fprintf(write_bench,"%c", temp2[ii]);
					ii++;
				}
				ii++;
				fprintf(write_bench,"(");
				
				
					int jj = 0;
					while(temp2[ii] != '\n')
					{
			
						temp3[jj] = temp2[ii];
						jj++;
						ii++;
						if(temp2[ii] == ',')
						{	
							temp3[jj] = '\0';
							if(strcmp(temp3,max_fault_impact_gate) == 0)
							{
								if(strcmp(temp, max_fault_impact_line) == 0)
								{
									fprintf(write_bench,"key_out%d,", current_key_no);
								}
								else
									fprintf(write_bench,"%s,", temp3);
							}
							else
								fprintf(write_bench,"%s,", temp3);
							jj = 0;
							ii++;
				
						}	
						if(temp2[ii] == ')')
						{
							temp3[jj] = '\0';
							if(strcmp(temp3,max_fault_impact_gate) == 0)
							{
								if(strcmp(temp, max_fault_impact_line) == 0)
								{
									fprintf(write_bench,"key_out%d)\n", current_key_no);
								}	
								else
									fprintf(write_bench,"%s)\n", temp3);
							}
							else
								fprintf(write_bench,"%s)\n", temp3);
							break;
						}
					}

				
				
			}	
	
		}
	
		current_key_no++;
	
	
	
	
	
	
		//fclose(read_hope_output);
		
		//fclose(write_fault);
		//fclose(write_fault_list);
		//fclose(read_bench);
		//fclose(read_bench1);
		fclose(read_bench2);
		fclose(write_bench);
	
	
		read_bench = fopen("security_mod.bench","r");
		write_bench = fopen("security.bench","w");
		while(!feof(read_bench))
		{
			fscanf(read_bench, "%[^\n]\n", temp);
			fprintf(write_bench,"%s\n", temp);
		}
		fclose(read_bench);
		fclose(write_bench);
	}
}
