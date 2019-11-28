#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

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
int first(job_s jobs[], int count, int time);
int *assign(int cpus[], int counter, int index);
int compare(int index1, int index2);

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
	memset(cpus, -1, sizeof(cpus));
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

		count++;
	}
	//Totaling the total duration
	int s, tt = 0;
	for(s=0;s<count;s++){
		tt+=jobs[s].duration;
	}

	/*
	 * Method to find the first job to be executed.
	 * If two jobs have the same deadline,
	 * the method will compare their remaining duration.
	 * If the two jobs have the same remaining duration,
	 * the job that arrived first is given precedence.
	 * All while considering current time,
	 * and whether the job is done.
	 *
	 * Return: the index of the job
	 */
	int first(job_s jobs[], int count, int time){
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

	/*
	 * This method compares between two jobs' indices and returns the index of the job with the higher priority
	 */
	int compare(int index1, int index2){
		int res;
		if((jobs[index1].remaining!=0)&&(jobs[index2].remaining!=0)){ //if the job we compare is not done yet
						if((jobs[index1].deadline < jobs[index2].deadline)&&(jobs[index1].remaining!=0)){
							res = index1;
						}else if((jobs[index1].deadline == jobs[index2].deadline)&&(jobs[index1].remaining < jobs[index2].remaining)&&(jobs[index1].remaining!=0)){
							res = index1;
						}else if((jobs[index1].remaining == jobs[index2].remaining)&&(jobs[index1].remaining!=0)){
							if(jobs[index1].arrival < jobs[index2].arrival){
								res = index1;
							}
						}else{
							res = index2;
						}
					}
		return res;
	}

	int time = jobs[0].arrival; //start time is arrival of first process
	int shortest_ind; //index of the job with the shortest duration to be run
	printf("Time\t");
	for(s=0; s<n_cpus; s++){
		printf("CPU%d\t",s);
	}
	printf("\n");
	int cpu_count = 0;//counts the number of cpus in use
	cpus[cpu_count] = first(jobs, count, time);

	int *assign(int cpus[], int counter, int index){
		if(cpus[counter]!=-1){
			int i;
			for(i=0; i<counter; i++){
				if(compare(i, index)==index){
					cpus[i] = index;
					break;
				}
			}
		}else{
			cpus[counter++] = index;
		}

		return cpus;
	}

	while(1){
		THERE:
		printf("%d\t.",time);
		shortest_ind = first(jobs, count, time);
		int *ar = assign(cpus, cpu_count, shortest_ind);
		for(s=0; s<cpu_count;s++){
			printf("%c\t",jobs[ar[cpu_count]].process);
			jobs[ar[cpu_count]].remaining--;
		}
		cpu_count++;
		if(cpu_count>=n_cpus-1){
					time++;
					cpu_count = (cpu_count+1)%n_cpus;
					goto THERE;
				}
		/*if(jobs[shortest_ind].id!=jobs[cpus[cpu_count]].id){
			cpu_count = (cpu_count+1)%n_cpus;
			cpus[cpu_count] = shortest_ind;
			for(s=0; s<cpu_count; s++){
				printf("\t");
			}
			printf("%c",jobs[cpus[cpu_count]].process);
			jobs[cpus[cpu_count]].remaining--;
			if(cpu_count>=n_cpus-1){
				time++;
				printf("\n");
			}
		}else{
			//for(s=0; s<cpu_count; s++){
				//printf("\t");
			//}
			printf("%c",jobs[cpus[cpu_count]].process);
			jobs[cpus[cpu_count]].remaining--;
			if(cpu_count>=n_cpus-1){
				time++;
				printf("\n");
			}
		}
	*/
		if(jobs[cpus[cpu_count]].remaining==0){
			jobs[cpus[cpu_count]].time_fin = time;
		}
		//printf("\n");
		for(s=0; s<count; s++){
			if(jobs[s].remaining!=0){
				goto THERE;
			}
		}
		break;
	}
	printf("%d\tIDLE\n", time);
	printf("Summary\n");
	for(s=0; s<count; s++){
		printf("%s\t%d\n", jobs[s].user, jobs[s].time_fin);
	}
	//return 0;
} //end main
