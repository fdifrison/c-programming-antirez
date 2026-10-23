// Hexdump like functionality
// https://www.youtube.com/watch?v=soiBgJjAmP8&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=19

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define  HEXDUMP_CHARS_PER_LINE 8

struct pls {
    long len;
    char str[50];
};


static void hexdump(void *p, size_t len) {
    const int final_pad = (int) (HEXDUMP_CHARS_PER_LINE - len % HEXDUMP_CHARS_PER_LINE) % HEXDUMP_CHARS_PER_LINE;
    unsigned char *byte = p;
    size_t line = 0;
    for (size_t j = 0; j < len; j++) {
        printf("%02X ", byte[j]);
        if ((j + 1) % HEXDUMP_CHARS_PER_LINE == 0 || j == len - 1) {
            if (j == len - 1) {
                for (size_t i = 0; i < final_pad; i++) printf("** ");
            }
            printf("\t");

            for (size_t i = line; i <= j; i++) {
                const int c = isprint(byte[i]) ? byte[i] : '.';
                printf("%c ", c);
            }
            printf("\n");
            line = j + 1;
        }
    }
}

int main() {
    struct pls ps;
    // for visualization lets set the entire memory to 255
    memset(&ps, 0xFF, sizeof(ps));
    ps.len = 10;
    // the compiler will resolve the pointer to the string passed as second argument
    // 11 is to take account for the terminator character
    memcpy(ps.str, "1234567890", 11);
    printf("%p\n", &ps);
    printf("%p\n", ps.str);
    hexdump(&ps, sizeof(ps));
    return 0;
}
