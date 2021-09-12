//Mutexes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>

void print_help(int argc, char* argv[])
{
    printf("Usage: %s number_of_philosophers\n"
           "Runs the dining philosophers problem with specified number of them\n"
           "--help - show this message", argv[0]);
}

pthread_barrier_t barrier;

typedef struct
{
    pthread_mutex_t* forks;
    int num_seats;

} philosophers_table;

philosophers_table table;

typedef struct
{
    pthread_t thread;
    int id;
    void* result;
} philosopher_info;

void think(int id)
{
    printf("%d thinks\n", id);
    sleep(rand() % 10);
}

pthread_mutex_t* left(int id)
{
    return &table.forks[id];
}

pthread_mutex_t* right(int id)
{
    return &table.forks[(id + 1) % table.num_seats];
}

void aquire_forks(int id)
{
    while(1)
    {
        printf("%d tries to aquire forks\n", id);
        pthread_mutex_lock(left(id));
        if (!pthread_mutex_trylock(right(id)))
        {
            return;
        }
        printf("%d failed to pickup second fork :(\n", id);
        pthread_mutex_unlock(left(id));
        sleep(rand() % 3);
    }

}

void drop_forks(int id)
{
    pthread_mutex_unlock(right(id));
    pthread_mutex_unlock(left(id));
}

void eat(int id)
{
    aquire_forks(id);
    printf("%d eats\n", id);
    sleep(rand() % 10);
    drop_forks(id);
}

void* philosopher(void* info)
{
    pthread_barrier_wait(&barrier);
    int id = *((int*) info);
    printf("%d says hello!\n", id);

    while(1)
    {
        think(id);
        eat(id);
    }

    return NULL;
}

void simulate(int num_ph)
{
    philosopher_info* philosophers = malloc(sizeof(philosopher_info) * num_ph);
    table.forks = malloc(sizeof(pthread_mutex_t) * (num_ph + 1));
    table.num_seats = num_ph;

    for (int i = 0; i < num_ph; i++)
    {
        pthread_mutex_init(table.forks + i, NULL);
    }

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

    free(table.forks);
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
