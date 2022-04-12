#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	int 		array_size;
	int 		counter;
	int * 		rand_arr;
	double		duration;
	clock_t		start;
	clock_t		end;

	array_size	= 100000;
	counter		= 0;
	rand_arr	= calloc(array_size, sizeof(int));
	srand(time(NULL));
	start		= clock();

	for (int i = 0; i < array_size; i++)
    {
        rand_arr[i] = rand() % 10;
        if (rand_arr[i] == 3)
        {
            counter++;
        }
    }

	end		= clock();
	duration= ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
	printf("There are %d 3s and it takes %fms", counter, duration);
	return 0;
}