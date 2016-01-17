#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	FILE *fpin;
	unsigned int  file_length = 0, i, flag;
	unsigned char ch;
	//we will read the file specified by file_path into array.
	unsigned char *array;
	char file_path[150] = {0};
	
	
	//prompt the user for which operation will be perfomed, 
	//i.e., encryption or decryption
	printf("Enter 0 to decrypt or 1 to encrypt\n");
	scanf("%d", &flag);  //is scanf safe to use in this situation?
	//test for valid input.  flag should be 0 or 1.
	if(!(flag ==0 || flag ==1))
	{
		perror("Invalid entry");
		return -1;
	}	
	//prompt the user for the input file path
	printf("enter path to input file\n");
	fgets(file_path, 150, stdin);
	
	//fgets will read the newline character.  
	//we want to get rid of it.
	for(i=0; i < sizeof(file_path); i++)
	{
		if(file_path[i]==10)
		{
			file_path[i] = 0;
		}
		if(file_path[i]==0)
		{
			break;
		}
	}
	
	//open the file.  if fopen fails the throw error.
	fpin = fopen(file_path, "r");
	if(fpin == NULL)
	{
		perror("File open failed.\n");
	}
	
	//determine size of file so we can allocate memory
	fseek(fpin, 0, SEEK_END);
	file_length = ftell(fpin);
	rewind(fpin);
	file_length = file_length - ftell(fpin);
	
	//*encryption and decryption proceed differently from here
	
	if(file_length%2 != 0)
	{
		perror("File length is not an even number of bytes.\n  This is inconsistent with expected hex-encoded data format.\n");
		fclose(fpin);
		return -1;
	}
		
	array = (unsigned char *)malloc((file_length/2) * sizeof(char));
	printf("array be %p\n", array);
	if(array == NULL)
	{
		perror("malloc failed.\n");
		fclose(fpin);
		return -1;
	}
	
	
	i=0;
	while(fscanf(fpin, "%2hhx", &ch) != EOF)
	{
		array[i] = ch;		
		printf("%02x", array[i]);
		i++;
	}
	printf("\n\n\narray = %p\nend = %p\n", array, &array[i-1]);
	printf("%d\n", i);

	
	
	
	
	return 0;
}
