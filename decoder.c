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

typedef struct
{
	int path;
	int where;
}
int M = 64;
int levels;
header* headers;
remainder* remainders;
int* final;
path* paths;

void decode_all(int* array, header h, int level)
{
	if(!level)
	{
		return finish(array);
	}
	
	int new_array[headers[level-1].size];
	int index = 0;
	for(int i = 0; i < h.size; i++)
	{
		int number = remainders[level].numbers[i] + array[i]*pow(2,M);
		for(int j = 0; j < headers[level-1].chunksize && index < headers[level-1].size; j++)
		{
			new_array[index] = number % headers[level-1].K;
			number = number / headers[level-1].K;
			index++;
		}
	} 
	return decode_all(new_array, headers[level - 1], level - 1);
}

int decode_one(void)
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
}
extract_data()
{


}


main(int argc, char*argv[])
{ 
	if(argc == 2)
	{
		int num = atoi(argv[1]);
		int n = num;
		path = malloc(sizeof(int)*levels + 1);
		for(int i = 0; i <= levels; i++);
		{
			path[i].path = n % headers[i].chunksize;
			n = n / headers[i].chunksize;
			path[i].where = n;
		}
		
	}
	else
	

}