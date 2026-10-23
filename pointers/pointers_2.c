#include <stdio.h>

void print_str(char *s) { // i take an integer representing a memory address
    while (*s) { // while the memory address does not point to the null address -> *s != 0
        putchar(*s); // print the value at the memory address
        s++; // move the memory address by 1 byte (since we are in a char array where each values occupies 1 byte)
    }
    printf("\n");
}

int main(void) {
    char s[] = "12345678";
    print_str(s);
    return 0;
}
