#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MaxThreads 1000
void* count3s_thread(void* id);
pthread_t tid[MaxThreads];

int t;         /* number of threads */
int * array;
int length;
int count;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void count3s()
{
   int i;
   count = 0;
   /* Create t threads */
   for(i = 0; i < t; i++)
   {
      pthread_create(&tid[i], NULL, count3s_thread, (void*)i);
   }

   /*** wait for the threads to finish ***/
   for(i = 0; i < t; i++)
   {
      pthread_join(tid[i], NULL);
   }
}

void* count3s_thread(void* id)
{
   int i;
   /* Compute portion of the array that this thread should work on */
   int length_per_thread = length / t;
   int start = (intptr_t)id * length_per_thread;

   for(i = start; i < start+length_per_thread; i++)
   {
      if(array[i] == 3)
      {
         pthread_mutex_lock(&m);
         count++;
         pthread_mutex_unlock(&m);
      }
   }
   return 0;
}


int main(int argc, char *argv[])
{
   int i;
   length = 1048576;  /*  2^20  */
   t = 8;  /*** be sure that t divides length!! ***/

   array = calloc(length, sizeof(int));

   /* initialize the array with random integers between 0 and 9 */
   srand(time(NULL));  /* seed the random number generator with current time */
   for(i = 0; i < length; i++)
   {
      array[i] = rand()%10;
   }

   clock_t start = clock();
   count3s();
   clock_t end = clock();
   double time_spent = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
   printf("It takes %fms\n", time_spent);

   printf("Parallel: The number of 3's is %d\n", count);

   count = 0;
   for (i = 0; i < length; i++)
      if (array[i] == 3)
         count++;
   printf("Serial: The number of 3's is %d\n", count);

   return 0;
}