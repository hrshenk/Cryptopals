
//Rand_Array will fill an array with random bytes
void Rand_Array(unsigned char *array, unsigned int array_size);

int main()
{
	//declarations
	unsigned char key[16];
	int flag;	
	
	//body
	//generate key
	Rand_Array(key, 16);
	flag = rand()%2;
	//flag == 0 do ECB, else do CBC
	
		
	return 0;
}


void Rand_Array(unsigned char *array, unsigned int array_size)
{
	unsigned int i=0;
	while(i < array_size)
	{
		array[i] = (unsigned char) (rand() % 256);
		i++;
	}
	return;	
}
