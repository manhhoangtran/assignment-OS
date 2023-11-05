#include "threadpool.h"
#include <stdlib.h>

void thread_pool_init(ThreadPool *pool, int num_threads, int job_size) {
    // Initialize job queue
    pool->jobs = (Job*)malloc(job_size * sizeof(Job));
    pool->job_count = 0;
    pool->job_size = job_size;
    pool->front = 0;

    // Initialize mutexes and semaphore
    pthread_mutex_init(&pool->lock, NULL);
    pthread_mutex_init(&pool->job_lock, NULL);
    sem_init(&pool->jobs_available, 0, 0);

    // Create worker threads
    pool->threads = (Thread*)malloc(num_threads * sizeof(Thread));
    pool->num_threads = num_threads;
    pool->stop_requested = 0;

    int i;
    for (i = 0; i < num_threads; i++) {
        pool->threads[i].id = i;
        WorkerInput* input = (WorkerInput*)malloc(sizeof(WorkerInput));
        input->pool = pool;
        input->thread = &pool->threads[i];
        pthread_create(&pool->threads[i].thread, NULL, worker_thread, (void*)input);
    }
}

void thread_pool_submit(ThreadPool *pool, Job job) {
    pthread_mutex_lock(&pool->lock);

    // Check if the job queue is full
    if (pool->job_count >= pool->job_size) {
        printf("job queue is full!\n"); // Print error message
        if (job.should_free && !job.is_freed) {
        	printf("Debug submit clean this job i= %d\n", job.id);  
            free(job.args); // Free args if necessary
            job.is_freed = 1;
        }
        pthread_mutex_unlock(&pool->lock); // Unlock the job queue mutex
        return;
    }

    // Add the job to the correct position in the job queue
    int insert_index = (pool->front + pool->job_count) % pool->job_size;
    pool->jobs[insert_index] = job;
    pool->job_count++;

    pthread_mutex_unlock(&pool->lock);

    // Signal that a job is available
    sem_post(&pool->jobs_available);
}

void* worker_thread(void* args) {
    WorkerInput* input = (WorkerInput*)args;
    ThreadPool* pool = input->pool;
    Thread* thread = input->thread;

    while (1) {
        // Wait for signal on the jobs_available semaphore
        sem_wait(&pool->jobs_available);

        if (pool->stop_requested)
            break;

        pthread_mutex_lock(&pool->lock);

        // Check if there are jobs in the queue
        if (pool->job_count > 0) {
            // Dequeue a job from the front of the queue
            Job job = pool->jobs[pool->front];
            pool->front = (pool->front + 1) % pool->job_size;
            pool->job_count--;

            pthread_mutex_unlock(&pool->lock);

            // Execute the job, considering whether it should be run safely using job_lock
            if (job.run_safely) {
                pthread_mutex_lock(&pool->job_lock);
                job.function(job.args);
                pthread_mutex_unlock(&pool->job_lock);
            } else {
                job.function(job.args);
            }

            // Free the arguments of the job if necessary
            if (job.should_free && !job.is_freed) {
                free(job.args);
                job.is_freed = 1;
            }
        } else {
            pthread_mutex_unlock(&pool->lock);
        }
    }

    printf("thread with id %d is finished.\n", thread->id); // Print thread finished message

    free(input); // Free the WorkerInput structure

    return NULL;
}


void thread_pool_stop(ThreadPool *pool) {
    // Set the stop_requested flag
    pool->stop_requested = 1;

    // Signal all worker threads to exit using sem_post
    int i;
    for (i = 0; i < pool->num_threads; i++) {
        sem_post(&pool->jobs_available);
    }
}

void thread_pool_wait(ThreadPool *pool) {
    // Wait for all worker threads to finish their work using pthread_join
    int i;
    for (i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i].thread, NULL);
    }
}


void thread_pool_clean(ThreadPool *pool) {
    // Clean the args of any job that should be freed
    int i;
    for (i = 0; i < pool->job_size; i++) {
        Job *job = &pool->jobs[i];
        if (job->should_free && !job->is_freed) {
            if (job->args != NULL) {
                printf("Debug pool clean i= %d\n", i);  // Print job->args
                //free(job->args);
                job->args = NULL;
                job->is_freed = 1;
            }
        }
    }

    // Release the memory allocated for the thread pool and its components
    pthread_mutex_destroy(&pool->lock);
    pthread_mutex_destroy(&pool->job_lock);
    sem_destroy(&pool->jobs_available);

    // Free the memory for the threads and jobs arrays
    free(pool->threads);
    free(pool->jobs);
}