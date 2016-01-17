/*Synopsis:  
 * Program to solve Matasano Cryptography challenge 8.
 * Detects ECB encryption in the text file provided by the challenge.
 * If the kth line (counting from 0) is the ECB enrypted line then the function
 * returns k+1 and prints k+1 to the screen.
 * Note, this code has no general purpose use for detecting ECB encryption,
 * it is taylored specifically for the aforementioned challenge.
 * See http://cryptopals.com
 */

#include <stdio.h>
int main()
{//Declarations
	FILE *fpin;
	unsigned int i=0, j=0, k=0, l=0;
	unsigned char ch;
	//each line of the provided file has 10 blocks where each block is 16 bytes (aes block size)
	//so arr will be a matrix where each row is a block.  then we'll compare the rows and if we
	//see two matching rows it is very likely ECB was used to encrypt that line.
	unsigned char arr[10][16];
	
	
//Body
	fpin = fopen("8.txt", "r");
	
	//k iterates through the lines in our input file
	for(k=0; k<205; k++)
	{
		//i increments each time we read a full block of 16 bytes
		//so each time i increments we'll fill another row of the matrix
		for(i=0; i<10; i++)
		{
			//j increments each time we read a byte
			//so the j loop fills a row with a block
			for(j=0; j<16; j++)
			{
				fscanf(fpin, "%2hhx", &ch);
				arr[i][j] = ch;
			}
			
		}
		//at the end of a line read in the newline char, then test
		//it for validity to ensure we're doing this correctly.
		fscanf(fpin, "%c", &ch);
		if(ch != '\n')
		{
			printf("A problemo?  ch not newline\n");
		}

		//now that we've read a line into our matrix, we'll compare rows
		//of the matrix.
		for(l=0;l<9;l++)
		{
			for(i=l+1; i<10; i++)
			{
				j=0;
				while(arr[l][j] == arr[i][j] && j<16)
				{
					j++;
					
				}
				//if j reached 16 in the while loop then we know 16 bytes
				//were equal between the lth and ith row.  Hence we found
				//the animal we're after.  We'll print the line number of
				//the ECB encrypted line and return the
				if(j==16)
				{
					printf("%d\n", k+1);
					return (k+1);
				}
			}
		}
		
	}
	return 0;
	
}
