#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define SIZE (int)pow(2,10) //size of page directory array as well as page table array

typedef struct pageTableEntry{
	int invalid; //1 if invalid, 0 if valid
	int frame; //will hold the frame number
}ptEntry;

typedef struct physicalMemoryEntry{
	int empty; //1 if frame is empty, 0 if frame is occupied
	int clean; //1 if frame is clean, 0 if frame is dirty
	int age; //age of frame
	unsigned long int value; //the value to be stored in physical memory
}pmEntry;

int number_frames; //number of frames in physical memory taken as command line argument
ptEntry ** page_dir; //an ptEntry pointer array of size 2^10 that will hold pointers (addresses) to page table
pmEntry * phys_mem; //pmEntry array of size number_frames that will represent physical memory
int globalAge = 0; //global counter to count "time" of program to be stored in pages for the LRU algorithm

int pageMask = 0x3FF; //mask the middle 10 bits to get
int offsetMask = 0xFFF; //mask to get the last 12 bits
int pageFault = 0; //counter for the number of page faults
int dirtyPageCount = 0; //counter for the number of dirty page that have been paged out

void init_pd(){
	page_dir = (ptEntry **)malloc(SIZE*sizeof(ptEntry *)); //an array of ptEntry pointers
	int i;
	for(i=0; i<SIZE; i++){
		page_dir[i] = NULL; //initialize all entries to NULL
	}
}

/*
 * An array of ptEntry structs
 */
ptEntry * init_pt(int sz){
	ptEntry * pt = (ptEntry *)malloc(sizeof(ptEntry) * sz); //array of ptEntry's structs
	if(pt == NULL){
		return NULL;
	}
	int i;
	for(i=0; i<sz; i++){
		pt[i].invalid = 1; //set PT entry to invalid
		pt[i].frame = 0; //set page value to empty
		//pt[i].age = 0; //set age to 0
	}
	return pt;
}

void init_phys_mem(int size){
	phys_mem = (pmEntry *)malloc(size*sizeof(pmEntry));
	int i;
	for(i=0; i<size; i++){
		phys_mem[i].empty = 1; //initialize all frames of physical memory to empty
		phys_mem[i].clean = 1; //set frame as clean
		phys_mem[i].age = 0; //set age to 0
		phys_mem[i].value = 0; //set value to 0
	}
}

/*
 * This function traverses through physical memory and looks for an empty frame.
 * If an empty frame is found, updates the frame
 * returns empty frame index if an empty frame was found or -1 if no empty frame was found
 */
int findFrame(unsigned int address, char access_type){
	int i;
	for(i=0; i<number_frames; i++){
		if(phys_mem[i].empty == 1){ //empty frame found
			phys_mem[i].value = address; //copy offset to frame
			phys_mem[i].empty = 0; //mark frame as not empty
			if(access_type == 'r'){
				phys_mem[i].clean = 1; //set frame to clean
			}else{
				phys_mem[i].clean = 0; //set frame to dirty
			}
			return i;
		}
	}
	return -1; //no empty frame found
}
/*
 * Function finds a victim frame using the Recently Used Algorithm.
 * The victim is a page that has not been used for the longest time.
 * Prefer clean frame, if no clean frame found then use a dirty frame.
 * Returns the index of the updated frame
 */
int replaceFrame(char access_type, unsigned long int address){
	int i, victimFrame = 0;
	unsigned int youngest = UINT_MAX;
	//loop with preference for clean frames first
	for(i=0; i<number_frames; i++){
		if((phys_mem[i].empty == 0)&&(phys_mem[i].age < youngest)&&(phys_mem[i].clean == 1)){ // find the page with the smallest age
			youngest = phys_mem[i].age; //update youngest
			victimFrame = i; //update victim frame
			}
	}

	if(youngest==UINT_MAX){ //if no clean frame was found
		dirtyPageCount++; //a dirty page will be swapped out at this stage so we update it
		//loop trying to find a dirty frame
		for(i=0; i<number_frames; i++){
			if((phys_mem[i].empty == 0)&&(phys_mem[i].age < youngest)){ // find the page with the smallest age
				youngest = phys_mem[i].age; //update youngest
				victimFrame = i; //update victim frame
			}
		}
	}

	unsigned int pd = phys_mem[victimFrame].value >> 22;
	unsigned int pt = (phys_mem[victimFrame].value >> 12) & pageMask;
	page_dir[pd][pt].invalid = 1; //set victim page to invalid
	phys_mem[victimFrame].age = globalAge; //update victim page's age
	phys_mem[victimFrame].value = address;
	phys_mem[victimFrame].empty = 0;
	if(access_type == 'r'){
		phys_mem[victimFrame].clean = 1; //set frame to clean
	}else{
		phys_mem[victimFrame].clean = 0; //set frame to dirty
	}


	return victimFrame;
}

/*
 * First check if page directory exists, if it does, check page table, if valid, you can use the frame that the page table entry points to
 * If page directory entry exists but not page table, create one, find frame and use it
 */
void mem_mangement(unsigned long logical_address, char access_type){
	unsigned int pd = logical_address >> 22;
	unsigned int pt = (logical_address >> 12) & pageMask;
	unsigned int offset = logical_address & offsetMask;
	//printf("access type = %c, logical address is = %lu, pd = %u, pt = %u, offset = %u\n", access_type, logical_address, pd, pt, offset);
	if(!((access_type == 'r')||(access_type == 'w'))){
		printf("Invalid access type\n");
		return;
	}
	unsigned long int upperLim = (unsigned long int)pow(2,32);
	if((logical_address > upperLim) || (logical_address < 0)){
		printf("Invalid logical address\n");
		return;
	}
	if(page_dir[pd] == NULL){ //if there is no entry in the page directory for that address
		pageFault++;
		page_dir[pd] = init_pt(SIZE); //set PD entry as the ptr to the new PT
		int index = findFrame(logical_address, access_type); //find an empty frame
		if(index!=-1){ //an empty frame was found
			page_dir[pd][pt].frame = index; //update PT with frame index
			page_dir[pd][pt].invalid = 0; //PT entry is now valid
			phys_mem[index].age = globalAge;
			printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
		}else{ //no empty frame was found
			int index = replaceFrame(access_type, logical_address); //page out
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
		}

	}else if((page_dir[pd] != NULL)&&(page_dir[pd][pt].invalid == 1)){ //if entry in PT is invalid
		pageFault++; //increment page fault counter
		int index = findFrame(logical_address, access_type); //find an empty frame
		if(index!=-1){ //an empty frame was found
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
		}else{ //no empty frame was found
			int index = replaceFrame(access_type, logical_address); //page out
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
		}
	}else if((page_dir[pd] != NULL)&&(page_dir[pd][pt].invalid == 0)){ //if PD entry exists and PT entry is valid
		int index = page_dir[pd][pt].frame;
		if(access_type == 'w'){ //if the access type is write
			phys_mem[index].clean = 0; //set frame to dirty
		}
		phys_mem[index].age = globalAge;
		printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
	}
}

int main(int argc, char ** argv){
	if(argc != 2){
		printf("Incorrect command line arguments");
		return 0;
	}
	sscanf(&argv[1][0], "%d", &number_frames);
	printf("number of frames = %d\n", number_frames);

	init_phys_mem(number_frames);
	init_pd();

	char access_type;
	unsigned long logical_address;
	while(scanf("%c ", &access_type)!=EOF){
		scanf("%lu ", &logical_address);
		globalAge++;
		//printf("Time: %d ", globalAge);
		mem_mangement(logical_address, access_type);
	}

	printf("Number of page faults: %d\n", pageFault);
	printf("Number of dirty pages that have been paged out: %d\n", dirtyPageCount);


	int i;
	for(i=0; i<SIZE; i++){
		if(page_dir[i]!=NULL){
			free(page_dir[i]);
		}
	}
	free(page_dir);
	free(phys_mem);
	return 0;
}
