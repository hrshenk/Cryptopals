/*Synopsis:  This program reads in a base64 encoded file and outputs the corresponding hex encoded file
*/

#include <stdio.h>
#include <stdlib.h>

#define file_in "base64_encoded.txt"
#define file_out "hex_encoded.txt"

int main()
{
    //declarations
    FILE *fpin, *fpout; //point to input and output files
    int i, j; //iterators
    int equal_count =0, flag; //will count how many '=' chars are used for padding, and will be set to one if valid char read
    unsigned int bitvector=0;  //base64 encoded chars will be read into ch.  We will use 24 bits of bitvector for conversions
    unsigned char ch, base64_array[64];  //Will be used to convert base64 chars to numbers


    //body
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
    //load file into memory
    fpin = fopen(file_in, "r");
    fpout = fopen(file_out, "w");

    //read base 64 char in, map it to corresponding number, shift bitvector and add that number to bitvector.
    //if ch does not map to a number in our table, then test if ch is '='  if so, shift bit vector and increment counter
    //if not, throw away ch (its likely a 'new line' character)
    i=0;
    while(fscanf(fpin,"%c", &ch)!=EOF)
    {
         flag = 0;
         for(j=0;j<64;j++)
            {
                if(base64_array[j] == ch) //map the base 64 encoded char to the number it represents
                {
                    bitvector = bitvector << 6;
                    bitvector += j;
                    i++;
                    flag = 1;
                }
            }
         if(flag == 0)  //if flag is 0 then ch didn't match an entry in our array and may not be a valid base64 encoded char.
                        //test to see if ch is '='  if not the simply read the next char.
         {
             if(ch == '=')  //if ch is padding then shift our bitvector and increment number of '=' chars found
             {
                 bitvector = bitvector << 6;
                 i++;
                 equal_count++;
                 flag=1;
             }
             else{printf("A non-valid base64 char was read with hex value %02x\ni has value %d\n", ch, i);}
         }
         if(((i!=0 && i%4) == 0)&&(flag == 1)) /*if 4 base64 encoded chars have been read then output the hex encoded chars
												only output when last char was valid*/
                            //if padding was detected, output as appropriate
         {
                fprintf(fpout, "%02X",((bitvector >> 16) & 0xFF));

                if(equal_count == 0 || equal_count == 1)
                {
                fprintf(fpout, "%02X",((bitvector >> 8) & 0xFF));
                }

                if(equal_count == 0)
                {
                fprintf(fpout, "%02X",((bitvector >> 0) & 0xFF));
                }
         }
    }
    fclose(fpin);
    fclose(fpout);
    return 0;
}
