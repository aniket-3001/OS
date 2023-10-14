#include "headers.h"
#include <signal.h>

int terminated_processes_count = 0;
struct proc terminated_processes[100] = {0}; // an array to store terminated processes

// signal handler for SIGUSR1 signal
void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        // implement scheduling logic
        struct proc p = find_max(ready_queue);

        while (1) {
            if (strcmp(p.state, "RUNNING") == 0) {
                p.state = "READY";
                kill(p.pid, SIGSTOP);
                delete_entry(ready_queue, p);
                insert(ready_queue, p);
            }
            else if (strcmp(p.state, "READY") == 0) {
                p.state = "RUNNING";
                kill(p.pid, SIGCONT);
            }
            else {
                // NEW process
                pid_t pid1 = fork();

                if (pid1 == -1) {
                    perror("Fork failed");
                    return;
                }
                else if (pid1 == 0) {
                    system(p.cmd);
                    exit(0);
                }
                else {
                    // Parent process
                    int status;
                    pid_t result = waitpid(pid1, &status, WNOHANG);

                    if (result == 0) {
                        // Child is still running, do other processing if needed
                        // ...
                    }
                    else if (result == -1) {
                        // Error handling for waitpid
                        perror("waitpid failed");
                        // Handle the error as needed
                        // ...
                    }
                    else {
                        // Child process has terminated
                        // Add it to the terminated_processes array and remove it from ready_queue
                        struct proc proc_to_be_deleted = search_by_pid(ready_queue, pid1);
                        terminated_processes[terminated_processes_count++] = proc_to_be_deleted;
                        delete_entry(ready_queue, p); // Remove from ready_queue
                    }
                }
            }
        }
    }

    sleep(TSLICE/1000);
}
