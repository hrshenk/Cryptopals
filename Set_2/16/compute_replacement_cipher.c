/*
This was written as a solution to the Matasano Crypto challenges challenge 16.  
See http://cryptopals.com  This is not production quality code and should never be used
in a production environment.

The program below was used to craft a modified ciphertext that will decrypt to a character array
containing the desired string ";admin=true;"  To exploit the vulnerability 32 'a' chars were submitted
as input to the create_token module.  I used the fact that I knew exactly what areas of the ciphertext
I have control of, but it isn't difficult to generalize the attack for when you don't know exactly what portion
of the ciphertext you control at the outset, at least if you're able to submit alot of input.  Anyhow, submitting
32 bytes gave me control over 2 adjacent ciphertext blocks.  Once the encryted token was generated, the
first of these encrypted blocks (hexencoded), the second plaintext block (here just 16 'a' chars), and the text we want to insert
were placed into a file named computation_parameters.txt.  This program reads in those three lines and xors them,
then prints the hexencoded result to stdout.  To execute the attack, replace the 1st encrypted block we have control
over with the output of this program and submit the new 'token' to the accept_token module for worldwide domination.
*/

#include <stdio.h>
#include <string.h>

int main()
{
    unsigned char array1[17], array2[17], array3[17], i;
    FILE *fp_in;
    if(!(fp_in = fopen("computation_parameters.txt", "r")))
    {
        puts("file open fail");
        return 0;
    }
    
    for(i=0; i<17; i++)
    {
        fscanf(fp_in, "%02X", array1+i);
    }
    array1[17] = 0;
    fgets(array2, 17, fp_in);
    for(i=0; i<16; i++)
    {
        array1[i] = array1[i]^array2[i];
    }
    fgets(array2, 17, fp_in);
    for(i=0; i<16; i++)
    {
        array1[i] = array1[i]^array2[i];
        printf("%02X", array1[i]);
    }
    puts("");
    return 0;
}