
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define CAMERAS 64
#define WORK 250000

uint64_t results[CAMERAS];
atomic_int next_camera;

/* Simulate processing one camera frame */
uint64_t process_frame(int camera)
{
    uint64_t x = (uint64_t)camera + 12345;

    for (int i = 0; i < WORK; i++) {
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        x *= 2685821657736338717ULL;
    }

    return x;
}

/* Function executed by each worker thread */
void *worker(void *arg)
{
    (void)arg;

    while (1) {
        int camera = atomic_fetch_add(&next_camera, 1);

        if (camera >= CAMERAS)
            break;

        results[camera] = process_frame(camera);
    }

    return NULL;
}

/* Get current time in seconds */
double get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1000000000.0;
}

/* Run using the specified number of threads */
double run_threads(int thread_count)
{
    pthread_t threads[CAMERAS];

    atomic_store(&next_camera, 0);

    double start = get_time();

    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&threads[i], NULL, worker, NULL) != 0) {
            printf("Thread creation failed\n");
            exit(1);
        }
    }

    for (int i = 0; i < thread_count; i++)
        pthread_join(threads[i], NULL);

    return get_time() - start;
}

int main(void)
{
    double start, serial_time;
    uint64_t total = 0;

    printf("TRAFFIC CAMERA THREAD PERFORMANCE\n");
    printf("Number of cameras: %d\n\n", CAMERAS);

    /* Sequential processing */
    start = get_time();

    for (int i = 0; i < CAMERAS; i++)
        results[i] = process_frame(i);

    serial_time = get_time() - start;

    for (int i = 0; i < CAMERAS; i++)
        total ^= results[i];

    printf("Sequential processing: %.4f seconds\n", serial_time);

    /* One thread per camera */
    double t64 = run_threads(64);

    printf("\nOne thread per camera (64 threads): %.4f seconds", t64);
    printf("\nSpeed-up: %.2fx\n", serial_time / t64);

    /* Thread pools */
    int pool_sizes[] = {4, 8, 16};

    for (int i = 0; i < 3; i++) {
        int n = pool_sizes[i];
        double t = run_threads(n);

        printf("\nThread pool (%d threads): %.4f seconds", n, t);
        printf("\nSpeed-up: %.2fx\n", serial_time / t);
    }

    printf("\nVerification checksum: %llu\n",
           (unsigned long long)total);

    return 0;
}
