#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>

struct Queue{
    int front, rear, size;
    int capacity;
    int *array;
};

/* Function to create a queue with a given capacity.
 * starts with a size of queue as 0
 */
struct Queue *createQueue(int capacity){
    struct Queue *queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue *queue){
	return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue *queue){
  if(queue==NULL){
    return INT_MIN;
  }
  return (queue->size == 0);
}

/* Function to add an item to the queue.
 * changes rear and size
 */
void enqueue(struct Queue *queue, int item){
    if (isFull(queue)){
        printf("customer %d leaves with a long shaggy mop of hair!\n", item);
        return;
    }
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("customer %d is waiting for a haircut\n", item);
}

/* Function to remove an item from queue.
 * changes front and size
 */
int dequeue(struct Queue *queue){
  if(queue==NULL){
    return INT_MIN;
  }
    if (isEmpty(queue)){
        return INT_MIN;
    }
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue *queue){
    if (isEmpty(queue)){
        return INT_MIN;
    }
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue *queue){
    if (isEmpty(queue)){
        return INT_MIN;
    }
    return queue->array[queue->rear];
}

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct thread_args{
    struct Queue * queue;
    int barber_f;
    int haircut_time;
};

void * barber(void * argp){
  struct thread_args *args = argp;
  struct Queue * queue = args->queue;
  int flag = args->barber_f;
  int haircut_time = args->haircut_time;

	  int s;
	  if(!isEmpty(queue)){
	 //	if(flag==0){
	 	    flag = 1;
	 	    pthread_mutex_lock(&mutex);
	 	    s = dequeue(queue);
	 	    pthread_mutex_unlock(&mutex);
	 	    if(s>0){ //meaning dequeue did not fail
	 	     printf("customer %d is styling a fresh haircut :)\n", s);
	 	    }
            sleep(haircut_time);
            flag = 0;
	 //	}
	  }else{
	      flag = 0;
	  }
  //return 0;
}

void customer(struct Queue * customers, int barber_flag, struct thread_args *args, int id, int end_time, int cust_freq, int haircut_time){
   
    if(barber_flag==0){
        enqueue(customers, id);
        args->queue = customers;
	    args->barber_f = barber_flag;
        args->haircut_time = haircut_time;
        barber(args);
    }else{
        enqueue(customers, id);
    }
    sleep(cust_freq);
}

int main(int argc, char* argv[]){
	if(argc!=7){
			printf("Please enter -c -n -s");
			return 0;
		}
	int wr_cnum; //number of chairs in the waiting room
	int cust_freq;//how often a customer arrives
	int end_time; //how long the program will run for
	sscanf(&argv[1][3], "%i", &wr_cnum);
	sscanf(&argv[2][5], "%i", &cust_freq);
	sscanf(&argv[3][8], "%i", &end_time);

	int barber_chairs[3]; //array of barber chairs, will take on 1 occupied and 0 as not occupied
	memset(barber_chairs, 0, sizeof(barber_chairs));
	int barber_flag = 0; //flag to check if barber is asleep. 0 if free 1 if occupied
	int wr_chairs[wr_cnum]; //array of waiting room chairs, will take on the id's of the customers
	memset(wr_chairs, 0, sizeof(wr_chairs));
	int haircut_time; //length of haircut (5-7s)
	srand(time(NULL));

	int cap = wr_cnum;
	struct Queue *customers = createQueue(cap); //will take on customer id

	struct thread_args *args = malloc(sizeof *args);
	if(args==NULL){
	    printf("Error allocating memory");
	    return 0;
	}

	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_create(&thread1, NULL, &barber, args);
	pthread_create(&thread2, NULL, &barber, args);
	pthread_create(&thread3, NULL, &barber, args);

	time_t endwait;
	time_t start = time(NULL);
	time_t seconds = end_time;
  endwait = start + seconds;

	int id = 0;
	while(start < endwait){
		cust_freq = rand() % (cust_freq + 1);
		haircut_time = rand() % (7 + 1 - 5) + 5;
        
        
        args->queue = customers;
	    args->barber_f = barber_flag;
        args->haircut_time = haircut_time;
        barber(args);
        endwait = endwait-haircut_time;
        sleep(cust_freq);
        endwait-=cust_freq;
        enqueue(customers, id++);
        //customer(customers, barber_flag, args, id++, end_time, cust_freq, haircut_time);

    start = time(NULL);
	}
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  pthread_mutex_destroy(&mutex);
	free(args);
	return 0;
}
