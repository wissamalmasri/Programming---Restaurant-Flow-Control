Author 
Wissam Almasri
  
Dining Request Management System

This project simulates a dining request management system using multithreaded programming in C++. Pro

Features
- Implements a producer-consumer system with thread synchronization using mutexes and condition variab
- Tracks and logs the status of the request queue.
- Supports customizable sleep times for producers and consumers.

Files
- `main.cpp`: Initializes the system, parses command-line arguments, and manages thread creation and sy
- `producer.h` and `producer.cpp`: Implements producer functionality to generate requests.
- `consumer.h` and `consumer.cpp`: Implements consumer functionality to process requests.
- `seating.h`: Defines enumerated types for request and consumer types.
- `log.h` and `log.cpp`: Provides logging utilities for tracking queue status and request handling.
- `makefile`: Automates the build process.

Usage
Prerequisites
- A C++ compiler supporting C++11 or later.

Compilation
Run the following command to compile the program:
make
```
Running the Program
Execute the program using:
```bash
./dining_request_system [options]
```
Options:
- `-s <maxRequests>`: Set the maximum number of requests to process (default: 120).
- `-g <gSleep>`: Sleep time (ms) for the General Table producer.
- `-v <vSleep>`: Sleep time (ms) for the VIP Room producer.
- `-x <xSleep>`: Sleep time (ms) for the TX consumer.
- `-r <rSleep>`: Sleep time (ms) for the Rev9 consumer.
Example
```bash
./dining_request_system -s 200 -g 100 -v 150 -x 50 -r 75
```
Output
The program logs:
1. Requests added and removed from the queue.
2. Total requests produced and consumed.
3. Time taken for processing.
Code Overview

Key Components
- **Producers**: Generate requests for General Table and VIP Room seating.
- **Consumers**: Process requests based on a queue.
- **Synchronization**: Mutexes and condition variables ensure thread-safe operations.

Logging
Logs include details about:
- The status of the request queue.
- Production and consumption statistics.

Constraints
- The queue can hold a maximum of 18 unconsumed requests.
- A maximum of 5 VIP Room requests can be in the queue simultaneously.

License
This project is licensed under the MIT License. See the LICENSE file for details.
