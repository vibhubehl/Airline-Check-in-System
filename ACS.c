#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "linkedList.h"
#include "arguementParser.c"
#include "errorGenerator.c"
/**   
 The time calculation could be confusing, check the exmaple of gettimeofday on tutorial for more detail.
 */


/* global variables */
// #define NQUEUE 5
#define NClerks 5
#define NQueque 2
#define IDLE -1

int NBuisnessQueQue = 0;
int NEconomyQueQue = 0;
int queue_status[2];
node* heads[2];
node* econonmyHead=NULL;
pthread_mutex_t quequeMutex[NQueque];
pthread_cond_t isClerkFree;
struct timeval init_time; // use this variable to record the simulation start time; No need to use mutex_lock when reading this variable since the value would not be changed by thread once the initial time was set.
double overall_waiting_time; //A global variable to add up the overall waiting time for all customers, every customer add their own waiting time to this variable, mutex_lock is necessary.
bool winner_selected[NQueque];
int queue_length[NQueque];// variable stores the real-time queue length information; mutex_lock needed


/* Other global variable may include: 
 1. condition_variables (and the corresponding mutex_lock) to represent each queue; 
 2. condition_variables to represent clerks
 3. others.. depend on your design
 */

// function entry for customer threads
void * customer_entry(void * cus_info){
	struct node * p_myInfo = (struct node*) cus_info;
	int cur_queue = p_myInfo->class_type;
	// wait till arrival times occur
	usleep(p_myInfo->arrival_time);
	fprintf(stdout, "A customer arrives: customer ID %d. \n", p_myInfo->user_id);
	printf("before\n");

	// quequeMutex must be locked before using the queque
	pthread_mutex_lock(&quequeMutex[cur_queue]);
	{
		/* Enqueue operation: get into either business queue or economy queue by using p_myInfo->class_type*/
		// check if customer is buisness or economy
		add_to_queque(&p_myInfo, &heads[cur_queue]);
		queue_length[cur_queue]++;
		fprintf(stdout, "A customer enters a queue: the queue ID %d, and length of the queue %d. \n", cur_queue, queue_length[cur_queue]);

		while (1) {
			pthread_cond_wait(&isClerkFree, &quequeMutex[cur_queue]);
			if ((heads[cur_queue] == p_myInfo) && !winner_selected[cur_queue]) {
				removeUsingId(p_myInfo->user_id, &heads[cur_queue]);
				queue_length[cur_queue]--;
				winner_selected[cur_queue] = true; // update the winner_selected variable to indicate that the first customer has been selected from the queue
				break;
			}
		}
	}
	// queque available for other threads
	pthread_mutex_unlock(&quequeMutex[cur_queue]);

	// /* Try to figure out which clerk awoken me, because you need to print the clerk Id information */
	usleep(10); // Add a usleep here to make sure that all the other waiting threads have already got back to call pthread_cond_wait. 10 us will not harm your simulation time.
	int clerk_woke_me_up = queue_status[cur_queue];
	queue_status[cur_queue] = IDLE;
	
	// /* get the current machine time; updates the overall_waiting_time*/
	
	
	// fprintf(stdout, "A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", /*...*/);
	
	usleep(p_myInfo->service_time);
	
	// /* get the current machine time; */
	// fprintf(stdout, "A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", /* ... */);\
	
	pthread_cond_signal(&isClerkFree); // Notify the clerk that service is finished, it can serve another customer
	
	pthread_exit(NULL);
	return NULL;
}

// function entry for clerk threads
// void *clerk_entry(void * clerkNum){
	
// 	while(TRUE){
		
// 		/* selected_queue_ID = Select the queue based on the priority and current customers number */
		
// 		pthread_mutex_lock(/* mutexLock of the selected queue */);
		
// 		queue_status[selected_queue_ID] = clerkID; // The current clerk (clerkID) is signaling this queue
		
// 		pthread_cond_broadcast(/* cond_var of the selected queue */); // Awake the customer (the one enter into the queue first) from the selected queue
		
// 		winner_selected[selected_queue_ID] = FALSE; // set the initial value as the customer has not selected from the queue.
		
// 		pthread_mutex_unlock(/* mutexLock of the selected queue */);
		
// 		pthread_cond_wait(/* convar of the current clerk */); // wait for the customer to finish its service
// 	}
	
// 	pthread_exit(NULL);
	
// 	return NULL;
// }



int main(int argc, char** argv) {
	// checking for invalid arguements
	if(argc < 2){
		errorGen(1);
		exit(0);
	}
	else if(argc > 2){
		errorGen(2);
		exit(2);
	}
	// name of the file containing customer info
	char* fileName = argv[1];
	int i = 0;
	queue_length[0] = false;
	queue_length[1] = false;
	heads[0]=NULL;
	heads[1]=NULL;
	queue_status[0] = IDLE;
	queue_status[1] = IDLE;

	// initialize all the condition variable and thread lock will be used
	
	/** Read customer information from txt file and store them in the structure you created 	
	// 	1. Allocate memory(array, link list etc.) to store the customer information.
	// 	2. File operation: fopen fread getline/gets/fread ..., store information in data structure you created
	// */
	// fileExtractor(&head, fileName);
	// create clerk thread (optional)
	// for(int i = 0; i < NClerks; i++){ // number of clerks
	// 	pthread_create(&clerkId[i], NULL, clerk_entry, (void *)&clerk_info[i]); // clerk_info: passing the clerk information (e.g., clerk ID) to clerk thread
	// }
	FILE *fptr;
	// get number of customers in file
	if( fopen(fileName, "r")){
		fptr = fopen(fileName, "r");
	}
	else{
		errorGen(3);
		exit(3);
	}
	
	int NCustomers = getNCustomers(fptr);
	pthread_t customId[NCustomers];

	//create customer thread
	for(i = 0; i < NCustomers; i++){ // number of customers
		// defining new node that will added in threads
		node* newNode = ( node*)malloc(sizeof(node));
		initializeCustomers(&newNode, fptr);
		pthread_create(&customId[i], NULL, customer_entry, (void *)newNode); //custom_info: passing the customer information (e.g., customer ID, arrival time, service time, etc.) to customer thread
		pthread_join(customId[i], NULL);
	}
	// wait for all customer threads to terminate
	// forEach customer thread{
	// 	pthread_join(...);
	// }
	// destroy mutex & condition variable (optional)
	
	// calculate the average waiting time of all customers
	return 0;
}