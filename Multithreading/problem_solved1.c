// a simple solution can be to allocate memory for the pointer i

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

void* routine(void* arg) {
    int index = *(int*)arg;
    printf("Thread %d: %d\n", index, primes[index]);
    free(arg);
}

int main() {
    pthread_t threads[10];

    for (int i = 0; i < 10; i++) {
        int* index = malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &routine, (void*) index);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}