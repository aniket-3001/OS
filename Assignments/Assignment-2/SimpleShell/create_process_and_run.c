#include "simple-shell.h"
#include "history.h"

int pipe_commands(char *command);
void print_history();
int background_commands(char* command);

struct timeval start_time;

int printCurrentTime() {
    // variable to store the current time
    time_t currentTime;
    
    // obtain current time
    time(&currentTime);
    
    // Convert the current time to the local time zone
    struct tm *localTime = localtime(&currentTime);
    
    // Extract the hour and minute components
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int seconds = localTime->tm_sec;
    
    int time = hour * 10000 + minute * 100 + seconds;
    
    // return the current time
    return time;
}

int check_for_history(char *command) {
    // Check if the command is "history"
    if (strcmp(command, "history") == 0) {
        return 1;
    }

    return 0;
}

int check_for_cd(char *command) {
    gettimeofday(&start_time, NULL);

    // Check if the command starts with "cd "
    if (strncmp(command, "cd ", 3) == 0) {
        return 1;
    }

    return 0;
}

int check_for_pipe(char *command) {
    // Check if the command contains a pipe character
    if (strchr(command, '|') != NULL) {
        return 1;
    }

    return 0;
}

int check_for_background(char *command) {
    // Check if the command contains a pipe character
    if (strchr(command, '&') != NULL) {
        return 1;
    }

    return 0;
}

int create_process_and_run(char *command) {
    if (check_for_history(command)) {
        print_history();
        return 0;
    }

    if (check_for_cd(command)) {
        char *path = command + 3; // Skip "cd " in the command

        if (chdir(path) == -1) {
            perror("chdir failed");
            return 1;
        }

        struct timeval end_time;
        gettimeofday(&end_time, NULL);
        long long duration = ((end_time.tv_sec - start_time.tv_sec) * 1000000) + ((end_time.tv_usec - start_time.tv_usec));

        // Record the "cd" command in the history array
        history_entries[history_counter].command = strdup(command);
        history_entries[history_counter].entries[0] = getpid(); // Current process ID
        history_entries[history_counter].entries[1] = printCurrentTime();
        history_entries[history_counter].entries[2] = (int)duration;
        history_counter++;

        return 0;
    }

    if (check_for_pipe(command)) {
        // Handle pipe commands
        return pipe_commands(command);
    }

    if (check_for_background(command)) {
        // Handle background commands
        return background_commands(command);
    }

    pid_t chld_PID;
    int status;

    // Record the start time
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    chld_PID = fork();

    if (chld_PID == -1) {
        perror("fork failed");
        return 1;
    } 
    
    else if (chld_PID == 0) {
        // child process
        
        if (execl("/bin/sh", "sh", "-c", command, NULL) == -1) {
            perror("execl failed");
            exit(EXIT_FAILURE);
        }

        exit(0);
    } 
    
    else {
        // parent process

        waitpid(chld_PID, &status, 0);

        if (WIFEXITED(status)) {
            // Get the execution time duration
            struct timeval end_time;
            gettimeofday(&end_time, NULL);
            long long duration = ((end_time.tv_sec - start_time.tv_sec) * 1000000) + ((end_time.tv_usec - start_time.tv_usec));

            // Record the command in the history array
            history_entries[history_counter].command = strdup(command);
            history_entries[history_counter].entries[0] = chld_PID;
            history_entries[history_counter].entries[1] = printCurrentTime();
            history_entries[history_counter].entries[2] = (int)duration;
            history_counter++;

            return WEXITSTATUS(status);
        } 
        
        else {
            perror("waitpid failed");
            return 1;
        }
    }
}
