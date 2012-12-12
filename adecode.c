#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <gmp.h>
#include <math.h>

int size;
mpz_t number;

void extract_arith(void)
{
	FILE* input = fopen("aencoded.bin", "r");
	fread(&size, sizeof(int), 1, input);
	mpz_inp_raw(number,input);
	fclose(input);
}

void adecode_all()
{
	FILE* out = fopen("adecoded.txt", "w");
	char output[size];	
	for(int i = 0; i < size; i++)
	{
		output[size - 1 - i] = mpz_fdiv_q_ui(number, number, 3) + '0';
	} 
	fwrite(output, sizeof(char), size, out);
}

int adecode_one(int n)
{
	return n;
}

int
main(int argc, char* argv[])
{
	extract_arith();
	if(argc == 2)
	{
		int num = atoi(argv[1]);
		if (num < size)
		{
			int p = adecode_one(num);
			printf("%d\n", p);
		}
		else
			printf("error!\n");
	}
	else
	{
		adecode_all();
	}
}
