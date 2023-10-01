#include <stdio.h>

int create_process_and_run(char* command) {
    int status = fork();

    if (status < 0) {
        printf("fork failiure\n");
        exit(0);
    }
    
    else if (status == 0) {
        printf("I am the child (%d)\n, getpid()");
    }
    
    else {
        int ret;
        int pid = wait(&ret);

        if (WIFEXITED(ret)) {
            printf("%d Exit = %d\n”, pid, WEXITSTATUS(ret)");
        } else {
            printf("Abnormal termination of %d\n", pid);
        }
        
        printf("I am the parent Shell\n");
    }

    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <command>\n", argv[0]);
        exit(1);
    }

    create_process_and_run(argv[1]);
}