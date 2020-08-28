/*************************************************************************************
 * babyyoda - used to test your semaphore implementation and can be a starting point for
 *			     your store front implementation
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "Semaphore.h"

// Semaphores that each thread will have access to as they are global in shared memory
Semaphore *empty = NULL;
Semaphore *full = NULL;

pthread_mutex_t buf_mutex;

int buffer = 0;
unsigned int consumed = 0;


/*************************************************************************************
 * producer_routine - this function is called when the producer thread is created.
 *
 *			Params: data - a void pointer that should point to an integer that indicates
 *							   the total number to be produced
 *
 *			Returns: always NULL
 *
 *************************************************************************************/

void *producer_routine(void *data) {

	time_t rand_seed;
	srand((unsigned int) time(&rand_seed));

	// The current serial number (incremented)
	unsigned int serialnum = 1;

	// We know the data pointer is an integer that indicates the number to produce
	int left_to_produce = *((int *) data);

	// Loop through the amount we're going to produce and place into the buffer
	while (left_to_produce > 0) {
		printf("Producer wants to put Yoda #%d into buffer...\n", serialnum);

		// Semaphore check to make sure there is an available slot
		full->wait();

		// Place item on the next shelf slot by first setting the mutex to protect our buffer vars
		pthread_mutex_lock(&buf_mutex);

		buffer++;
		serialnum++;
		left_to_produce--;

		pthread_mutex_unlock(&buf_mutex);

		printf("   Yoda put on shelf.\n");

		// Semaphore signal that there are items available
		empty->signal();

		// random sleep but he makes them fast so 1/20 of a second
		usleep((useconds_t) (rand() % 200000));
	}
	return NULL;
}


/*************************************************************************************
 * consumer_routine - this function is called when the consumer thread is created.
 *
 *       Params: data - a void pointer that should point to a boolean that indicates
 *                      the thread should exit. Doesn't work so don't worry about it
 *
 *       Returns: always NULL
 *
 *************************************************************************************/

void *consumer_routine(void *data) {
	(void) data;

	printf("Consumer wants to buy a Yoda...\n");

	// Semaphore to see if there are any items to take
	empty->wait();

	// Take an item off the shelf
	pthread_mutex_lock(&buf_mutex);

	printf("   Consumer bought Yoda #%d.\n", buffer);
	buffer--; // Decrement the buffer as a slot opens up
	consumed++; // Increment the number consumed

	pthread_mutex_unlock(&buf_mutex);

	// Consumers wait up to one second
	usleep((useconds_t) (rand() % 1000000));

	full->signal();

	printf("Consumer goes home.\n");

	return NULL;
}


/*************************************************************************************
 * main - Standard C main function for our storefront.
 *
 *		Expected params: ./babyyoda <BUFFER_SIZE> <NUM_CONSUMERS> <MAX_ITEMS>
 *				BUFFER_SIZE - maximum number of items that can be stocked at once
 *				NUM_CONSUMERS - number of consumer threads in the store
 *				MAX_ITEMS - how many items will be produced before the shopkeeper closes
 *
 *************************************************************************************/

int main(int argv, const char *argc[]) {

	// Get our argument parameters
	if (argv < 2) {
		printf("Invalid parameters. Format: %s <max_items>\n", argc[0]);
		exit(0);
	}

	// User input on the size of the buffer
	unsigned int BUFFER_SIZE = (unsigned int) strtol(argc[1], NULL, 10);
	int NUM_CONSUMERS = (unsigned int) strtol(argc[2], NULL, 10);
	unsigned int MAX_ITEMS = (unsigned int) strtol(argc[3], NULL, 10);

	printf("Can only place %d babyyodas on the shelf.\n", BUFFER_SIZE);
	printf("%d customers want babyyodas.\n", NUM_CONSUMERS);
	printf("Producing %d babyyodas today.\n", MAX_ITEMS);

	// Initialize our semaphores
	empty = new Semaphore(0);
	full = new Semaphore(BUFFER_SIZE); // The full semaphore is limited to the buffer size

	pthread_mutex_init(&buf_mutex, NULL); // Initialize our buffer mutex

	pthread_t producer;
	pthread_t consumer[NUM_CONSUMERS]; // Initialize array of consumers

	// Launch our producer thread
	pthread_create(&producer, NULL, producer_routine, &MAX_ITEMS);

	// Launch our consumer threads through the array
	for (int i = 0; i < NUM_CONSUMERS; i++) {
		pthread_create(&consumer[i], NULL, consumer_routine, NULL);
	}

	// Wait for our producer thread to finish up
	pthread_join(producer, NULL);

	printf("The manufacturer has completed his work for the day.\n");

	printf("Waiting for consumer to buy up the rest.\n");

	// Give the consumers a second to finish snatching up items
	while (consumed < BUFFER_SIZE)
		sleep(1);

	// Now make sure they all exited
	for (int i=0; i<NUM_CONSUMERS; i++) {
		pthread_join(consumer[i], NULL);
	}

	// We are exiting, clean up
	delete empty;
	delete full;

	printf("Producer/Consumer simulation complete!\n");
}
