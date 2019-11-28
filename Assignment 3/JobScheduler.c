#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 30

typedef struct job{
	char user[30];
	char process;
	int arrival;
	int duration;
	int remaining;
}job_s;
struct job jobs[SIZE];

int find_first(int arrival[], int duration[], int current_time);
int shortest(job_s jobs[], int count, int time);

int main(){
	char str[30];

	int i;
	for(i=0;i<4;i++){ //skip the header
		scanf("%s",str);
	}

	int count = 0;
	int num=0;
	while((scanf("%s",str))!=EOF){
		strcat(jobs[count].user, str);//copy string str into user

		scanf("%s",&jobs[count].process); //scan process

		scanf("%d",&num); //scan arrival
		jobs[count].arrival = num;

		scanf("%d",&num); //scan duration
		jobs[count].duration = num;
		jobs[count].remaining = jobs[count].duration; //set remaining to duration

		count++;
	}
	//Totaling the total duration
	int s, tt = 0;
	for(s=0;s<count;s++){
		tt+=jobs[s].duration;
	}

	/*
	 * Method to find the job with the shortest remaining duration.
	 * If two jobs have the same remaining duration,
	 * the job that arrived first is given precedence.
	 * All while considering current time
	 *
	 * Return: the index of the job
	 */
	int shortest(job_s jobs[], int count, int time){
		int short_ind = 0;
		FLAG: if(jobs[short_ind].remaining==0){
			short_ind++;
		}
		if(jobs[short_ind].remaining!=0){
			for(s=0; s<count; s++){
				if((jobs[s].remaining < jobs[short_ind].remaining)&&(jobs[s].arrival <= time)&&(jobs[s].remaining!=0)){
					short_ind = s;
				}else if((jobs[s].remaining == jobs[short_ind].remaining)&&(jobs[s].arrival <= time)&&(jobs[s].remaining!=0)){
					if(jobs[s].arrival < jobs[short_ind].arrival){
						short_ind = s;
					}
				}
			}
		}else{
			goto FLAG;
		}
		return short_ind;
	}
	int time = jobs[0].arrival;
	int shortest_ind; //index of the job with the shortest duration to be run
	printf("Time\tJob\n");
	while(1){
		shortest_ind = shortest(jobs, count, time);
		if(shortest_ind == -1){
			printf("test");
			break;
		}
		printf("%d\t",time);
		printf("%c\n",jobs[shortest_ind].process);
		jobs[shortest_ind].remaining--;
		time++;
		if(time>=tt+jobs[0].arrival){
			printf("%d\tIDLE\n", time);
			break;
		}

	}

	//return 0;
} //end main
