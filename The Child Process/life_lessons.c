#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("Hello\n");

    for (int i = 0; i < 3; i++) {
        if (fork() == 0) {
            char* args[3] = {"echo", "Hi", NULL}; // we need to NULL terminate the array
            execvp(args[0], args);
            exit(0);
        }

        wait(NULL);
    }

    printf("World\n");
}
