#include <iostream>
#include <mpi.h>

using namespace std;

int my_abs(int n)
{
	if(n<0)
		return -n;
	return n;
}

int main(int argc, char * argv[])
{
	int comm_size, current_rank;
	int * my_vector = NULL, local_total;
	int divisions, length;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);

	if(current_rank == 0)
	{		
		cout << "Insert length : " ;
		cin >> length;

		my_vector = new int[length];
		divisions = length/comm_size;

		for(size_t i=0 ; i<length ; ++i)
		{
			cout << "Insert number at pos " << i << " : ";
			cin >> my_vector[i];
		}
	}
	
	MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&divisions, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if(my_vector == NULL)
		my_vector = new int[length];

	MPI_Bcast(my_vector, length, MPI_INT, 0, MPI_COMM_WORLD);

	local_total = 0;
	for(size_t i=current_rank*divisions ; i<((current_rank+1)*divisions) ; ++i)
		local_total += my_vector[i];

	int odd_total;

	for(size_t j=1,f ; j<comm_size ; j<<=1)
	{		
		if(!((current_rank/j)&1))
		{
			MPI_Sendrecv(&local_total, 1, MPI_INT, current_rank+j, 0, 
				&odd_total, 1, MPI_INT, current_rank+j, 0, 
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		else
		{
			MPI_Sendrecv(&local_total, 1, MPI_INT, my_abs(current_rank-j), 0, 
				&odd_total, 1, MPI_INT, my_abs(current_rank-j), 0, 
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		local_total += odd_total;
	}

	if(current_rank == 0)
		cout << "Total : " << local_total << endl;

	MPI_Finalize();

	return 0;
}