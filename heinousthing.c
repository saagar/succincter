#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>
#include <math.h>

#define SIZE 4
#define LEVELS 3

int mainarray[SIZE];
int arrindex = 0;
int M = 64;

typedef struct
{
	int chunksize;
	int K;
	int size;
} header;

typedef struct
{
	long long* numbers;
} remainder;

header headers[LEVELS + 1];
remainder remainders[LEVELS];
int* final;

void make_remainder(header h, int level)
{
	if (level == LEVELS)
		return;
	remainders[level].numbers = malloc(sizeof(int) * ceil((double)h.size/h.chunksize));
	if(remainders[level].numbers == NULL)
	{
		printf("ERROR!\n");
		exit(1);
	}	
}

void make_headers(void)
{
	int oldk = 3;
	int size = SIZE;
	for(int i = 0; i <= LEVELS; i++)
	{
		int newk = 0;
		int r = 1;
		int n;
		while(newk < 3)
		{
			r++;
			n = floor((65*log(2.0) + log((double) r))/log((double) oldk));
			newk = ceil(pow(oldk, n)/pow(2,M));
		}
		headers[i].chunksize = n;
		headers[i].K = oldk;
		oldk = newk;
		headers[i].size = size;
		make_remainder(headers[i], i);
		size = ceil((double)size/n);
	}
}	


void rand_trit(void)
{
	for(int i = 0; i < SIZE; i++)
	{
		mainarray[i] = rand() % 3;
		printf("array[%d] = %d\n", i, mainarray[i]);
	}
}

void encode_last(int* array, header h);
{
	double counter = 0;
	while (h.K > 0)
	{
		h.K = h.K / 2;
		counter++;
	}	
	final = malloc(ceil(counter/8)*h.size);
		
		for
		
	return;
}

void encode(int* array, header h, int level)
{
	if (level == LEVELS)
	{	
		encode_last(array, h);
	}
	int counter = 0;
	int index = 0;
	int divisor = pow(2, M);
	int new_array[headers[level+1].size];
	while(counter < h.size)
	{
		int total = 0;
		for (int i = 0; i < num; i++)
		{
			total = total + pow(h.K,i)*array[counter];
			counter++;
		}
		
		int m = total % divisor;
		int k = total / divisor;
		
		new_array[index] = k;
		remainders[level].numbers[index] = m;
		index++;
	}
	return encode(new_array, headers[level+1], level + 1);
}

int main(void)
{
	srand(time(NULL));
	rand_trit();
	make_headers();
	encode(mainarray, headers[0], 0, SIZE);
}



