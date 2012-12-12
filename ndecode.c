#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <gmp.h>
#include <math.h>

int size;
uint8_t* array;
void extract_naive(void)
{
	FILE* input = fopen("nencoded.bin", "r");
	fread(&size, sizeof(int), 1, input);
	array = malloc(sizeof(uint8_t) * (size/4 + 1));
	fread(array, sizeof(uint8_t), size/4 + 1, input);
	fclose(input);
}

void ndecode_all(void)
{
	FILE* output = fopen("ndecoded.txt", "w");
	char num;
	int where = 0;
	int counter = 0;
	int check = 0;
	uint8_t reader;
	while(counter < size)
	{
		if(check == 0)
		{
			reader = array[where];
			num = (0x03 & (reader >> 6))+ '0';
			counter++;
			check++;
			fwrite(&num, sizeof(char), 1, output);
		}
		else if(check == 1)
		{
			num = (0x03 & (reader >> 4))+ '0';
			counter++;
			check++;
			fwrite(&num, sizeof(char), 1, output);
		}
		else if(check == 2)
		{
			num = (0x03 & (reader >> 2))+ '0';
			counter++;
			check++;
			fwrite(&num, sizeof(char), 1, output);	
		}
		else
		{
			num = (0x03 & (reader))+ '0';
			counter++;
			where++;
			fwrite(&num, sizeof(char), 1, output);
			check = 0;
		}
	}
	fclose(output);
}

int
main(void)
{
	extract_naive();
	ndecode_all();
}