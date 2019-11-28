#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 20
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct Queue *incoming_buffer;
struct Queue *outgoing_buffer;

typedef struct Application{
	char name[30];
	char action_s[10];
	int action; //receivce = 0, send = 1
	int production_time;
	int ID;
} Application;

/* constructor to create an application 
 * 
 */
struct Application *createApp(char *name, char *action_s, int production_time, int ID){
	struct Application *application = (struct Application*) malloc(sizeof(struct Application));
	strcat(application->name, name);
	strcat(application->action_s, action_s);
	application->production_time = production_time;
	if((action_s[0] == 'S')||(action_s[0] == 's')){
		application->action = 1;
	}else if((action_s[0] == 'R')||(action_s[0] == 'r')){
		application->action = 0;
	}//assuming correct input
	application->ID = ID;
	return application;
}

struct Queue{
    int front, rear, size;
    int capacity;
    Application *array;
};

/* Constructor function to create a queue with a given capacity.
 * starts with a size of queue as 0
 */
struct Queue *createQueue(int capacity){
    struct Queue *queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (Application*) malloc(queue->capacity * sizeof(Application));
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
void enqueue(struct Queue *queue, Application item){
    if (isFull(queue)){
        return;
    }
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

/* Function to remove an item from queue.
 * changes front and size
 */
Application dequeue(struct Queue *queue){
  if(queue==NULL){
      return;
  }
    if (isEmpty(queue)){
        return;
    }
    Application item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

typedef struct{
    int n, s;
}Inputs;


void *application_thread(void *input){
    struct Application *app = (struct Application*)input;
    sleep(app->production_time);
    
    if(app->action == 0){
        if(!isFull(incoming_buffer)){
            pthread_mutex_lock(&mutex);
            enqueue(incoming_buffer, *app);
            pthread_mutex_unlock(&mutex);
        }else{
            while(isFull(incoming_buffer));
            pthread_mutex_lock(&mutex);
            enqueue(incoming_buffer, *app);
            pthread_mutex_unlock(&mutex);
        }
        
    }else{
        if(!isFull(outgoing_buffer)){
            pthread_mutex_lock(&mutex);
            enqueue(outgoing_buffer, *app);
            pthread_mutex_unlock(&mutex); 
        }else{
            while(isFull(outgoing_buffer));
            pthread_mutex_lock(&mutex);
            enqueue(outgoing_buffer, *app);
            pthread_mutex_unlock(&mutex); 
        }
        
    }
    pthread_exit(0);
}


void *device(void* inputs){
    Inputs *args = inputs;
    int n = args->n;
    int s = args->s;
	int mode = 1; //receiving = 0, sending = 1
	int counter = 0;
	sleep(2); //start transfer initiation
	if((isEmpty(incoming_buffer)) && (isEmpty(outgoing_buffer))){
	    mode = 2;
	}
	int c_time = 1;
	while(1){
	    printf("Device is idle:\t%d\n",c_time++);
	if(mode==1){
	    printf("Device is Sending:\t%d\n",c_time++);
	    counter = 0;
	    while(counter<n){
	    pthread_mutex_lock(&mutex);
	    Application item = dequeue(outgoing_buffer);
	    pthread_mutex_unlock(&mutex);
	    sleep(s); //transfer duration (s)
	    printf("%d:\t%s has sent\n", c_time, item.name);
	    c_time++;
	    counter++;
	    if(isFull(incoming_buffer)||(counter>=n)){
	        mode = 0;
	        break;
	    }
	    if((isEmpty(incoming_buffer)) && (isEmpty(outgoing_buffer))){
	    mode = 2;
	    break;
	    }
	    }//end while
	}else if(mode==0){
	    printf("Device is receiving:\t%d\n", c_time++);
	    counter = 0;
	    while(counter<n){
	    pthread_mutex_lock(&mutex);
	    Application item = dequeue(incoming_buffer);
	    pthread_mutex_unlock(&mutex);
	    sleep(s); //transfer duration (s)
	    printf("%d:\t%s has received\n", c_time, item.name);
	    c_time++;
	    counter++;
	    if(isFull(incoming_buffer)||(counter>=n)){
	        mode = 1;
	        break;
	    }
	    if((isEmpty(incoming_buffer)) && (isEmpty(outgoing_buffer))){
	    mode = 2;
	    break;
	    }
	    }//end while
	}else if(mode==2){
	    printf("Device is idle:\t%d\n",c_time);
	    break;
	}
	c_time++;
	}
	pthread_exit(0);
}

Application** allocate(){
    const struct Application **apps = malloc(MAX * sizeof(Application*));
    return apps;
}

int main(int argc, char* argv[]){
    Application **apps = allocate();
	if (argc!=3){
		printf("Please enter n and s\n");
	}
	int n; //max number of messages that can be transferred at a given time
	int s; //time it takes for a message to be transferred in seconds
	sscanf(&argv[1][0], "%i", &n);
	sscanf(&argv[2][0], "%i", &s);
	Inputs *args = malloc(sizeof *args);
	args->n = n;
	args->s = s;
	char str1[30];
	char str2[30];
	int production_time;
	int i;
	for(i=0;i<4;i++){ //skip the header
		scanf("%s",str1);
	}
	
	outgoing_buffer = createQueue(4);
	incoming_buffer = createQueue(4);

	int count = 0;
	while((scanf("%s",str1))!=EOF){
		scanf("%s", str2);
		scanf("%d", &production_time);
		apps[count] = createApp(str1, str2, production_time, count);
		//cannot enqueue here because of queue limit
		count++;
	}
	
	pthread_t device_t;
	pthread_create(&device_t, NULL, &device, (void *)args);//pass n, s to device thread
	
	pthread_t applications_t[count];
	for(i=0; i<count; i++){
	    pthread_create(&applications_t[i], NULL, &application_thread, (void *)apps[i]);
	}
	
	
	pthread_join(device_t, NULL);
	for(i=0; i<count; i++){
	    pthread_cancel(applications_t[i]);
	}
	pthread_cancel(device_t);
	free(apps);
	free(args);
	return 0;
}