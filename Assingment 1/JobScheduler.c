#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 30
int ID = 0;

typedef struct job{
	int id;
	char user[30];
	char process;
	int arrival;
	int duration;
	int deadline;
	int remaining;
	int time_fin;
}job_s;
struct job jobs[SIZE];
int shortest(job_s jobs[], int count, int time);

int main(int argc, char *argv[]){
	if(argc!=2){
		printf("Please enter the number of CPUs");
		return 0;
	}
	int n_cpus = (*argv[1]) - '0'; //The number of CPUs
	if(n_cpus<=0){
		printf("Not enough CPUs to process jobs.");
		return 0;
	}
	int cpus[n_cpus]; //array of cpus to hold the index of a job

	char str[30];

	int i;
	for(i=0;i<5;i++){ //skip the header
		scanf("%s",str);
	}

	int count = 0;
	int num=0;
	while((scanf("%s",str))!=EOF){
		jobs[count].id = ID++;

		strcat(jobs[count].user, str);//copy string str into user

		scanf("%s",&jobs[count].process); //scan process

		scanf("%d",&num); //scan arrival
		jobs[count].arrival = num;

		scanf("%d",&num); //scan duration
		jobs[count].duration = num;
		jobs[count].remaining = jobs[count].duration; //set remaining to duration

		scanf("%d", &num); //scan deadline
		jobs[count].deadline = num;

		printf("deadline %d ", jobs[count].deadline);

		count++;
	}
	//Totaling the total duration
	int s, tt = 0;
	for(s=0;s<count;s++){
		tt+=jobs[s].duration;
	}

	/*
	 * Method to find the job with the shortest remaining duration.
	 * If two jobs have the same deadline,
	 * the method will compare their remaining duration.
	 * If the two jobs have the same remaining duration,
	 * the job that arrived first is given precedence.
	 * All while considering current time,
	 * and whether the job is done.
	 *
	 * Return: the index of the job
	 */
	int shortest(job_s jobs[], int count, int time){
		int short_ind = 0;
		FLAG: if(jobs[short_ind].remaining==0){
			short_ind++;
		}
		if(jobs[short_ind].remaining!=0){ //if the job we compare is not done yet
			for(s=0; s<count; s++){
				if((jobs[s].deadline < jobs[short_ind].deadline)&&(jobs[s].arrival <= time)&&(jobs[s].remaining!=0)){
					short_ind = s;
				}else if((jobs[s].deadline == jobs[short_ind].deadline)&&(jobs[s].remaining < jobs[short_ind].remaining)&&(jobs[s].arrival <= time)&&(jobs[s].remaining!=0)){
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
	printf("Time\tJob\t");
	for(s=0; s<n_cpus; s++){
		printf("CPU%d\t",s);
	}
	printf("\n");
	while(1){
		shortest_ind = shortest(jobs, count, time);
		printf("%d\t",time);
		printf("%c\n",jobs[shortest_ind].process);
		jobs[shortest_ind].remaining--;
		time++;
		if(jobs[shortest_ind].remaining==0){
			jobs[shortest_ind].time_fin = time;
		}
		if(time>=tt+jobs[0].arrival){
			printf("%d\tIDLE\n", time);
			printf("Summary\n");
			for(s=0; s<count; s++){
				printf("%s\t%d\n", jobs[s].user, jobs[s].time_fin);
			}
			break;
		}

	}

	//return 0;
} //end main
