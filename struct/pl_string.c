#include <stdio.h>
#include <stdlib.h>

struct pls {
    uint32_t len;
    uint32_t refcount;
    char str[];
    // C99 feature until its empy it contributes to 0 bytes
    // The effective dimension of the string will depend on the malloc operation in the
    // create where we will set to len +1 the size of this char array.
};

struct pls *ps_create(char *str, int len) {
    // we are allocating the bear struct (8bytes) + the size that the str[] will have
    // i.e. the len of the string  + the terminator /0
    struct pls *p = malloc(sizeof(*p) + len + 1);
    p->len = len;
    p->refcount = 1;
    for (int i = 0; i < len; i++) {
        p->str[i] = str[i];
    }
    p->str[len] = '\0';
    return p;
}

void ps_print(char *str) {
    // the str part has to be inserted at the end of the 8 bytes in the flexible array member
    // hence from the pointer to str* that is equivalent to the str[] in the struct,
    // i move back of the dimension of the bstruct (8 bytes) to have a
    // pointer to the struct itself
    struct pls *ps = (struct pls *) (str - sizeof(struct pls));
    for (int i = 0; i < (int) ps->len; i++) {
        putchar(ps->str[i]);
    }
    putchar('\n');
}

int main() {
    struct pls *ps = ps_create("Hello World", 11);
    ps_print(ps->str);
    return 0;
}
