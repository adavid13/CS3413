#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
//#include <pthread.h>

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
	return (queue->size == 0);
}

/* Function to add an item to the queue.
 * changes rear and size
 */
void enqueue(struct Queue *queue, int item){
    if (isFull(queue)){
        return;
    }
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}

/* Function to remove an item from queue.
 * changes front and size
 */
int dequeue(struct Queue *queue){
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

//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * barber(){
	//pthread_mutex_lock(&mutex);
	int yhis = 0;
	//pthread_mutex_unlock(&mutex);
}

int main(int argc, char* argv[]){
	if(argc!=4){
			printf("Please enter -c -n -s");
			return 0;
		}
	int wr_cnum; //number of chairs in the waiting room
	int cust_freq;//how often a customer arrives
	int end_time; //how long the program will run for
	sscanf(&argv[1][2], "%i", &wr_cnum);
	sscanf(&argv[2][2], "%i", &cust_freq);
	sscanf(&argv[3][2], "%i", &end_time);

	int barber_chairs[3]; //array of barber chairs, will take on 1 occupied and 0 as not occupied
	int barbers[3]; //array of barbers,
	int wr_chairs[wr_cnum]; //array of waiting room chairs, will take on the id's of the customers
	int haircut_time; //length of haircut (5-7s)
	srand(time(NULL));

	/*pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_create(&thread1, NULL, &barber, NULL);
	pthread_create(&thread2, NULL, &barber, NULL);
	pthread_create(&thread3, NULL, &barber, NULL);
	*/
	int cap = wr_cnum+3;
	struct Queue *customers = createQueue(cap); //will take on customer id
	time_t endwait;
	time_t start = time(NULL);
	time_t seconds = end_time;
	endwait = start + seconds;

	int count = 0, id = 0;
	while(start < endwait){
		cust_freq = rand() % (cust_freq + 1);
		haircut_time = rand() % (7 + 1 - 5) + 5;
		printf("Haircut length = %d\n", haircut_time);
		enqueue(customers, id++); //adds a customer with ID id into the customer queue
		sleep(haircut_time);
		start = time(NULL);

		printf("time: %s\n", ctime(&start));
	}

	//pthread_mutex_destory(&mutex);
	return 0;
}
