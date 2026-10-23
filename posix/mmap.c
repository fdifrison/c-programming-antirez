// https://www.youtube.com/watch?v=yKavhObop5I&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=24
#include <sys/mman.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>

int main(void) {
    int fd = open("mmap.c", O_RDONLY);
    // should be 3, since 0,1,2 are the file descriptor of read write and execute
    printf("File descriptor no = %d\n", fd);

    // In case of error doesnt return NULL but the last available address in memory (2**64-1 on this machine)
    // It returns MAP_FAILED defined by the standard posix (void*) -1
    void *mem = mmap(NULL, 100, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    printf("File mapped at pointer = %p\n", mem);

    char *buffer = mem;
    for (int i = 0; i < 1000; i++) {
        printf("%c", buffer[i]);
    }

    // printf("Page size in this kernel machine = %ld\n", sysconf(_SC_PAGESIZE) / 1000);

    return 0;
}
