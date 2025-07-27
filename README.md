<p align="center">
  <img src="./public/lite-store%20logo.png" alt="Logo" width="200" />
</p>

# Lite-Store

Lite store aims to be a simple **in-memory data store** that is usable via an **HTTP server**, both written completely in C. It is powered with multi-threading paired with mutual exclusion of shared data, to prevent critical section problems. Lite-store also persists data across shut-downs making it a durable solution.

This project is built with two main components:

1. The HTTP server
1. The in-memory data store

---

## Table of contents

1. [Project Overview](#project-overview)
1. [Setup Guide and Working](#setup-guide-and-working)
1. [In-depth Run Through](#in-depth-run-through)
    - [Flow Diagram](#flow-diagram)
    - [HTTP Server with Multithreading](#http-server-with-multithreading)
        - [Request to Response cycle](#request-to-response-cycle)
        - [pthreads](#pthreads)
    - [Data Store with mutual exclusion](#data-store-with-mutual-exclusion)
        - [Hash Table](#hash-table)
        - [Hash Function](#hash-function)
    - [Persistence](#persistence)
1. [Mutual Exclusion (mutex)](#mutual-exclusion-mutex)
1. [Complexities](#complexities)
1. [Redis vs Lite-Store](#redis-vs-lite-store)
1. [Future Improvements](#future-improvements)
1. [Resources](#resources)

---

## Project Overview

Lite-store is a simple in-memory data store that is made usable via an HTTP server. It is written completely in the C programming language, due to the language's characteristics of being extremely fast, providing efficient control over memory, being lightweight and portable, and most importantly - it is a great language for computer science students to learn system-level concepts.

This project is built in two parts, like mentioned before. An HTTP server that listens for requests, parses them and sends them along to the in-memory data store to execute the intended queries. The in-memory data store allows you to perform basic CRUD operations to a hash table using name value pairs. Creating a name-value pair via a `POST` request, retrieving an already created pair via a `GET` request, or deleting a stored value using a `DELETE` request. The guide for usage is available in the [Setup Guide and Working](#setup-guide-and-working) section of the documentation.

The project, to remain usable at high workloads and remain fault-tolerant, is persistent to disk (non-volatile memory), and executes requests using parallel threading - pthreads.

---

## In-depth Run Through

### Flow Diagram

### HTTP Server with Multithreading

#### Request to Response cycle

The client begins by sending an http request to the server, which begins to parse it. As it parses the http request, it builds a `request` structure to store the various parts of the request:

```
typedef struct {
    char name[50];
    char value[50];
} header;

typedef struct {
    char method[10];
    char path[100];
    char version[50];

    header headers[100];
    int header_count;

    char body[1024];
    int body_length;
} http_request;
```

The request is then managed by sending it over to the right handler to process the many types of requests it can be. A response is then formed and sent back to the client. The entire process is very well logged and all errors are gracefully handled with proper 400 and 500 responses returned.

#### pthreads

To avail the benefits of multi-threading, and since this was built on a UNIX based OS, we use POSIX threads (pthreads). `pthreads` provides us with functionality to create, wait for, cancel, and terminate threads. The main advantage of multithreading is parallel processing, and processing of tasks on other threads without taking up the main thread. Here, each request is processed on a newly created thread, while the main thread is the one which constantly listens for requests to create new threads to process it, and it doesn't end up blocking a second request while processing a first one.

![multithreading flowchart for web servers](/public/pthreads-flowchart.png)

### Data Store with mutual exclusion

The in-memory data store is essentially a **hash table** on memory. We decide the hash table index for a particular name-value pair by using a **hash function**.

#### Hash Table

The hash table is defined every time we run the program, and is erased out of memory every time we quit the program. Each entry of the hash table is a node in a linked list. Our hash table has a maximum capacity of `1024` entries.

Hash tables are prone to collisions, which occurs when a hash function maps two different keys (in this case the name) to the same index to store the value. Using a technique called **chaining**, we avoid collisions. Chaining is the process of linking each entry at the index to the next one and so on, allowing us to store infinite entries with the same index.

![chaining diagram](/public/chaining.png)

#### Hash Function

The hash function used here is called DJB2.

```
// DJB2 hash function
unsigned long hash_function(const char* name){
    if(!name){
        perror("name not found (hash function)");
        return 0;
    }

    unsigned long hash = 5381;

    int c;
    while((c = *name++)){
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}
```

The DJB2 hash function, created by Daniel J. Bernstein, is a simple and effective algorithm that generates a hash value by starting with a base value (5381) and iteratively multiplying it by 33 and adding the ASCII values of the characters in the input string. This method is known for its good distribution and speed, making it a popular choice for string hashing.

### Persistence

In order to make the server **durable**, we want to make sure that on unplanned shutdowns, data remains persisted and can be accessed again on server restart.

By default, this system stores and retrieves values from a `data.csv` file, and will create one in the root directory of the project if there isn't one already.

---

## Mutual Exclusion (mutex)

Since we have a store of data that is accessed constantly, it becomes important to make sure that no two executing processes or threads execute their critical section simultaneously while causing unpredictable results to the data.

To prevent this, we use a synchronisation technique with the help of a **mutex** - `pthread_mutex_t`.

Three mutexes are used in this program:

-   counter_mutex
-   write_mutex
-   read_count_mutex

These mutexes are also utilized in a very specific manner, so as to solve the readers-writes problem. More information about this can be found in the [resources](#resources) section of the document.

---

## Complexities

### Time Complexity for Closed Addressing (Chaining)

| **Activity**     | **Best Case Complexity** | **Average Case Complexity** | **Worst Case Complexity** |
| ---------------- | ------------------------ | --------------------------- | ------------------------- |
| Searching        | O(1)                     | O(1)                        | O(n)                      |
| Insertion        | O(1)                     | O(1)                        | O(n)                      |
| Deletion         | O(1)                     | O(1)                        | O(n)                      |
| Space Complexity | O(m+n)                   | O(m+n)                      | O(m+n)                    |

---

## Redis vs Lite-Store

This project is a lightweight, multi-threaded key-value store written in C, built to mirror the core functionality of Redis. It operates entirely in-memory, using a hash table to provide fast, near O(1) lookups for string-based data. Like Redis, it handles concurrent client requests over a network and includes a basic persistence mechanism, saving the entire dataset to a CSV file upon shutdown. This foundation successfully emulates Redis's primary architecture of an in-memory database with network access and data persistence.

While this server captures the spirit of Redis, it's important to understand its limitations. Redis uses a highly-optimized single-threaded event loop for most operations, which can outperform a traditional multi-threaded model by avoiding locking overhead. Its persistence is also far more robust, offering both point-in-time snapshots (RDB) and command logging (AOF). Most significantly, Redis is a full-fledged data structure server supporting lists, sets, and hashes, whereas this project is strictly a key-value store for strings. While this project is certainly not production ready, it serves as a great foundation to build upon.

---

## Future Improvements

This project is always looking for improvements, and anyone reading this can clone it onto their system and improve upon it. If you're wondering where you can help, here are some ideas:

-   Support for More Data Structures like `Lists`, `Hashes` and `Sets`
-   Advanced Persistence (AOF) by logging data onto disk after every operation instead of a snapshot on shut-down
-   Key Expiration with TTL (Time-To-Live)
-   Data Replication for high data availability
-   A more advanced and improved networking model using I/O multiplexing (`epoll` or `kqueue`)

--- 

## Resources:

- [Making a simple HTTP webserver in C by Jan Pieter](https://bruinsslot.jp/post/simple-http-webserver-in-c/#fn1)
- [https-server by 7etsuo](https://github.com/7etsuo/http-server)
- [Multithreading in C - GFG](https://www.geeksforgeeks.org/c/multithreading-in-c/)
- [Mutex lock for Linux Thread Synchronization - GFG](https://www.geeksforgeeks.org/linux-unix/mutex-lock-for-linux-thread-synchronization/)
- [Readers-Writers Problem - GFG](https://www.geeksforgeeks.org/operating-systems/readers-writers-problem-set-1-introduction-and-readers-preference-solution/)

---

Thanks for reading so far down. Raise any issue and contribute using pull-requests at your will! 

If you like this project, please consider [starring us and/or watching us on GitHub](https://github.com/iammohitvs/lite-store) ❤️