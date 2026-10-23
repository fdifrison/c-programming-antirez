// https://www.youtube.com/watch?v=TM4jgODgdFY&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=24


#include <limits.h>
#include <stdint.h>
#include <stdio.h>

/* layout in memoria IIII CCCC*/
struct {
    int i;
    unsigned char a[4];
};

/* layout in memoria
 * IIII
 * CCCC
 * they start from the same offset in memory, hence i can have only one of the two
 */
struct foo {
    // unanimous union
    // like for struct. the memory layout of a union is represented by the
    // biggest element + padding
    union {
        int i;
        unsigned char a[4];
    };
};


int main(void) {
    struct foo f;
    f.i = INT_MAX; // max possible integer on this machine
    // 2147483647 [255,255,255,127] we are automatically filling both the structures
    // teh same objects are stored in the same memory space!
    printf("%d [%d,%d,%d,%d]\n", f.i, f.a[0], f.a[1], f.a[2], f.a[3]);

    return 0;
}
