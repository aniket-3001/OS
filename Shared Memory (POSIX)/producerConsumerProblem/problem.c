#include <stdio.h>

typedef struct cookiejar_t {
    int cookie;
    volatile int empty;
} cookiejar_t;

cookiejar_t* cookiejar;

void homer() {
    for (int i = 0; i < 5; i++) {
        while (cookiejar->empty) {
            /*Loop endlessly*/
        }

        printf(“Homer ate Cookie-%d\n”, cookiejar->cookie);
        cookiejar->empty = 1;
    }
    
    cleanup_and_exit();
}

void marge() {
    for (int i =0 ; i < 5; i++) {
        while (!cookiejar->empty) {
            /*Loop endlessly*/
        }

        printf(“Marge bake Cookie-%d\n”, ++cookiejar->cookie);
        cookiejar->empty = 0;
    }
    
    cleanup_and_exit();
}

cookiejar_t* cookiejar;

int main() {
    cookiejar = setup(); // setup(): 1) shm_open (...) 2) ftruncate (...) 3) mmap (...)
    cookiejar->empty=1;

    if(fork() == 0) homer();
    if(fork() == 0) marge();

    wait(NULL); // wait for Homer process
    wait(NULL); // wait for Marge process

    cleanup(); // cleanup(): 1) munmap (...) 2) close (...) 3) shm_unlink (...)
    return 0;
}

// Compiler Optimization Impact: The issue is that the
// compiler may optimize the code in a way that it assumes cookiejar->empty never changes its value because it sees that 
// in the main function, it is set to 1 and it's never explicitly modified again. 
// The compiler might optimize by replacing all occurrences of cookiejar->empty with the constant value 1.