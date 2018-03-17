#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#define MAX_INPUT 2000
#define MAX_OUTPUT 2000

int main(int argc, char *argv[])
{
	// INPUT IN THE COMMAND LINE WILL BE: ./out <circuit_name> <circuit_name.bench> <circuitname.v>
	FILE *read_bench, *write_verilog, *write_temp1, *write_temp2;
	
	char location1[100], location2[100];
	char circuit_name[100];
	strcpy(circuit_name,argv[1]);
	strcpy(location1,argv[1]);
	strcpy(location2,argv[1]);
	//strcat(location1,"/128/");		//For encrypted benches
	//strcat(location2,"/128/"); 
	strcat(location1,"/");			//For unencypted benches
	strcat(location2,"/");
	strcat(location1,argv[2]);
	strcat(location2,argv[3]);

	read_bench = fopen(location1,"r");
	write_verilog =fopen(location2,"w");
	write_temp1 = fopen("tempraryfile1.txt", "w");
	write_temp2 = fopen("tempraryfile2.txt", "w");
	char arr[10000];
	char input_list[MAX_INPUT][100];
	char output_list[MAX_OUTPUT][100];
	int no_of_input = 0;
	int no_of_output = 0;
	int gate_number = 1;
	int line_number = 1;
	int flag = 0;
	int wire_count = 0;
	fprintf(write_temp2,"module %s(clk", circuit_name);
	while(!feof(read_bench))
	{
		fscanf(read_bench,"%[^\n]\n", arr);
		int ii = 0;
		char temp[100];
		char temp1[100];
		char temp2[10][100];
		
		if(arr[ii] == '#')
		{
			continue;
		}
		if(arr[ii] == 'I' && arr[ii+1] == 'N' && arr[ii+2] == 'P' && arr[ii+3] == 'U' && arr[ii+4] == 'T')
		{
			while(arr[ii] != '(')
			{
				ii++;
			}
			ii++;
			int jj = 0;
			while(arr[ii] != ')')
			{
				temp[jj] = arr[ii];
				ii++;
				jj++;
			}
			temp[jj] = '\0';
			strcpy(input_list[no_of_input], temp);
			fprintf(write_temp2,", %s", temp);
			no_of_input++;

		}
		else if(arr[ii] == 'O' && arr[ii+1] == 'U' && arr[ii+2] == 'T' && arr[ii+3] == 'P' && arr[ii+4] == 'U' && arr[ii+5] == 'T')
		{
			while(arr[ii] != '(')
			{
				ii++;
			}
			ii++;
			int jj = 0;
			while(arr[ii] != ')')
			{
				temp[jj] = arr[ii];
				ii++;
				jj++;
			}
			temp[jj] = '\0';
			strcpy(output_list[no_of_output], temp);
			fprintf(write_temp2,", %s", temp);
			no_of_output++;

		}
		else
		{
			if(flag == 0)
			{
				fprintf(write_temp2,");\n");
				fprintf(write_temp2,"input clk");
				for(int mm = 0; mm < no_of_input; mm++)
				{
					fprintf(write_temp2,", %s", input_list[mm]);
				}
				fprintf(write_temp2,";\n");
				fprintf(write_temp2,"output ");
				fprintf(write_temp2,"%s", output_list[0]);
				for(int mm = 1; mm < no_of_output; mm++)
				{
					fprintf(write_temp2,", %s", output_list[mm]);
				}
				fprintf(write_temp2,";\n");
				fprintf(write_temp2,"wire clk");
				
				for(int mm = 0; mm < no_of_input; mm++)
				{
					fprintf(write_temp2,", %s", input_list[mm]);
				}
				
				
				flag = 1;
			}
			int jj = 0;
			while(arr[ii] != '=')
			{
				temp[jj] = arr[ii];
				ii++;
				jj++;
			}
			jj--;
			ii++;
			temp[jj] = '\0';
			if(wire_count == 0)
			{
				fprintf(write_temp2,";\n");
				fprintf(write_temp2,"wire ");
				fprintf(write_temp2,"%s",temp);
				wire_count++;
			}
			else if(wire_count <= 10)
			{
				fprintf(write_temp2,", %s",temp);
				wire_count++;
			}
			else
			{
				wire_count = 0;
				fprintf(write_temp2,";\n");
				fprintf(write_temp2,"wire ");
				fprintf(write_temp2,"%s",temp);
				wire_count++;
			}
			int kk = 0;
			while(arr[ii] != '(')
			{
				temp1[kk] = arr[ii];
				ii++;
				kk++;
			}
			temp1[kk] = '\0';
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
					gate_input++;
					ll = 0;
					ii = ii+2;
				}
			}
			temp2[gate_input][ll] = '\0';
			if(strcmp(temp1, " AND") == 0)
			{
				if(gate_input == 1)
				{
					fprintf(write_temp1,"AN2X1 gate%d(.O (%s), .I1 (%s), .I2 (%s));\n", gate_number, temp, temp2[0], temp2[1]);
					gate_number++;
				}
				if(gate_input == 2)
				{
					fprintf(write_temp1,"AN3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2]);
					gate_number++;
				}
				if(gate_input == 3)
				{
					fprintf(write_temp1,"AN4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
				}
				if(gate_input == 4)
				{
					fprintf(write_temp1,"AN4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"AN2X1 gate%d(.O (%s), .I1 (%s), .I2 (line%d));\n", gate_number, temp, temp2[4], line_number);
					gate_number++;
					line_number++;
				}
				if(gate_input == 5)
				{
					fprintf(write_temp1,"AN4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"AN3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (line%d));\n", gate_number, temp, temp2[4], temp2[5], line_number);
					gate_number++;
					line_number++;
				}
				if(gate_input == 6)
				{
					fprintf(write_temp1,"AN4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"AN4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (line%d));\n", gate_number, temp, temp2[4], temp2[5], temp2[6], line_number);
					gate_number++;
					line_number++;
				}
			}
			
			if(strcmp(temp1, " NAND") == 0)
			{
				if(gate_input == 1)
				{
					fprintf(write_temp1,"ND2X1 gate%d(.O (%s), .I1 (%s), .I2 (%s));\n", gate_number, temp, temp2[0], temp2[1]);
					gate_number++;
				}
				if(gate_input == 2)
				{
					fprintf(write_temp1,"ND3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2]);
					gate_number++;
				}
				if(gate_input == 3)
				{
					fprintf(write_temp1,"ND4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
				}
				if(gate_input == 4)
				{
					fprintf(write_temp1,"AN4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"ND2X1 gate%d(.O (%s), .I1 (%s), .I2 (line%d));\n", gate_number, temp, temp2[4], line_number);
					gate_number++;
					line_number++;
				}
				if(gate_input == 5)
				{
					fprintf(write_temp1,"AN4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"ND3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (line%d));\n", gate_number, temp, temp2[4], temp2[5], line_number);
					gate_number++;
					line_number++;
				}
				if(gate_input == 6)
				{
					fprintf(write_temp1,"AN4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"ND4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (line%d));\n", gate_number, temp, temp2[4], temp2[5], temp2[6], line_number);
					gate_number++;
					line_number++;
				}
			}
			
			if(strcmp(temp1, " OR") == 0)
			{
				if(gate_input == 1)
				{
					fprintf(write_temp1,"OR2X1 gate%d(.O (%s), .I1 (%s), .I2 (%s));\n", gate_number, temp, temp2[0], temp2[1]);
					gate_number++;
				}
				if(gate_input == 2)
				{
					fprintf(write_temp1,"OR3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2]);
					gate_number++;
				}
				if(gate_input == 3)
				{
					fprintf(write_temp1,"OR4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
				}
				if(gate_input == 4)
				{
					fprintf(write_temp1,"OR4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"OR2X1 gate%d(.O (%s), .I1 (%s), .I2 (line%d));\n", gate_number, temp, temp2[4], line_number);
					gate_number++;
					line_number++;
				}
				if(gate_input == 5)
				{
					fprintf(write_temp1,"OR4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"OR3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (line%d));\n", gate_number, temp, temp2[4], temp2[5], line_number);
					gate_number++;
					line_number++;
				}
				if(gate_input == 6)
				{
					fprintf(write_temp1,"OR4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"OR4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (line%d));\n", gate_number, temp, temp2[4], temp2[5], temp2[6], line_number);
					gate_number++;
					line_number++;
				}
			}
			
			if(strcmp(temp1, " NOR") == 0)
			{
				if(gate_input == 1)
				{
					fprintf(write_temp1,"NR2X1 gate%d(.O (%s), .I1 (%s), .I2 (%s));\n", gate_number, temp, temp2[0], temp2[1]);
					gate_number++;
				}
				if(gate_input == 2)
				{
					fprintf(write_temp1,"NR3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2]);
					gate_number++;
				}
				if(gate_input == 3)
				{
					fprintf(write_temp1,"NR4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
				}
				if(gate_input == 4)
				{
					fprintf(write_temp1,"OR4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"NR2X1 gate%d(.O (%s), .I1 (%s), .I2 (line%d));\n", gate_number, temp, temp2[4], line_number);
					gate_number++;
					line_number++;
				}
				if(gate_input == 5)
				{
					fprintf(write_temp1,"OR4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"NR3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (line%d));\n", gate_number, temp, temp2[4], temp2[5], line_number);
					gate_number++;
					line_number++;
				}
				if(gate_input == 6)
				{
					fprintf(write_temp1,"OR4X1 gate%d(.O (line%d), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, line_number, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
					fprintf(write_temp1,"NR4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (line%d));\n", gate_number, temp, temp2[4], temp2[5], temp2[6], line_number);
					gate_number++;
					line_number++;
				}
			}

			if(strcmp(temp1, " XOR") == 0)
			{
				if(gate_input == 1)
				{
					fprintf(write_temp1,"XOR2X1 gate%d(.O (%s), .I1 (%s), .I2 (%s));\n", gate_number, temp, temp2[0], temp2[1]);
					gate_number++;
				}
				if(gate_input == 2)
				{
					fprintf(write_temp1,"XOR3X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2]);
					gate_number++;
				}
				if(gate_input == 3)
				{
					fprintf(write_temp1,"XOR4X1 gate%d(.O (%s), .I1 (%s), .I2 (%s), .I3 (%s), .I4 (%s));\n", gate_number, temp, temp2[0], temp2[1], temp2[2], temp2[3]);
					gate_number++;
				}
			}
			
			if(strcmp(temp1, " NOT") == 0)
			{
				if(gate_input == 0)
				{
					fprintf(write_temp1,"INVX1 gate%d(.O (%s), .I (%s));\n", gate_number, temp, temp2[0]);
					gate_number++;
				}
			}	
			
			if(strcmp(temp1, " DFF") == 0)
			{
				if(gate_input == 0)
				{
					fprintf(write_temp1,"DFFX1 gate%d(.Q (%s), .QB (line%d), .D(%s), .CK(clk));\n", gate_number, temp, line_number, temp2[0]);
					gate_number++;
					line_number++;
				}
			}
		}
		
	}
	fprintf(write_temp2,";\n");
	
	wire_count = 0;
	for(int mm = 1; mm < line_number; mm++)
	{
		if(wire_count == 0)
		{
			fprintf(write_temp2,"wire ");
			fprintf(write_temp2,"line%d",mm);
			wire_count++;
		}
		else if(wire_count <= 10)
		{
			fprintf(write_temp2,", line%d",mm);
			wire_count++;
		}
		else
		{
			wire_count = 0;
			fprintf(write_temp2,";\n");
			fprintf(write_temp2,"wire ");
			fprintf(write_temp2,"line%d",mm);
			wire_count++;
		}
	}
	fprintf(write_temp2,";\n");
	fclose(read_bench);
	fclose(write_temp1);
	fclose(write_temp2);
	write_temp1 = fopen("tempraryfile1.txt", "r");
	write_temp2 = fopen("tempraryfile2.txt", "r");
	while(!feof(write_temp2))
	{
		fscanf(write_temp2,"%[^\n]\n", arr);
		fprintf(write_verilog, "%s\n", arr);
	}
	while(!feof(write_temp1))
	{
		fscanf(write_temp1,"%[^\n]\n", arr);
		fprintf(write_verilog, "%s\n", arr);
	}
	fprintf(write_verilog, "endmodule");
	fclose(write_temp1);
	fclose(write_temp2);
	fclose(write_verilog);
	
}
