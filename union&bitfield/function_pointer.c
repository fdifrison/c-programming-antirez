// https://www.youtube.com/watch?v=OIseV5lcx8w&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=25

#include <stdio.h>
#include <stdlib.h>

/* pointers can also point to the position in memory where a specific
 * function is stored. the name of a function has its pointer as well
*/


void hello() {
    printf("hello world\n");
}

void ciao() {
    printf("ciao\n");
}

/*
 * The second argument is defining a prototype of function im expecting
 * a function that returns void and accept 0 parameters
 */
void salute_n_times(int n, void (*x)(void)) {
    while (n--) {
        x();
    }
}

// comparator required by the qsort
int compare_int(const void *a, const void *b) {
    // im converting the generic pointers to the actual int pointers i need
    const int *a_pointer = a;
    const int *b_pointer = b;
    // not safe for arbitrary big numbers
    // return a_pointer[0] - b_pointer[0];
    if (a_pointer[0] > b_pointer[0])
        return 1;
    if (a_pointer[0] < b_pointer[0])
        return -1;
    return 0;
};


int main(void) {
    printf("main is recorded at %p\n", main);

    // This is how you write a function pointer
    int (*myfunc)(void);

    myfunc = main;

    printf("myfunc points at main %p\n", main);

    void (*func)(void);
    func = hello;
    func();
    func = ciao;
    salute_n_times(3, func);

    // example using qsort
    int a[10];
    for (int i = 0; i < 10; i++) {
        a[i] = rand() & 15;
        /*
         *bitwise and is a  mask
         *  1101 0011  (rand() = 211)
         *& 0000 1111  (mask = 15)
         *  -----------
         *  0000 0011  (result = 3)
         */
    }

    for (int i = 0; i < 10; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");


    qsort(a, 10, sizeof(int), compare_int);

    printf("After qsort:\n");

    for (int i = 0; i < 10; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}
