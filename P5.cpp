#include<iostream>
#include<random>
#include"mpi.h"

using namespace std;

int main(int argc,char ** argv) {

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	const int Vector_size = 50; // array size
	
	int eq_block = Vector_size / size;
	int rem = Vector_size % size;
	if (rank < rem) ++eq_block;

	int* loc_arr = NULL;

	if (eq_block) loc_arr = new int[eq_block];

	if (rank == 0) {
		int* array = new int[Vector_size];
		for (int i = 0; i < Vector_size; ++i) array[i] = rand(); // assuming rand() max value is 32k

		int* counts = new int[size];
		int* dis = new int[size];

		int block = Vector_size / size;
		int remv = Vector_size % size;
		if (remv) ++block;

		counts[0] = block;
		dis[0] = 0;

		for (int i = 1; i < size; ++i) {
			block = Vector_size / size;
			if (i < rem) ++block;
			dis[i] = dis[i - 1] + counts[i - 1];
			counts[i] = block;
		}

		MPI_Scatterv(array, counts, dis, MPI_INT, loc_arr, eq_block, MPI_INT, 0, MPI_COMM_WORLD);

	}
	else {
		MPI_Scatterv(NULL, NULL, NULL, NULL, loc_arr, eq_block, MPI_INT, 0, MPI_COMM_WORLD);
	}


	long long int local_sum{};
	long long int global_sum{};

	for (int i = 0; i < eq_block; ++i) local_sum += loc_arr[i] * loc_arr[i];

	MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		cout << "The given matrix product or dot product of given vector is " << global_sum << "\n";
	}


	MPI_Finalize();

	return 0;
}