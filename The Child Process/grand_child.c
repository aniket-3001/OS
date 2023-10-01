#include <stdio.h>

int create_process_and_run(char* command) {
    int status = fork();

    if (status < 0) {
        printf("fork1 failiure\n");
        exit(0);
    }
    
    else if (status == 0) {
        int status2 = fork();

        if (status2<0) printf("fork2 failiure");

        else if (status2 == 0) {
            printf("Child will not live like zonbies\n");
        } 
        
        else {
            exit(0);
        }
    } 
    
    else {
        printf("I am the parent\n");
    }

    return 0;
}