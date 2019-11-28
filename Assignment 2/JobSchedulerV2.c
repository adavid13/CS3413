#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CHARS 50
#define SIZE 20

int find_first(int arrival[], int duration[], int current_time);
void preempt(int z);

int main(int argc, char *argv[]){

	if(argc!=2){
		printf("Please enter number of processors");
		return 0;
	}
	int n = ((*argv[1]) - '0');
	printf(" n = %d\n", n);
	
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
	int find_first(int arrival[], int duration[], int current_time){ 
		int first, s;
		int e, initial;
		if(count<=1){
			return 0;
		}else{
			for(e=0; e<count; e++){
				if(duration[e]>0){
					initial = e;
					goto FLAG;
				}
			}
			FLAG:if(arrival[initial] != arrival[initial+1]){
				first = initial;
			}else{
				int min = initial;
				for(s=1; s<count; s++){
					if((duration[s]!=0)&&(arrival[s] <= current_time)&&(duration[s]<duration[min])){
						min = s;
						break;
					}
				}
				first = min;
			}//end if
			return first;
		}//end if
	}//end find_first

	int remaining[count]; //array of remaining time of processes
	for(s=0;s<count;s++){
		remaining[s] = duration[s]; //populate remaining array with the duration value of elements
	}
	int z, u, current_time = arrival[0], all = 0;
	printf("Time:\tJob:\n");
	while(all<count){
		if(current_time > tt){
			printf("%d\tIDLE\n", current_time);
			exit (0);
		}
		s = find_first(arrival, remaining, current_time);
			HERE1:s = find_first(arrival, remaining, current_time);
			for(i=0;i<remaining[s];i++){
				if(remaining[s]>0){
					printf("%d\t", current_time);
					current_time++;
					printf("%c\n", process[s]);
					remaining[s]--;
					for(z=0;z<count;z++){ //check for pre-emption
						if((duration[z] < remaining[s])&&(arrival[z] <= current_time)){ 
							goto HERE2;
						} //end if
					} //end for					
				}//end if
			} //end for

		HERE2:if(remaining[z]>0){
			for(u=0;u<duration[z];u++){
				printf("%d\t", current_time);
				current_time++;
				printf("%c\n", process[z]);
				remaining[z]--;
				for(z=0;z<count;z++){ //check for pre-emption
					if((duration[z] < remaining[s])&&(arrival[z] <= current_time)){ 
						goto HERE2;
					} //end if
				} //end for
			} //end for
		       	all++;
			goto HERE1;
		}//end if
	}//end while
	return 0;
} //end main
