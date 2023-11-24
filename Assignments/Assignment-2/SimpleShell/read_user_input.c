#include "simple-shell.h"

char* read_user_input() {
    char* ip = NULL;
    size_t ip_size = 0;

    if (getline(&ip, &ip_size, stdin) == -1) {
        exit(EXIT_FAILURE);
    }

    // Remove the newline character at the end, if present
    size_t ip_len = strlen(ip);
    if (ip_len > 0 && ip[ip_len - 1] == '\n') {
        ip[ip_len - 1] = '\0';
    }

    // Check for backslashes or quotes in the input
    if (strchr(ip, '\\') != NULL || strchr(ip, '"') != NULL || strchr(ip, '\'') != NULL) {
        printf("Error: Backslashes or quotes are not allowed in the command.\n");
        free(ip);
        return NULL;
    }

    return ip;
}