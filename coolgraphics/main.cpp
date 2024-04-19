#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow*& _window);
int init(GLFWwindow*& window);
void CreateTriangle();
void CreateShaders();

int main()
{
	GLFWwindow* window;
	int setup = init(window);
	if (setup != 0) return setup;

	CreateTriangle();
	CreateShaders();

	//create viewport
	glViewport(0, 0, 800, 600);

	//game render loop
	while (!glfwWindowShouldClose(window)) {
		//input handeling (TODO)

		//rendering
		//background color set & render!
		glClearColor(1.0, 0.5, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

		//events pollen
		glfwPollEvents();
	}
	return 0;
}

void processInput(GLFWwindow*& _window) {
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, true);
	}
}

int init(GLFWwindow*& _window) {
	//Import GLFW major en minor versions
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//main window
	_window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//set context
	glfwMakeContextCurrent(_window);

	//glad spul
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -2;
	}

	return 0;
}

void CreateTriangle() {
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	unsigned int VAO;
	glGenBuffers(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
}

void CreateShaders() {

}