#include "headers.h"


char* read_user_input() { 
    char* ip = NULL;
    size_t ip_size = 0;

    if (getline(&ip, &ip_size, stdin) == -1) {
        perror("getline failed");
        exit(1);
    }

    // Remove trailing newline if present
    int ip_len = strlen(ip);
    if (ip_len > 0 && ip[ip_len - 1] == '\n') {
        ip[ip_len - 1] = '\0';
    }

    // Validate user input format
    const char* submit_prefix = "submit ./";
    int submit_prefix_len = strlen(submit_prefix);

    if (strncmp(ip, submit_prefix, submit_prefix_len) != 0) {
        fprintf(stderr, "Invalid input format. Expected format: 'submit ./<executable> <priority>'\n");
        free(ip);
        exit(1);
    }

    // Parse the input to extract executable and priority
    char* executable;
    int priority;

    sscanf(ip, "submit %s %d", executable, &priority);

    // Check if priority is within the valid range (1 to 4)
    if (priority < 1 || priority > 4) {
        fprintf(stderr, "Invalid priority. Priority should be between 1 and 4.\n");
        free(ip);
        exit(1);
    }

    return ip;
}