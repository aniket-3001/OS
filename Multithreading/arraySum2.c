// this code demonstrates how to use the second parameter of pthread_join to return a value from the thread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

void* routine(void* arg) {
    int index = *(int*)arg;
    int sum = 0;
    
    for (int i = 0; i < 5; i++) {
        sum += primes[index+i];
    }

    printf("local sum: %d\n", sum);
    *(int*)arg = sum;
    return (void*)arg;
}

int main() {
    pthread_t threads[2];
    int i;

    for (i = 0; i < 2; i++) {
        int* a = malloc(sizeof(int));
        *a = i*5;

        if (pthread_create(&threads[i], NULL, &routine, a) != 0) {
            perror("Failed to create thread");
        }
    }

    int sum = 0;

    for (i = 0; i < 2; i++) {
        int* a;
        
        if (pthread_join(threads[i], (void**)&a) != 0) {
            perror("Failed to join thread");
        }

        sum += *a;
        free(a);
    }

    return 0;
}