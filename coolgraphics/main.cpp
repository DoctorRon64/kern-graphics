#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Forward Declarations
void processInput(GLFWwindow*& window);
int init(GLFWwindow*& window);

//Create Stuff
void CreateTriangle(unsigned int& VAO, int& size);
void CreateShaders();
void CreateProgram(unsigned int& programId, const char* vertexSrc, const char* fragmentSrc);

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

	// Create viewport
	glViewport(0, 0, screenWidth, screenHeight);

	// Activate the shader program
	glUseProgram(shaderProgram);
	glUniform2f(glGetUniformLocation(shaderProgram, "iResolution"), screenWidth, screenHeight);

	// Game render loop
	while (!glfwWindowShouldClose(window)) {

		// Input handling
		processInput(window);

		// Rendering
		glClearColor(1.0f, 0.0f, 0.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), (float)glfwGetTime());

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
	// Define vertices of the triangle
	float vertices[] = {
		1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
	   -1.0f, -1.0f, 0.0f,
	   -1.0f,  1.0f, 0.0f
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Set size of vertices array
	size = sizeof(vertices);
}

void CreateShaders()
{
	// Create shader program
	CreateProgram(shaderProgram, "shaders/vertex.glsl", "shaders/cineshaderlava.glsl");
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