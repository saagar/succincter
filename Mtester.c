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
FILE* output;
int mainarray[SIZE];
int arrindex = 0;
mpz_t number, m, k, divisor, try;

typedef struct
{
	unsigned int chunksize;
	unsigned int K;
	unsigned int size;
} header;

header* headers;
int* final;

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

int main(void)
{
	srand(time(NULL));
	rand_trit();

	mpz_init(number);
	mpz_init(m);
	mpz_init(k);
	mpz_init(try);
	mpz_ui_pow_ui(try, 2, M);
	mpz_init_set(divisor, try);
	mpz_clear(try);
	for(LEVELS = 1; LEVELS < 6; LEVELS++)
	{
		output = fopen("encoded.bin", "w");
		make_headers();
		int l[3] = {SIZE,LEVELS, M};
		fwrite(&l, sizeof(int), 3, output);
		encode(mainarray, headers[0], 0);
		fclose(output);
		struct stat st;
		stat("encoded.bin", &st);
		int size = st.st_size;
		printf("%d,%d\n",LEVELS, size);
	}
	mpz_clear(number);
	mpz_clear(m);
	mpz_clear(k);
	mpz_clear(divisor);
	
}



