#include "pipe.h"


// pipes using dup2

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

        // Redirect standard output to the writing end of the pipe
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        char message[] = "Hello my dear good Parent\n";
        printf("%s", message);
        exit(0);
    } else {
        // Parent process
        close(fd[1]); // Close the writing end of the pipe

        // Redirect standard input to the reading end of the pipe
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }

        // Read data from the pipe
        ssize_t bytes_read = read(STDIN_FILENO, buff, sizeof(buff));

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
