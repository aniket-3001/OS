#include "headers.h"

int global_arrival_time = 0;

int cmp_entries(struct entry e1, struct entry e2) {
    // compare two elements e1 and e2 of type struct entry
    // return 1 if e1 has more priority than e2; otherwise, return 0
    if (e1.p.process.priority == e2.p.process.priority)
        return e1.arrival_time < e2.arrival_time;

    return e1.p.process.priority > e2.p.process.priority;
}

void exchange(struct entry *e1, struct entry *e2) {
    struct entry temp = *e1;
    *e1 = *e2;
    *e2 = temp;
}

void insert(struct Heap *H, struct proc x) {
    // H is a reference to the heap of type struct Heap
    // x is a struct proc input that needs to be inserted
    // priority is the priority of the process
    // Output: insert x in the heap
    if (H->heap_size == H->capacity)
        perror("Heap overflow");

    H->heap_size = H->heap_size + 1;
    H->arr[H->heap_size].p.process = x;
    H->arr[H->heap_size].p.priority = x.priority;
    H->arr[H->heap_size].arrival_time = global_arrival_time;
    global_arrival_time = global_arrival_time + 1;
    int i = H->heap_size;

    while (i > 1 && cmp_entries(H->arr[i], H->arr[PARENT(i)])) {
        exchange(&H->arr[i], &H->arr[PARENT(i)]);
        i = PARENT(i);
    }
}

struct proc find_max(struct Heap *H) {
    // H is a reference to the heap of type struct Heap
    // Output: return the process with the maximum priority, if multiple
    // processes have the maximum priority, return the one that arrived first
    if (H->heap_size == 0)
        perror("Heap underflow");

    return H->arr[1].p.process;
}

void heapify(struct Heap *H, int i) {
    // H is a reference to the heap of type struct Heap
    // i is the index at which the heapify needs to be performed
    // Output: node at index i satisfies the max heap property
    int l = LEFT(i);
    int r = RIGHT(i);
    int largest = i;

    if (l <= H->heap_size && cmp_entries(H->arr[l], H->arr[i]))
        largest = l;
    if (r <= H->heap_size && cmp_entries(H->arr[r], H->arr[largest]))
        largest = r;

    if (largest != i) {
        exchange(&H->arr[i], &H->arr[largest]);
        heapify(H, largest);
    }
}

struct proc extract_max(struct Heap *H) {
    // H is a reference to the heap of type struct Heap
    // Output: delete the entry with the maximum priority from the heap, if multiple
    // processes have the maximum priority, delete the one that arrived first
    // return the process of the deleted entry
    struct proc max = find_max(H);
    H->arr[1] = H->arr[H->heap_size];
    H->heap_size = H->heap_size - 1;
    heapify(H, 1);
    return max;
}

void delete_entry(struct Heap *H, struct proc x) {
    // H is a reference to the heap of type struct Heap
    // x is the struct proc element to be deleted
    // Output: delete the entry with process x from the heap

    int i;

    for (i = 1; i <= H->heap_size; i++) {
        if (H->arr[i].p.process.pid == x.pid) {
            // Element found, replace it with the last element in the heap
            H->arr[i] = H->arr[H->heap_size];
            H->heap_size = H->heap_size - 1;

            // Perform heapify operation to maintain the max-heap property
            heapify(H, i);
            break;
        }
    }
}

struct proc search_by_pid(struct Heap *H, pid_t target_pid) {
    // H is a reference to the heap of type struct Heap
    // target_pid is the PID of the process to search for
    // Output: return the process with the specified PID

    for (int i = 1; i <= H->heap_size; i++) {
        if (H->arr[i].p.process.pid == target_pid) {
            return H->arr[i].p.process; // Process with matching PID found
        }
    }

    // If the process with the specified PID is not found, you can handle it as needed.
    // For example, you can return an error code or take other appropriate actions.
    // Here, we are returning a dummy process with a PID of -1 to indicate that no matching process was found.
    struct proc dummy;
    dummy.pid = -1;
    return dummy;
}
