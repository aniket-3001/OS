void scheduler() {
    while (true) {
        lock(process_table);
        foreach (Process p: scheduling_algorithm(process_table)) {
            if (p->state != READY) {
                continue;
            }

            p->state = RUNNING;
            unlock(process_table);
            swtch(scheduler_process, p);
            // p is done for now..
            lock(process_table);
        }

        unlock(process_table);
    }
}