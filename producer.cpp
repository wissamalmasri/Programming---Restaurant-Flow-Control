/* CS 480 Operating Systems
* Assignment 4
 * Nov 14, 2024
 *
 * Author:
 * Wissam Almasri
 */



#include "producer.h"
#include "log.h"
#include <pthread.h>
#include <unistd.h>
#include <queue>

//External declarations to access shared resources defined in other files
//Shared queue for holding requests
extern std::queue<RequestType> requestQueue;
//Mutex to protect the request queue
extern pthread_mutex_t queueMutex;
//Condition variables for queue state
extern pthread_cond_t queueNotEmpty, queueNotFull;
//Tracks how many requests each producer has created
extern unsigned int produced[RequestTypeN];
//Tracks the number of each request type in the queue
extern unsigned int inRequestQueue[RequestTypeN];
//Max requests and sleep times for producers
extern int maxRequests, gSleep, vSleep;

// Producer thread function
void* produce(void* arg) {
    // Cast the argument to a RequestType and determine the sleep time for the producer
    RequestType type = *(RequestType*)arg;
    int sleepTime = (type == GeneralTable) ? gSleep : vSleep;

    while (true) {
        // Lock the queue mutex to safely access shared resources
        pthread_mutex_lock(&queueMutex);

        // Check if the maximum number of requests has been reached
        if (produced[GeneralTable] + produced[VIPRoom] >= maxRequests) {
            pthread_mutex_unlock(&queueMutex); // Unlock the mutex before exiting
            break;
        }


        // Wait until there is space in the queue or specific conditions for VIP requests are met
        while (requestQueue.size() >= 18 || (type == VIPRoom && inRequestQueue[VIPRoom] >= 5)) {
            pthread_cond_wait(&queueNotFull, &queueMutex);
        }




        // Add the request type to the queue
        requestQueue.push(type);
        produced[type]++; // Increment the count for the produced requests of this type
        //increment the count of this request type in the queue
        inRequestQueue[type]++;
        //Log the addition of the request
        output_request_added(type, produced, inRequestQueue);

        //Signal consumers that the queue is not empty
        pthread_cond_signal(&queueNotEmpty);
        // Unlock the queue mutex
        pthread_mutex_unlock(&queueMutex);
        // Simulate request production by sleeping
        usleep(sleepTime * 1000);
    }

    return nullptr;
}
