// assuming there is only one car at the gas station

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int fuel = 0;

void* fuel_filling(void* arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutexFuel);
        fuel += 10;
        printf("10 units of fuel added. Total fuel: %d\n", fuel);
        pthread_mutex_unlock(&mutexFuel);
        pthread_cond_signal(&condFuel);
        sleep(1);
    }

    return NULL;
}

void* car(void* arg) {
    pthread_mutex_lock(&mutexFuel);

    while (fuel < 40) {
        printf("Waiting for fuel to reach 40 units.\n");
        pthread_cond_wait(&condFuel, &mutexFuel);
        // equivalent to:
        // pthread_mutex_unlock(&mutexFuel);
        // wait for signal
        // pthread_mutex_lock(&mutexFuel);
    }

    fuel -= 40;
    printf("40 units of fuel consumed. Total fuel: %d\n", fuel);
    pthread_mutex_unlock(&mutexFuel);

    return NULL;
}

int main() {
    pthread_t fuelFillingThread, carThread;
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&condFuel, NULL);
    pthread_create(&carThread, NULL, car, NULL);
    pthread_create(&fuelFillingThread, NULL, fuel_filling, NULL);
    pthread_join(carThread, NULL);
    pthread_join(fuelFillingThread, NULL);
    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);
    return 0;
}