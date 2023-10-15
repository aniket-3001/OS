#include "headers.h"
#include <unistd.h>


// ------------------------------
// Heap implementation

#define PARENT(i) (i/2)
#define LEFT(i) (2*i)
#define RIGHT(i) (2*i + 1)

struct proc {
    char* cmd; // execute using system()
    pid_t pid;
    int priority; // 1-4
    int execution_time; // in milliseconds
    int wait_time; // in milliseconds
    char* state; // ready/running
    // once the process gets dumped in the terminated queue, it means that it has been terminated and the state field is no longer required
};

struct pair {
    struct proc* process;
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

int global_arrival_time = 0;

int cmp_entries(struct entry e1, struct entry e2) {
    if (e1.p.priority < e2.p.priority) {
        return e1.arrival_time < e2.arrival_time;
    }

    return e1.p.priority < e2.p.priority;
}

void exchange(struct Heap *H, int i, int j) {
    struct entry temp = H->arr[i];
    H->arr[i] = H->arr[j];
    H->arr[j] = temp;
}

void insert(struct Heap *H, struct pair x) {
    if (H->size == H->capacity) {
        perror("Heap overflow");
        return;
    }

    H->size++;
    H->arr[H->size].p = x;
    H->arr[H->size].arrival_time = global_arrival_time++;

    int i = H->size;

    while (i > 1 && cmp_entries(H->arr[i], H->arr[PARENT(i)])) {
        exchange(H, i, PARENT(i));
        i = PARENT(i);
    }
}

struct pair find_max(struct Heap *H) {
    if (H->size == 0) {
        perror("Heap underflow");
    }

    return H->arr[1].p;
}

void heapify(struct Heap* H, int i) {
    int l = LEFT(i);
    int r = RIGHT(i);
    int largest;

    if (l <= H->size && cmp_entries(H->arr[l], H->arr[i])) {
        largest = l;
    } else {
        largest = i;
    }

    if (r <= H->size && cmp_entries(H->arr[r], H->arr[largest])) {
        largest = r;
    }

    if (largest != i) {
        exchange(H, i, largest);
        heapify(H, largest);
    }
}

struct pair extract_max(struct Heap *H) {
    struct pair max = find_max(H);
    H->arr[1] = H->arr[H->size--];
    heapify(H, 1);
    return max;
}

void heapsort(struct Heap *H) {
    int i;

    for (i = H->size; i >= 2; i--) {
        exchange(H, 1, i);
        H->size--;
        heapify(H, 1);
    }
}

struct pair extract(struct Heap *H, pid_t pid) {
    // search for the process with the given pid, remove it from the heap, and return it

    for (int i = 1; i <= H->size; i++) {
        if (H->arr[i].p.process->pid == pid) {
            struct pair p = H->arr[i].p;
            H->arr[i] = H->arr[H->size--];
            heapify(H, i);
            return p;
        }
    }

}

struct Heap* ready_queue;
struct proc* terminated_queue[100];

// ------------------------------
// timer functions

struct itimerval timer;

void set_timer() {
    timer.it_value.tv_sec = TSLICE;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = TSLICE;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer failed");
        exit(1);
    }
}

void stop_timer() {
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer failed");
        exit(1);
    }
}

// ------------------------------
// signal handlers

void signal_handler(int signal) {
    if (signal == SIGCHLD) {
        int status;
        pid_t child_pid;

        while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
            // remove the process from the ready queue
            struct pair p = extract(ready_queue, child_pid);

            // add the process to the terminated queue
            terminated_queue[child_pid] = p.process;
        }
    }

    else if (signal == SIGALRM) {
        // scheduling logic

        heapsort(ready_queue);

        // stop the timer
        stop_timer();

        int n = 1;
        int i = 0;

        while (i < ready_queue->size) {
            struct proc* process = ready_queue->arr[i].p.process;

            if (strcmp(process->state, "RUNNING") == 0) {
                process->state = "READY";
                kill(process->pid, SIGSTOP);
                
                // delete the entry from the priority queue and then add it back
                extract(ready_queue, process->pid);
                insert(ready_queue, (struct pair) {process, process->priority});
            }

            i++;
        }

        i = 0;

        while (n <= NCPU && i < ready_queue->size) {
            struct proc* process = ready_queue->arr[i].p.process;

            if (strcmp(process->state, "READY") == 0) {
                process->state = "RUNNING";
                kill(process->pid, SIGCONT);
                n++;
            }

            i++;
        }

        set_timer();
    }
}

// ------------------------------
// main body

int TSLICE, NCPU;

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    int NCPU = atoi(argv[1]);
    int TSLICE = atoi(argv[2]);

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed\n");
        return 1;
    }

    if (pid == 0) {
        // Child process (scheduler)

        // setting up signal handlers
        struct sigaction sig;
        memset(&sig, 0, sizeof(sig));
        sig.sa_handler = signal_handler;
        sigaction(SIGCHLD, &sig, NULL);
        sigaction(SIGALRM, &sig, NULL);

        close(pipefd[1]);
        char user_command[100];
        char executable[100];

        ready_queue = malloc(sizeof(struct Heap));

        if (ready_queue == NULL) {
            perror("malloc failed to allocate memory for the heap");
            return 1;
        }

        ready_queue->arr = malloc(100 * sizeof(struct entry));

        if (ready_queue->arr == NULL) {
            perror("malloc failed to allocate memory for the heap array");
            return 1;
        }

        ready_queue->size = 0;
        ready_queue->capacity = 100;

        set_timer();

        while (1) {
            read(pipefd[0], user_command, sizeof(user_command));

            struct proc* process = malloc(sizeof(struct proc));
            process->execution_time = 0;
            process->wait_time = 0;

            if (sscanf(user_command, "submit %s %d", executable, &process->priority) != 2) {
                perror("sscanf failed");
                return 1;
            }

            process->cmd = executable;

            pid_t pid = fork();

            if (pid == -1) {
                perror("Fork failed\n");
                return 1;
            }

            if (pid == 0) {
                raise(SIGSTOP);
                
                if (system(process->cmd) == -1) {
                    perror("system failed");
                    return 1;
                }

                raise(SIGCHLD);
                exit(0);
            }

            else {
                process->pid = pid;
                process->state = "READY";
            }

            // Add the process to the ready queue
            insert(ready_queue, (struct pair) {process, process->priority});
        }

        exit(0);
    } else {
        // Parent process (shell)
        char user_command[100];
        close(pipefd[0]);

        while (1) {
            printf("SimpleShell $ ");

            if (fgets(user_command, sizeof(user_command), stdin) == NULL) {
                perror("Error reading user input");
                break;
            }

            // Remove the newline character if present
            user_command[strcspn(user_command, "\n")] = '\0';

            // Write the user command to the pipe
            write(pipefd[1], user_command, sizeof(user_command));
        }

        wait(NULL); // wait for the scheduler
    }

    return 0;
}
