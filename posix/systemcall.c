// Using the System Call open() to open a file without using libc implementation
// https://www.youtube.com/watch?v=QWLJ7CBAu_I&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=22
// https://www.youtube.com/watch?v=yKavhObop5I&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=24
#include <ctype.h>
#include <fcntl.h> //see man 2 open
#include <stdio.h>
#include <unistd.h>
#include <sys/errno.h>

#define  HEXDUMP_CHARS_PER_LINE 16

static void hexdump(void *p, size_t len) {
    const size_t final_pad = (HEXDUMP_CHARS_PER_LINE - len % HEXDUMP_CHARS_PER_LINE) % HEXDUMP_CHARS_PER_LINE;
    unsigned char *byte = p;
    size_t line = 0;
    for (size_t j = 0; j < len; j++) {
        printf("%02X ", byte[j]);
        if ((j + 1) % HEXDUMP_CHARS_PER_LINE == 0 || j == len - 1) {
            if (j == len - 1) {
                for (size_t i = 0; i < final_pad; i++) printf("** ");
            }
            printf("\t");

            printf("| ");
            for (size_t i = line; i <= j; i++) {
                const int c = isprint(byte[i]) ? byte[i] : '.';
                printf("%c ", c);
            }
            printf(" |\n");
            line = j + 1;
        }
    }
}




/**
 * A fundamental difference of lib.c calls and system calls is that the first are optimized
 * for us using predefined buffers (e.g. reading a whole line until the  new line simbol)
 * and therefore more efficients (and safe) in the average usage. IF instead we know what we are doing, we can implement
 * our own buffer and leverage the sys calls to achieve better performance custom tailored
 * to our specific use case.
 */
int main(void) {
    int fd = open("systemcall.c", O_RDONLY);
    // the system function open register its output in a global variable called errno
    // that is how perror is able to know in advance which error to print
    printf("Error number is: %d\n", errno);
    // in system call a failure is a -1 not a NULL
    if (fd == -1) {
        perror("Unable to open file");
        return 1;
    }

    char buffer[HEXDUMP_CHARS_PER_LINE];
    // size_t with sign, since system call can return -1
    ssize_t n_read = 0;
    while (1) {
        n_read = read(fd, buffer, sizeof(buffer));
        if (n_read == -1) {
            perror("Reading file");
            return 1;
        }
        if (n_read == 0) break;
        hexdump(buffer, n_read);
    }

    close(fd);
    return 0;
}
