#ifndef HISTORY_H
#define HISTORY_H

struct history
{
    char* command;
    int entries[3];

    // 0: pid
    // 1: start time
    // 2: execution time duration
};

extern struct history history_entries[30000]; // Define the array here
extern int history_counter; // Define the count here

#endif
