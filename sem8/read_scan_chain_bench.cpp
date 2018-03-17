// This program will read the can chain inserted verilog file and will encrypt the scan chain. <128 bit encryption> 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SDFF 1000		//May have to change for very large circuits
#define ENCRYPT_NO 128		//128 bit encryption

void insertionSort(int arr[], int n);

char circuit_name[100];
char SDFF_output_name[MAX_SDFF][50];
int NO_OF_SDFF; 			// Stores the actual number of scanff

int     main( int argc, char *argv[] )
{
	
	FILE *fp;

	char input_file[100],output_file[100];                              
    strcpy(circuit_name, argv[1]);
    strcpy(input_file,circuit_name);
    strcat(input_file,"/s5378_0_xor_encrypted_scan.v"); 
    strcpy(output_file,circuit_name);
    strcat(output_file,"/s5378_0_xor_encrypted_scan_encrypted.v");

  	fp = fopen(input_file,"r");		// Opening the file scan_chain_ckt.v 

	if( fp == NULL )  
    {
    	perror ("Error opening file: scan_chain_ckt.v");
    	exit(0);
 	}
      
    fseek(fp, 0, SEEK_END);		// Shifting the cursor to the end of the file
    long fsize = ftell(fp);		// Stores the size of the entire file
    fseek(fp, 0, SEEK_SET);  	// Shifting the cursor to the beginning of the file
    char *string = (char*)malloc(fsize + 1);
    char *new_string = (char*)malloc(2*fsize + 1);		//new_string will store the modified bench mark containing the scan encryption
    fread(string, fsize, 1, fp);
    fclose(fp);
      
    string[fsize] = '\0';
    // 'string' contains the contents of the file inNum_<NODE_NAME> as a properly 0-terminated C-string
    //*********************************************************************************************************************************//
    // First we have to get the list of outputs of scan flip flops which will be used in encryption later
    int sdff_flag = 0,k=0;
    char temp[50];
    for(int i=0; i<= fsize; i++)
    {
        if(string[i]=='.' && string[i+1]=='T' && string[i+2] == 'D' && string[i+3] == '(')
        {
            memset(temp, '\0', sizeof(temp));
            i=i+4;k=0;
            
            while(string[i]!=')')
            {
                temp[k++]=string[i++];
            }
            if(strcmp(temp,"test_si")!=0)
                strcpy(SDFF_output_name[sdff_flag++],temp);         //skipping the first test input which is "test_si"
        }


    }
    strcpy(SDFF_output_name[sdff_flag++],"test_so");
    NO_OF_SDFF = sdff_flag;     //Stores the actual number of scan flip flops

    /*for(int i=0;i<NO_OF_SDFF;i++)
        printf("%s\n",SDFF_output_name[i]);
    */
    //printf("********%d\n", NO_OF_SDFF);
    
    //*********************************************************************************************************************************//
    // We have to select 128 random flip flops from the list SDFF_output_name. Creating a set of non repeating random number
    int rand_no[ENCRYPT_NO], x = 0, rtemp,sel=0;
    memset(rand_no,0,sizeof(rand_no));
    while(sel<ENCRYPT_NO)
    {
        rtemp=rand()%NO_OF_SDFF;
        
        for (x = 0; x < sel; x++)
        {
            if(rand_no[x]==rtemp)
            {
                break;
            }
        }
        if(x==sel)
        {
            rand_no[sel++]=rtemp;
        }
    }

    // rand_no contains the location of the d flip flops which have to be encrypted
    
    
    //insertionSort(rand_no, ENCRYPT_NO);
    
    int rand_temp[ENCRYPT_NO];
       
    for(int y=0;y<ENCRYPT_NO;y++)
    {
        printf("%d\n", rand_no[y]);
        //rand_temp[y]=rand_no[y];
    }
    //exit(0);
    //*********************************************************************************************************************************//
    // Making a list of affecting flip flops (i.e. the flip flops leving the ones in the array rand_no[])
    int *affect_ff = (int*)malloc((NO_OF_SDFF-ENCRYPT_NO)*sizeof(int));
    int size_affect_ff = NO_OF_SDFF-ENCRYPT_NO;
    
    k=0;
    for(int j=0;j<NO_OF_SDFF; j++)    
    {
        int flag=0;
        for(int i=0;i<ENCRYPT_NO; i++)    
        {
            if(rand_no[i]==j)
            {
                //printf("**%d\n",j);
                flag=1;
                break;
            }
        }
        if(flag==0)
            affect_ff[k++] = j;
    }

    /*for(int i=0,k=0;i<ENCRYPT_NO-1;i++)
    {
        while(rand_temp[i+1]!=rand_temp[i]+1)
        {
            affect_ff[k++] = rand_temp[i]+1;
            rand_temp[i] = rand_temp[i]+1;
            //printf("%d\n", affect_ff[k-1]);     
        }
    }*/
    
    printf("Size: %d\t%d\n", size_affect_ff,k);
    for(int y=0;y<(NO_OF_SDFF-ENCRYPT_NO);y++)
    {
        printf("%d\n", affect_ff[y]);
    }
    
    //*********************************************************************************************************************************//
    // We start writing the modified bench in the output FILE
    int i,index=0;
    
    memset(new_string, '\0', sizeof(new_string));	// Clearing the value of new_string
    int line_flag=0;                    //For changing the line
    // First we have to insert the test keys in the module definition.
    for(i = 0,k = 0; i<=fsize;)
    {

		if(string[i]==')' && string[i+1]==';')		//Checking for the end of module definition
		{
			i=i-1;
			for (int j=1;j<=128;j++)
			{
				/*line_flag++;
                if(line_flag==5)
                {
                    strcat(new_string,"\n\t");
                    line_flag=0;
                }*/

                memset(temp, '\0', sizeof(temp));	// Clearing the value of temp
				sprintf(temp, ", test_key_%d", j);
				strcat(new_string, temp);
                
			}
			strcat(new_string, ");");
			break;									// Once test_key values are inserted in the module definition, then we break out of the loop	
			
		}

		else new_string[k++] = string[i++];
    } 

    index = i+3;		//The pointer is now at the space after the ";" which is at the end of module defintion
    line_flag=0;
    //printf("%c\n",string[index]);
    //Now we have to insert the test keys in the input definition
    k = strlen(new_string);
    for(i = index;i<=fsize;)
    {	
    	if(string[i]==';')		//Checking for the end of input definition
		{
			
			for (int j=1;j<=128;j++)
			{
				/*line_flag++;
                if(line_flag==5)
                {
                    strcat(new_string,"\n\t");
                    line_flag=0;
                }*/

                memset(temp, '\0', sizeof(temp));	// Clearing the value of temp
				sprintf(temp, ", test_key_%d", j);
				strcat(new_string, temp);
			}
			strcat(new_string, ";");
			break;									// Once test_key values are inserted in the input definition, then we break out of the loop	
			
		}

		else new_string[k++] = string[i++];

    }

    index = i+1;	//The pointer is now at the space after the ";" which is at the end of input defintion
    int flag=1;
    line_flag=0;
    // Now we have to insert the wires of the encyption structure in wire definition
    k = strlen(new_string);
    for(i = index;i<=fsize;)
    {	
    	while(flag)		//Copying the output definition codes exactly as it is
    		{
    			new_string[k++] = string[i++];
    			if(string[i]=='w' && string[i+1]=='i' && string[i+2]=='r' && string[i+3]=='e')	//This means you've reached the end of output definition
    				{
    					//printf("\n*********************\n%d\n",i);
    					flag=0;
    				}
    		}


    	if(string[i]==';')		//Checking for the end of wire definition
		{
			
			for (int j=1;j<=128;j++)
			{
				/*line_flag++;
                if(line_flag==1)
                {
                    strcat(new_string,"\n\t");
                    line_flag=0;
                }*/

                memset(temp, '\0', sizeof(temp));	// Clearing the value of temp
				sprintf(temp, ", int_xnor_or_%d", j);
				strcat(new_string, temp);

				memset(temp, '\0', sizeof(temp));	// Clearing the value of temp
				sprintf(temp, ", int_or_and_%d", j);
				strcat(new_string, temp);

				memset(temp, '\0', sizeof(temp));	// Clearing the value of temp
				sprintf(temp, ", int_and_sff_%d", j);
				strcat(new_string, temp);

			}
			strcat(new_string, ";");
			break;									// Once test_key values are inserted in the wire definition, then we break out of the loop	
			
		}

		else new_string[k++] = string[i++];

    }
//***********************************************************************************************************************************//
    //Encrypting the flip flops
    index = i+1;    //The pointer is now at the space after the ";" which is at the end of wire defintion
    k = strlen(new_string);
    flag=1;
    int dff_index=-1, encrypt_flag=0, end_flag=1;                 //Stores the index of the dff number
    int encrypt_index=0;
    for(i = index;i<=fsize;)
    {   
        while(flag)     //Copying the codes after wire definition as it is, until SDFF is reached
            {
                new_string[k++] = string[i++];
                if(string[i]=='D' && string[i+1]=='F' && string[i+2]=='Z' && string[i+3]=='X' && string[i+4]=='1')  //This means you've reached the end of output definition
                    {
                        flag=0;
                    }
            }
        while(dff_index<(NO_OF_SDFF-1))
        {    
            //Checking whether the flip flop is to be encypted or not
            printf("------------%d\n",dff_index);
            if(string[i]=='D' && string[i+1]=='F' && string[i+2]=='Z' && string[i+3]=='X' && string[i+4]=='1')
            {
                dff_index++;
                encrypt_flag=0;
                end_flag=1;
                //printf("------------%d\n",dff_index);
                
                for(int j=0;j<ENCRYPT_NO;j++)
                {
                    if (dff_index==rand_no[j])
                    {
                         encrypt_flag = 1;
                         //printf("*********%d\n", rand_no[j]);
                    }
                }
                while(end_flag)
                {   
                    if(encrypt_flag==1 && string[i]=='.' && string[i+1]=='S' && string[i+2]=='E' && string[i+3]=='L')
                    {
                        sprintf(temp,".SEL(int_and_sff_%d)",rand_no[encrypt_index++]);                                                  //*****************************//
                        strcat(new_string, temp);
                        k = strlen(new_string);
                        i=i+13;     //skipping the test_se part
                    }
                    else if(string[i] == ';')
                    {   
                        new_string[k++] = string[i++];
                        end_flag=0;
                    }
                    else new_string[k++] = string[i++];
    
                }
            }
            
            else new_string[k++] = string[i++];
        
        }
    break;    

    }

//*******************************************************************************************************************************//
    //Adding the XNOR and the AND gates of scan encryption modules
    strcat(new_string,"\n");
    k = strlen(new_string);
    
    index = i;
    char temps1[100], temps2[100], temps3[100];
    for(int i=0; i<ENCRYPT_NO; i++)
    {
        sprintf(temps1,"XNR2X1 scan1gate%d(.O (int_xnor_or_%d), .I1 (key_%d), .I2 (test_key_%d));\n", i,i,i,i);
        sprintf(temps2,"AN2X1 scan3gate%d(.O (int_and_sff_%d), .I1 (test_se), .I2 (int_or_and_%d));\n", i,i,i);
        strcat(new_string,temps1);
        strcat(new_string,temps2);

    }

    //Adding the OR gates of scan encryption module
    int affect_ff_index = 0;
    for(int i=0; i<ENCRYPT_NO; i++)
    {
        sprintf(temps1,"OR2X1 scan2gate%d(.O (int_or_and_%d), .I1 (int_xnor_or_%d), .I2 (%s));\n", i,i,i,SDFF_output_name[affect_ff[affect_ff_index++]]);
        strcat(new_string,temps1);
        
        if(affect_ff_index == size_affect_ff)
            affect_ff_index=0;
    }
    // Copying the rest of the bench file as it is
    k = strlen(new_string);
    for(i = index;i<fsize;)
        new_string[k++] = string[i++];
//*******************************************************************************************************************************//    
    //printing the list
    FILE *fk;
    fk = fopen(output_file,"w");

    //for(int i=0; i<NO_OF_SDFF; i++)
    //	fprintf(fk,"%s\n",output_name[i]);

    for(int i=0; i<strlen(new_string); i++)
    	fprintf(fk,"%c",new_string[i]);

    fclose(fk);
    return 0;	   

 }


 /* Function to sort an array using insertion sort*/
void insertionSort(int arr[], int n)
{
   int i, key, j;
   for (i = 1; i < n; i++)
   {
       key = arr[i];
       j = i-1;
 
       /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
       while (j >= 0 && arr[j] > key)
       {
           arr[j+1] = arr[j];
           j = j-1;
       }
       arr[j+1] = key;
   }
}