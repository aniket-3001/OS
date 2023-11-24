#include <iostream>

#define SIZE 9
#define ITERATIONS 1000

float A[SIZE + 2], B[SIZE + 2];

// Function to initialize arrays
void initialize_arrays() {
    // Initialize arrays with zeros
    for (int i = 0; i < SIZE + 2; i++) {
        A[i] = B[i] = 0.0;
    }

    // Set boundary conditions
    A[0] = B[0] = 0.0;
    A[SIZE + 1] = B[SIZE + 1] = 1.0;
}

// Function for iterative averaging
void iterative_averaging() {
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int j = 1; j <= SIZE; j++) {
            B[j] = (A[j - 1] + A[j + 1]) / 2.0;
        }

        // Swap arrays A and B
        std::swap(A, B);
    }
}

// Function to print the final values of array A
void print_final_result() {
    std::cout << "Final values of array A:\n";
    for (int i = 0; i < SIZE + 2; i++) {
        std::cout << A[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    // Initialize arrays
    initialize_arrays();

    // Perform iterative averaging
    iterative_averaging();

    // Print the final result
    print_final_result();

    return 0;
}
