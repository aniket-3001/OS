#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef struct cookiejar_t {
    int cookie;
    sem_t jar_empty;
    sem_t jar_full;
} cookiejar_t;

cookiejar_t* cookiejar;

void cleanup_and_exit() {
    // Unmap shared memory
    if (munmap(cookiejar, sizeof(cookiejar_t)) == -1) {
        perror("munmap");
        exit(1);
    }

    // Unlink shared memory
    if (shm_unlink("/my_shared_memory") == -1) {
        perror("shm_unlink");
        exit(1);
    }

    exit(0);
}

void homer() {
    for (int i = 0; i < 5; i++) {
        sem_wait(&cookiejar->jar_full);
        printf("Homer ate Cookie-%d\n", cookiejar->cookie);
        sem_post(&cookiejar->jar_empty);
    }
    
    cleanup_and_exit();
}

void marge() {
    for (int i = 0; i < 5; i++) {
        sem_wait(&cookiejar->jar_empty);
        printf("Marge bake Cookie-%d\n", ++cookiejar->cookie);
        sem_post(&cookiejar->jar_full);
    }
    
    cleanup_and_exit();
}

cookiejar_t* setup() {
    int fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    if (ftruncate(fd, sizeof(cookiejar_t)) == -1) {
        perror("ftruncate");
        exit(1);
    }
    
    cookiejar_t* shared_memory = (cookiejar_t*)mmap(NULL, sizeof(cookiejar_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    return shared_memory;
}

int main() {
    cookiejar = setup();

    cookiejar->cookie = 0;
    sem_init(&cookiejar->jar_empty, 1, 1);
    sem_init(&cookiejar->jar_full, 1, 0);

    if (fork() == 0)
        homer();
    if (fork() == 0)
        marge();

    wait(NULL); // wait for Homer process
    wait(NULL); // wait for Marge process

    sem_destroy(&cookiejar->jar_empty);
    sem_destroy(&cookiejar->jar_full);

    // Unmap shared memory and unlink it
    if (munmap(cookiejar, sizeof(cookiejar_t)) == -1) {
        perror("munmap");
        exit(1);
    }

    if (shm_unlink("/my_shared_memory") == -1) {
        perror("shm_unlink");
        exit(1);
    }

    return 0;
}
