#include "simple-shell.h"
#include "history.h"

char* read_user_input();
int create_process_and_run(char *command);
void SIGINT_history();

// custom handler to handle SIGINT signal
void my_handler(int signum) {
    if (signum == SIGINT) {
        printf("\n");
        SIGINT_history();
    }
}

int launch (char *command) {
    int status;
    status = create_process_and_run(command);
    return status;
}

// function for specifically reading the bash file
int run_sh_file() {
    // Open the "commands.sh" file for reading
    FILE *file = fopen("commands.sh", "r");

    if (file == NULL) {
        perror("Error opening commands.sh");
        return 1;
    }

    char *command = NULL;
    size_t command_size = 0;

    // Read and execute commands line by line
    while (getline(&command, &command_size, file) != -1) {
        // Remove the newline character at the end of the command
        size_t length = strlen(command);

        if (length > 0 && command[length - 1] == '\n') {
            command[length - 1] = '\0';
        }

        // Execute the command using create_process_and_run
        int result = launch(command);

        if (result != 0) {
            fprintf(stderr, "Error executing command: %s\n", command);
        }
    }

    // Free the dynamically allocated memory
    free(command);

    if (file != NULL) {
        fclose(file);
    }

    return 0;
}

int main() {
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = my_handler;

    sigaction(SIGINT, &sig, NULL);

    do {
        printf("group_51@possum:~$ ");
        char* command = read_user_input();

        if (command != NULL) {
            if (strcmp(command, "run commands.sh") == 0) {
                run_sh_file();
            }

            else {
                int status = launch(command);
                free(command); // free the allocated memory for the command

                if (status != 0) {
                    printf("Command exited with status %d\n", status);
                }
            }
        }
    } while (1);

    return 0;
}

