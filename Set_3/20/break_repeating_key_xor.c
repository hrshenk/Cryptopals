#include <stdio.h>
#include <stdlib.h>

//declarations
unsigned char solve_for_key_byte(char **matrix, unsigned int row_count, unsigned int column);
int break_repeating_key_xor(FILE *fp_ciphertext, unsigned int key_len, unsigned char *key);

int break_repeating_key_xor(FILE *fp_ciphertext, unsigned int key_len, unsigned char *key)
{
    //determine file length
    fseek(fp_ciphertext, 0, SEEK_END);
    unsigned int file_len = ftell(fp_ciphertext);
    rewind(fp_ciphertext);
    //file expected to be formatted as hex encoded bytes, so actual # of bytes read in is halfed
    file_len = file_len / 2;
    unsigned int row_count = file_len / key_len;
    //build a matrix s.t. each column is comprised of text xor with same byte
    char **row = (char **)malloc(row_count * sizeof(char *));
    if(row == NULL) return -1;
    int i;
    for(i=0; i<row_count; ++i)
    {
        row[i] = (char *)malloc(key_len);
        if(row[i] == NULL) return -1;
        row[i][0] = 1;
    }
    //read from the file into our matrix
    int j;
    unsigned int r;
    for(i=0; i<row_count; ++i)
    {
        for(j=0; j<key_len; ++j)
        {
            fscanf(fp_ciphertext, "%02x", &r);
            row[i][j] = (unsigned char)r;
            
        }
    }
    //solve for each byte of the key
    for(i=0; i<key_len; ++i)
    {
        
        key[j] = solve_for_key_byte(row, row_count, i);
    }
    return 0;
}

unsigned char solve_for_key_byte(char **matrix, unsigned int row_count, unsigned int column)
{
   unsigned char ret;
   unsigned int i, j, score, max_score = 0;
   for(i=0; i<256; i++)
   {
       score = 0;
       //get the score for the current column/byte
       for(j=0; j<row_count; ++j)
       {
           switch(matrix[j][column]^i)
           {
                case ' ':
                    score += 14;
                case 'e':
                    score += 12;
                case 't':
                    score += 9;
                case 'a':
                    score += 8;
                case 'o':
                    score += 7;
                case 'i':
                    score += 7;
                case 'n':
                    score += 6;
                case 's':
                    score += 6;
                case 'r':
                    score += 6;
                
           }
           
       }
       if(score > max_score)
           {
               max_score = score;
               ret = i;
            }
      
   }
   return ret;
}
