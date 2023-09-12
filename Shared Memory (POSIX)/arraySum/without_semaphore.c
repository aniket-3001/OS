typedef struct shm_t {
    int A[SIZE];
    int sum;
} shm_t;

int main() {
    shm_t* shm = setup();

    int chunks = SIZE/NPROCS;

    for (int i = 0; i < NPROCS; i++) {
        if (fork() == 0) {
            int local = 0;
            int start = i*chunks;
            int end = start+chunks;

            for (int i = start; i < end; i++) local += shm->array[i];

            shm->sum += local;

            cleanup_and_exit();
        }
    }

    for(int i=0; i<NPROCS; i++) wait(NULL);

    cleanup();
    return 0;
}