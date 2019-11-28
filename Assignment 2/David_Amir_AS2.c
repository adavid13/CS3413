#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CHARS 50
#define SIZE 20

int find_first(int arrival[], int deadline[], int current_time, int remaining[]);
void preempt(int z);

int main(int argc, char *argv[]){

	if(argc!=2){
		printf("Please enter number of processors");
		return 0;
	}
	int proc = ((*argv[1]) - '0');
	printf(" n is = %d\n", proc);

	int cpu_flag[proc];
	
	char str[20];
	int num = 0;
	//char line[60];

	int i;
	for(i=0;i<5;i++){ //skip the header
		scanf("%s",str);
	}

	char user[SIZE][20];
	char process[SIZE];
	int arrival[SIZE];
	int duration[SIZE];
	int deadline[SIZE];
	int count = 0;
	memset(str, '\0', sizeof(str));
	memset(user, '\0', sizeof(user));

	while((scanf("%s",str))!=EOF){

		strcat(user[count], str); //copy username to user array

		scanf("%s",str); //scan process
		process[count] = *str;

		scanf("%d",&num); //scan arrival
		arrival[count] = num;

		scanf("%d",&num); //scan duration
		duration[count] = num;

		scanf("%d",&num); //scan deadline
		deadline[count] = num;
		printf("user: %s\n",user[count]);

		count++;
	}
	//Totaling the total duration
	int s, tt = 0;
	for(s=0;s<count;s++){
		tt+=duration[s];
	}

	/*
	 * Returns index of the first process to start
	 *
	 */
	int find_first(int arrival[], int deadline[], int current_time, int remaining[]){ 
		int first, s;
		int e, initial;
		if(count==1){
			return 0;
		}else{
			for(e=0; e<count; e++){
				if(remaining[e]>0){
					initial = e;
					goto FLAG;
				}
			}
			FLAG:if(count == 1){
				first = initial;
			}else{
				int min = initial;
				for(s=0; s<count; s++){ //compare the first element with the rest of the elements
					if((remaining[s]!=0)&&(arrival[s] <= current_time)&&(deadline[s]<deadline[min])){
						min = s;
						goto THIS;
					}else if((remaining[s]!=0)&&(arrival[s] <= current_time)&&(deadline[s]==deadline[min])&&(remaining[s]<remaining[min])){
						min = s;
						goto THIS;
					}else if((remaining[s]!=0)&&(arrival[s] <= current_time)&&(deadline[s]==deadline[min])&&(remaining[s]==remaining[min])&&(arrival[s]<arrival[min])){
						min = s;
						goto THIS;
					}//end if
				}//end for
				THIS: first = min;
			}//end if
			return first;
		}//end if
	}//end find_first

	int remaining[count]; //array of remaining time of processes
	for(s=0;s<count;s++){
		remaining[s] = duration[s]; //populate remaining array with the duration value of elements
	}
	int k, u, pr, all = 0, current_time = arrival[0];
	printf("Time:\t");
	for(k=0; k<proc; k++){
		printf("CPU%d\t",k);
	}
	printf("\n");
	int line_lim = proc;
	int alldone = 0;

	while(all<=count){
		THERE: if(alldone==1){
			printf("\n%d\tIDLE\n", ++current_time);
			return 0;
		}
		printf("%d\t", current_time);
		THIS2:for(k=0;k<proc; k++){
			int l = find_first(arrival, deadline, current_time, remaining);
			FLAG1:for(u=0; u<duration[l]; u++){
				if(remaining[l]!=0){
					printf("%c",process[l]);
					line_lim--;
					if(line_lim==0){
						printf("\n");
						current_time++;
						printf("%d\t", current_time);
						line_lim = proc; //reset line_lim
						
					}else{
						printf("\t");
					}//end if
					remaining[l]--;
				
					//preemption
					for(pr=0; pr<count; pr++){
						if((pr!=l)&&(deadline[pr]<deadline[l])&&(remaining[pr]!=0)&&(arrival[pr]<=current_time)){//if there is a process with an earlier deadline
							l=pr;
							goto FLAG1;
						}//end if
						if((pr!=l)&&(deadline[pr]==deadline[l])&&(remaining[pr]<remaining[k])&&(remaining[pr]!=0)&&(arrival[pr]<=current_time)){//shorter duration
							l=pr;
							goto FLAG1;
						}//end if
					}//end for
				}//end if
			}//end for
			all++;
		}//end for

		int ti;
		for(ti=0; ti<count; ti++){
			if(remaining[ti]!=0){
				alldone = 0;
				goto THIS2;
			}//end if
		}//end for
		alldone = 1;
		goto THERE;
	}//end while
	return 0;
}//end main