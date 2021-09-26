#include<iostream>
#include<random>
#include"mpi.h"

using namespace std;

int main(int argc,char ** argv) {

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	const int array_size = 100; // size of the array
	int array[array_size];

	// Populating the array with random values
	for (int i = 0; i < array_size; ++i) {
		array[i] = rand();
	}

	int equal_size = array_size / size;

	

	int res{};

	if (equal_size) {
		for (int i = rank * equal_size; i < (rank + 1) * equal_size; ++i) {
			res += array[i];
		}
	}

	int rem = array_size % size;
	if (rem) {
		if (rank + 1 <= rem) res += array[size * equal_size + rank];
	}

	if (rank) {
		if (equal_size) {
			MPI_Send(&res, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
		}
		else if(rank < array_size) {
			MPI_Send(&res, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
		}
	}
	else {
		int res2;
		if (equal_size) {
			for (int i = 1; i < size; ++i) {
				MPI_Recv(&res2, 1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				res += res2;
			}
		}
		else {
			for (int i = 1; i < array_size; ++i) {
				MPI_Recv(&res2, 1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				res += res2;
			}
		}
		cout << "The sum of the array elements are " << res << "\n";
	}
	

	MPI_Finalize();

	return 0;
}