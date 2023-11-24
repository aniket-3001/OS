#include <stdio.h>
#include <stdlib.h>

// Function to calculate the nth Fibonacci number
long long fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    
    else {
        long long a = 0, b = 1, temp;

        for (int i = 2; i <= n; i++) {
            temp = a + b;
            a = b;
            b = temp;
        }

        return b;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("not sufficient arguments\n");
        return 1;
    }

    int n = atoi(argv[1]);
    
    if (n < 0) {
        printf("Please input a whole number.\n");
        return 1;
    }

    long long res = fibonacci(n);

    printf("fib(%d) = %lld\n", n, res);

    return 0;
}
