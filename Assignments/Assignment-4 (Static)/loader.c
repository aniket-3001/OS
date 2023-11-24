#include "loader.h"

#define PAGE_SIZE 4096

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

struct results {
  int res;
  int num_page_faults;
  int num_page_allocations;
  double internal_fragmentation;
};

void* arr[50]; // array to store the pages, so that we can unmap them later
int idx = 0;

struct results loader_results; // struct type to store the results

int flags = MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS;
int prot = PROT_READ | PROT_WRITE | PROT_EXEC;

void loader_cleanup() {
  int result;

  // closing the ELF file
  if (fd != -1) {
    result = close(fd);
  }

  if (result == -1) {
    perror("Error while closing ELF file");
  }

  // unmapping the mapped pages
  for (int i = 0; i < idx; i++) {
    result = munmap(arr[i], PAGE_SIZE);

    if (result == -1) {
      perror("Error while unmapping page");
    }
  }

  // unmapping the ELF header
  if (ehdr != MAP_FAILED) {
    result = munmap(ehdr, sizeof(Elf32_Ehdr));
  }

  if (result == -1) {
    perror("Error while unmapping binary content");
  }
}

void seg_fault_handler(int signum, siginfo_t *info, void *context) {
  if (signum == SIGSEGV) {
    loader_results.num_page_faults++;
    loader_results.num_page_allocations++;

    void* fault_addr = info->si_addr; // this is the address that caused the segfault

    // iterating over the program headers to find the segment that contains the faulting address
    for (int i = 0; i < ehdr->e_phnum; i++) {
      void* start_addr = (void*)(uintptr_t)phdr[i].p_vaddr; // starting address of the segment
      void* end_addr = (void*)(uintptr_t)(phdr[i].p_vaddr + phdr[i].p_memsz); // ending address of the segment

      // check if the faulting address is within the segment
      if (fault_addr >= start_addr && fault_addr < end_addr) {
        // printf("start_addr: %p\n", start_addr);
        // printf("fault_addr: %p\n", fault_addr);
        // printf("end_addr: %p\n", end_addr);

        int num_pages = (phdr[i].p_memsz + PAGE_SIZE - 1) / PAGE_SIZE; // number of pages in the segment
        int page_idx = ((uintptr_t)fault_addr - (uintptr_t)start_addr) / PAGE_SIZE; // index of the page that contains the faulting address

        void* page = mmap(start_addr + page_idx * PAGE_SIZE, PAGE_SIZE, prot, flags, fd, phdr[i].p_offset + page_idx * PAGE_SIZE); // mapping the page
        
        if (page == MAP_FAILED) {
          perror("Mmap failure while allocating memory for segment");
          exit(1);
        }

        // setting the file pointer at the appropriate offset
        if (lseek(fd, phdr[i].p_offset + page_idx * PAGE_SIZE, SEEK_SET) == -1) {
          perror("Error while seeking to segment offset");
          exit(1);
        }

        int remaining_bytes = phdr[i].p_memsz - (page_idx * PAGE_SIZE); // number of bytes remaining to be read
        int bytes_to_read = (remaining_bytes < PAGE_SIZE) ? remaining_bytes : PAGE_SIZE; // number of bytes to be read

        // printf("num_pages: %d\n", num_pages);
        // printf("page_idx: %d\n", page_idx);
        // printf("memory size: %d\n", phdr[i].p_memsz);
        // printf("remaining_bytes: %d\n", remaining_bytes);
        // printf("bytes_to_read: %d\n", bytes_to_read);
        // printf("\n");

        // reading into the page
        if (read(fd, page, bytes_to_read) == -1) {
          perror("Error while reading segment from file");
          exit(1);
        }

        arr[idx++] = page; // storing the page in the array

        if (fault_addr + PAGE_SIZE > end_addr) {
          loader_results.internal_fragmentation += PAGE_SIZE - (end_addr - fault_addr);
        }

        return;
      }
    }
  }
}

void load_and_run_elf(char** exe) {
  fd = open(exe[1], O_RDONLY);

  if (fd == -1) {
    perror("Error while opening the executable");
    exit(1);
  }

  char elf_magic[4];

  // checking if the file is an ELF executable
  if (read(fd, elf_magic, 4) != 4 || elf_magic[0] != 0x7f || elf_magic[1] != 'E' || elf_magic[2] != 'L' || elf_magic[3] != 'F') {
    perror("Not an ELF executable");
    exit(1);
  }

  ehdr = mmap(NULL, sizeof(Elf32_Ehdr), PROT_READ, MAP_PRIVATE, fd, 0); // mapping the ELF header

  if (ehdr == MAP_FAILED) {
    perror("Map failed in ehdr");
    exit(1);
  }

  phdr = (Elf32_Phdr*)((char*)ehdr + ehdr->e_phoff); // typecasting to char* to avoid pointer arithmetic errors

  int (*_start)() = (int (*)())(uintptr_t)(ehdr->e_entry); // typecasting entrypoint
  loader_results.res = _start(); // calling _start()
}

int main(int argc, char** argv)
{
  struct sigaction sig;
  memset(&sig, 0, sizeof(sig));
  sig.sa_sigaction = seg_fault_handler;
  sig.sa_flags = SA_SIGINFO; // contains info about the faulting address

  // initializing the result storing struct
  loader_results.res = 0;
  loader_results.num_page_faults = 0;
  loader_results.num_page_allocations = 0;
  loader_results.internal_fragmentation = 0;

  if (sigaction(SIGSEGV, &sig, NULL) == -1) {
    perror("Error in SIGSEGV handler");
  }

  if (argc != 2) {
    printf("Usage: %s <ELF Executable> \n", argv[0]);
    exit(1);
  }

  load_and_run_elf(argv);

  printf("Result: %d\n", loader_results.res);
  printf("Total page faults: %d\n", loader_results.num_page_faults);
  printf("Total page allocations: %d\n", loader_results.num_page_allocations);
  printf("Total internal fragmentation (KB): %lf\n", loader_results.internal_fragmentation / 1024);
  
  loader_cleanup();

  return 0;
}
