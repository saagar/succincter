#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <gmp.h>
#include <math.h>

typedef struct
{
	int chunksize;
	int K;
	int size;
} header;

typedef struct
{
	int path;
	int where;
} path;
FILE* input;
int M = 64;
int levels;
int size;
header* headers;
mpz_t** remainders;
int* final;
path* paths;

void finish(int* array)
{
	FILE* output = fopen("decoded.txt", "w");
	char c;
	for(int i = 0; i < headers[0].size; i++)
	{
		c = array[i] + '0';
		fwrite(&c, sizeof(char), 1, output);
	}
	fclose(output);
	return;
}


void decode_all(int* array, header h, int level)
{
	if(!level)
	{
		return finish(array);
	}
	
	int* new_array = malloc(sizeof(int)*headers[level-1].size);
	int index = headers[level-1].size -1;
	mpz_t number, spill;
	mpz_init(number);
	mpz_init(spill);	
	for(int i = h.size-1; i >= 0; i--)
	{
		mpz_ui_pow_ui(spill, 2, M);
		mpz_set(number, remainders[level-1][i]);
		mpz_addmul_ui(number, spill, array[i]);
		for(int j = 0; j < headers[level-1].chunksize && index >= 0; j++)
		{
			
			new_array[index] = mpz_fdiv_q_ui(number, number, headers[level-1].K);
			index--;
		}
	} 
	return decode_all(new_array, headers[level - 1], level - 1);
}

/*int decode_one(void)
{
	int num = final[path[levels].where];
	for(int i = levels - 1; i >=0; i--) 
	{	
		num = num * 2^M + remainders[i].numbers[path[i].where];
		for(int j = 0; j < path[i].path; j++)
		{
			num = num / headers[i].K;
		} 
		num = num % headers[i].K;
	}
	return num;
}*/

void extract_last(header h)
{
	final = malloc(sizeof(int)*h.size);
	int counter = 0;
	while (h.K > 0)
	{
		h.K = h.K / 2;
		counter++;
	}	
	int index = -1;
	int k;
	int count = 0;
	int cycler = 1;
	uint8_t reader;
	while(fread(&reader, sizeof(uint8_t), 1, input))
	{
		while(cycler != 256)
		{
			if(!(count % counter)) 
			{
				index++;
				k = 1;
			}
			if(reader & cycler)
			{
				final[index] = final[index] | k;
			}
			cycler = cycler << 1;
			k = k << 1;
			count++;
		}
		cycler = 1;
	}
}

void extract_data(void)
{
	input = fopen("encoded.bin", "r");
	fread(&size, sizeof(int), 1, input);
	fread(&levels, sizeof(int), 1, input);
	headers = malloc(sizeof(header) * (levels+1));
	fread(headers, sizeof(header), levels + 1, input);
	remainders = malloc(sizeof(mpz_t*)*levels);
	for(int i = 0; i < levels; i++)
	{
		remainders[i] = malloc(sizeof(mpz_t)*headers[i + 1].size);
		for(int j = 0; j < headers[i + 1].size; j++)
			mpz_inp_raw(remainders[i][j], input);
	}
	
	extract_last(headers[levels]);
}

void print_remainders(void)
{
	for(int i = 0; i < levels; i++)
	{
		printf("\n");
		for(int j = 0; j < headers[i + 1].size; j++)
			gmp_printf ("%Zd\n", remainders[i][j]);
	}
}

int
main(int argc, char*argv[])
{ 
	extract_data();	
	print_remainders();
	decode_all(final, headers[2], 2);
}