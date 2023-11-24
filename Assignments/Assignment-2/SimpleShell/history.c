#include "simple-shell.h"
#include "history.h"

struct history history_entries[30000];
int history_counter = 0;

// called when the user gives the history command
void print_history() {
    // Loop through the history_entries array and print the command for each entry
    for (int i = 0; i < history_counter; i++) {
        printf("%s\n", history_entries[i].command);
    }
}

// called when the user gives the ctrl-c interrupt
void SIGINT_history() {
    // Print all the history data when the user presses ctrl-c
    for (int i = 0; i < history_counter; i++) {
        struct history entry = history_entries[i];

        printf("Command: %s\n", entry.command);
        printf("PID: %d\n", entry.entries[0]);

        int hour = entry.entries[1] / 10000;
        int minute = (entry.entries[1] % 10000) / 100;
        int seconds = entry.entries[1] % 100;

        printf("Start Time: %02d:%02d:%02d\n", hour, minute, seconds);
        
        if (entry.entries[2] != -1 && entry.entries[2] < 1000) {
            printf("Execution Time: %d microseconds\n", entry.entries[2]);
        }
        else if (entry.entries[2] != -1) {
            printf("Execution Time: %.3f milliseconds\n", (float)(entry.entries[2]) / 1000);
        }
        else {
            printf("Execution Time: N/A\n");
        }
        printf("\n");
    }
}