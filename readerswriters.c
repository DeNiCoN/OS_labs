//Semaphores
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

void print_help(int argc, char* argv[])
{
    printf("Usage: %s num_readers num_writers\n"
           "Runs the readers and writers problem with specified number of them\n"
           "--help - show this message", argv[0]);
}

pthread_barrier_t barrier;

typedef struct
{
    unsigned reader_count;
    sem_t semaphore;
    pthread_mutex_t reader_mutex;
    pthread_mutex_t queue_mutex;
} shared_mutex;

shared_mutex database;

typedef struct
{
    pthread_t thread;
    int id;
    void* result;
} thread_info;

void shared_mutex_init(shared_mutex* m)
{
    sem_init(&m->semaphore, 1, 1);
    pthread_mutex_init(&m->reader_mutex, NULL);
    pthread_mutex_init(&m->queue_mutex, NULL);
}

void shared_mutex_destroy(shared_mutex* m)
{
    pthread_mutex_destroy(&m->queue_mutex);
    pthread_mutex_destroy(&m->reader_mutex);
    sem_destroy(&m->semaphore);
}

void shared_mutex_reader_lock(shared_mutex* m)
{
    pthread_mutex_lock(&m->queue_mutex);
    pthread_mutex_lock(&m->reader_mutex);
    m->reader_count += 1;
    if (m->reader_count == 1)
        sem_wait(&m->semaphore);
    pthread_mutex_unlock(&m->reader_mutex);
    pthread_mutex_unlock(&m->queue_mutex);
}

void shared_mutex_reader_unlock(shared_mutex* m)
{
    pthread_mutex_lock(&m->reader_mutex);
    m->reader_count -= 1;
    if (m->reader_count == 0)
        sem_post(&m->semaphore);
    pthread_mutex_unlock(&m->reader_mutex);
}

void shared_mutex_writer_lock(shared_mutex* m)
{
    pthread_mutex_lock(&m->queue_mutex);
    sem_wait(&m->semaphore);
    pthread_mutex_unlock(&m->queue_mutex);
}

void shared_mutex_writer_unlock(shared_mutex* m)
{
    sem_post(&m->semaphore);
}

void wait()
{
    //Min 1 second, max 5 seconds
    unsigned int time = rand() % 4000000 + 1000000;
    usleep(time);
}

void* reader(void* info)
{
    pthread_barrier_wait(&barrier);
    int id = *((int*) info);
    printf("reader %d says hello!\n", id);

    while(1)
    {
        wait();
        printf("reader %d wants to read\n", id);
        shared_mutex_reader_lock(&database);
        printf("reader %d reads: %u\n", id, database.reader_count);
        wait();
        shared_mutex_reader_unlock(&database);
    }

    return NULL;
}

void* writer(void* info)
{
    pthread_barrier_wait(&barrier);
    int id = *((int*) info);
    printf("writer %d says hello!\n", id);

    while(1)
    {
        wait();
        printf("writer %d wants to write\n", id);
        shared_mutex_writer_lock(&database);
        printf("writer %d writes\n", id);
        wait();
        shared_mutex_writer_unlock(&database);
    }

    return NULL;
}

void simulate(int num_readers, int num_writers)
{
    thread_info* readers = malloc(sizeof(thread_info) * num_readers);
    thread_info* writers = malloc(sizeof(thread_info) * num_writers);

    pthread_barrier_init(&barrier, NULL, num_readers + num_writers + 1);
    shared_mutex_init(&database);

    for (int i = 0; i < num_readers; i++)
    {
        readers[i].id = i;
        pthread_create(&readers[i].thread, NULL, reader, &readers[i].id);
    }

    for (int i = 0; i < num_writers; i++)
    {
        writers[i].id = i;
        pthread_create(&writers[i].thread, NULL, writer, &writers[i].id);
    }

    pthread_barrier_wait(&barrier);


    for (int i = 0; i < num_writers; i++)
    {
        writers[i].id = i;
        pthread_join(writers[i].thread, &writers[i].result);
    }

    for (int i = 0; i < num_readers; i++)
    {
        readers[i].id = i;
        pthread_join(readers[i].thread, &readers[i].result);
    }
    shared_mutex_destroy(&database);

    pthread_barrier_destroy(&barrier);

    free(writers);
    free(readers);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        print_help(argc, argv);
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0)
    {
        print_help(argc, argv);
        return 1;
    }

    srand(time(NULL));

    simulate(atoi(argv[1]), atoi(argv[2]));

    printf("Simulation finished!\n");

    return 0;
}
