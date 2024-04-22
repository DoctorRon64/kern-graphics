#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Forward Declaration
void processInput(GLFWwindow*& window);
int init(GLFWwindow*& window);

void CreateTriangle(unsigned int& vao, int& size, unsigned int& shaderProgram);
void CreateShaders();
void CreateProgram(unsigned int& programID, const char* shaderFiles[]);

//util
bool CheckShaderCompilation(unsigned int shaderID, const char* shaderType);
bool CheckProgramLinking(unsigned int programID);
void logError(const std::string& errorMessage);
void loadFile(const char* filename, char*& output);

//programs ID
unsigned int coolProgram;

int main()
{
	GLFWwindow* window;
	int setup = init(window);
	if (setup != 0) return setup;

	unsigned int triangleVAO;
	int triangleSize;
	CreateTriangle(triangleVAO, triangleSize, coolProgram);
	CreateShaders();

	//create viewport
	glViewport(0, 0, 800, 600);

	//game render loop
	while (!glfwWindowShouldClose(window)) {

		//Input
		processInput(window);

		//rendering
		glClearColor(1.0f, 0.0f, 0.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(coolProgram);

		//what object do I want to draw
		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, triangleSize);

		//Swap & Poll
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

	//Import GLFW major en minor versions
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//main window
	window = glfwCreateWindow(800, 600, "My Cool Shaders", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//set context
	glfwMakeContextCurrent(window);

	//glad stuff
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -2;
	}

	return 0;
}

void CreateTriangle(unsigned int& VAO, int& size, unsigned int& shaderProgram) {
	//vertices of a triangle
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	//Vertex Array Object
	glGenBuffers(1, &VAO);
	glBindVertexArray(VAO);

	//Vertex Buffer Object
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 4. draw the object
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//someOpenGLFunctionThatDrawsOurTriangle();

	size = sizeof(vertices);
}

void CreateShaders() {
	const char* shaderFiles[] = { "shaders/vertex.glsl", "shaders/fragment.glsl" };
	CreateProgram(coolProgram, shaderFiles);
}

void CreateProgram(unsigned int& shaderProgram, const char* shaderFiles[])
{
	// vertex Shader
	char* vertexSource;
	loadFile(shaderFiles[0], vertexSource);

	unsigned int vertexShaderID;
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexSource, NULL);
	glCompileShader(vertexShaderID);

	if (!CheckShaderCompilation(vertexShaderID, "VERTEX"))
		return;

	// fragment Shader
	char* fragmentSource;
	loadFile(shaderFiles[1], fragmentSource);

	unsigned int fragmentShaderID;
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShaderID);

	if (!CheckShaderCompilation(fragmentShaderID, "FRAGMENT"))
		return;

	// Create Program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderID);
	glAttachShader(shaderProgram, fragmentShaderID);
	glLinkProgram(shaderProgram);

	if (!CheckProgramLinking(shaderProgram))
		return;

	glUseProgram(shaderProgram);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	delete[] vertexSource;
	delete[] fragmentSource;
}

bool CheckShaderCompilation(unsigned int shaderID, const char* shaderType)
{
	int  success;
	char infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		logError("ERROR::SHADER::" + std::string(shaderType) + "::COMPILATION_FAILED\n" + infoLog);
		return false;
	}
	return true;
}

bool CheckProgramLinking(unsigned int programID)
{
	int success;
	char infoLog[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 512, nullptr, infoLog);
		logError("ERROR::PROGRAM::LINKING_FAILED\n" + std::string(infoLog));
		return false;
	}

	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 512, nullptr, infoLog);
		logError("ERROR::PROGRAM::VALIDATION_FAILED\n" + std::string(infoLog));
		return false;
	}

	return true;
}

void logError(const std::string& errorMessage)
{
	std::cerr << errorMessage << std::endl;
}

void loadFile(const char* filename, char*& output) {
	//open file
	std::ifstream file(filename, std::ios::binary);

	//if the file was opend good
	if (file.is_open()) {
		//get length of file
		file.seekg(0, file.end);
		std::streamoff fileLength = file.tellg();
		file.seekg(0, file.beg);

		//allocate memory for the char pointer
		output = new char[fileLength + 1];

		//read data as a block
		file.read(output, fileLength);

		//add null terminator to end of char pointer
		output[fileLength] = '\0';

		//close the file
		file.close();
	}
	else {
		//if the file could not open make the charpointer NULL
		output = NULL;
	}
}