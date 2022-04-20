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

struct padded_int
{
  int value;
  char padding[60];
} private_count[MaxThreads];
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
   int start = (int)id * length_per_thread;

   for(i = start; i < start+length_per_thread; i++)
   {
      if(array[i] == 3)
      {
         private_count[(int)id].value++;
      }
   }
   pthread_mutex_lock(&m);
   count += private_count[(int)id].value;
   pthread_mutex_unlock(&m);

   return 0;
}


int main(int argc, char *argv[])
{
   int i;
   length = 1048576;  /*  2^20  */
   t = 64;  /*** be sure that t divides length!! ***/

   array = calloc(length, sizeof(int));

   /* initialize the array with random integers between 0 and 9 */
   srand(time(NULL));  /* seed the random number generator with current time */
   for(i = 0; i < length; i++)
   {
      array[i] = rand()%10;
   }

   count3s();  /* run the threads, wait till they finish */

   printf("The number of 3's is %d\n", count);

   /* As a test, let us count 3's the slow, serial way. */
   count = 0;
   for(i = 0; i < length; i++)
      if(array[i] == 3)
         count++;
   printf("The number of 3's is %d\n", count);

   return 0;
}