#include <stdio.h>

void increment(int *x) {
    printf("This was x:%d\n", *x);
    *x = *x + 1;
    printf("This is now x:%d\n", *x);
};

int main(void) {
    int x = 5;
    // with * i define a pointer; 0 is the NULL pointer *y = NULL is equivalent
    int *y = 0;
    int *z = NULL;
    printf("This is x:%d\n", x);
    printf("This is y:%p\n", y); //0x0 in hexadecimal
    printf("This is z:%p\n", z);

    // with & im asking the memory address of a variable
    y = &x;
    printf("This is x memory address %p and its value is %d\n", y, *y);

    // Dereferencing: im saying: the address memory associated to y (of type int)
    // has to store the value 10.
    *y = 10;
    // equivalent to y[0] = 10, since y[0] pints to the beginning
    // of the 4 or 8 bit of the first int
    // since eith y=&x i hav0 told that y point to the memory address of x
    // im de facto replacing the value of x
    printf("This is now x:%d\n", x);
    increment(&x);

    // all the pointers have the same size given a specific machine
    int **c = &y;
    printf("the size of pointers y , z and c is : %d %d %d bytes (*8=bits)\n",
           (int) sizeof(y), (int) sizeof(z), (int) sizeof(c));
    printf("While x is an int and has size: %d bytes\n", (int) sizeof(x));

    // an array name is always the pointer to that same array
    char str[] = "123456789";
    printf("The string array points to %p and has value '%s'\n", str, str);
    char *p = str;
    // with the notation *p we are pointing at the memory address  of the first
    // element in the array, same with p[0]. Hence, i can access the subsequent
    // values, by moving the pointer.
    printf("The second letter in the array is '%c'\n", *(p+1));
    printf("The third letter in the array is '%c'\n", p[2]);
    // The pointer moves of the dimension of its type. If for example we
    // try to use a pointer of type short (16 bit) to the char[] containing the
    // string, adding +1 means moving by 2 letters (each letter is 8 bit).
    short *s = (short*)str;
    printf("s[0] starts at %c\n", s[0]);
    printf("s[1] starts at %c\n", s[1]);
    return 0;
}
