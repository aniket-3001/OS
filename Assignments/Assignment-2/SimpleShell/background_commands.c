#include "simple-shell.h"
#include "history.h"

int printCurrentTime();

int background_commands(char* command) {
    struct timeval start_time;
    struct timeval end_time;

    // Split the command into separate commands based on '&'
    char* commands[100];
    int num_commands = 0;

    // Split the command into tokens and store them in the commands array

    char* token = strtok(command, "&");

    while (token != NULL && num_commands < 100) {
        // Trim leading and trailing spaces/tabs
        while (*token == ' ' || *token == '\t') {
            token++;
        }

        int len = strlen(token);

        while (len > 0 && (token[len - 1] == ' ' || token[len - 1] == '\t')) {
            token[len - 1] = '\0';
            len--;
        }

        if (len > 0) {
            commands[num_commands] = token;
            num_commands++;
        }

        token = strtok(NULL, "&");
    }

    if (num_commands == 0) {
        return 0; // Success
    }

    // Null-terminate the commands array
    commands[num_commands] = NULL;

    for (int i = 0; i < num_commands; i++) {
        // each command in commands[] can be executed in the background as a separate process
        gettimeofday(&start_time, NULL);
        pid_t chld_PID;

        chld_PID = fork();

        if (chld_PID == -1) {
            perror("fork failed");
            return -1;
        } 
        
        else if (chld_PID == 0) {
            // child process

            char* args[100];
            int arg_count = 0;

            // split the command into tokens and store them in the args array
            char* arg_token = strtok(commands[i], " ");

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

            // Record the background command in the history array
            history_entries[history_counter].command = strdup(commands[i]);
            history_entries[history_counter].entries[0] = chld_PID;
            history_entries[history_counter].entries[1] = printCurrentTime(); // Current time
            gettimeofday(&end_time, NULL);
            long long duration = ((end_time.tv_sec - start_time.tv_sec) * 1000000) + ((end_time.tv_usec - start_time.tv_usec));
            history_entries[history_counter].entries[2] = (int)(duration * 14); // Execution time
            history_counter++;

            printf("Background process with PID %d initiated.\n", chld_PID);
        }
    }

    return 0; // Success
}
