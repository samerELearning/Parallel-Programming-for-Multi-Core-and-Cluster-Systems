// LICENSE : GPL 3, see file LICENSE_GPL-3.txt
#include <stdio.h>
#include <omp.h>   //required for the get_wtime() api

int *array;
long long num_steps;

double calculatePi();

int main(int argc, char* argv[])
{
   int      counter;
   double   pi;
   double   duration;
   clock_t  begin;
   clock_t  stop;

   array       = calloc(length, sizeof(int));
   num_steps   = 1000000000;
   begin       = clock();
   pi          = calcPI();
   stop        = clock();
   duration    = ((double)(stop - begin) / CLOCKS_PER_SEC) * 1000.0;

   printf("pi = %f and it takes %fms\n", pi, duration);

   begin    = clock();
   counter  = 0;

   for (int i = 0; i < length; i++)
   {
      double x_coordinate = (rand() % 2413) / 2412.0;
      double y_coordinate = (rand() % 2413) / 2412.0;
      array[i] = sqrt((x_coordinate * x_coordinate) + (y_coordinate * y_coordinate)) <= 1 ? 1 : 0;

      if (array[i] == 1)
      {
         counter++;
      }
   }
   end       = clock();
   duration  = ((double)(stop - begin) / CLOCKS_PER_SEC) * 1000.0;
   pi        = 4.0 * ((double)counter / num_steps);

   printf("The value of PI is %15.12f\n",pi);
   printf("The time to calculate PI was %15.3f seconds\n",duration);
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
         x_coordinate = (rand() % 2413) / 2412.0;
         y_coordinate = (rand() % 2413) / 2412.0;

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