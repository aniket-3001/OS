#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>

// ------------------ Priority Queue APIs ------------------ //

#define PARENT(i) (i/2)
#define LEFT(i) (2*i)
#define RIGHT(i) (2*i + 1)

struct proc {
    char cmd[100];
    pid_t pid;
    int priority;
    int execution_time;
    int wait_time;
    char state[10]; // READY/RUNNING
    // once the process gets terminated, it gets dumped in the terminated_arr
};

struct pair {
    struct proc process;
    int priority;
};

struct entry {
    struct pair p;
    int arrival_time;
};

struct Heap {
    struct entry* arr;
    int size;
    int capacity;
};

struct proc make_process(char* cmd, int priority) {
    struct proc p;

    if (strcpy(p.cmd, cmd) == NULL) {
        perror("Error in copying command");
    }

    p.priority = priority;
    p.execution_time = 0;
    p.wait_time = 0;


    if (strcpy(p.state, "READY") == NULL) {
        perror("Error in copying state");
    }


    return p;
}

// Print the contents of the priority queue
void print_heap(struct Heap* H) {
    printf("\n");

    for (int i = 1; i <= H->size; i++) {
        printf("%s %d %d\n", H->arr[i].p.process.cmd, H->arr[i].p.priority, H->arr[i].p.process.pid);
    }

    printf("\n");
}

// Comparison function to compare two entries in the heap
int cmp_entries(struct entry e1, struct entry e2) {
    if (e1.p.priority == e2.p.priority) {
        return e1.arrival_time < e2.arrival_time;
    }

    return e1.p.priority > e2.p.priority;
}

// Swap two entries in the heap
void exchange(struct Heap *H, int i, int j) {
    struct entry temp = H->arr[i];
    H->arr[i] = H->arr[j];
    H->arr[j] = temp;
}

// Insert a new process into the priority queue
void insert(struct Heap *H, struct proc x) {
    if (H->size == H->capacity) {
        perror("Heap overflow");
        return;
    }

    H->size++;
    struct pair p;
    p.process = x;
    p.priority = x.priority;
    H->arr[H->size].p = p;
    H->arr[H->size].arrival_time = H->size; // Use a different arrival time mechanism

    int i = H->size;

    while (i > 1 && cmp_entries(H->arr[i], H->arr[PARENT(i)])) {
        exchange(H, i, PARENT(i));
        i = PARENT(i);
    }
}

// Find the process with the highest priority in the queue
struct proc find_max(struct Heap* H) {
    if (H->size == 0) {
        perror("Heap underflow");
    }

    return H->arr[1].p.process;
}

// Reorganize the heap after extracting the maximum element
void Heapify(struct Heap* H, int i) {
    int l = LEFT(i);
    int r = RIGHT(i);
    int largest = i;

    if (l <= H->size && cmp_entries(H->arr[l], H->arr[i])) {
        largest = l;
    }
    if (r <= H->size && cmp_entries(H->arr[r], H->arr[largest])) {
        largest = r;
    }

    if (largest != i) {
        exchange(H, i, largest);
        Heapify(H, largest);
    }
}

// Extract and return the process with the highest priority
struct proc extract_max(struct Heap* H) {
    struct proc max = H->arr[1].p.process;
    H->arr[1] = H->arr[H->size--];
    Heapify(H, 1);
    return max;
}

struct proc extract_by_pid(struct Heap* H, pid_t pid) {
    int i;

    for (i = 1; i <= H->size; i++) {
        if (H->arr[i].p.process.pid == pid) {
            break;
        }
    }

    if (i > H->size) {
        perror("Process not found");
    }

    struct proc p = H->arr[i].p.process;
    H->arr[i] = H->arr[H->size--];
    Heapify(H, i);
    return p;
}

struct Heap* ready_queue = NULL;
struct proc terminated_arr[100];
int num_terminated = 0;
int NCPU, TSLICE;
pid_t scheduler_pid;

void print_terminated_arr() {
    for (int i = 0; i < num_terminated; i++) {
        struct proc p = terminated_arr[i];
        
        printf("\n");
        printf("--------------------\n");
        printf("Process Information:\n");
        printf("  Command:       %s\n", p.cmd);
        printf("  PID:           %d\n", p.pid);
        printf("  Priority:      %d\n", p.priority);
        printf("  Execution Time: %d\n", p.execution_time);
        printf("  Wait Time:     %d\n", p.wait_time);
        printf("  State:         %s\n", p.state);
        printf("--------------------\n");
        printf("\n");
    }
}

// ------------------ Priority Queue APIs ------------------ //

// ------------------ Timer APIs ------------------ //

struct itimerval timer;

void start_timer() {
    timer.it_value.tv_sec = TSLICE;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = TSLICE;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("Error in setting timer");
    }
}

void stop_timer() {
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("Error in setting timer");
    }
}

// ------------------ Timer APIs ------------------ //

// ------------------ Signal Handler APIs ------------------ //

void sig_alarm_handler(int signum) {
    if (signum == SIGALRM) {
        stop_timer();
        
        int n;
        int i;

        for (i = 1; i < ready_queue->size; i++) {
            struct proc process = ready_queue->arr[i].p.process;

            // add a check for terminated processes using waitpid and wnohang
            while (waitpid(process.pid, NULL, WNOHANG) == process.pid) {

                if (strcpy(process.state, "TERMINATED") == NULL) {
                    perror("Error in copying state");
                }

                process.execution_time += TSLICE;
                terminated_arr[num_terminated++] = process;
                extract_by_pid(ready_queue, process.pid);
                i--;
                break;
            }

            if (strcmp(ready_queue->arr[i].p.process.state, "RUNNING") == 0) {

                if (strcpy(process.state, "READY") == NULL) {
                    perror("Error in copying state");
                }

                process.wait_time += TSLICE;
                kill(process.pid, SIGSTOP);
                extract_by_pid(ready_queue, process.pid);
                insert(ready_queue, process);
            }

            else {
                process.wait_time += TSLICE;
            }
        }

        n = 1, i = 1;

        while (n <= NCPU && i <= ready_queue->size) {
            struct proc process = ready_queue->arr[i].p.process;

            if (strcmp(process.state, "READY") == 0) {

                if (strcpy(process.state, "RUNNING") == NULL) {
                    perror("Error in copying state");
                }

                process.wait_time += TSLICE;
                kill(process.pid, SIGCONT);
                n++;
            }

            i++;
        }

        start_timer();
    }

    else if (signum == SIGINT) {
        stop_timer();
        print_terminated_arr();
        raise(SIGKILL);
        free(ready_queue->arr);
        free(ready_queue);
    }
}

// ------------------ Signal Handler APIs ------------------ //

// main function starts below

void signal_handler(int signum) {
    if (signum == SIGINT) {
        kill(scheduler_pid, SIGINT);
    }
}

#include <stdio.h>
#include <string.h>

int isSubstring(const char *str, const char *sub) {
    int strLen = strlen(str);
    int subLen = strlen(sub);

    for (int i = 0; i <= strLen - subLen; i++) {
        int j;
        for (j = 0; j < subLen; j++) {
            if (str[i + j] != sub[j]) {
                break;
            }
        }
        if (j == subLen) {
            return 1; // Substring found
        }
    }

    return 0; // Substring not found
}

int main(int argc, char** argv) {
    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);
    char input[100];

    int pipefd[2];

    pipe(pipefd);

    pid_t pid = fork();

    if (pid == 0) {
        // setup signal handler
        struct sigaction sig;
        memset(&sig, 0, sizeof(sig));
        sig.sa_handler = &sig_alarm_handler;
        sigaction(SIGALRM, &sig, NULL);
        sigaction(SIGINT, &sig, NULL);

        if (close(pipefd[1]) == -1) {
            perror("Error in closing pipe");
        }
        char exe[20];
        int priority;
        
        ready_queue = (struct Heap*)malloc(sizeof(struct Heap));

        if (ready_queue == NULL) {
            perror("Error in allocating memory");
        }

        ready_queue->arr = (struct entry*)malloc(100 * sizeof(struct entry));

        if (ready_queue->arr == NULL) {
            perror("Error in allocating memory");
        }

        ready_queue->size = 0;
        ready_queue->capacity = 100;

        start_timer();

        while (1) {
            read(pipefd[0], input, 100);

            sscanf(input, "submit %s %d", exe, &priority);
            struct proc p = make_process(exe, priority);

            pid_t process_pid = fork();

            if (process_pid == 0) {
                raise(SIGSTOP);
                // execute the command using system

                if (system(p.cmd) == -1) {
                    perror("Error in executing command");
                }

                kill(scheduler_pid, SIGCHLD);
                exit(0);
            }

            else {
                p.pid = process_pid;
                p.wait_time = TSLICE;
            }

            insert(ready_queue, p);
        }
    }

    else {
        if (close(pipefd[0]) == -1) {
            perror("Error in closing pipe");
        }

        scheduler_pid = pid;

        while (1) {
            printf("SimpleShell $ ");
            
            if (fgets(input, 100, stdin) == NULL) {
                perror("Error in reading input");
            }

            if (!isSubstring(input, "submit")) {
                printf("Invalid command\n");
                continue;
            }

            write(pipefd[1], input, strlen(input) + 1);
        }
    }
}
