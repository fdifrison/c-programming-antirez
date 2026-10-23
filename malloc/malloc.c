#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


/**
 * N.B. even with a zero-byte inside the string the print won't stop since
 * we specify in advance the number of char to be printed (the len stored at str[0])
 */
void buf_print(char *str) {
    // the first char represents the len of the string that is stored in the buffer
    // that's why they are called Prefixed Length String
    unsigned char *len_prt = (unsigned char *) str;
    for (int j = 0; j < len_prt[0]; j++) {
        putchar(str[j + 1]);
    }
    printf("\n");
};

/**
 * return the len of a string in O(1) since we know exactly where the len
 * of the string is stored and we dont need to look for the NULL terminator at the end of the string
 */
uint32_t *str_32b_len(char *str) {
    uint32_t *len_prt = (uint32_t *) (str - 4);
    return len_prt;
}

void buf_32_free(char *str) {
    // i need to 'free' in the point where i 'malloc'
    // hence 4 bytes (the length) before the beginning of the string
    free(str - 4);
}


/**
 * we need to take into account that our 32bit string is
 * passed with the pointer to the beginning of the string,
 * hence the pointer should start 4 bytes before
 */
void buf_print_32(char *str) {
    const uint32_t *len_prt = str_32b_len(str);
    for (int j = 0; j < (int) len_prt[0]; j++) {
        putchar(str[j]);
    }
    printf("\n");
};


/**
 *
 * @param s buffer
 * @param str str to insert in the buffer
 * @param len str len
 * Insert into the buffer a Prefixed Length String : a string which size is
 * specified in the first char before the actual string itself.
 *
 * @warning This function do not check for buffer overflows
 */
void buf_init(char *s, char *str, int len) {
    // we want to store a pointer to the beginning of the buffer that will
    // contain the len of the string we are  going to insert
    unsigned char *len_prt = (unsigned char *) s;
    *len_prt = len;
    for (int j = 0; j < len; j++) {
        // s[0] contains the len of str1
        s[j + 1] = str[j];
    }
};

/**
 * Use malloc to allocate a prefix length string in memory and return a pointer to it
 */
char *str_init(char *str, int len) {
    char *s = malloc(1 + len); // we ned an additional byte to store the len of the string
    s[0] = (char) len;
    for (int j = 0; j < len; j++) {
        // s[0] contains the len of str1
        s[j + 1] = str[j];
    }
    return s;
};


/**
 *A 32 bit string
 */
char *str_init_32b(char *str, int len) {
    // the +1 in the end is to c-type string interoperability, so that we will have also the NULL terminator
    char *s = malloc(4 + len + 1);
    uint32_t *len_prt = (uint32_t *) s;
    len_prt[0] = len;
    s += 4; // shift the 4 byte to skip at the beginning of the string
    for (int j = 0; j < len; j++) {
        s[j] = str[j];
    }
    s[len] = 0; // this is the +1 value, the NULL terminator for the printf
    return s; // we return the pointer to the beginning of the string
};

int main(void) {
    char buf[256];
    buf_init(buf, "Hello World", 11);
    buf_print(buf);

    //  Now what done so far is non practical since we would need to predefine
    //  the buffers and the strings we want to store. What we want to do is allocate
    //  a specific memory for each string we are storing and return a pointer to it.
    //  To do this we use the malloc() function

    char *my_str = str_init("Hello World", 11);
    buf_print(my_str);

    // Now we want to increase the number of byte allocated for the len of the
    // string in order to be able to use bigger strings

    // lets start with 32 bit = 4 byte using stdint uint32
    char *my_32b_str = str_init_32b("Hello World in 32 bit prefixed length c-compatible", 50);
    printf("my_32b_str is c-compatible= %s\n", my_32b_str);
    buf_print_32(my_32b_str);

    printf("my string length is hex %p\n",*str_32b_len(my_32b_str));
    printf("my string length is decimal %u\n",*str_32b_len(my_32b_str));
    return 0;
}
