#include "pipe.h"


int main() {
    int fd[2], status;
    char buff[100];

    // Create a pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork a child process
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) {
        // Child process
        close(fd[0]); // Close the reading end of the pipe

        char message[] = "Hello my good Parent";
        write(fd[1], message, sizeof(message));
        exit(0);
    } 
    
    else {
        // Parent process
        close(fd[1]); // Close the writing end of the pipe

        // Read data from the pipe
        ssize_t bytes_read = read(fd[0], buff, sizeof(buff));

        if (bytes_read == -1) {
            perror("read");
            exit(1);
        }

        buff[bytes_read] = '\0'; // Null-terminate the received data
        printf("My obedient child says: %s\n", buff);

        // Wait for the child process to finish
        wait(&status);
    }

    return 0;
}
