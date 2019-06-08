#include <iostream>
#include <mpi.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include <string>
#include <assert.h>

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLFWwindow* init();
void draw();

const GLuint WIDTH = 800, HEIGHT = 600;

int main(int argc, char **argv)
{
	int rank, size, tag = 0, i = 0;
	MPI_Status status;
	int *sendBuffer, receiveBuffer;
	
	//keep it simple. Try to draw a triangle first
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	sendBuffer = (int *)malloc(size*sizeof(int));
	
	if (rank == 0) {
		for (int i = 0; i < size; i++) {
			sendBuffer[i] = i * i;
		}
	}
	MPI_Scatter(sendBuffer, 1, MPI_INT, &receiveBuffer, 1, MPI_INT, 0, MPI_COMM_WORLD);
	receiveBuffer = rank * 2;
	if (rank != 0) {
		 
		printf("I'm thread %d !", rank);
	}
	
	MPI_Gather(&receiveBuffer, 1, MPI_INT, sendBuffer, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(rank == 0){
		int result = 0;
		for (int i = 0; i < size; i++) {
			printf("(%d) - received %d\n", rank, sendBuffer[i]);
			result += sendBuffer[i];
		}
		printf("Result is: %d", result);
		draw();
		//loop here
		glfwTerminate();
	}	
	//
	//
	//
	////if (rank == 0) {
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
	//
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow* init() {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	return window;
}

void draw() {
	GLFWwindow* window = init();
	Shader shader("shaders/shader.vs", "shaders/shader.frag");
	
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, // Left  
		0.5f, -0.5f, 0.0f, // Right 
		0.0f,  0.5f, 0.0f  // Top   
	};
	
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	GLint colorLoc = glGetUniformLocation(shader.Program, "inputColor");
	assert(colorLoc > -1);
	glUseProgram(shader.Program);
	glUniform4f(colorLoc, 1.0f, 0.0f, 1.0f, 1.0f);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		shader.Use();
	
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	//return 0;
}