#include "headers.h" // contains all the required headers and some other definitions


shm_t* setup(int* shm_fd) {
    shm_t* shm;

    // Create a shared memory object
    *shm_fd = shm_open("/myshm", O_CREAT | O_RDWR, 0666);

    if (*shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    // Set the size of the shared memory object
    if (ftruncate(*shm_fd, sizeof(shm_t)) == -1) {
        perror("ftruncate failed");
        exit(1);
    }

    // Map the shared memory object into memory
    shm = (shm_t*)mmap(0, sizeof(shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, *shm_fd, 0);

    if (shm == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // initialize sample array
    for (int i = 0; i < SIZE; i++) shm->A[i] = i + 1;

    return shm;
}

void cleanup(int shm_fd, shm_t* shm) {
    // Unmap the shared memory
    if (munmap(shm, sizeof(shm_t)) == -1) {
        perror("munmap");
        exit(1);
    }

    // Close the file descriptor for the shared memory object
    if (close(shm_fd) == -1) {
        perror("close");
        exit(1);
    }

    // Delete the shared memory object
    if (shm_unlink("/myshm") == -1) {
        perror("shm_unlink");
        exit(1);
    }
}

int main() {
    int shm_fd;
    shm_t* shm = setup(&shm_fd);
    sem_init(&shm->mutex, 1, 1);
    int chunks = SIZE / NPROCS;

    for (int i = 0; i < NPROCS; i++) {
        if (fork() == 0) {
            int local = 0;
            int start = i * chunks;
            int end = start + chunks;

            for (int j = start; j < end; j++) local += shm->A[j];

            sem_wait(&shm->mutex);
            shm->sum += local;
            sem_post(&shm->mutex);

            exit(0);
        }
    }

    for (int i = 0; i < NPROCS; i++) wait(NULL);

    printf("Sum = %d\n", shm->sum);

    cleanup(shm_fd, shm);
    return 0;
}