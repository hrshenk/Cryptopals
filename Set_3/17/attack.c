#include <stdlib.h>
#include <stdio.h>


int attack()
{
    FILE *fp_in;
    //open the file to retrieve the encrypted token provided by create_token_server()
    fp_in = fopen("token.txt", "r");
    //determine length of token
    fseek(fp_in, 0, SEEK_END);
    int token_len = ftell(fp_in)/2;
    rewind(fp_in);
    //allocate space for the token
    unsigned char* token = calloc(token_len, sizeof(char));
    unsigned char* decrypted_token = calloc(token_len, sizeof(char));
    int i;
    //read the file into token
    for(i=0; i<token_len; i++)
    {
        fscanf(fp_in, "%2hhx", token+i);
    }
    fclose(fp_in);
    int block_count = token_len/16;
    int j, k, l, m, n;
    unsigned char iv[16], tamper_block[16], target_block[16], solution[16] = {0};
    FILE *fp_out;
    
    for(i=0; i<block_count-1; i++)
    {
        for(j=0; j<16; j++)
        {
            iv[j] = token[(16*i)+j];
            tamper_block[j] = iv[j];
            target_block[j] = token[(16*(i+1))+j];
            solution[j]=0;
        }

        for(j=0; j<16; j++)
        {
            for(k=0; k<j; k++)
            {
                tamper_block[15-k] = (solution[15-k]^iv[15-k])^(j+1);
            }
            
            for(k=0; k<255; k++)//k iterates over all possible bytes until we get the expected padding scheme.
            {
                if((i==(block_count-2) && j==0) && iv[15] == k) 
                {
                    puts("skip");
                    k++;
                }
                tamper_block[15-j]=k;
                fp_out = fopen("token.txt", "w");
                //write the tampered block to the file
                for(l=0; l<16; l++)
                {
                    fprintf(fp_out, "%02X", tamper_block[l]);
                }
                //write the target block to the file
                for(l=0; l<16; l++)
                {
                    fprintf(fp_out, "%02X", target_block[l]);
                }
                fclose(fp_out);
                if(accept_token_server())
                {
                    solution[15-j]=(j+1)^(k^iv[15-j]);
                    if(solution[15-j]==1)
                    {
                        puts("check");
                        printf("i: %d, j: %d\n", i, j);
                    }
                    break;
                }
            }
        }
        for(j=0; j<16; j++)
        {
            decrypted_token[((16*i)+j)] = solution[j];
        }
    }
    for(i=0; i<token_len-16; i++)
    {
        if(decrypted_token[i]<=16) printf("%02x", decrypted_token[i]);
        else printf("%c", decrypted_token[i]);
    }
    puts("");
    free(decrypted_token);
    free(token);
    return 0;
}