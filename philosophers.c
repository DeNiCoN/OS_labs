//Mutexes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

void print_help(int argc, char* argv[])
{
    printf("Usage: %s number_of_philosophers\n"
           "Runs the dining philosophers problem with specified number of them\n"
           "--help - show this message", argv[0]);
}

pthread_barrier_t barrier;

pthread_mutex_t table;

typedef struct
{
    pthread_t thread;
    int id;
    void* result;
} philosopher_info;

void* philosopher(void* info)
{
    pthread_barrier_wait(&barrier);

    int id = *((int*) info);
    printf("%d says hello!\n", id);
    return NULL;
}

void simulate(int num_ph)
{
    philosopher_info* philosophers = malloc(sizeof(philosopher_info) * num_ph);

    pthread_barrier_init(&barrier, NULL, num_ph + 1);

    for (int i = 0; i < num_ph; i++)
    {
        philosophers[i].id = i;
        pthread_create(&philosophers[i].thread, NULL, philosopher, &philosophers[i].id);
    }

    pthread_barrier_wait(&barrier);

    pthread_barrier_destroy(&barrier);
    for (int i = 0; i < num_ph; i++)
    {
        pthread_join(philosophers[i].thread, &philosophers[i].result);
    }

    free(philosophers);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
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

    simulate(atoi(argv[1]));

    printf("Simulation finished!\n");

    return 0;
}
