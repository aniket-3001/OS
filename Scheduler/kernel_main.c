void kernel_main() {
    setup();
    init_all_cpus();
    start_init_process(); // on CPU-0

    for(int cpu=1; cpu<numCPUs; cpu++) {
        launch(cpu, scheduler);
    }
    
    scheduler(); // on CPU-0
}