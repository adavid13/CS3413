 #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CHARS 50
#define SIZE 20

int find_first(int arrival[], int duration[], int current_time);
void preempt(int z);

int main(){
	char str[20];
	char line[60];

	int i;
	for(i=0;i<4;i++){ //skip the header
		scanf("%s",str);
	}

	char user[SIZE][20];
	char process[SIZE];
	int arrival[SIZE];
	int duration[SIZE];
	int count = 0;
	memset(str, '\0', sizeof(str));
	memset(user, '\0', sizeof(user));

	while((scanf("%s",str))!=EOF){

		strcat(user[count], str);
		strcpy(line,str);
		strcat(line,"	");

		scanf("%s",str);
		process[count] = *str;
		strcat(line,str);
		strcat(line, "	");

		scanf("%s",str);
		arrival[count] = (*str - '0');
		strcat(line,str);
		strcat(line, "	");

		scanf("%s",str);
		duration[count] = (*str - '0');
		strcat(line,str);
		strcat(line, "\n");
		count++;
	}
	//Totaling the total duration
	int s, tt = 0, time, remain=0, min, finnish_time, waitTot=0, taTot;
	for(s=0;s<count;s++){
		tt+=duration[s];
	}

	int remaining[count]; //array of remaining time of processes
	for(s=0;s<count;s++){
		remaining[s] = duration[s]; //populate remaining array with the duration value of elements
	}
	remaining[9]=9999;
    for(time=0;remain!=count;time++)
    {
	printf("%c\n",process[time]);
        min=9;
        for(i=0;i<count;i++)
        {
            if(arrival[i]<=time && remaining[i]<remaining[min] && remaining[i]>0)
            {
                min=i;
            }
        }
        remaining[min]--;
        if(remaining[min]==0)
        {
            remain++;
            finnish_time=time+1;
            printf("\nP[%d]\t|\t%d\t|\t%d",min+1,finnish_time-arrival[min],finnish_time-duration[min]-arrival[min]);
            waitTot+=finnish_time-duration[min]-arrival[min];
            taTot+=finnish_time-arrival[min];
        }
    }

}//end main