#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  if (fd != -1) {
    close(fd);
  }
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** exe) {
  fd = open(exe[1], O_RDONLY);

  // error handling for opening ELF file
  if (fd == -1) {
    perror("Encountered an error while opening ELF file");
    return;
  }

  // 1. Load entire binary content into the memory from the ELF file.

  ehdr = mmap(NULL, sizeof(Elf32_Ehdr), PROT_READ, MAP_PRIVATE, fd, 0); // loading binary content using mmap

  // error handling for loading binary content
  if (ehdr == MAP_FAILED) {
    perror("Encountered an error while mapping binary content");
    return;
  }

  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c

  phdr = (Elf32_Phdr *)((char *)ehdr + ehdr->e_phoff); // char typecaste for pointer arithmetic and phdr typecaste for pointer type

  int i; // declaring a variable in the required scope to store the index of the segment containing the entrypoint method
  Elf32_Phdr targetSegment; // declaring a variable in the required scope to store the segment containing the entrypoint method

  // compiling through the segments of the executable to find the segment containing the entrypoint method, i.e PT_LOAD
  for (i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      if (ehdr->e_entry >= phdr[i].p_vaddr && ehdr->e_entry < phdr[i].p_vaddr + phdr[i].p_memsz) { // checking if the entrypoint method is in the segment
        targetSegment = phdr[i]; // storing the segment containing the entrypoint method
        break; // since we have found the segment containing the entrypoint method, we break out of the loop
      }
    }
  }

  if (targetSegment.p_type != PT_LOAD) { // if the entrypoint method is not in any of the segments, we return
    printf("Entrypoint method not found in any of the segments\n");
    return;
  }

  // error handling for finding the segment containing the entrypoint method
  if (i == ehdr->e_phnum) { // if the entrypoint method is not in any of the segments, we return
    printf("Entrypoint method not found in any of the segments\n");
    return;
  }

  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content

  void *virtual_mem = mmap((void *)targetSegment.p_vaddr, targetSegment.p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_FIXED, fd, targetSegment.p_offset); // allocating memory using mmap

  // error handling for allocating memory
  if (virtual_mem == MAP_FAILED) {
    perror("Encountered an error while mapping virtual memory");
    return;
  }

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.

  // virtual_mem + (ehdr->e_entry - phdr[i].p_vaddr) is the address of the _start method
  int (*_start)() = (int (*)())(virtual_mem + (ehdr->e_entry - targetSegment.p_vaddr)); // typecasting the address to that of function pointer matching "_start" method in fib.c

  // 6. Call the "_start" method and print the value returned from the "_start"
  int result = _start();
  printf("User _start return value = %d\n",result);
}