// LICENSE : GPL 3, see file LICENSE_GPL-3.txt
#include <stdio.h>
#include <omp.h>   //required for the get_wtime() api

int total;
int length;
int *array;
long long num_steps;

double calculatePi();

int main(int argc, char* argv[])
{
   double start, stop;
   double x, pi, sum=0.0;
   int i;
   num_steps = 1000000000;
   step = 1./(double)num_steps;
   start = omp_get_wtime();   //uses openmp's timer api
   for (i=0; i<num_steps; i++)
   {
      x = (i + .5)*step;
      sum = sum + 4.0/(1.+ x*x);
   }
   
   pi = sum*step;
   stop = omp_get_wtime();  //uses openmp's timer api

   printf("The value of PI is %15.12f\n",pi);
   printf("The time to calculate PI was %15.3f seconds\n",stop - start);
   return 0;
}
