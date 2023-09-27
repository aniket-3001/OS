// compile using mpicc file_name.c
// run using mpirun -np 4 ./a.out

#include <mpi.h>
#include <stdio.h>

#define SIZE 16 // Replace with your desired array size

int main(int argc, char **argv) {
    int rank, nprocs;

    MPI_Init(&argc, &argv);

    // 1. Get to know your world
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int array[SIZE]; // Initialize and assume (SIZE % nprocs = 0)

    // Initialize the array with sequential values
    for (int i = 0; i < SIZE; i++) {
        array[i] = i;
    }

    // 2. Calculate local sum
    int my_sum = 0, chunk = SIZE / nprocs;

    for (int i = rank * chunk; i < (rank + 1) * chunk; i++) {
        my_sum += array[i];
    }

    // 3. All non-root processes send results to the root process (rank=0)
    if (rank > 0) {
        MPI_Send(&my_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } 
    
    else { // Executed only at rank = 0
        int total_sum = my_sum, tmp;
        for (int src = 1; src < nprocs; src++) {
            MPI_Status status;
            MPI_Recv(&tmp, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
            total_sum += tmp;
        }
        printf("Total sum: %d\n", total_sum); // Print the total sum at rank=0
    }

    MPI_Finalize();

    return 0;
}
