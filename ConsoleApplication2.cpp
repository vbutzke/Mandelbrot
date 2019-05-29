#include <iostream>
#include <mpi.h>

int main(int argc, char **argv)
{
	int rank, size, tag = 0, i = 0;
	MPI_Status status;
	char msg[20];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if (rank == 0) {
		strcpy_s(msg, "Hello thread!");
		for (i = 1; i < size; i++) {
			MPI_Send(msg, 13, MPI_CHAR, i, tag, MPI_COMM_WORLD);
			printf("Message sent!");
		}
	} else {
		MPI_Recv(msg, 20, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
		printf("Received message, hello from other thread!");
	}
	
	MPI_Finalize();
	return 0;
}