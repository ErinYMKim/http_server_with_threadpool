#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdbool.h>
#include <pthread.h>

// Job for thread pool.
typedef struct job
{
    void (*function)(void* arg); // function to be executed as part of the job
    void* arg; // argument to be passed to the function
} job_t;

// Structure for pool of threads.
typedef struct thread_pool
{
    job_t* jobs; // an array of jobs to be executed
    int num_jobs; // number of jobs currently in the pool
    int capacity; // maximum number of jobs the pool can hold
    pthread_t* threads; // an array of worker threads
    int num_threads; // number of worker threads
    pthread_mutex_t lock; // mutex for protecting shared data
    pthread_cond_t cond; // condition variable for signaling when jobs are available
    bool shutdown; // flag to indicate if the thread pool is being shut down
}thread_pool_t;

// function to create a new thread pool
thread_pool_t* thread_pool_create(int num_threads, int capacity);

// function to destroy a thread pool
void thread_pool_destroy(thread_pool_t* pool);

// function to add a job to the thread pool
int thread_pool_add_job(thread_pool_t* pool, void (*function)(void*), void* arg);
#endif