#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Enable both ECB and CBC mode. Note this can be done before including aes.h or at compile-time.
// E.g. with GCC by using the -D flag: gcc -c aes.c -DCBC=0 -DECB=1
#define CBC 0
#define ECB 1

#include "aes.h"


//static void phex(uint8_t* str);
static void test_encrypt_ecb(void);
static void test_decrypt_ecb(void);
uint8_t* binary_to_hex(char *str);
//static void hex_to_binary(void);
//static void test_encrypt_ecb_verbose(void);




int main(void)
{

#ifdef AES128
    printf("\nTesting AES128\n\n");
#elif defined(AES192)
    printf("\nTesting AES192\n\n");
#elif defined(AES256)
    printf("\nTesting AES256\n\n");
#else
    printf("You need to specify a symbol between AES128, AES192 or AES256. Exiting");
    return 0;
#endif

    
    
    test_decrypt_ecb();
    //test_encrypt_ecb();
    //test_encrypt_ecb_verbose();

    return 0;
}


// prints string as hex
/*static void phex(uint8_t* str)
{

#ifdef AES128
    uint8_t len = 16;
#elif defined(AES192)
    uint8_t len = 24;
#elif defined(AES256)
    uint8_t len = 32;
#endif

    unsigned char i;
    for(i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}*/

/*static void test_encrypt_ecb_verbose(void)
{
    // Example of more verbose verification

    uint8_t i, buf[64], buf2[64];

    // 128bit key
    uint8_t key[16] =        { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
    // 512bit text
    uint8_t plain_text[64] = { (uint8_t) 0x6b, (uint8_t) 0xc1, (uint8_t) 0xbe, (uint8_t) 0xe2, (uint8_t) 0x2e, (uint8_t) 0x40, (uint8_t) 0x9f, (uint8_t) 0x96, (uint8_t) 0xe9, (uint8_t) 0x3d, (uint8_t) 0x7e, (uint8_t) 0x11, (uint8_t) 0x73, (uint8_t) 0x93, (uint8_t) 0x17, (uint8_t) 0x2a,
                               (uint8_t) 0xae, (uint8_t) 0x2d, (uint8_t) 0x8a, (uint8_t) 0x57, (uint8_t) 0x1e, (uint8_t) 0x03, (uint8_t) 0xac, (uint8_t) 0x9c, (uint8_t) 0x9e, (uint8_t) 0xb7, (uint8_t) 0x6f, (uint8_t) 0xac, (uint8_t) 0x45, (uint8_t) 0xaf, (uint8_t) 0x8e, (uint8_t) 0x51,
                               (uint8_t) 0x30, (uint8_t) 0xc8, (uint8_t) 0x1c, (uint8_t) 0x46, (uint8_t) 0xa3, (uint8_t) 0x5c, (uint8_t) 0xe4, (uint8_t) 0x11, (uint8_t) 0xe5, (uint8_t) 0xfb, (uint8_t) 0xc1, (uint8_t) 0x19, (uint8_t) 0x1a, (uint8_t) 0x0a, (uint8_t) 0x52, (uint8_t) 0xef,
                               (uint8_t) 0xf6, (uint8_t) 0x9f, (uint8_t) 0x24, (uint8_t) 0x45, (uint8_t) 0xdf, (uint8_t) 0x4f, (uint8_t) 0x9b, (uint8_t) 0x17, (uint8_t) 0xad, (uint8_t) 0x2b, (uint8_t) 0x41, (uint8_t) 0x7b, (uint8_t) 0xe6, (uint8_t) 0x6c, (uint8_t) 0x37, (uint8_t) 0x10 };

    memset(buf, 0, 64);
    memset(buf2, 0, 64);

    // print text to encrypt, key and IV
    printf("ECB encrypt verbose:\n\n");
    printf("plain text:\n");
    for(i = (uint8_t) 0; i < (uint8_t) 4; ++i)
    {
        phex(plain_text + i * (uint8_t) 16);
    }
    printf("\n");

    printf("key:\n");
    phex(key);
    printf("\n");

    // print the resulting cipher as 4 x 16 byte strings
    printf("ciphertext:\n");
    for(i = 0; i < 4; ++i)
    {
        AES_ECB_encrypt(plain_text + (i*16), key, buf+(i*16), 16);
        phex(buf + (i*16));
    }
    printf("\n");
}*/


uint8_t* binary_to_hex(char *str)       // str contains the prompt for which 128 binary string (key/input)
{
  // Enter the 128 bit key/data in binary
  // binaryval is the 128 bit binary value (in string) which is the input
  // hexadecimalval is the hexadecimal value of the input
     
  char binaryval[128];  // Stores the 128 bit data (string) containing the binary numbers
   
  // Taking the input from the user
  printf("Enter the binary %s number: ", str);
  scanf ("%[^\n]%*c", binaryval);
     
  uint8_t *bin = (uint8_t*)malloc(16*sizeof(uint8_t));
  //uint8_t bin[16];
  int flag = 0;
  char temp[8];
  //printf("Length of binary string = %d\n", (int)strlen(binaryval));

  for(int i = 0; i < (int)strlen(binaryval); i = i+8)
   {
      strncpy(temp, binaryval + i, 8);
      bin[flag++] = (uint8_t)strtol(temp, NULL, 2);
   }

   return bin;
        
  //uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
  // corresponding binary = 00101011011111100001010100010110001010001010111011010010101001101010101111110111000101011000100000001001110011110100111100111100
}



static void test_encrypt_ecb(void)
{
#ifdef AES128
    //uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    uint8_t *key = binary_to_hex("key");
    uint8_t out[] = {0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97};
    // corresponding binary = 00111010110101110111101110110100000011010111101000110110011000001010100010011110110010101111001100100100011001101110111110010111
#endif

  uint8_t *in = binary_to_hex("INPUT");
  //uint8_t in[]  = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
  // corresponding binary = 01101011110000011011111011100010001011100100000010011111100101101110100100111101011111100001000101110011100100110001011100101010
  uint8_t buffer[16];

  AES_ECB_encrypt(in, key, buffer, 16);

  printf("ECB encrypt: ");

  if(0 == memcmp((char*) out, (char*) buffer, 16))
  {
    printf("SUCCESS!\n");
  }
  else
  {
    printf("FAILURE!\n");
  }
}






static void test_decrypt_ecb(void)
{
#ifdef AES128
    uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    uint8_t in[] = {0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97};
#endif

  uint8_t out[]  = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
  uint8_t buffer[16];

  AES_ECB_decrypt(in, key, buffer, 16);

  printf("ECB decrypt: ");

  if(0 == memcmp((char*) out, (char*) buffer, 16))
  {
    printf("SUCCESS!\n");
  }
  else
  {
    printf("FAILURE!\n");
  }
}

