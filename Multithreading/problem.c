#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

void* routine(void* arg) {
    int index = *(int*)arg;
    printf("Thread %d: %d\n", index, primes[index]);
    return NULL;
}

int main() {
    pthread_t threads[10];

    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, &routine, &i);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// the issue lies with the fact that we are passing the value of i as reference, and not the value of i at the time of creation of the thread. Hence, the value of i is bound to change by the time the thread is created.
// this leads to ambiguity in the output, as the value of i is not fixed, and some of the numbers are repeated/missed