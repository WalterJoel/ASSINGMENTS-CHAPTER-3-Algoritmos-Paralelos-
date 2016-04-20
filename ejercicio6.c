/*Multimplicacion de una matriz con un vector*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define MATRIXVECTOR_SIZE		4

int main(void)
{
	int matrix[MATRIXVECTOR_SIZE][MATRIXVECTOR_SIZE] = {{1, 0, 0, 0},
														{0, 1, 0, 0},
														{0, 0, 1, 0},
														{0, 0, 0, 1}};
	int rvector[MATRIXVECTOR_SIZE]={5,6,1,2};
	int SIZE = sizeof(int);
	int comm_sz, my_rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int real_size=comm_sz-1;
	if(real_size!=MATRIXVECTOR_SIZE)
	{
		if (my_rank == 0)
			printf("Es necesario de %d procesos esclavos\n",MATRIXVECTOR_SIZE);
		MPI_Finalize();
		return 0;
	}
	int size_SUBMATRIX=sqrt(MATRIXVECTOR_SIZE);
	int rec;
	if (my_rank < real_size)
	{
		int l=0;
		for (int i = size_SUBMATRIX*((my_rank/size_SUBMATRIX)%size_SUBMATRIX); i < size_SUBMATRIX*(((my_rank/size_SUBMATRIX)%size_SUBMATRIX)+1); ++i)
		{
			rec = 0.0;
			for (int j = size_SUBMATRIX*(my_rank%size_SUBMATRIX); j < size_SUBMATRIX*((my_rank%size_SUBMATRIX)+1); j++)
				rec += matrix[i][j]*rvector[j];
			MPI_Send(&rec, SIZE, MPI_INT, real_size, my_rank*size_SUBMATRIX+l,MPI_COMM_WORLD);
			++l;
		}
	}
	else
	{
		int vector[MATRIXVECTOR_SIZE*size_SUBMATRIX];
		for (int k=0; k<MATRIXVECTOR_SIZE*size_SUBMATRIX; ++k)
		{
			int owner=k/size_SUBMATRIX;
			MPI_Recv(&rec, SIZE, MPI_INT, owner,k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			vector[k]=rec;
		}
		printf("%d\n", vector[0]+vector[2]);
		printf("%d\n", vector[1]+vector[3]);
		printf("%d\n", vector[4]+vector[6]);
		printf("%d\n", vector[5]+vector[7]);
	}
	MPI_Finalize();
	return 0;
}