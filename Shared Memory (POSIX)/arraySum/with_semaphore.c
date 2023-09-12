typedef struct shm_t {
    int A[SIZE];
    int sum;
    sem_t mutex;
} shm_t;

int main() {
    shm_t* shm = setup();
    sem_init(&shm->mutex, 1, 1);
    int chunks = SIZE/NPROCS;

    for (int i = 0; i < NPROCS; i++) {
        if (fork() == 0) {
            int local = 0;
            int start = i*chunks;
            int end = start+chunks;

            for(int i = start; i < end; i++) local += shm->array[i];

            sem_wait(&shm->mutex);
            shm->sum += local;
            sem_post(&shm->mutex);
            cleanup_and_exit();
        }
    }

    for(int i = 0; i < NPROCS; i++) wait(NULL);

    cleanup();
    return 0;
}