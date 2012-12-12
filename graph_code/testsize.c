#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <gmp.h>
#include <math.h>
#include <sys/stat.h>

int SIZE;
#define LEVELS 2
#define M 32
FILE* output;
int* mainarray;
int arrindex = 0;
mpz_t number, m, k, divisor, try;

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
	struct stat st;
	stat("encoded.bin", &st);
	int size = st.st_size;
	printf("%d,", size);
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
	struct stat st;
	stat("aencoded.bin", &st);
	int size = st.st_size;
	printf("%d,", size);
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
	struct stat st;
	stat("nencoded.bin", &st);
	int size = st.st_size;
	printf("%d\n", size);
}

int main(void)
{
	mpz_init(number);
	mpz_init(m);
	mpz_init(k);
	mpz_init(try);
	mpz_ui_pow_ui(try, 2, M);
	mpz_init_set(divisor, try);
	
	for(SIZE = 1; SIZE <=10001; SIZE = SIZE + 1000)
	{
		srand(time(NULL));
		rand_trit();
		tencode();
		system("./arith_coder -e -t bits -v trits.txt > aencoded.bin");
		nencode();
	}
	
	mpz_clear(try);
	mpz_clear(number);
	mpz_clear(m);
	mpz_clear(k);
	mpz_clear(divisor);

}



