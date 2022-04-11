#include <stdio.h>
#include <stdlib.h>

int main()
{
	int		i;//number of iterations
	double  duration;
	double  pi;
	clock_t start;
	clock_t end;
	int 	counter;

	i 		= 1000;
	start	= clock();
	counter = 0;

	srand(time(NULL));
	//loop

	end		= clock();
	duration= ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
	pi 		= 4.0 * ((double)counter / i);
	printf("Serial of pi is %f and it takes %fms\n", pi, duration);

	return 0;
}