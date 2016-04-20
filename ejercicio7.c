#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define TESTS 20

int main()
{
	int comm_sz, i, my_rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int temp = 0;
	if(comm_sz != 2)
	{
		if(my_rank==0)
			printf("error\n");
		MPI_Finalize();
		return 0;
	}
	if(my_rank == 0)
	{
		double prom_time=0;
		for(i=0 ; i<TESTS; i++)
		{
			//clock_t ini_start = clock();
			double ini_start = MPI_Wtime();
			MPI_Send(&temp, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&temp, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//clock_t fin_start = clock();
			double fin_start = MPI_Wtime();
			//double time_elapsed = (((double)(fin_start - ini_start))/CLOCKS_PER_SEC);
			double time_elapsed = fin_start - ini_start;
			prom_time+=time_elapsed;
		}
		prom_time=prom_time/TESTS;
		printf("Tiempo promedio: %f\n",prom_time);
	}
	else if(my_rank == 1)
	{
		for(i=0 ; i<TESTS; i++)
		{
			MPI_Recv(&temp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(&temp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
	return 0;
}