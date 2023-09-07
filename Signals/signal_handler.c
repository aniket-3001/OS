#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Function prototype for fib
int fib(int n);


// Signal handler function
static void my_handler(int signum) {
    static int counter = 0;
    
    // Handling SIGINT signal (Ctrl+C)
    if (signum == SIGINT) {
        char buff1[23] = "\nCaught SIGINT signal\n";
        write(STDOUT_FILENO, buff1, 23); // Writing a message to stdout
        
        // Allowing only one more SIGINT before exiting
        if (counter++ >= 1) {
            char buff2[20] = "Cannot handle more\n";
            write(STDOUT_FILENO, buff2, 20);
            exit(0);
        }
    } 
    // Handling SIGCHLD signal (Child process terminated)
    else if (signum == SIGCHLD) {
        char buff1[23] = "Caught SIGCHLD signal\n";
        write(STDOUT_FILENO, buff1, 23); // Writing a message to stdout
    }
}

int main() {
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = my_handler;
    
    // Setting up signal handlers for SIGINT and SIGCHLD
    sigaction(SIGINT, &sig, NULL);
    sigaction(SIGCHLD, &sig, NULL);
    
    int n;
    while (1) {
        printf("Input i: \n");
        scanf("%d", &n);
        
        if (fork() == 0) {
            // Child process calculates and prints Fibonacci number
            printf("Fib(%d) = %d\n", n, fib(n));
            exit(0);
        } else {
            // Parent process waits for the child to terminate
            wait(NULL);
        }
    }
    return 0;
}
