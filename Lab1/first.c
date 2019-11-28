#include <pthread.h>
#include <stdio.h>

static int arg = 0;
static  pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void* fun1(){
	int i;
	for(i=0; i<100; i++){
	  pthread_mutex_lock(&mutex1);
	  arg++;
	  pthread_mutex_unlock(&mutex1);
      
	}
	return 0;
	
}

void* fun2(){
	int j;
	for(j=0; j<100; j++){
       	  pthread_mutex_lock(&mutex1);
	  arg--;
	   pthread_mutex_unlock(&mutex1);
	}
	return 0;
}

int main(){
	pthread_t thread1;
	pthread_t thread2;
	      
	pthread_create(&thread1, NULL, &fun1,NULL);
       	pthread_create(&thread2, NULL, &fun2,NULL);
	
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_mutex_destroy(&mutex1);
        printf("Value = %d\n", arg);		
	return 0;
}
