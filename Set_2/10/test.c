#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
	char *str;
	unsigned char *array;
	
	array = (unsigned char *)malloc(15);
	printf("array be %p\n", array);
	
	if(array == NULL)
	{printf("wtf\n");}
	
	
	
	str = (char *) malloc(15);
	if(str == NULL)
	{printf("wtf\n");}
	strcpy(str, "tutorialspoint");
	printf("String = %s, Address = %p\n", str, str);
	free(str); 
	return 0;
}
