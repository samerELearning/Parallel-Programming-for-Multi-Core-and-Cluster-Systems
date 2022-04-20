// LICENSE : GPL 3, see file LICENSE_GPL-3.txt
#include <stdio.h>
#include <omp.h>   //required for the get_wtime() api

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


double calculatePi()
{
   int      total;
   int      local;
   double   x_coordinate;
   double   y_coordinate;

   total = 0;
   local = 0;

   omp_set_num_threads(8);
   srand(time(NULL));

   #pragma omp parallel firstprivate(local)
   {
      #pragma omp for
      for (int i = 0; i < length; i++)
      {
         x_coordinate = (rand() % 2412) / 2412.0;
         y_coordinate = (rand() % 2412) / 2412.0;

         array[i] = sqrt((x_coordinate * x_coordinate) + (y_coordinate * y_coordinate)) <= 1 ? 1 : 0;

         if (array[i] == 1)
         {
            local++;
         }
      }
      #pragma omp critical
      {
         total += local;
      }
   }
   return ((double)total / num_steps) * 4.0;
}