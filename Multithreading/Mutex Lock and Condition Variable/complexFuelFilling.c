// assuming there are multiple cars at the gas station

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;
int fuel = 0;

void* fuel_filling(void* arg) {
    for (int i = 0; i < 16; i++) {
        pthread_mutex_lock(&mutexFuel);
        fuel += 10;
        printf("10 units of fuel added. Total fuel: %d\n", fuel);
        pthread_mutex_unlock(&mutexFuel);
        pthread_cond_broadcast(&condFuel);
        sleep(1);
    }

    return NULL;
}

void* car(void* arg) {
    pthread_mutex_lock(&mutexFuel);

    while (fuel < 40) {
        printf("Waiting for fuel to reach 40 units. Current fuel: %d\n", fuel);
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
    pthread_t tid[5];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&condFuel, NULL);

    for (int i = 0; i < 5; i++) {
        if (i == 4) {
            pthread_create(&tid[i], NULL, &fuel_filling, NULL);
        } else {
            pthread_create(&tid[i], NULL, &car, NULL);
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);
    return 0;
}