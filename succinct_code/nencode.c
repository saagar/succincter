#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <gmp.h>
#include <math.h>
#include <sys/stat.h>

#define SIZE 50000
int mainarray[SIZE];

void rand_trit(void)
{
	srand(time(NULL));
	FILE* trits = fopen("trits.txt", "w");
	for(int i = 0; i < SIZE; i++)
	{
		mainarray[i] = rand() % 3;
		char c = mainarray[i] + '0';
		fwrite(&c,sizeof(char), 1, trits);
	}
	fclose(trits);
}

int
main(void)
{
	rand_trit();

	FILE* output = fopen("nencoded.bin", "w");
	int l = SIZE;
	fwrite(&l, sizeof(int), 1, output);
	int counter = 0;
	int where = 0;
	uint8_t writer;
	while(counter < SIZE)
	{
		if(where == 0)
		{
			writer = 0;
			writer = writer | (mainarray[counter] << 6);
			counter++;
			where++;
		}
		else if(where == 1)
		{
			writer = writer | (mainarray[counter] << 4);
			counter++;		
			where++;

		}
		else if(where == 2)
		{
			writer = writer | (mainarray[counter] << 2);
			counter++;		
			where++;
		}
		else
		{
			writer = writer | (mainarray[counter]);
			counter++;	
			fwrite(&writer, sizeof(uint8_t), 1, output);
			where = 0;
		}
	}
	fclose(output);
	struct stat st;
	stat("nencoded.bin", &st);
	int size = st.st_size;
	printf("%d\n", size);
}

