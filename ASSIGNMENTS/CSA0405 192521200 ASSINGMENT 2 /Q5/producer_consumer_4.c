
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 5
#define ITEMS 20
#define THREADS 4

int buffer[SIZE], in = 0, out = 0;
sem_t empty, full;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
long consumed_sum = 0;

void *producer(void *arg)
{
    int id = *(int *)arg;

    for (int i = 1; i <= ITEMS; i++) {
        int item = id * ITEMS + i;

        sem_wait(&empty);
        pthread_mutex_lock(&lock);

        buffer[in] = item;
        in = (in + 1) % SIZE;

        pthread_mutex_unlock(&lock);
        sem_post(&full);
    }
    return NULL;
}

void *consumer(void *arg)
{
    (void)arg;

    for (int i = 0; i < ITEMS; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&lock);

        int item = buffer[out];
        out = (out + 1) % SIZE;
        consumed_sum += item;

        pthread_mutex_unlock(&lock);
        sem_post(&empty);
    }
    return NULL;
}

int main(void)
{
    pthread_t p[THREADS], c[THREADS];
    int ids[THREADS];

    sem_init(&empty, 0, SIZE);
    sem_init(&full, 0, 0);

    for (int i = 0; i < THREADS; i++) {
        ids[i] = i;
        pthread_create(&p[i], NULL, producer, &ids[i]);
        pthread_create(&c[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(p[i], NULL);
        pthread_join(c[i], NULL);
    }

    printf("Producers: %d, Consumers: %d\n", THREADS, THREADS);
    printf("Items consumed: %d\n", THREADS * ITEMS);
    printf("Sum = %ld (expected %d)\n",
           consumed_sum,
           (THREADS * ITEMS * (THREADS * ITEMS + 1)) / 2);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&lock);

    return 0;
}
