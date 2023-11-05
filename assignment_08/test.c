#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "threadpool.h"

void job1(void *args) {
    int *id = (int *)args;
    sleep(4);
    printf("Job with id %d is done.\n", *id);
}

void job2(void *args) {
    int *id = (int *)args;
    printf("Job with id %d is done.\n", *id);
}

void job3(void *args) {
    int *number = (int *)args;
    *number += 1;
}

void job4(void *args) {
    int *id = (int *)args;
    sleep(1);
    printf("Job with id %d is done.\n", *id);
}

// spawn 7 jobs and request for stop while 5 of them are running.
int test1() {
    freopen("outputs/output1.txt", "w", stdout);

    ThreadPool pool;
    thread_pool_init(&pool, 5, 100);
    // creating seven jobs and submitting to threadpool.
    for (int i = 0; i < 7; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        Job job;
        job.id = i;
        job.function = job1;
        job.args = (void *)number;
        job.run_safely = 0;
        job.is_freed = 0;
        job.should_free = 1;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);

    // the outputs should have 5 finished jobs and 5 finished threads.
    FILE *output_file = fopen("outputs/output1.txt", "r");
    if (output_file) {
        char line[256];
        int finished_jobs = 0;
        int finished_threads = 0;
        while (fgets(line, sizeof(line), output_file)) {
            if (strstr(line, "Job with id")) {
                finished_jobs++;
            } else if (strstr(line, "thread with id")) {
                finished_threads++;
            }
        }
        fclose(output_file);

        if (finished_jobs == 5 && finished_threads == 5) {
            return 1;
        }
    } else {
        printf("Failed to open the output file.\n");
    }
    return 0;
}

// spawning only 5 jobs on a threadpool with 5 threads. 
int test2() {
    freopen("outputs/output2.txt", "w", stdout);

    ThreadPool pool;
    thread_pool_init(&pool, 5, 100);
    for (int i = 0; i < 5; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        Job job;
        job.id = i;
        job.function = job1;
        job.args = (void *)number;
        job.run_safely = 0;
        job.is_freed = 0;
        job.should_free = 1;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);

    // the outputs should have 5 finished jobs and 5 finished threads.
    FILE *output_file = fopen("outputs/output2.txt", "r");
    if (output_file) {
        char line[256];
        int finished_jobs = 0;
        int finished_threads = 0;
        while (fgets(line, sizeof(line), output_file)) {
            if (strstr(line, "Job with id")) {
                finished_jobs++;
            } else if (strstr(line, "thread with id")) {
                finished_threads++;
            }
        }
        fclose(output_file);

        if (finished_jobs == 5 && finished_threads == 5) {
            return 1;
        }
    } else {
        printf("Failed to open the output file.\n");
    }
    return 0;
}

// spawning only 2 jobs on a threadpool with 5 threads.
int test3() {
    freopen("outputs/output3.txt", "w", stdout);

    ThreadPool pool;
    thread_pool_init(&pool, 5, 100);
    for (int i = 0; i < 2; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        Job job;
        job.id = i;
        job.function = job1;
        job.args = (void *)number;
        job.run_safely = 0;
        job.is_freed = 0;
        job.should_free = 1;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);

    // checking number of finished threads and finished jobs. Keep in mind that this test is not 
    // checking the order of the outputs but they matter.
    FILE *output_file = fopen("outputs/output3.txt", "r");
    if (output_file) {
        char line[256];
        int finished_jobs = 0;
        int finished_threads = 0;
        while (fgets(line, sizeof(line), output_file)) {
            if (strstr(line, "Job with id")) {
                finished_jobs++;
            } else if (strstr(line, "thread with id")) {
                finished_threads++;
            }
        }
        fclose(output_file);

        if (finished_jobs == 2 && finished_threads == 5) {
            return 1;
        }
    } else {
        printf("Failed to open the output file.\n");
    }
    return 0;
}

// submitting 15 instant jobs to a threadpool with 5 threads.
int test4() {
    freopen("outputs/output4.txt", "w", stdout);

    ThreadPool pool;
    thread_pool_init(&pool, 5, 100);
    for (int i = 0; i < 15; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        Job job;
        job.id = i;
        job.function = job2;
        job.args = (void *)number;
        job.run_safely = 0;
        job.is_freed = 0;
        job.should_free = 1;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);
    // checking if all 15 jobs and 5 threads are finished.
    FILE *output_file = fopen("outputs/output4.txt", "r");
    if (output_file) {
        char line[256];
        int finished_jobs = 0;
        int finished_threads = 0;
        while (fgets(line, sizeof(line), output_file)) {
            if (strstr(line, "Job with id")) {
                finished_jobs++;
            } else if (strstr(line, "thread with id")) {
                finished_threads++;
            }
        }
        fclose(output_file);

        if (finished_jobs == 15 && finished_threads == 5) {
            return 1;
        }
    } else {
        printf("Failed to open the output file.\n");
    }
    return 0;
}

// submitting 15 jobs to a threadpool with capacity of 15. 
int test5() {
    freopen("outputs/output5.txt", "w", stdout);

    ThreadPool pool;
    thread_pool_init(&pool, 5, 15);
    for (int i = 0; i < 15; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        Job job;
        job.id = i;
        job.function = job2;
        job.args = (void *)number;
        job.run_safely = 0;
        job.is_freed = 0;
        job.should_free = 1;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);
    // checking if all 15 jobs and 5 threads are finished.
    FILE *output_file = fopen("outputs/output5.txt", "r");
    if (output_file) {
        char line[256];
        int finished_jobs = 0;
        int finished_threads = 0;
        while (fgets(line, sizeof(line), output_file)) {
            if (strstr(line, "Job with id")) {
                finished_jobs++;
            } else if (strstr(line, "thread with id")) {
                finished_threads++;
            }
        }
        fclose(output_file);

        if (finished_jobs == 15 && finished_threads == 5) {
            return 1;
        }
    } else {
        printf("Failed to open the output file.\n");
    }
    return 0;
}

// submitting 20 jobs to a threadpool with capacity of 15
int test6() {
    freopen("outputs/output6.txt", "w", stdout);

    ThreadPool pool;
    thread_pool_init(&pool, 5, 15);
    for (int i = 0; i < 20; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        Job job;
        job.id = i;
        job.function = job2;
        job.args = (void *)number;
        job.run_safely = 0;
        job.is_freed = 0;
        job.should_free = 1;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);
    // checking if 15 jobs are finished, 5 are discarded and 5 threads are finished as well.
    FILE *output_file = fopen("outputs/output6.txt", "r");
    if (output_file) {
        char line[256];
        int finished_jobs = 0;
        int finished_threads = 0;
        int max_size_count = 0;
        while (fgets(line, sizeof(line), output_file)) {
            if (strstr(line, "Job with id")) {
                finished_jobs++;
            } else if (strstr(line, "thread with id")) {
                finished_threads++;
            } else if (strstr(line, "job queue is full!")) {
                max_size_count++;
            }
        }
        fclose(output_file);

        if (finished_jobs == 15 && finished_threads == 5 && max_size_count == 5) {
            return 1;
        }
    } else {
        printf("Failed to open the output file.\n");
    }
    return 0;
}

// submitting 100 jobs to a threadpool with capacity of 15
int test7() {
    freopen("outputs/output7.txt", "w", stdout);

    ThreadPool pool;
    thread_pool_init(&pool, 5, 15);
    for (int i = 0; i < 100; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        Job job;
        job.id = i;
        job.function = job2;
        job.args = (void *)number;
        job.run_safely = 0;
        job.is_freed = 0;
        job.should_free = 1;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);
    // checking if 15 jobs are finished, 85 are discarded and 5 threads are finished as well.
    FILE *output_file = fopen("outputs/output7.txt", "r");
    if (output_file) {
        char line[256];
        int finished_jobs = 0;
        int finished_threads = 0;
        int max_size_count = 0;
        while (fgets(line, sizeof(line), output_file)) {
            if (strstr(line, "Job with id")) {
                finished_jobs++;
            } else if (strstr(line, "thread with id")) {
                finished_threads++;
            } else if (strstr(line, "job queue is full!")) {
                max_size_count++;
            }
        }
        fclose(output_file);

        if (finished_jobs == 15 && finished_threads == 5 && max_size_count == 85) {
            return 1;
        }
    } else {
        printf("Failed to open the output file.\n");
    }
    return 0;
}

// testing a job that should run safely by subitting 100 jobs to a threadpool with size 5 and capacity of 15
int test8() {
    freopen("outputs/output8.txt", "w", stdout);
    
    int* number = (int *)malloc(sizeof(int));
    *number = 0;
    ThreadPool pool;
    thread_pool_init(&pool, 5, 15);
    for (int i = 0; i < 100; i++) {
        Job job;
        job.id = i;
        job.function = job3;
        job.args = (void *)number;
        job.run_safely = 1;
        job.is_freed = 0;
        job.should_free = 0;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);

    // checking if the *number is 15 since 85 jobs are discarded and also there are no race conditions when increasing number value.
    int result = *number;
    free(number);
    if (result == 15) {
        return 1;
    }
    return 0;
}

// testing run safely by submitting 10000 jobs to a threadpool of size 10000 with 5 threads.
int test9() {
    freopen("outputs/output9.txt", "w", stdout);
    
    int* number = (int *)malloc(sizeof(int));
    *number = 0;
    ThreadPool pool;
    thread_pool_init(&pool, 5, 10000);
    for (int i = 0; i < 10000; i++) {
        Job job;
        job.id = i;
        job.function = job3;
        job.args = (void *)number;
        job.run_safely = 1;
        job.is_freed = 0;
        job.should_free = 0;
        thread_pool_submit(&pool, job);
    }
    sleep(2);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);
    // checking if the number is successfully increased 10000 times without threading problems. 
    // if job.run_safely = 0, you would most likely see an output less than 10000.
    int result = *number;
    free(number);
    if (result == 10000) {
        return 1;
    }
    return 0;
}

// testing if each job is only executed once and each thread is only 
// terminated once by submitting 1000 jobs to a threadpool of 17 threads.
int test10() {

    int num_jobs = 1000;
    int num_threads = 17;

    freopen("outputs/output10.txt", "w", stdout);

    ThreadPool pool;
    thread_pool_init(&pool, num_threads, 1000);
    for (int i = 0; i < num_jobs; i++) {
        int *number = (int *)malloc(sizeof(int));
        *number = i;
        Job job;
        job.id = i;
        job.function = job2;
        job.args = (void *)number;
        job.run_safely = 0;
        job.is_freed = 0;
        job.should_free = 1;
        thread_pool_submit(&pool, job);
    }
    sleep(1);
    thread_pool_stop(&pool);
    thread_pool_wait(&pool);
    thread_pool_clean(&pool);

    freopen("/dev/tty", "w", stdout);
    // reading job IDs and thread IDs to make sure they are unique.
    int finished_jobs[num_jobs];
    for (int i = 0; i < num_jobs; i++) {
        finished_jobs[i] = 0;
    }

    int finished_threads[num_threads];

    for (int i =0; i < num_threads; i++) {
        finished_threads[i] = 0;
    }

    FILE *output_file = fopen("outputs/output10.txt", "r");
    if (output_file) {
        char line[256];
        while (fgets(line, sizeof(line), output_file)) {
            int job_id = -1;
            int thread_id = -1;
            if (sscanf(line, "Job with id %d is done.", &job_id) == 1) {
                if (job_id >= 0 && job_id < num_jobs) {
                    finished_jobs[job_id]++;
                }
            }
            else if (sscanf(line, "thread with id %d is finished.", &thread_id) == 1)
            {
                if (thread_id >= 0 && thread_id < num_threads) {
                    finished_threads[thread_id]++;
                }
            }
            
        }
        fclose(output_file);
        // checking if all jobs are finished successfully. 
        int all_jobs_finished = 1;
        for (int i = 0; i < num_jobs; i++) {
            if (finished_jobs[i] != 1) {
                all_jobs_finished = 0;
                break;
            }
        }
        // checking if all threads are finished successfully.
        int all_threads_finished = 1;
        for (int i = 0; i < num_threads; i++) {
            if (finished_threads[i] != 1) {
                all_threads_finished = 0;
                break;
            }
        }
        return all_jobs_finished & all_threads_finished;
    } else {
        printf("Failed to open the output file.\n");
    }
    return 0;
}

void log_test(int test_id, int test_result, int one_line) {
    if(one_line == 1)
    {
        switch(test_result){
        case 1:
            printf("*");
            break;
        case 0:
            printf("_");
            break;
        default:
            printf("?");
        }
        return;
    }
    
    if (test_result == 1) {
        printf("Test %d passed successfully.\n", test_id);
    } else if (test_result == 0) {
        printf("Test %d failed.\n", test_id);
    } else {
        printf("Test %d ended with unexpected result.", test_id);
    }
    
}

int main(int argc, char** argv) {

    int one_line = 1;
    
    int repeat = 1;
    if(argc == 2)
    	repeat = atoi(argv[1]);

    for(int i=0; i<repeat; i++)
    {
        int passed = 0;
        int result = 0;

        result = test1();
        log_test(1, result, one_line);
        passed += result;

        result = test2();
        log_test(2, result, one_line);
        passed += result;

        result = test3();
        log_test(3, result, one_line);
        passed += result;

        result = test4();
        log_test(4, result, one_line);
        passed += result;

        result = test5();
        log_test(5, result, one_line);
        passed += result;

        result = test6();
        log_test(6, result, one_line);
        passed += result;

        result = test7();
        log_test(7, result, one_line);
        passed += result;

        result = test8();
        log_test(8, result, one_line);
        passed += result;

        result = test9();
        log_test(9, result, one_line);
        passed += result;

        result = test10();
        log_test(10, result, one_line);
        passed += result;

        if(one_line == 1)
            printf(" %2d %d/10\n", i, passed);
        else
            printf("Repeat %2d: %d passed out of 10 tests\n", i, passed);
    }
}
