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
int M;
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

void make_path(int n)
{
	paths = malloc(sizeof(int)*(levels + 1));
	for(int i = 0; i < levels + 1; i++)
	{
		paths[i].where = n;
		paths[i].path = n % headers[i].chunksize;
		n = n / headers[i].chunksize;
	}
}

void decode_all(int* array, header h, int level)
{
	if(!level)
	{
		return finish(array);
	}
	
	int* new_array = malloc(sizeof(int)*headers[level-1].size);
	int index = 0;
	mpz_t number, spill;
	mpz_init(number);
	mpz_init(spill);	
	for(int i = 0; i < h.size; i++)
	{
		mpz_ui_pow_ui(spill, 2, M);
		mpz_set(number, remainders[level-1][i]);
		mpz_addmul_ui(number, spill, array[i]);
		int min = headers[level-1].chunksize > (headers[level-1].size - index)?(headers[level-1].size - index):headers[level-1].chunksize;
		
		for(int j = 0; j < min; j++)
		{
			new_array[index + min - j - 1] = mpz_fdiv_q_ui(number, number, headers[level-1].K);
		}
		index += min;
	} 
	return decode_all(new_array, headers[level - 1], level - 1);
}

int decode_one(void)
{
	int num = final[paths[levels].path];
	mpz_t number, spill;
	mpz_init(number);
	mpz_init(spill);
	mpz_ui_pow_ui(spill, 2, M);
	for(int i = levels - 1; i >=0; i--) 
	{	
		mpz_set(number, remainders[i][paths[i + 1].where]);
		mpz_addmul_ui(number, spill, num);
		for(int j = headers[i].chunksize - 1; j > paths[i].path; j--)
		{
			mpz_fdiv_q_ui(number, number, headers[i].K);
		} 
		mpz_fdiv_r_ui(number, number, headers[i].K);
		num =  mpz_get_ui(number);
	}
	return num;
}

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
	fread(&M, sizeof(int), 1, input);
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


int
main(int argc, char*argv[])
{ 
	extract_data();	
	if(argc == 2)
	{
		int num = atoi(argv[1]);
		if (num < size)
		{
			make_path(num);
			int p = decode_one();
			printf("%d\n", p);
		}
		else
			printf("error!\n");
	}
	else	
		decode_all(final, headers[2], 2);
}