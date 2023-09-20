// Include the header file containing MPI APIs
#include <mpi.h>
#include <stdio.h> // Include for printf

int main(int argc, char **argv) {
    // Initialize the MPI runtime
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    // Get the total number of processes in MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    // Get the rank of this process in MPI_COMM_WORLD
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("My rank is %d in world of size %d\n", rank, nprocs);

    // Terminate the MPI runtime
    MPI_Finalize();

    return 0;
}
