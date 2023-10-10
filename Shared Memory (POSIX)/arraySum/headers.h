#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

#define SIZE 1000 // Define the size of the shared memory array
#define NPROCS 4   // Define the number of processes

typedef struct shm_t {
    int A[SIZE];
    int sum;
    sem_t mutex;
} shm_t;