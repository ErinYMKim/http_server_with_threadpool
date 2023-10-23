#include <stdlib.h>
#include <stdio.h>
#include "thread_pool.h"

// Function run by the threads
void* thread_function(void* arg)
{
    thread_pool_t* pool = arg;
    while(true)
    {
        pthread_mutex_lock(&(pool->lock));

        while(pool->num_jobs == 0 && !pool->shutdown)
        {
            pthread_cond_wait(&(pool->cond), &(pool->lock));
        }

        if(pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->lock));
            pthread_exit(NULL);
        }

        void (*function)(void *) = pool->jobs[pool->num_jobs-1].function;
        void* arg = pool->jobs[pool->num_jobs-1].arg;
        pool->num_jobs--;

        pthread_mutex_unlock(&(pool->lock));

        // Execute the job's function with its argument
        function(arg);
    }
    pthread_exit(NULL);
}

// Functions to manage the thread pool
thread_pool_t* thread_pool_create(int num_threads, int capacity)
{
    thread_pool_t* pool = malloc(sizeof(thread_pool_t));
    pool->jobs = malloc(sizeof(job_t) * capacity);
    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    pool->capacity = capacity;
    pool->num_threads = num_threads;
    pool->num_jobs = 0;
    pool->shutdown = false;
    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->cond), NULL);
    for(int i = 0; i < num_threads; i++)
    {
        pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
    }
    return pool;
}

void thread_pool_destroy(thread_pool_t* pool)
{
    pthread_mutex_lock(&(pool->lock));

    pool->shutdown = true;
    pthread_cond_broadcast(&(pool->cond));

    pthread_mutex_unlock(&(pool->lock));

    // Wait for all threads to finish and clean up resources
    for(int i = 0; i< pool->num_threads; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->cond));

    free(pool->jobs);
    free(pool->threads);
    free(pool);
}

// Function to add a job to the thread pool.
int thread_pool_add_job(thread_pool_t* pool, void (*function)(void*), void* arg){
    int is_added;
    pthread_mutex_lock(&(pool->lock)); // Lock the mutex.
    // Only add the job to the queue if there is free space.
    if(pool->num_jobs < pool->capacity) {
        pool->jobs[pool->num_jobs].function = function;
        pool->jobs[pool->num_jobs].arg = arg;
        pool->num_jobs++;
        // printf("pool.num_jobs:%d\n", pool->num_jobs);
        pthread_cond_signal(&(pool->cond)); // Notify waiting thread that work has been added.
        is_added = 1;
    }
    else {
        is_added = 0;
    }
    printf("pool.num_jobs:%d, pool_capacity:%d, is_added:%d\n", pool->num_jobs, pool->capacity, is_added);
    pthread_mutex_unlock(&(pool->lock)); // Unlock the mutex.
    return is_added;
}
