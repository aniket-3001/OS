#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;

void* routine(void* arg) {
    if (pthread_mutex_trylock(&mutex) == 0) {
        printf("Thread %d got the lock\n", (int)pthread_self());
        sleep(1);
        pthread_mutex_unlock(&mutex);
    } else {
        printf("Thread %d did not get the lock\n", (int)pthread_self());
    }

    return NULL;
}

int main() {
    pthread_t tid[5];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < 5; i++) {
        pthread_create(&tid[i], NULL, routine, NULL);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}

