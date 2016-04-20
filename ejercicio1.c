#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>


int find_bin(double data, double* bin_maxes, int nbins){
    for (int i=0;i<nbins-1;i++){
        if ((bin_maxes[i] <= data) && (data <= bin_maxes[i+1])){
            return i;
        }
    }
}

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);

    int comm_sz, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int n, nbins;
    double bin_width;
    double* data_to_send;
    double* bin_maxes;

    if (my_rank == 0) {
        n = argc-1;
        nbins = 5;
        double max = -1000000, min = 1000000000;
        double data[n];
        bin_maxes = new double[nbins+1];
        data_to_send = new double[n/nbins+1];
        int* local_bin_counts= new int[nbins];
        int n_maxes = nbins+1;
        int* bin_counts = new int[nbins];
        bin_counts = (int*) malloc((nbins)*sizeof(int));
        local_bin_counts = (int*) malloc((nbins)*sizeof(int));
        for(int j = 0; j < nbins; ++j){
                bin_counts[j] = 0;
        }
        for(int j = 0; j < nbins; ++j){
                local_bin_counts[j] = 0;
        }
        for (int i=0; i<n; i++){
            data[i] = atof(argv[i+1]);
            if (data[i]>max)
                max = data[i];
            if (data[i]<min)
                min = data[i];
        }
        bin_width = (max - min) / nbins;
        for (int i=0; i<nbins+1;i++){
            bin_maxes[i] = min + bin_width*(i);
        }

        for(int i = 1; i < comm_sz; ++i) {
            for (int j = 0; j < n/(comm_sz-1); j++) {
                data_to_send[j] = data[((i-1)*n/(comm_sz-1))+j];
                //printf("%d -> %f\n", i,data_to_send[j]);
            }

            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&nbins, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&bin_width, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
            MPI_Send(bin_maxes, n_maxes, MPI_DOUBLE, i, 3, MPI_COMM_WORLD);
            MPI_Send(data_to_send, n/nbins, MPI_DOUBLE, i, 4, MPI_COMM_WORLD);
        }
        for(int i = 1; i < comm_sz; ++i){
            MPI_Recv(local_bin_counts, nbins, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int j = 0; j < nbins; ++j){
                bin_counts[j] += local_bin_counts[j];
            }
        }
        for (int i=0;i<nbins;i++){
            printf("bin: %d -> %d elements \n", i, bin_counts[i]);
        }
    }
    else {

        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&nbins, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&bin_width, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        bin_maxes = (double *) malloc((nbins+1)*sizeof(double));
        int n_data = n/nbins+1,n_maxes=nbins+1;
        MPI_Recv(bin_maxes, n_maxes, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        data_to_send = new double[n/nbins+1];
        MPI_Recv(data_to_send, n_data, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int* local_bin_counts= new int[nbins];
        local_bin_counts = (int *) malloc((nbins)*sizeof(int));
        int bin;
        for(int j = 0; j < nbins; ++j){
                local_bin_counts[j] = 0;
        }
        for (int i=0;i<n_data;i++){
            //printf("%d -> %f\n", my_rank,data_to_send[i]);
            bin = find_bin(data_to_send[i],bin_maxes,n_maxes);
            local_bin_counts[bin]++;
        }

        MPI_Send(local_bin_counts, nbins, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    //num_range(n, bin_maxes, bin_width, nbins, data, 0, MPI_COMM_WORLD);

    MPI_Finalize(); 

    return 0;
}