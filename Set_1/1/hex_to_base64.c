#include <stdio.h>
#include <stdlib.h>

int main()
{   /*  Note, this code will not work if certain conditions are not met on the HEX encoded string read
    in by the file.  In particular if there is an odd number of hex characters being read in then some
    padding of the hex with leading zeros may be necessary.  Also, if the number of bits read in is not
    a multiple of 6 then the code may fail to produce the desired outcome.  Put simply, this code was
    written to read in 96 hex encoded characters and convert them to base64 encoded characters*/

    //declare variables
    unsigned int hex;  //will hold hex values to convert to base64
    unsigned int i;  //declare a few iterators
    FILE *fpin;
    unsigned int base64_array[64];  //will be filled with base64 digits

    //fill in array with base 64 bit characters
    //I do this with a for loop, but it can be done manually one time.
    for(i=0; i<64; i++)
    {
        if(i<26)
        {
            base64_array[i] = (i+65);
        }
        if(26<=i && i<52)
        {
            base64_array[i] = i+71;
        }
        if(52<=i && i<62)
        {
            base64_array[i] = (i-4);
        }
        if(i==62)
        {
            base64_array[i] = '+';
        }
        if(i==63)
        {
            base64_array[i] = '/';
        }
    }
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//This is the body of the code.  Anything requiring thought occurs here.

    fpin = fopen ("c:\\users\\hayden\\documents\\c\\hex.txt", "r");  //get pointer to file containing HEX
    while (fscanf(fpin, "%6hhhhhhX", &hex) != EOF) //read hex encoded characters in 6 at a time.  This will give us 24
                                                //bits to encode to base64.
        {

            // >> is the bitwise shift operator.  Thanks Larry.  & is bitwise AND operator.
            printf("%c%c%c%c", base64_array[(hex>>18)&0x3F], base64_array[(hex>>12)&0x3F], base64_array[(hex>>6)&0x3F],base64_array[hex&0x3F]);


        }
        printf("\n");
    //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    return 0;
}
