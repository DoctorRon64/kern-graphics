#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Forward Declaration
void processInput(GLFWwindow*& window);
int init(GLFWwindow*& window);

void CreateTriangle(GLuint &triangleVAO, int &size);
void CreateShaders();
void createProgram(GLuint& programID, const char* shaderFiles[]);

//util
void loadFile(const char* filename, char*& output);

//programs ID
GLuint coolProgram;

int main()
{
	GLFWwindow* window;
	int setup = init(window);
	if (setup != 0) return setup;

	GLuint triangleVAO;
	int triangleSize;
	CreateTriangle(triangleVAO, triangleSize);
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

void CreateTriangle(GLuint& vao, int& size) {
	//vertices of a triangle
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	//Vertex Array Object
	glGenBuffers(1, &vao);
	glBindVertexArray(vao);

	//Vertex Buffer Object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW /*Willen we het updaten of static houden*/);
	
	//the position of an vertex is an vertex attribute
	//tells OpenGL how to interpret vertex data for a specific attribute.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	size = sizeof(vertices);
}

void CreateShaders() {
	const char* shaderFiles[] = { "shaders/vertex.shader", "shaders/fragment.shader" };
	createProgram(coolProgram, shaderFiles);
}

void createProgram(GLuint& programID, const char* shaderFiles[]) {

	//Create a GL Program with a Vertex & Fragment Shader
	char* vertexSource;
	char* fragmentSource;

	//load in from shader files
	loadFile(shaderFiles[0], vertexSource);
	loadFile(shaderFiles[1], fragmentSource);

	GLuint vertexShaderID, fragmentShaderID;

	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexSource, nullptr);
	glCompileShader(vertexShaderID);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderID, 512, nullptr, infoLog);
		std::cout << "o no error while compilng vertex shader\n" << infoLog << std::endl;
	}

	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderID, 512, nullptr, infoLog);
		std::cout << "o no error while compilng vertex shader\n" << infoLog << std::endl;
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glGetShaderiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, 512, nullptr, infoLog);
		std::cout << "o no link program something\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	delete vertexSource;
	delete fragmentSource;
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