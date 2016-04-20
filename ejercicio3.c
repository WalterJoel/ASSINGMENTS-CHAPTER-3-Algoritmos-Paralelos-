#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int Global_sum(int val, int my_rank, int comm_sz, MPI_Comm comm) {
    int sum = val;
    int tmp;
    int partner;
    int done = 0;
    unsigned bitmask = (unsigned) 1;
    while (!done && bitmask < comm_sz) {
        partner = my_rank ^ bitmask;
        if (my_rank < partner) {
            if (partner < comm_sz) {
                MPI_Recv(&tmp, 1, MPI_INT, partner, 0, comm, 
                      MPI_STATUS_IGNORE);
                sum += tmp;
            }
            bitmask <<= 1;
        } else {
            MPI_Send(&sum, 1, MPI_INT, partner, 0, comm); 
            done = 1;
        }
    }
    return sum;
}  

int main(int argc, char **argv){
    
    MPI_Init(NULL, NULL);
    int limit = atof(argv[1]);
    int comm_sz, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm comm = MPI_COMM_WORLD;;
    int val;
    int total;

    srandom(my_rank+1);
    val = random() % limit;
    printf("thread %d -> val = %d\n", my_rank, val);

    total = Global_sum(val, my_rank, comm_sz, comm);

    if (!my_rank)
        printf("The total is %d\n", total);

    MPI_Finalize();
    return 0;
}