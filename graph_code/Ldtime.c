#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <gmp.h>
#include <math.h>
#include <sys/stat.h>

#define SIZE 500000
int LEVELS;
#define M 32

typedef struct
{
	unsigned int chunksize;
	unsigned int K;
	unsigned int size;
} header;

typedef struct
{
	int path;
	int where;
} path;


FILE* output;
int* mainarray;
int arrindex = 0;
mpz_t number, m, k, divisor, try;
clock_t t;
FILE* input;
int levels;
int size;
mpz_t** remainders;
int* final;
path* paths;
uint8_t* array;
header* headers;

void make_headers(void)
{
	headers = malloc(sizeof(header)*(LEVELS + 1));
	mpz_t newk, oldk, thing, div;
	mpz_init(newk);
	mpz_init(thing);
	mpz_ui_pow_ui(thing, 2, M);
	mpz_init_set(div, thing);	
	mpz_init_set_ui(oldk, 3);
	int size = SIZE;
	for(int i = 0; i <= LEVELS; i++)
	{
		mpz_set_ui(newk, 0);
		int r = 1;
		int n;
		while(mpz_get_ui(newk) < 3)
		{
			r++;
			double bottom = mpz_get_d(oldk);
			n = floor(((M+1)*log(2.0) + log((double) r))/log(bottom));
			mpz_pow_ui(thing, oldk, n);
			mpz_cdiv_q(newk, thing, div);
		}
		headers[i].chunksize = n;
		headers[i].K = mpz_get_ui(oldk);
		mpz_set(oldk,newk);
		headers[i].size = size;
		size = ceil((double)size/n);
	}
	fwrite(headers, sizeof(header), LEVELS + 1, output);
	mpz_clear(oldk);
	mpz_clear(div);
	mpz_clear(thing);
	mpz_clear(newk);
	
}	


void rand_trit(void)
{
	FILE* trits = fopen("trits.txt", "w");
	mainarray = malloc(sizeof(int)*SIZE);
	for(int i = 0; i < SIZE; i++)
	{
		mainarray[i] = rand() % 3;
		char c = mainarray[i] + '0';
		fwrite(&c,sizeof(char), 1, trits);
	}
	fclose(trits);
}

void encode_last(int* array, header h)
{
	double counter = 0;
	while (h.K > 0)
	{
		h.K = h.K / 2;
		counter++;
	}	
	int bytes = ceil(h.size*counter/8);
	int cycler = 1;
	uint8_t writer = 0;
	for(int i = 0; i < h.size; i++)
	{
		int k = 1;
		for(int j = 0; j < counter; j++)
		{
			if(k & array[i])
			{
				writer = writer | cycler;
			}
			k = k << 1;
			cycler = cycler << 1;
			if (cycler == 256)
			{
				fwrite(&writer, sizeof(uint8_t), 1, output);
				writer = 0;
				cycler = 1;
			}
		}
	}
	fwrite(&writer, sizeof(uint8_t), 1, output);
	return;
}

void encode(int* array, header h, int level)
{
	int allzeroes = 1;
	if (level == LEVELS || h.size == 1)
	{	
		return encode_last(array, h);
	}
	int counter = 0;
	int index = 0;
	int new_array[headers[level+1].size];
	char* total = malloc(sizeof(char)*h.size);
	if(total == NULL)
	{
		printf("ERROR!\n");
		exit(1);
	}
	char* start = total;
	while(counter < h.size)
	{
		if(h.size - counter < h.chunksize)
		{
			for(int i = 0; i < h.chunksize - h.size + counter; i++)
				total++;
			int tot = h.size - counter;
			for (int i = 0; i < tot; i++)
			{
				total[i] = array[counter] + '0';
				counter++;
			}
		}
		else
		{
			for (int i = 0; i < h.chunksize; i++)
			{
				total[i] = array[counter] + '0';
				counter++;
			}
		}
		mpz_set_str(number, total, h.K);
		mpz_fdiv_qr(k,m,number,divisor);
		if(mpz_sgn(k))
			allzeroes = 0;
		new_array[index] = mpz_get_ui(k);
		unsigned int remains = mpz_get_ui(m);
		fwrite(&remains, sizeof(unsigned int), 1, output);
		index++;
	
	}
	free(start);
	if (allzeroes)
		return;
	return encode(new_array, headers[level+1], level + 1);
}
int tencode(void)
{
	output = fopen("encoded.bin", "w");
	int l[3] = {SIZE,LEVELS, M};
	fwrite(&l, sizeof(int), 3, output);
	make_headers();
	encode(mainarray, headers[0], 0);
	fclose(output);
}

void aencode(void)
{
	FILE* output = fopen("aencoded.bin", "w");
	int l = {SIZE};
	fwrite(&l, sizeof(int), 1, output);
	mpz_t numba;
	rand_trit();
	char total[SIZE + 1];
	total[SIZE] = '\0';
	for(int i = 0; i < SIZE; i++)
	{
		total[i] = mainarray[i] + '0';
	}
	mpz_init_set_str(numba, total, 3);
	mpz_out_raw (output, numba);
	fclose(output);
	mpz_clear(numba);
}

void nencode(void)
{
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
}



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
		num = mpz_get_ui(number);
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
	int q;
	input = fopen("encoded.bin", "r");
	fread(&size, sizeof(int), 1, input);
	fread(&levels, sizeof(int), 1, input);
	fread(&q, sizeof(int), 1, input);
	fread(headers, sizeof(header), levels + 1, input);
	remainders = malloc(sizeof(mpz_t*)*levels);
	for(int i = 0; i < levels; i++)
	{
		remainders[i] = malloc(sizeof(mpz_t)*headers[i + 1].size);
		for(int j = 0; j < headers[i + 1].size; j++)
		{
			unsigned int remains;
			fread(&remains, sizeof(unsigned int), 1, input);
			mpz_init_set_ui(remainders[i][j], remains);
		}
	}
	extract_last(headers[levels]);
}

void tdecode(void)
{
	t = clock();
	extract_data();	
	decode_all(final, headers[LEVELS], LEVELS);
	t = clock() - t;
	printf ("%f\n",((double)t)/CLOCKS_PER_SEC);
}

int main(void)
{
	mpz_init(number);
	mpz_init(m);
	mpz_init(k);
	mpz_init(try);
	mpz_ui_pow_ui(try, 2, M);
	mpz_init_set(divisor, try);
	
	for(LEVELS = 1; LEVELS < 6; LEVELS++)
	{
		srand(time(NULL));
		rand_trit();
		tencode();
		tdecode();
	}
	
	mpz_clear(try);
	mpz_clear(number);
	mpz_clear(m);
	mpz_clear(k);
	mpz_clear(divisor);

}



