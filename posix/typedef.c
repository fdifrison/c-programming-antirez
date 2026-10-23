// Intro to typedef
// https://www.youtube.com/watch?v=3w73xjUSUEU&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=21
#include <stdio.h>

// typedef allows to define named alias to specific type
typedef int error_code;

// im defining a new datatype named fract that has defined struct
typedef struct {
    int numerator;
    int denominator;
} fraction;

int main(void) {
    fraction f;
    f.numerator = 1;
    f.denominator = 2;
    return 0;
}
