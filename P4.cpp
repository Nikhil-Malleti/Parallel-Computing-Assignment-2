#include<iostream>
#include<random>
#include"mpi.h"

using namespace std;

int main(int argc,char ** argv) {

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	const int array_size = 100; // array size
	int Distribution_type = 3; // 1 For block , 2 = cyclic , 3 = block cyclic , for 3 define block size
	int B_size = 2;

	if (Distribution_type == 1) {
		int eq_block = array_size / size;
		int rem = array_size % size;
		if (rank < rem) ++eq_block;

		int* loc_arr = NULL;

		if (eq_block) loc_arr = new int[eq_block];

		if (rank == 0) {
			int* array = new int[array_size];
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

	}
	else if (Distribution_type == 2) {
		int total_size = ((array_size + size - 1) / size) * size; // round up the size to next multiple of size
		int chunck_size = total_size / size;
		int* loc_arr = new int[chunck_size];

		if (rank == 0) {
			int* array = new int[total_size];
			for (int i = 0; i < array_size; ++i) array[i] = i;
			for (int i = array_size; i < total_size; ++i) array[i] = 0;

			int noi = chunck_size;

			for (int i = 0; i < noi; ++i) {
				MPI_Scatter(array + i * size, 1, MPI_INT, loc_arr + i, 1, MPI_INT, 0, MPI_COMM_WORLD);
			}

		}
		else {
			for (int i = 0; i < chunck_size; ++i) {
				MPI_Scatter(NULL, NULL, NULL, loc_arr + i, 1, MPI_INT, 0, MPI_COMM_WORLD);
			}
		}

		int local_sum{};
		int global_sum{};

		for (int i = 0; i < chunck_size; ++i) local_sum += loc_arr[i];

		MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

		if (rank == 0) {
			cout << "The sum of the elements in the array is " << global_sum << "\n";
		}

	}
	else {

		if (B_size == 0) {
			cout << " Block size is zero \n";
			exit(-1);
		}
		else if (B_size > array_size) {
			cout << " Block size is too large \n";
			exit(-1);
		}

		int single_B_size = B_size * size;
		int total_size = ((array_size + single_B_size - 1) / single_B_size) * single_B_size; // round up the size to next multiple of single_B_size
		int chunck_size = (total_size / single_B_size) * B_size; // Total size of Blocks that will be transfered in single iteration
		int* loc_arr = new int[chunck_size];
		int noi = total_size / single_B_size;

		if (rank == 0) {
			int* array = new int[total_size];
			for (int i = 0; i < array_size; ++i) array[i] = i;
			for (int i = array_size; i < total_size; ++i) array[i] = 0;

			

			for (int i = 0; i < noi; ++i) {
				MPI_Scatter(array + i * single_B_size, B_size, MPI_INT, loc_arr + i * B_size, B_size, MPI_INT, 0, MPI_COMM_WORLD);
			}

		}
		else {

			for (int i = 0; i < noi; ++i) {
				MPI_Scatter(NULL, NULL, NULL, loc_arr + i * B_size, B_size, MPI_INT, 0, MPI_COMM_WORLD);
			}
		}

		int local_sum{};
		int global_sum{};

		for (int i = 0; i < chunck_size; ++i) local_sum += loc_arr[i];

		MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

		if (rank == 0) {
			cout << "The sum of the elements in the array is " << global_sum << "\n";
		}

	}


	MPI_Finalize();

	return 0;
}