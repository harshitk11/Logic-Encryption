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
	
		FILE *read_hope_output, *write_zero, *write_one, *write_fault, *write_fault_list, *read_bench, *write_bench, *read_bench1, *read_bench2;
	
		read_hope_output = fopen ("s5378.txt", "r");
		//write_zero = fopen("s5378_zero_stat.txt","w");
		//write_one = fopen("s5378_one_stat.txt","w");
		write_fault = fopen("s5378_fault_stat.txt","w");
		//write_fault_list = fopen("s5378_fault_list.txt","w");
		int no_of_faults_detected;
		char temp[10000];
		char temp1[10000], temp2[500], temp3[100];
		int stuck_at_zero_index = 0;
		int stuck_at_one_index = 0;
		int max_fault_impact_value = 0;
		char max_fault_impact_gate[100];
	
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
	
		/*for(int ii = 0; ii < stuck_at_zero_index; ii++)
		{
			fprintf(write_zero,"%s\t%d\n", stuck_at_zero[ii], stuck_at_zero_count[ii]);
		}
		for(int ii = 0; ii < stuck_at_one_index; ii++)
		{
			fprintf(write_one,"%s\t%d\n", stuck_at_one[ii], stuck_at_one_count[ii]);
		}*/
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
					}
					fprintf(write_fault,"%s\t%d\n", stuck_at_zero[ii], stuck_at_zero_count[ii]);
					//fprintf(write_fault_list,"%s\t /0\n", stuck_at_zero[ii]);
				}
			}
			for(int jj = 0; jj< stuck_at_one_index; jj++)
			{
				if(stuck_at_one_count[jj] > max_fault_impact_value)
				{
					max_fault_impact_value = stuck_at_one_count[jj];
					strcpy(max_fault_impact_gate, stuck_at_one[jj]);	
				}
				fprintf(write_fault,"%s\t%d\n", stuck_at_one[jj], stuck_at_one_count[jj]);
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
					}
					fprintf(write_fault,"%s\t%d\n", stuck_at_one[ii], stuck_at_one_count[ii]);
					//fprintf(write_fault_list,"%s\t /1\n", stuck_at_one[ii]);
				}
			}
			for(int jj = 0; jj< stuck_at_zero_index; jj++)
			{
				if(stuck_at_zero_count[jj] > max_fault_impact_value)
				{
					max_fault_impact_value = stuck_at_zero_count[jj];
					strcpy(max_fault_impact_gate, stuck_at_zero[jj]);	
				}
				fprintf(write_fault,"%s\t%d\n", stuck_at_zero[jj], stuck_at_zero_count[jj]);
				//fprintf(write_fault_list,"%s\t /0\n", stuck_at_zero[jj]);
			}
		}
	
		printf("%s\t%d\n", max_fault_impact_gate, max_fault_impact_value);
	
	
		read_bench = fopen("s5378.bench","r");
		write_bench = fopen ("s5378_mod.bench","w");
		read_bench1 = fopen("s5378.bench","r");
		read_bench2 = fopen("s5378.bench","r");
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
		fprintf(write_bench,"\n");
		fprintf(write_bench,"key_out%d = XOR(%s, key_in%d)\n", current_key_no,max_fault_impact_gate, current_key_no);
	
	
	
		while(!feof(read_bench2))
		{
			fscanf(read_bench2,"%[^\n]\n", temp2);
			int ii = 0;
		
			
			
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
	
		current_key_no++;
	
	
	
	
	
	
		fclose(read_hope_output);
		//fclose(write_zero);
		//fclose(write_one);
		fclose(write_fault);
		//fclose(write_fault_list);
		fclose(read_bench);
		fclose(read_bench1);
		fclose(read_bench2);
		fclose(write_bench);
	
	
		read_bench = fopen("s5378_mod.bench","r");
		write_bench = fopen("s5378.bench","w");
		while(!feof(read_bench))
		{
			fscanf(read_bench, "%[^\n]\n", temp);
			fprintf(write_bench,"%s\n", temp);
		}
		fclose(read_bench);
		fclose(write_bench);
	}
}
