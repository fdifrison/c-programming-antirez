// https://www.youtube.com/watch?v=TM4jgODgdFY&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=24


#include <stdio.h>

/* in reality the specification for bitfields is not wildly used
 * because on moder machine we are not sure of possible paddings, memory sharding etc..
 * exception made for some very strictly defined protocol like IP or for structures
 * that are kept only in memory like redisObject
 * TLDR; bitfields gives us very few guarantees (e.g. we ned to check if the machine if little or big endian)
 */
struct foo {
    /* the semicolon ':' is called bitfield operation, we are setting each field
     * exactly how many bits the fields must occupy*/
    unsigned char a: 4;
    unsigned char b: 4;
    unsigned char c: 8;
};


int main(void) {
    struct foo f;
    // my expectation in sizeof = 2bytes 4+4+8 bits
    printf("sizeof(f)=%zu\n", sizeof(f));

    // since i specified that field 'a' must be at max 4 bits
    // it can hold from 0-15, and since it is unsigned it has
    // wrap around, hence 16 -> 0
    f.a = 16;
    f.b = 2;
    f.c = 3;
    printf("%d %d %d", f.a, f.b, f.c);

    return 0;
}
