#include <stdio.h>
#include <string.h>

int main()
{
    char token_s[400], converter[2], *dummy;
    unsigned char token_h[200];
    int i, j=0, len;
    puts("Enter your token:");
    fgets(token_s, 200, stdin);
    len = strlen(token_s);
    len = len-1;
    token_s[len]='\0';
    if(len % 2 != 0)
    {
        puts("invalid token");
        return 1;
    }
    else
    {
        for(i=0; i<len; i++)
        {
            converter[0]= token_s[i];
            i++;
            converter[1]= token_s[i];
            token_h[j] = strtoul(converter, &dummy, 16);
            printf("%02X", token_h[j]);
            j++;
            
        }
    }
    
    return 0;
}