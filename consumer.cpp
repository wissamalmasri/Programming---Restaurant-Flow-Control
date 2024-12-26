/* CS 480 Operating Systems
* Assignment 4
 * Nov 14, 2024
 *
 * Authors:
 * YeonJun Lee
 * Wissam Almasri
 */


#include "consumer.h"
#include "log.h"
#include <pthread.h>
#include <unistd.h>
#include <queue>

// External declarations to access shared resources defined in other files
// The shared queue holding requests
extern std::queue<RequestType> requestQueue;
// Mutex to protect the request queue
extern pthread_mutex_t queueMutex;
// Condition variables for queue state
extern pthread_cond_t queueNotEmpty, queueNotFull;
// Tracks how many requests each consumer has handled
extern unsigned int consumed[ConsumerTypeN][RequestTypeN];
// Tracks the number of each request type in the queue
extern unsigned int inRequestQueue[RequestTypeN];
// Max requests and sleep times for consumers
extern int maxRequests, xSleep, rSleep;

// Forward declaration for the barrier signaling function
void signalBarrier();

// Consumer thread function
void* consume(void* arg) {
    //Cast the argument to a Consumertype and determine the sleep time for the consumer
    ConsumerType type = *(ConsumerType*)arg;
    int sleepTime = (type == TX) ? xSleep : rSleep;

    while (true) {
        // Lock the queue mutex to safely access shared resources
        pthread_mutex_lock(&queueMutex);

        // Wait until the queue is not empty or all requests are consumed
        while (requestQueue.empty()) {
            // Check if all requests have been consumed
            if (consumed[TX][GeneralTable] + consumed[TX][VIPRoom] +
                consumed[Rev9][GeneralTable] + consumed[Rev9][VIPRoom] >= maxRequests) {
                // Notify the main thread that all requests are consumed
                signalBarrier();
                // Unlock the mutex before exiting
                pthread_mutex_unlock(&queueMutex);
                return nullptr;
            }
            // Wait for a signal that the queue is not empty
            pthread_cond_wait(&queueNotEmpty, &queueMutex);
        }

        // Consume a request from the front of the queue
        RequestType reqType = requestQueue.front(); // Get the request type at the front of the queue
        requestQueue.pop(); // Remove the request from the queue

        // Update the consumer's statistics
        consumed[type][reqType]++;
        inRequestQueue[reqType]--;
        output_request_removed(type, reqType, consumed[type], inRequestQueue); // Log the request removal

        // Signal producers that the queue is not full
        pthread_cond_signal(&queueNotFull);

        pthread_mutex_unlock(&queueMutex);//Unlock the queue mutex

        usleep(sleepTime * 1000);// Simulate consumption by sleeping for the specified time (convert ms to μs)
    }
}
