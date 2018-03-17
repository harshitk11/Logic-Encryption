   

    #include <stdio.h>
    #include <string.h>
    #include <stdint.h>
    #include <stdlib.h>
     

   /* int main()

    {

        // binaryval is the 128 bit binary value (in string) which is the input
        // hexadecimalval is the hexadecimal value of the input
        
        char binaryval[128];
     

        printf("Enter the binary number: ");

        

        scanf ("%[^\n]%*c", binaryval);
        //printf("sxx\n");
        uint8_t bin[16];int flag = 0;
        char temp[8];

        printf("Length of binary string = %d\n", (int)strlen(binaryval));

        for(int i = 0; i < (int)strlen(binaryval); i = i+8)
        {
            strncpy(temp, binaryval + i, 8);
            bin[flag++] = (uint8_t)strtol(temp, NULL, 2);
        }


        for (int i = 0; i< flag; i++)
            printf("%02x\n",bin[i]);
        
        printf("%d", flag);

        uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
        // corresponding binary = 00101011011111100001010100010110001010001010111011010010101001101010101111110111000101011000100000001001110011110100111100111100

        int length = sizeof(key)/sizeof(key[0]);

        /*for(int j = 0; j< length; j++)
        {
            printf("%d\n",key[j] );
        }
        */
       // return 0;

    //}
FILE *fp
void printBits(size_t const size, void const * const ptr);
//assumes little endian
int main(int argv, char* argc[])
{
        uint8_t i = 23;
        //uint ui = UINT_MAX;
        float f = 23.45f;
        printBits(sizeof(i), &i);
        //printBits(sizeof(ui), &ui);
        printBits(sizeof(f), &f);
        return 0;
}
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            fprintf(fp, "%u\t", byte);
        }
    }
    //puts("");
}


