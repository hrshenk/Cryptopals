#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "20.h"
#define MAX_STRING_LEN 1000
#define MAX_LINES 500

int main()
{
    FILE *fp_in = fopen("20.txt", "r");
    char temp[MAX_STRING_LEN] = {0};
    char cipher_matrix[MAX_LINES][2 * MAX_STRING_LEN];
    int  count, min = 0, i = 0, j,  row_count;
    
    //fill a matrix with the cipher texts
    while(fgets(temp, MAX_STRING_LEN, fp_in) && i < MAX_LINES)
    {
        if(temp[strlen(temp)-1] == '\n') temp[strlen(temp)-1] = '\0';
        count = base64_decode(temp, cipher_matrix[i]);
        if(count < min || min == 0) min = count;
        memset(temp, 0, MAX_STRING_LEN);
        ++i;
    }
    row_count = i;
    fclose(fp_in);
     //open a file and create a ciphertext equivalent to repeating key xor text
     //with key length min.
    FILE *fp_out = fopen("20_ciphertext.txt", "w+");
    for(i=0; i<row_count; ++i)
    {
        for(j=0; j<min; ++j)
        {
            fprintf(fp_out, "%2hhx", cipher_matrix[i][j]);
        }
    }
    
    //break repeating key xor
    puts("line 38 20.c0");
    unsigned char *key = (unsigned char *)malloc(min*sizeof(unsigned char));
    printf("\nmin/keylen = %d\n", min);
    rewind(fp_out);
    break_repeating_key_xor(fp_out, min, key);
    puts("If you can read these then we got the right key:\n\n");
    for(i=0; i<row_count; ++i)
    {
        for(j=0; j<min; ++j)
        {
            printf("%c", cipher_matrix[i][j] ^ key[i]);
        }
        printf("\n");
    }
    fclose(fp_out);
    return 0;
}