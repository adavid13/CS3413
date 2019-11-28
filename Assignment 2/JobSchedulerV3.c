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
//		strcpy(line,str);
//		strcat(line,"	");

		scanf("%s",str); //scan process
		process[count] = *str;
//		strcat(line,str);
//		strcat(line, "	");

		scanf("%d",&num); //scan arrival
		arrival[count] = num;
//		strcat(line,str);
//		strcat(line, "	");

		scanf("%d",&num); //scan duration
		duration[count] = num;
//		strcat(line,str);
//		strcat(line, "	");

		scanf("%d",&num); //scan deadline
		deadline[count] = num;
		printf("user: %s\n",user[count]);
//		strcat(line,str);
//		strcat(line, "\n");
		count++;
	}
	//Totaling the total duration
	int s, tt = 0;
	for(s=0;s<count;s++){
		tt+=duration[s];
	}

	//index of the first process to start
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
			}else if(count>1){
				int min = initial;
				for(s=1; s<count; s++){ //compare the first element with the rest of the elements
					if((remaining[s]!=0)&&(arrival[s] <= current_time)&&(deadline[s]<deadline[min])){
						min = s;
						break;
					}else if((remaining[s]!=0)&&(arrival[s] <= current_time)&&(deadline[s]==deadline[min])&&(remaining[s]<remaining[min])){
						min = s;
						break;
					}else if((remaining[s]!=0)&&(arrival[s] <= current_time)&&(deadline[s]==deadline[min])&&(remaining[s]==remaining[min])&&(arrival[s]<arrival[min])){
						min = s;
						break;
					}//end if
				}//end for
				first = min;
			}//end if
			return first;
		}//end if
	}//end find_first

	int remaining[count]; //array of remaining time of processes
	for(s=0;s<count;s++){
		remaining[s] = duration[s]; //populate remaining array with the duration value of elements
	}
	int k, u, current_time = arrival[0], all = 0;
	printf("Time:\t");
	for(k=0; k<proc; k++){
		printf("CPU%d\t",k);
	}
	printf("\n");


	while(all<count){
		int inline_counter = 0;		
		int z = 0;
		if(current_time > tt){
			printf("%d\tIDLE\n", current_time);
			exit (0);
		}
		s = find_first(arrival, deadline, current_time, remaining);
			HERE1:s = find_first(arrival, deadline, current_time, remaining);
			for(i=0;i<remaining[s];i++){
				if(remaining[s]>0){
						if(inline_counter==proc){				
							printf("\n%d", current_time);
							current_time++;
							inline_counter=0;	
						}
						printf("\t%c", process[s]);
						inline_counter++;
						remaining[s]--;
						for(z=0;z<count;z++){ //check for pre-emption
							if((remaining[z]!=0)&&(arrival[z]<=current_time)&&(deadline[z]<deadline[s])){ 
								goto HERE2;
							}else if((remaining[z]!=0)&&(arrival[z]<=current_time)&&(deadline[z]==deadline[s])&&(duration[z]<remaining[s])){
								goto HERE2;
							}//end if
						} //end for
				}//end if
				all++;
			} //end for

		HERE2:if(remaining[z]>0){
			for(u=0;u<remaining[z];u++){
				if(inline_counter==proc){
					printf("\n%d", current_time);
					current_time++;
					inline_counter=0;
				}
				printf("\t%c", process[z]);
				inline_counter++;
				remaining[z]--;
				for(k=0;k<count;k++){ //check for pre-emption
					if((remaining[k]>0)&&(arrival[k]<=current_time)&&(deadline[k]<deadline[z])){
						z=k;
						goto HERE2;
					}else if((remaining[k]>0)&&(arrival[k]<=current_time)&&(deadline[k]==deadline[z])&&(duration[k]<remaining[z])){
						z=k;
						goto HERE2;
					}else{
						goto HERE1;
					}//end if
				} //end for
			} //end for
			
			all++;
		}//end if
		//all++;
	}//end while
	return 0;
} //end main
