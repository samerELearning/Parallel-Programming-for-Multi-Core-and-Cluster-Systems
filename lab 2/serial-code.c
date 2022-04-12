#include <stdio.h>
#include <stdlib.h>

int main()
{
	int 		array_size;
	int 		counter;
	int * 		rand_arr;
	double		duration;
	clock_t		start;
	clock_t		end;

	array_size	= 1000;
	counter		= 0;
	rand_arr	= calloc(array_size, sizeof(int));
	srand(time(NULL));
	start		= clock();

	//for loop

	end		= clock();
	duration= ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
	return 0;
}