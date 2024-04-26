#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//loading textures
#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"

//glm opensource
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Forward Declarations
void processInput(GLFWwindow*& window);
int init(GLFWwindow*& window);

//Create Stuff
void CreateShaders();
void CreateProgram(unsigned int& programId, const char* vertexSrc, const char* fragmentSrc);
void CreateTriangle(unsigned int& VAO, int& size);
unsigned int loadTexture(const char* path);

//util
void loadFile(const char* fileName, char*& output);
void InfoShaderLog(unsigned int shaderId, int success, char* infoLog);

// Program ID
unsigned int shaderProgram;
unsigned int screenWidth = 800;
unsigned int screenHeight = 600;

int main()
{
	GLFWwindow* window;
	int setup = init(window);
	if (setup != 0) return setup;

	unsigned int triangleVAO;
	int triangleSize;
	CreateTriangle(triangleVAO, triangleSize);
	CreateShaders();

	unsigned int boxTex = loadTexture("textures/cardbox.jpg");

	// Create viewport
	glViewport(0, 0, screenWidth, screenHeight);

	// Activate the shader program
	glUseProgram(shaderProgram);

	// Game render loop
	while (!glfwWindowShouldClose(window)) {

		// Input handling
		processInput(window);

		// Rendering
		glClearColor(1.0f, 0.0f, 0.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// Bind vertex array and draw
		glBindVertexArray(triangleVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}

void processInput(GLFWwindow*& window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int init(GLFWwindow*& window) {

	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create main window
	window = glfwCreateWindow(800, 600, "My Cool Shaders", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set current context
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -2;
	}

	return 0;
}

void CreateTriangle(unsigned int& VAO, int& size)
{
	// need 24 vertices for normal/uv-mapped Cube
	float vertices[] = {
		// positions            //colors            // tex coords   // normals
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       1.f, 0.f, 0.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       1.f, 0.f, 0.f,

		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 2.f,       0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 2.f,       0.f, 0.f, 1.f,

		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   -1.f, 1.f,      -1.f, 0.f, 0.f,
		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   -1.f, 0.f,      -1.f, 0.f, 0.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, -1.f,      0.f, 0.f, -1.f,
		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, -1.f,      0.f, 0.f, -1.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   3.f, 0.f,       0.f, 1.f, 0.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   3.f, 1.f,       0.f, 1.f, 0.f,

		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,

		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       -1.f, 0.f, 0.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       -1.f, 0.f, 0.f,

		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,

		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   2.f, 0.f,       0.f, 1.f, 0.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   2.f, 1.f,       0.f, 1.f, 0.f
	};

	unsigned int indices[] = {  // note that we start from 0!
		// DOWN
		0, 1, 2,   // first triangle
		0, 2, 3,    // second triangle
		// BACK
		14, 6, 7,   // first triangle
		14, 7, 15,    // second triangle
		// RIGHT
		20, 4, 5,   // first triangle
		20, 5, 21,    // second triangle
		// LEFT
		16, 8, 9,   // first triangle
		16, 9, 17,    // second triangle
		// FRONT
		18, 10, 11,   // first triangle
		18, 11, 19,    // second triangle
		// UP
		22, 12, 13,   // first triangle
		22, 13, 23,    // second triangle
	};

	int stride = (3 + 3 + 2 + 3) * sizeof(float);
	//3 position // 3 colors //2 tex coords //3 normals
	size = sizeof(vertices) / stride;

	// Generate vertex array object (VAO)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Generate vertex buffer object (VBO)
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Generate element buffer object (EBO)
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// Set size of vertices array
}

void CreateShaders()
{
	// Create shader program
	CreateProgram(shaderProgram, "shaders/vertex.glsl", "shaders/fragment.glsl");
}

void CreateProgram(unsigned int& programId, const char* vertex, const char* fragment) {

	char* vertexSrc;
	char* fragmentSrc;
	loadFile(vertex, vertexSrc);
	loadFile(fragment, fragmentSrc);

	unsigned int vertexShaderId, fragmentShaderId;

	// Create vertex shader
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexSrc, nullptr);
	glCompileShader(vertexShaderId);

	int success;
	char infoLog[512];
	success = 0;

	InfoShaderLog(vertexShaderId, success, infoLog);

	// Create fragment shader
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentSrc, nullptr);
	glCompileShader(fragmentShaderId);

	InfoShaderLog(fragmentShaderId, success, infoLog);

	// Create shader program
	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	glGetProgramiv(programId, GL_LINK_STATUS, &success);


	if (!success) {
		glGetProgramInfoLog(programId, 512, nullptr, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete shader objects
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	// Free allocated memory for shader source code
	delete vertexSrc;
	delete fragmentSrc;
}

void InfoShaderLog(unsigned int shaderId, int success, char* infoLog) {
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void loadFile(const char* fileName, char*& output)
{
	std::ifstream file(fileName, std::ios::binary);
	if (file.is_open()) {
		file.seekg(0, file.end);
		std::streamoff length = file.tellg();
		file.seekg(0, file.beg);

		output = new char[length + 1];
		file.read(output, length);
		output[length] = '\0';
		file.close();
	}
	else {
		output = nullptr;
	}
}

unsigned int loadTexture(const char* path) 
{
	//Gen & Bind texture Id
	unsigned int textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	//set Texture Paramters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//LoadTexture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		std::cout << "Error loading texture: " << path << std::endl;
	}
	
	//Set data
	stbi_image_free(data);

	//unload texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureId;
}