#include <stdio.h>
#include <stdlib.h>

struct fract {
    int num;
    int den;
};


/**
 *struct in c are basically a new custom datatype, thus i can return them from a
 *function. However, when then we will initialize a variable of type struct frac
 *calling this functions the compiler will need to do a copy of the bytes from the
 *struct created inside the function and the one to which we are assigning the result
 *If the struct is big this can easily become a burden in terms of performance.
 *Hence what we usually do is to return a pointer to the struct.
 */
struct fract create_fract_bad(int num, int den) {
    struct fract f;
    f.num = num;
    f.den = den;
    return f;
}

struct fract *create_fract_good(int num, int den) {
    // The pointer *f inside size of is equivalent to 'struct fract'
    // because id f is a pointer, with *f im taking the value inside that pointer
    struct fract *f = malloc(sizeof(*f));
    // malloc error checking in case there is no memory available
    if (f == NULL) return NULL;
    f->num = num;
    f->den = den;
    return f;
}

void print_fract(struct fract *fract) {
    printf("%d/%d", fract->num, fract->den);
    printf("\n");
}

int main() {
    // bad performance we are copying the bytes from the struct
    // created inside the function to the one declared in the main
    struct fract a = create_fract_bad(1, 2);

    print_fract(&a);
    // if im using a pointer to the struct i cannot use
    // the dot notation to access its fields but the arrows
    struct fract *b = create_fract_good(a.num, a.den);
    b->num = 3;
    b->den = 4;

    print_fract(b);
    return 0;
}
