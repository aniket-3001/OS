#include "simple-shell.h"
#include "history.h"

// For each command in the pipeline:
//   - Create pipes
//   - Fork a child process
//   - In the child process:
//     - Redirect stdin/stdout using dup2
//     - Execute the command using execv
//   - In the parent process, close unused pipe ends and wait for the child to complete.

int printCurrentTime();

void pipe_commands(char *command) {
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Split the whole command into separate commands based on |
    char *commands[100];
    int num_commands = 0;

    // Split the command into tokens and store them in the commands array
    char *token = strtok(command, "|");

    while (token != NULL && num_commands < 100) {
        if (token[0] == ' ') {
            token++;
        }

        commands[num_commands++] = token;
        token = strtok(NULL, "|");
    }

    if (num_commands == 0) {
        return;
    }

        if (num_commands == 1) {
        // If there is only one command, execute it normally
    }

    // Null-terminate the commands array
    commands[num_commands] = NULL;

    int pipefd[2]; // pipe file descriptors
    pid_t chld_PID;
    int status;

    int prev_pipe_read = STDIN_FILENO; // read from stdin

    // Concatenate the individual commands with the pipe symbol "|"
    char piped_cmd[100] = "";

    for (int i = 0; i < num_commands; i++) {
        strcat(piped_cmd, commands[i]);

        if (i < num_commands - 1) {
            strcat(piped_cmd, " | ");
        }
    }

    for (int i = 0; i < num_commands; i++) {
        // Create a pipe for each command except the last one
        if (i < num_commands - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe failed");
                exit(EXIT_FAILURE);
            }
        }

        chld_PID = fork();

        if (chld_PID == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } 
        
        else if (chld_PID == 0) {
            // child process

            // Redirect input from the previous command's output
            if (i != 0) {
                if (dup2(prev_pipe_read, STDIN_FILENO) == -1) {
                    perror("dup2 failed");
                    exit(EXIT_FAILURE);
                }
                
                close(prev_pipe_read); // Close the old pipe read end
            }

            // Redirect output to the next command's input
            if (i < num_commands - 1) {
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("dup2 failed");
                    exit(EXIT_FAILURE);
                }

                close(pipefd[0]); // Close the pipe read end
            }

            // Closing all pipe ends
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipefd[j]);
            }

            // Split the current command into tokens
            char *args[100];
            int arg_count = 0;

            char *arg_token = strtok(commands[i], " ");

            while (arg_token != NULL && arg_count < 100) {
                args[arg_count] = arg_token;
                arg_count++;
                arg_token = strtok(NULL, " ");
            }

            args[arg_count] = NULL;

            if (execvp(args[0], args) == -1) {
                exit(EXIT_FAILURE);
            }
        } 
        
        else {
            // parent process

            // closing pipe ends

            if (i < num_commands - 1) {
                close(pipefd[1]);
            }

            if (i != 0) {
                close(prev_pipe_read);
            }

            waitpid(chld_PID, &status, 0);

            if (WIFEXITED(status)) {
                // Record the concatenated piped command in the history array
                if (i == 0) {
                    history_entries[history_counter].command = strdup(piped_cmd);
                    history_entries[history_counter].entries[0] = chld_PID;
                    history_entries[history_counter].entries[1] = printCurrentTime(); // Current time
                    struct timeval end_time;
                    gettimeofday(&end_time, NULL);
                    long long duration = ((end_time.tv_sec - start_time.tv_sec) * 1000000) + ((end_time.tv_usec - start_time.tv_usec));
                    history_entries[history_counter].entries[2] = (int)duration; // Execution time
                    history_counter++;
                }
            } 
            
            else {
                perror("waitpid failed");
                exit(EXIT_FAILURE);
            }

            prev_pipe_read = pipefd[0];
        }
    }
}