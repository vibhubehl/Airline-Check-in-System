#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "linkedList.h"
#include "arguementParser.c"
#include "errorGenerator.c"
#include <sys/time.h>
#include "sample_gettimeofday.c"
/**   
 The time calculation could be confusing, check the exmaple of gettimeofday on tutorial for more detail.
 */

/* global variables */
// #define NQUEUE 5
#define NClerks 5
#define NQueque 2
#define IDLE -1

int queue_status[2];
node *heads[2];
pthread_mutex_t quequeMutex[NQueque];
pthread_cond_t isClerkFree[NClerks];
pthread_cond_t condVarQueue[NQueque];
double totalTime[NQueque];
int totalCustomer[NQueque];
pthread_mutex_t waitTimeMutex[NQueque];
bool winner_selected[NQueque];
int queue_length[NQueque]; // variable stores the real-time queue length information; mutex_lock needed
struct timeval init_time;  // use this variable to record the simulation start time; No need to use mutex_lock when reading this variable since the value would not be changed by thread once the initial time was set.

/* Other global variable may include: 
 1. condition_variables (and the corresponding mutex_lock) to represent each queue; 
 2. condition_variables to represent clerks
 3. others.. depend on your design
 */

// function entry for customer threads
void *customer_entry(void *cus_info)
{
	struct node *p_myInfo = (struct node *)cus_info;
	int cur_queue = p_myInfo->class_type;
	// wait till arrival times occur
	usleep(p_myInfo->arrival_time * 100000);
	fprintf(stdout, "A customer arrives: customer ID %d. \n", p_myInfo->user_id);

	// quequeMutex must be locked before using the queque
	pthread_mutex_lock(&quequeMutex[cur_queue]);

	/* Enqueue operation: get into either business queue or economy queue by using p_myInfo->class_type*/
	// check if customer is buisness or economy
	double startTime = getCurrentSimulationTime();
	double finishTime;
	add_to_queque(p_myInfo, &heads[cur_queue]);
	queue_length[cur_queue]++;
	fprintf(stdout, "A customer enters a queue: the queue ID %d, and length of the queue %d. \n", cur_queue, queue_length[cur_queue]);
	// queue_enter_time = getCurSystemTime();

	while (1)
	{
		pthread_cond_wait(&condVarQueue[cur_queue], &quequeMutex[cur_queue]);
		if ((heads[cur_queue]->user_id == p_myInfo->user_id) && !winner_selected[cur_queue])
		{
			removeUsingId(p_myInfo->user_id, &heads[cur_queue]);
			finishTime = getCurrentSimulationTime();
			queue_length[cur_queue]--;
			winner_selected[cur_queue] = true; // update the winner_selected variable to indicate that the first customer has been selected from the queue
			break;
		}
	}
	// calculating average time
	pthread_mutex_lock(&waitTimeMutex[cur_queue]);
	totalTime[cur_queue] += (finishTime - startTime);
	totalCustomer[cur_queue]++;
	pthread_mutex_unlock(&waitTimeMutex[cur_queue]);

	// /* Try to figure out which clerk awoken me, because you need to print the clerk Id information */
	usleep(10); // Add a usleep here to make sure that all the other waiting threads have already got back to call pthread_cond_wait. 10 us will not harm your simulation time.
	int clerk_woke_me_up = queue_status[cur_queue];
	queue_status[cur_queue] = IDLE;

	// queque available for other threads
	pthread_mutex_unlock(&quequeMutex[cur_queue]);

	// /* get the current machine time; updates the overall_waiting_time*/

	fprintf(stdout, "A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", getCurrentSimulationTime(), p_myInfo->user_id, clerk_woke_me_up);
	usleep(p_myInfo->service_time * 100000);

	// /* get the current machine time; */
	fprintf(stdout, "A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", getCurrentSimulationTime(), p_myInfo->user_id, clerk_woke_me_up);

	pthread_cond_signal(&isClerkFree[clerk_woke_me_up - 1]); // Notify the clerk that service is finished, it can serve another customer

	pthread_exit(NULL);
	return NULL;
}

// function entry for clerk threads
void *clerk_entry(void *clerkNum)
{
	int clerkID = (int)clerkNum;
	int queueID;
	while (1)
	{
		pthread_mutex_lock(&quequeMutex[0]);
		pthread_mutex_lock(&quequeMutex[1]);
		/* selected_queue_ID = Select the queue based on the priority and current customers number */
		if (queue_status[0] != IDLE || queue_status[1] != IDLE)
		{
			pthread_mutex_unlock(&quequeMutex[0]);
			pthread_mutex_unlock(&quequeMutex[1]);
			usleep(1);
			continue;
		}
		else if (queue_length[1] > 0)
		{
			queueID = 1;
			pthread_mutex_unlock(&quequeMutex[0]);
		}
		else if (queue_length[0] > 0)
		{
			queueID = 0;
			pthread_mutex_unlock(&quequeMutex[1]);
		}
		else
		{
			pthread_mutex_unlock(&quequeMutex[0]);
			pthread_mutex_unlock(&quequeMutex[1]);
			usleep(1);
			continue;
		}

		queue_status[queueID] = clerkID; // The current clerk (clerkID) is signaling this queue

		winner_selected[queueID] = false; // set the initial value as the customer has not selected from the queue.
		pthread_cond_broadcast(&condVarQueue[queueID]); // Awake the customer (the one enter into the queue first) from the selected queue
		pthread_cond_wait(&isClerkFree[clerkID - 1], &quequeMutex[queueID]); // wait for the customer to finish its service
		pthread_mutex_unlock(&quequeMutex[queueID]);
	}

	pthread_exit(NULL);

	return NULL;
}

int main(int argc, char **argv)
{
	// checking for invalid arguements
	if (argc < 2)
	{
		errorGen(1);
		exit(0);
	}
	else if (argc > 2)
	{
		errorGen(2);
		exit(2);
	}
	// name of the file containing customer info
	char *fileName = argv[1];
	int i = 0;
	queue_length[0] = 0;
	queue_length[1] = 0;
	heads[0] = NULL;
	heads[1] = NULL;
	queue_status[0] = IDLE;
	queue_status[1] = IDLE;
	pthread_t clerkId[NClerks];

	// initialize all the condition variable and thread lock will be used
	for (i = 0; i < NQueque; i++)
	{
		pthread_cond_init(&condVarQueue[i], NULL);
		pthread_mutex_init(&quequeMutex[i], NULL);
		totalTime[i] = 0.0;
		winner_selected[i] = false;
		totalTime[i] = 0;
		totalCustomer[i] = 0;
		pthread_mutex_init(&waitTimeMutex[i], NULL);
	}

	/** Read customer information from txt file and store them in the structure you created 	
	// 	1. Allocate memory(array, link list etc.) to store the customer information.
	// 	2. File operation: fopen fread getline/gets/fread .
	.., store information in data structure you created
	// */
	for (i = 0; i < NClerks; i++)
	{
		pthread_cond_init(&isClerkFree[i], NULL);						 // number of clerks
		pthread_create(&clerkId[i], NULL, clerk_entry, (void *)(i + 1)); // clerk_info: passing the clerk information (e.g., clerk ID) to clerk thread
	}
	FILE *fptr;
	// get number of customers in file
	if (fopen(fileName, "r"))
	{
		fptr = fopen(fileName, "r");
	}
	else
	{
		errorGen(3);
		exit(3);
	}

	int NCustomers = getNCustomers(fptr);
	pthread_t customId[NCustomers];
	// get starting time for customer threads
	gettimeofday(&init_time, NULL);
	//create customer thread
	for (i = 0; i < NCustomers; i++)
	{ // number of customers
		// defining new node that will added in threads
		node *newNode = (node *)malloc(sizeof(node));
		initializeCustomers(&newNode, fptr);
		pthread_create(&customId[i], NULL, customer_entry, (void *)newNode); //custom_info: passing the customer information (e.g., customer ID, arrival time, service time, etc.) to customer thread
	}

	// displayLinkedList(&heads[0], 1);
	// wait for all customer threads to terminate
	for (i = 0; i < NCustomers; i++)
	{
		pthread_join(customId[i], NULL);
	}

	printf("The average waiting time for all customers in the system is: %.2f seconds. \n", (totalTime[0]+totalTime[1])/(totalCustomer[0]+totalCustomer[1]));
	printf("The average waiting time for all business-class customers is: %.2f seconds. \n", (totalTime[1])/(totalCustomer[1]));
	printf("The average waiting time for all economy-class customers is: %.2f seconds. \n", (totalTime[0])/(totalCustomer[0]));

	// destroy mutex & condition variable (optional)

	// calculate the average waiting time of all customers
	return 0;
}