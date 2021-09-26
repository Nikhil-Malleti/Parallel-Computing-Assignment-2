#include<iostream>
#include<random>
#include"mpi.h"

using namespace std;

int main(int argc,char ** argv) {

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	const int array_size = 50; // array size

	int eq_block = array_size / size;
	int rem = array_size % size;
	if (rank < rem) ++eq_block;
	
	int* loc_arr = NULL;

	if (eq_block) loc_arr = new int[eq_block];

	if (rank == 0) {
		int *array = new int[array_size];
		for (int i = 0; i < array_size; ++i) array[i] = i;
		
		int* counts = new int[size];
		int* dis = new int[size];

		int block = array_size / size;
		int remv = array_size % size;
		if (remv) ++block;

		counts[0] = block;
		dis[0] = 0;

		for (int i = 1; i < size; ++i) {
			block = array_size / size;
			if (i < rem) ++block;
			dis[i] = dis[i - 1] + counts[i - 1];
			counts[i] = block;
		}

		MPI_Scatterv(array, counts, dis, MPI_INT, loc_arr, eq_block, MPI_INT, 0, MPI_COMM_WORLD);

	}
	else {
		MPI_Scatterv(NULL, NULL, NULL, NULL, loc_arr, eq_block, MPI_INT, 0, MPI_COMM_WORLD);
	}
	

	int local_sum{};
	int global_sum{};

	for (int i = 0; i < eq_block; ++i) local_sum += loc_arr[i];

	MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		cout << "The sum of the elements in the array is " << global_sum << "\n";
	}

	MPI_Finalize();

	return 0;
}