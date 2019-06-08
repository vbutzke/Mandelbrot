#include <iostream>
#include <mpi.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main(int argc, char **argv)
{
	int rank, size, tag = 0, i = 0;
	MPI_Status status;
	int *sendBuffer, receiveBuffer;
	char msg[20];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	sendBuffer = (int *)malloc(size*sizeof(int));
	//receiveBuffer = (int*)malloc(size * sizeof(int));
	//sendBuffer = rank * rank;
	for (int i = 0; i < size; i++) {
		sendBuffer[i] = i * i;
	}

	MPI_Scatter(sendBuffer, 1, MPI_INT, &receiveBuffer, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (rank != 0) {
		printf("I'm thread %d !", rank);
	}

	MPI_Gather(&receiveBuffer, 1, MPI_INT, sendBuffer, 1, MPI_INT, 0, MPI_COMM_WORLD);
	

	if(rank == 0){
		for (int i = 0; i < size; i++)
			printf("(%d) - received %d\n", rank, sendBuffer[i]);
		
	}	

	

	//if (rank == 0) {
	//	//root
	//	//faz um scatter
	//	strcpy_s(msg, "Hello thread!");
	//	for (i = 1; i < size; i++) {
	//		MPI_Send(msg, 13, MPI_CHAR, i, tag, MPI_COMM_WORLD);
	//		printf("Message sent!");
	//	}
	//	//gather
	//	//loop de desenho
	//
	//} else {
	//	//filhos
	//	//processa e manda de volta
	//	MPI_Recv(msg, 20, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
	//	printf("Received message, hello from other thread! %p", msg);
	//	//cálculo
	//}
	
	MPI_Finalize();
	return 0;
}

GLfloat* mandelbrot_equation(float x, float y) {
	GLfloat result[] = { 1.0f, 1.0f, 1.0f };
	float xReduce = x - (1 / 4);
	float ho = sqrt(pow(xReduce, 2) + pow(y, 2));
	float theta = atanf(y / xReduce);
	float hoPoint = 0.5f - (0.5f * cos(theta));

	if (ho <= hoPoint) {
		result[0] = result[1] = result[2] = 0.0f;
		//pertence a Mandelbrot, pinta diferente
	}
	else {
		//não pertence
	}
	return result;
}