#include<iostream>
#include<random>
#include"mpi.h"

using namespace std;

void iittp_barrier(MPI_Comm communicator) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size == 1) return;

	if (rank == 0) {
		int *buf = new int(size);
		for (int i = 1; i < size; ++i) {
			MPI_Recv(buf, 1, MPI_INT, i, i, communicator, MPI_STATUSES_IGNORE);
		}
		for (int i = 1; i < size; ++i) {
			MPI_Send(&rank, 1, MPI_INT,i,i,communicator);
		}
	}
	else {
		MPI_Send(&rank, 1, MPI_INT, 0, rank, communicator);
		int temp;
		MPI_Recv(&temp, 1, MPI_INT, 0, rank, communicator, MPI_STATUSES_IGNORE);
	}

}

int main(int argc,char ** argv) {

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	cout << "Before IITTP barrier in rank " << rank << "\n" << flush;

	iittp_barrier(MPI_COMM_WORLD);

	cout << "After IITTP barrier in rank " << rank << "\n" << flush;

	MPI_Finalize();

	return 0;
}