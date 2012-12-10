#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <gmp.h>
#include <math.h>

#define SIZE 400000
#define LEVELS 3

FILE* output;
int mainarray[SIZE];
int arrindex = 0;
int M = 64;

typedef struct
{
	unsigned int chunksize;
	unsigned int K;
	unsigned int size;
} header;

header headers[LEVELS + 1];
int* final;

void make_headers(void)
{
	mpz_t newk, oldk, divisor, try;
	mpz_init(try);
	mpz_ui_pow_ui(try, 2, M);
	mpz_init_set(divisor, try);
	mpz_init(newk);
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
			mpz_pow_ui(try, oldk, n);
			mpz_cdiv_q(newk, try, divisor);
		}
		headers[i].chunksize = n;
		headers[i].K = mpz_get_ui(oldk);
		mpz_set(oldk,newk);
		headers[i].size = size;
		size = ceil((double)size/n);
	}
	fwrite(headers, sizeof(header), LEVELS + 1, output);
	mpz_clear(newk);
}	


void rand_trit(void)
{
	FILE* trits = fopen("trits.txt", "w");
	for(int i = 0; i < SIZE; i++)
	{
		mainarray[i] = rand() % 3;
	}
	fwrite(mainarray, sizeof(int), SIZE, trits);
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
	printf("level = %d\n", level);
	int allzeroes = 1;
	mpz_t number, m, k, divisor, try;
	mpz_init(number);
	mpz_init(m);
	mpz_init(k);
	mpz_init(try);
	mpz_ui_pow_ui(try, 2, M);
	mpz_init_set(divisor, try);
	if (level == LEVELS || h.size == 1)
	{	
		return encode_last(array, h);
	}
	int counter = 0;
	int index = 0;
	int new_array[headers[level+1].size];
	char* total = malloc(sizeof(char)*h.size);
	while(counter < h.size)
	{
		for (int i = 0; i < h.chunksize && i < h.size; i++)
		{
			total[i] = array[counter] + '0';
			counter++;
		}
		mpz_set_str(number, total, h.K);
		mpz_fdiv_qr(k,m,number,divisor);
		gmp_printf ("num = %Zd, k = %Zd and m: %Zd\n", number, k, m);
		if(mpz_sgn(k))
			allzeroes = 0;
		new_array[index] = mpz_get_ui(k);
		if (!mpz_out_raw (output, m))
			printf("ERROR!\n");
		index++;
	
	}
	if (allzeroes)
		return;
	mpz_clear (number);
	mpz_clear (m);
	mpz_clear (k);
	mpz_clear (divisor);
	return encode(new_array, headers[level+1], level + 1);
}

int main(void)
{
	srand(time(NULL));
	rand_trit();

	output = fopen("encoded.bin", "w");
	int l[2] = {SIZE,LEVELS};
	fwrite(&l, sizeof(int), 2, output);
	make_headers();
	encode(mainarray, headers[0], 0);
	fclose(output);
}



