#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define NUM_THREADS 8

pthread_mutex_t mutexFuel = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexWater = PTHREAD_MUTEX_INITIALIZER;
int fuel = 100;
int water = 100;

void* routine(void* arg) {
    if (rand() % 2) {
        pthread_mutex_lock(&mutexFuel);
        sleep(1);
        pthread_mutex_lock(&mutexWater);
    }
    else {
        pthread_mutex_lock(&mutexWater);
        sleep(1);
        pthread_mutex_lock(&mutexFuel);
    }

    fuel += 50;
    water = fuel;
    printf("Fuel: %d, Water: %d\n", fuel, water);
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexWater);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, routine, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}