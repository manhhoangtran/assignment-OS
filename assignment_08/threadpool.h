#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <semaphore.h>

typedef void (*Function)(void *);

// Job structure
typedef struct {
    int id; // Unique job ID
    Function function; // Pointer to the function to execute
    void *args; // Arguments for the function
    int run_safely; // Argument that will specify if job should be executed safely using job_lock.
    int should_free; // Argument that will specify if *arags should be freed after the execution is done.
    int is_freed; // Argument that will specify if the *args is freed already.
} Job;

// Thread structure
typedef struct {
    int id; // Thread ID
    pthread_t thread; // pthread struct for the thread
} Thread;

// ThreadPool structure
typedef struct {
    Job *jobs; // Array to store job queue
    int job_count; // Number of jobs in the queue
    int job_size; // Maximum size of the job queue
    int front; // Pointer to the front of the queue
    pthread_mutex_t lock; // Mutex for queue access
    pthread_mutex_t job_lock; // Mutex for running job safely;
    sem_t jobs_available; // Semaphore to signal job availability
    Thread *threads; // Array of worker threads
    int num_threads; // Number of worker threads
    int stop_requested; // Flag to request thread pool termination
} ThreadPool;

typedef struct {
    ThreadPool* pool;
    Thread* thread;
} WorkerInput;

// Function prototypes
void thread_pool_init(ThreadPool *pool, int num_threads, int job_size);
void thread_pool_submit(ThreadPool *pool, Job job);
void thread_pool_wait(ThreadPool *pool);
void thread_pool_stop(ThreadPool *pool);
void thread_pool_clean(ThreadPool *pool);
void* worker_thread(void* args);

#endif // THREADPOOL_H
