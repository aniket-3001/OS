#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdatomic.h>
#include <unistd.h>

extern int NCPU, TSLICE;

extern pid_t scheduler_pid;
extern pid_t shell_pid;

struct proc {
    char* cmd; // execute using system()
    pid_t pid;
    int priority; // 1-4
    int execution_time; // in milliseconds
    int wait_time; // in milliseconds
    char* state; // new, ready, running
    // once the process gets dumped in the terminated queue, it means that it has been terminated and the state field is no longer required
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
    struct entry *arr;
    int capacity;  // maximum size of the heap
    int heap_size; // number of elements in the heap
};

// take input using:
extern char* read_user_input();

extern struct Heap* ready_queue;
extern struct proc terminated_processes[100];
extern int terminated_processes_count;

// Priority Queue APIs
extern int cmp_entries(struct entry e1, struct entry e2);
extern void exchange(struct entry *e1, struct entry *e2);
extern void insert(struct Heap *heap, struct proc x);
struct proc find_max(struct Heap *heap);
extern void heapify(struct Heap *heap, int i);
extern struct proc extract_max(struct Heap *heap);
extern void delete_entry(struct Heap *H, struct proc x);
extern struct proc search_by_pid(struct Heap *H, pid_t target_pid);

#define PARENT(i) (i / 2)
#define LEFT(i) (2 * i)
#define RIGHT(i) (2 * i + 1)

// Scheduler APIs
extern void signal_handler(int signum);

