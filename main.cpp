/* CS 480 Operating Systems
* Assignment 4
 * Nov 14, 2024
 *
 * Authors:
 * YeonJun Lee
 * Wissam Almasri
 */

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include "seating.h"
#include "log.h"
#include "producer.h"
#include "consumer.h"

// Global shared data
// Mutex to protect access to the request queue
pthread_mutex_t queueMutex;
// Condition variables for thread synchronization
pthread_cond_t queueNotEmpty, queueNotFull, barrierCond;
// Mutex to synchronize the barrier condition
pthread_mutex_t barrierMutex;
// Flag to indicate if all requests have been consumed
bool allConsumed = false;

// Queue and counters
std::queue<RequestType> requestQueue; // Queue to hold the requests
// Counter for produced requests General Table and VIP Room
unsigned int produced[RequestTypeN] = {0, 0};
// Matrix to track consumed requests by consumer type
unsigned int consumed[ConsumerTypeN][RequestTypeN] = {{0, 0}, {0, 0}};
// Tracks the number of requests in the queue
unsigned int inRequestQueue[RequestTypeN] = {0, 0};

// Command line arguments for sleep times and request limits
// Defaults for maximum requests and sleep times
int maxRequests = 120, gSleep = 0, vSleep = 0, xSleep = 0, rSleep = 0;

// Function to parse command-line arguments
void parseArguments(int argc, char* argv[]) {
    int option;
    // Loop through command line options using getopt
    while ((option = getopt(argc, argv, "s:g:v:x:r:")) != -1) {
        switch (option) {
            case 's': maxRequests = std::stoi(optarg); break; // Maximum number of requests to produce
            case 'g': gSleep = std::stoi(optarg); break; // Sleep time for General Table producer
            case 'v': vSleep = std::stoi(optarg); break; // Sleep time for VIP Room producer
            case 'x': xSleep = std::stoi(optarg); break; // Sleep time for TX consumer
            case 'r': rSleep = std::stoi(optarg); break; // Sleep time for Rev9 consumer
        }
    }
}

// Ensure all threads exit and resources are cleaned up
void cleanup(pthread_t producer1, pthread_t producer2, pthread_t consumer1, pthread_t consumer2) {
    // Join all threads to ensure clean termination
    pthread_join(producer1, nullptr); // Wait for producer1 to finish
    pthread_join(producer2, nullptr); // Wait for 2
    pthread_join(consumer1, nullptr); // Wait for 1
    pthread_join(consumer2, nullptr); // Wait for 2

    // Destroy synchronization primitives to free resources
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&queueNotEmpty);
    pthread_cond_destroy(&queueNotFull);
    pthread_cond_destroy(&barrierCond);
    pthread_mutex_destroy(&barrierMutex);
}

int main(int argc, char* argv[]) {
    parseArguments(argc, argv);

    // Initialize synchronization primitives
    pthread_mutex_init(&queueMutex, nullptr);
    pthread_cond_init(&queueNotEmpty, nullptr);
    pthread_cond_init(&queueNotFull, nullptr);
    pthread_cond_init(&barrierCond, nullptr);
    pthread_mutex_init(&barrierMutex, nullptr);

    // Create local variables for enumerated types for clarity
    RequestType generalTable = GeneralTable; // Type for general table requests
    RequestType vipRoom = VIPRoom; // Type for VIP room requests
    ConsumerType tx = TX; // TX consumer type
    ConsumerType rev9 = Rev9; // Rev9 consumer type

    // Create producer and consumer threads
    pthread_t producer1, producer2, consumer1, consumer2;
    pthread_create(&producer2, nullptr, produce, (void*)&vipRoom); // Producer thread for VIP rooms
    pthread_create(&producer1, nullptr, produce, (void*)&generalTable); // Producer thread for general tables
    pthread_create(&consumer1, nullptr, consume, (void*)&tx); // Consumer thread for TX
    pthread_create(&consumer2, nullptr, consume, (void*)&rev9); // Consumer thread for Rev9

    // Wait for all requests to be consumed using a barrier
    pthread_mutex_lock(&barrierMutex); // Lock the barrier mutex
    while (!allConsumed) { // Wait until all requests are consumed
        // Wait on the barrier condition
        pthread_cond_wait(&barrierCond, &barrierMutex);
    }
    pthread_mutex_unlock(&barrierMutex); // Unlock the barrier mutex

    // Convert consumed data into a pointer array for output function
    unsigned int* consumedPtrs[ConsumerTypeN];
    for (int i = 0; i < ConsumerTypeN; i++) {
        // Assign each row of the consumed matrix
        consumedPtrs[i] = consumed[i];
    }

    // Output the final production and consumption history
    output_production_history(produced, consumedPtrs);


    // Perform cleanup of threads and resources
    //cleanup(producer1, producer2, consumer1, consumer2);

    return 0;
}

// This function should be called from the consumer threads when all requests are handled
void signalBarrier() {
    pthread_mutex_lock(&barrierMutex); // Lock the barrier mutex
    allConsumed = true; // Set the flag to indicate all requests are consumed
    pthread_cond_signal(&barrierCond); // Signal the barrier condition to wake waiting threads
    pthread_mutex_unlock(&barrierMutex); // Unlock the barrier mutex
}
