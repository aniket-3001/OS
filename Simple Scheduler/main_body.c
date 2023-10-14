#include "headers.h"

struct Heap *ready_queue = NULL;
int NCPU, TSLICE;

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <NCPU> <TSLICE>\n", argv[0]);
        return 1;
    }

    int shell_pid = getpid();
    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process (scheduler)

        ready_queue = (struct Heap *) malloc(sizeof(struct Heap));
        struct proc terminated_processes[100]; // an array to store terminated processes
        ready_queue->heap_size = 0;
        ready_queue->capacity = 100;
        ready_queue->arr = (struct entry *) malloc(ready_queue->capacity * sizeof(struct entry));

        close(pipefd[1]);  // Close the write end of the pipe in the child
        char* command;
        char buffer[100];

        struct sigaction sig;
        memset(&sig, 0, sizeof(sig));
        sig.sa_handler = signal_handler;
        // custom signals
        sigaction(SIGUSR1, &sig, NULL);

        while (1) {
            ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer));

            if (bytes_read == -1) {
                perror("Read failed");
                return 1;
            }

            command = strdup(buffer);
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer

            if (command == NULL) {
                perror("Strdup failed");
                return 1;
            }

            int priority;
            char* cmd;

            sscanf(command, " submit %s %d", cmd, &priority);

            // create a process
            struct proc process;
            process.cmd = strdup(cmd);
            process.execution_time = 0;
            process.wait_time = 0;
            process.pid = 0;
            process.priority = priority;
            process.state = "NEW";

            // add the process to the ready queue
            insert(ready_queue, process);

            raise(SIGUSR1);
        }

        exit(0);
    } else {
        // Parent process (shell)

        int scheduler_pid = pid;
        close(pipefd[0]);  // Close the read end of the pipe in the parent

        do {
            printf("Simple Shell$ ");
            char* user_command = read_user_input();

            // Send the command to the child process
            if (write(pipefd[1], user_command, strlen(user_command) + 1) == -1) {
                perror("Write failed");
                return 1;
            }

        } while (1);

        close(pipefd[1]);  // Close the write end of the pipe in the parent
    }

    return 0;
}