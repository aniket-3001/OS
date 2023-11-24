
#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdatomic.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <elf.h>
#include <setjmp.h>
#include <errno.h>
#include <stdint.h>

void load_and_run_elf(char** exe);
void loader_cleanup();
void seg_fault_handler(int signum, siginfo_t *info, void *context);
