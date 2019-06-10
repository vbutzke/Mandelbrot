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

GLfloat* dummyFunc(GLfloat* pixels);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLFWwindow* init();
void draw(GLfloat* vertices);

const GLuint WIDTH = 800, HEIGHT = 600;

int main(int argc, char **argv)
{
	int rank, size, tag = 0, i = 0;
	MPI_Status status;
	float* buffer = NULL;
	float* data = (float*)malloc(9 * sizeof(float));
	float* imageData = NULL;
	float* pixel = NULL;

	//keep it simple. Try to draw a triangle first
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if (rank == 0) {
		data[0] = -0.5f;
		data[1] = -0.5f;
		data[2] = 0.0f;
		data[3] = 0.5f;
		data[4] = -0.5f;
		data[5] = 0.0f;
		data[6] = 0.0f;
		data[7] = 0.5f;
		data[8] = 0.0f;
		printf("Data built!\n");
	}
	
	buffer = (float*)malloc(3 * sizeof(float)); //size
	printf("(%d) Scattering ! \n", rank);
	MPI_Scatter(data, 3, MPI_FLOAT, buffer, 3, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	printf("Data in buffer: %g, %g, %g \n", buffer[0], buffer[1], buffer[2]);
	//pixel = mandelbrot do buffer
	printf("(%d) Buffer out ! \n", rank);
	
	//compute the mandelbrot for the buffer
	if (rank == 0) {
		imageData = (float*)malloc(9 * sizeof(float));
		printf("(%d) Image here ! \n", rank);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	printf("(%d) Gathering !\n ", rank);
	MPI_Gather(buffer, 3, MPI_FLOAT, imageData, 3, MPI_FLOAT, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("Data in imageData: %g, %g, %g, %g, %g, %g, %g, %g, %g\n",
			imageData[0], imageData[1], imageData[2],
			imageData[3], imageData[4], imageData[5],
			imageData[6], imageData[7], imageData[8]);
		printf("(%d) Go draw yo ! \n", rank);
		
		printf("(%d) Bye baby bye bye ! \n", rank);
	}

	free(buffer);
	free(data);
	free(pixel);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	if (rank == 0) {
		draw(imageData);
		glfwTerminate();
	}
	free(imageData);
	
	return 0;
}

GLfloat* dummyFunc(GLfloat* pixels) {
	return pixels;
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

void draw(GLfloat* vertices) {
	GLFWwindow* window = init();
	Shader shader("shaders/shader.vs", "shaders/shader.frag");
	
	GLfloat vertices2[] = {
		vertices[0], vertices[1], vertices[2], // Left  
		vertices[3], vertices[4], vertices[5], // Right 
		vertices[6], vertices[7], vertices[8]  // Top   
	};
	
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
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