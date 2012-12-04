#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>
#include <math.h>

#define SIZE 4

int mainarray[SIZE];
int returnarray[2*SIZE];
int decodearray[SIZE];
int arrindex = 0;
int M = 2;

int numlevels = 0;


void rand_trit(void)
{
	for(int i = 0; i < SIZE; i++)
	{
		mainarray[i] = rand() % 3;
		printf("array[%d] = %d\n", i, mainarray[i]);
	}
}

void store(int num)
{
	for(long long i = 1; i <= 4294967295; i = i*2)
	{
		returnarray[arrindex] = num & i;
		arrindex++;
	}
	return;
}

void encode(int* array, int size, int base, int num)
{
	if (base == 2 || size == 1)
	{	
		for(int i = 0; i < size; i++)
			printf("%d\n", array[i]);
			printf("%d\n", numlevels);
		//	returnarray[i + arrindex] = array[i];
		return;
	}
	int counter = 0;
	int index = 0;
	int divisor = pow(2, M);
	int top = pow(base, num);
	int new_array[size/num + 1];
	printf("M%d\n",M);
	//store(M);
	while(counter < size)
	{
		int total = 0;
		for (int i = 0; i < num; i++)
		{
			total = total + pow(base,i)*array[counter];
			counter++;
		}
		
		int m = total % divisor;
		int k = total / divisor;
		
		//store(m);
		printf("m%d\n",m);
		printf("k%d\n",k);

		new_array[index] = k;
		index++;
	}
	numlevels++;
	return encode(new_array, index, 1 + top/divisor, num);
}



int main(void)
{
	srand(time(NULL));
	rand_trit();
	encode(mainarray, SIZE, 3, 2);
}



