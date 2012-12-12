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
	FILE* output = fopen("aencoded.bin", "w");
	int l = {SIZE};
	fwrite(&l, sizeof(int), 1, output);
	mpz_t number;
	rand_trit();
	char total[SIZE + 1];
	total[SIZE] = '\0';
	for(int i = 0; i < SIZE; i++)
	{
		total[i] = mainarray[i] + '0';
	}
	mpz_init_set_str(number, total, 3);
	mpz_out_raw (output, number);
	fclose(output);
	mpz_clear(number);
	struct stat st;
	stat("aencoded.bin", &st);
	int size = st.st_size;
	printf("%d\n", size);
}

