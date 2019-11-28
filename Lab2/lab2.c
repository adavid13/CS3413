#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int avg = 0;
int min = INT_MAX;
int max = INT_MIN;
int count = 0;

int pipe_avg[2];
int pipe_min[2];
int pipe_max[2];

void* findavg(){
  int val;
  while(1){
    read(pipe_avg[0], &val, sizeof(int));
    if ( val == 0 ) {
      avg=avg/(count-1);
      break;
    }
    avg+=val;
  }
}

void* findmin(){
  int val;
  while(1){
    read(pipe_min[0], &val, sizeof(int));
    if ( val == 0 ) break;
    if(val<min){
      min = val;
    }
  }
}

void* findmax(){
  int val;
  while(1){
    read(pipe_max[0], &val, sizeof(int));
    if (val == 0){
      break;
    }

    if(val>max){
      max = val;
    }

  }
}

int main(){

  pipe(pipe_avg);
  pipe(pipe_max);
  pipe(pipe_min);

  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;
  pthread_create(&thread1, NULL, &findavg, NULL);
  pthread_create(&thread2, NULL, &findmin, NULL);
  pthread_create(&thread3, NULL, &findmax, NULL);

  int cond = 0;
  int num = 1;
  while(num!=0){
    scanf("%d", &num);
    write(pipe_avg[1], &num, sizeof(int));
    write(pipe_min[1], &num, sizeof(int));
    write(pipe_max[1], &num, sizeof(int));
    count++;
  }//end while

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  printf("Average = %d\n", avg);
  printf("Minimum = %d\n", min);
  printf("Maximum = %d\n", max);


}
