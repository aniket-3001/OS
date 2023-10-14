#include <stdio.h>
#include "dummy_main.h"

int fib(int n) {
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}

int main(int argc, char **argv) {
    int n = 45;
    printf("Fibonacci of %d is %d\n", n, fib(n));
    return 0;
}