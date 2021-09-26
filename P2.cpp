#include<iostream>
#include<random>
#include"mpi.h"

using namespace std;

int main(int argc,char ** argv) {

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	const int array_size = 10; // array size
	int array[array_size];

	for (int i = 0; i < array_size; ++i) {
		array[i] = rand();
	}
	
	if (size > 1) {

		MPI_Send(array, array_size, MPI_INT, (rank + 1) % size, (rank + 1) % size, MPI_COMM_WORLD);

		MPI_Recv(array, array_size, MPI_INT, (rank + size - 1) % size, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	}

	cout << "The array received from " << (rank + size - 1) % size << " to " << rank << " is :\n";
	for (int i = 0; i < array_size; ++i) cout << array[i] << " ";
	cout << "\n";


	MPI_Finalize();

	return 0;
}