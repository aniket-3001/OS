#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 1024
#define NTHREADS 4

int A[SIZE]; // Global array for demonstration

int array_sum(int low, int high) {
    int sum = 0;
    for (int i = low; i < high; i++) {
        sum += A[i];
    }
    return sum;
}

typedef struct {
    int low;
    int high;
    int sum;
} thread_args;

void *thread_func(void *ptr) {
    thread_args *t = (thread_args *)ptr;
    t->sum = array_sum(t->low, t->high);
    return NULL;
}

int main(int argc, char *argv[]) {
    int size;
    printf("Enter the array size: ");
    scanf("%d", &size);

    if (size < 1) {
        printf("Array size must be greater than 0.\n");
        return 1;
    }

    // Initialize the sample array (you can replace this with your own initialization)
    for (int i = 0; i < size; i++) {
        A[i] = i + 1; // Sample data, you can modify this as needed
    }

    int result = 0;

    if (size < SIZE) {
        result = array_sum(0, size);
    } else {
        pthread_t tid[NTHREADS];
        thread_args args[NTHREADS];
        int chunk = size / NTHREADS;

        for (int i = 0; i < NTHREADS; i++) {
            args[i].low = i * chunk;
            args[i].high = (i + 1) * chunk; // or args[i].low + chunk
            pthread_create(&tid[i], NULL, thread_func, (void *)&args[i]);
        }

        for (int i = 0; i < NTHREADS; i++) {
            pthread_join(tid[i], NULL);
            result += args[i].sum;
        }

        for (int i = 0; i < NTHREADS; i++) {
            printf("Thread id: %ld, low: %d, high: %d, sum: %d\n", tid[i], args[i].low, args[i].high - 1, args[i].sum);
        }
    }

    printf("Total Sum is %d\n", result);

    return 0;
}
