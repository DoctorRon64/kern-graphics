#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//loading textures
#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"

//glm opensource
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

//Forward Declarations
void processInput(GLFWwindow*& window);
int init(GLFWwindow*& window, int width, int height);

//Create Stuffa
void CreateShaders();
void CreateProgram(unsigned int& programId, const char* vertexSrc, const char* fragmentSrc);
void CreateGeometry(unsigned int& VAO, unsigned int& EBO, int& size, int& numIndices);
unsigned int loadTexture(const char* path);
void RenderCube(unsigned int boxNormalMap, unsigned int boxTexture, glm::mat4 world);
void RenderSkybox(glm::mat4 world);
void RenderTerrain(glm::mat4 world);
unsigned int GeneratePlane(const char* heightmap, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID);

//util
glm::vec3 getRandCol();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xpos, double ypos);
void loadFile(const char* fileName, char*& output);
void InfoShaderLog(unsigned int shaderId, int success, char* infoLog);

/////////////////////////////////////////////////////////////////////////
//Variables
/////////////////////////////////////////////////////////////////////////

// Program ID
unsigned int shaderProgram, skyProgram, terrainProgram;
unsigned int screenWidth = 1200;
unsigned int screenHeight = 800;

glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -0.5f, -0.5f));
glm::vec3 camPos = glm::vec3(100.0f, 12.5f, -5.0f);
glm::mat4 view;
glm::mat4 projection;

unsigned int boxVAO, boxEBO;
int boxSize, boxIndexCount;
static float FOV = 80.0f;
float degrees = 45.0f;
glm::vec3 boxTransformSize = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 boxTrans = glm::vec3(0.0f, 0.0f, 0.0f);

float lastX, lastY;
bool firstMouse = true;
float camYaw, camPitch;
glm::quat camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0));
bool keys[1024];

//Terrain Data
unsigned int terrainVAO, terrainIndexCount, HeightMapId;

int main()
{
	GLFWwindow* window;
	int setup = init(window, screenWidth, screenHeight);
	if (setup != 0) return setup;

	CreateShaders();
	CreateGeometry(boxVAO, boxEBO, boxSize, boxIndexCount);

	// Create viewport
	glViewport(0, 0, screenWidth, screenHeight);

	//Matrices!
	unsigned int boxTexure = loadTexture("textures/cardbox.jpg");
	unsigned int boxNormalMap = loadTexture("textures/cardbox_normal.png");
	terrainVAO = GeneratePlane("textures/heightmap.jpg", GL_RGBA, 4, 100.0f, 5.0f, terrainIndexCount, HeightMapId);

	view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	projection = glm::perspective(glm::radians(FOV), screenWidth / (float)screenHeight, 0.1f, 1000.0f);

	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, camPos);
	world = glm::scale(world, boxTransformSize);
	world = glm::rotate(world, glm::radians(degrees), glm::vec3(0, 1, 0));

	// Game render loop
	while (!glfwWindowShouldClose(window)) {
		// Input handling
		processInput(window);

		RenderSkybox(world);
		RenderTerrain(world);
		RenderCube(boxNormalMap, boxTexure, world);


		// Rendering
		glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void RenderSkybox(glm::mat4 world) {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH);

	glUseProgram(skyProgram);

	//Building World Matrix up
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(skyProgram, "lightDir"), 1, glm::value_ptr(lightDir));
	glUniform3fv(glGetUniformLocation(skyProgram, "camPos"), 1, glm::value_ptr(camPos));

	// Bind vertex array and draw
	glBindVertexArray(boxVAO);
	glDrawElements(GL_TRIANGLES, boxIndexCount, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH);
}

void RenderTerrain(glm::mat4 world) {
	glEnable(GL_DEPTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(terrainProgram);

	glm::mat4 world2 = glm::mat4(1.0);

	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(terrainProgram, "lightDir"), 1, glm::value_ptr(lightDir));
	glUniform3fv(glGetUniformLocation(terrainProgram, "camPos"), 1, glm::value_ptr(camPos));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, HeightMapId);

	// Bind vertex array and draw
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);

	glDisable(GL_DEPTH);
	glDisable(GL_CULL_FACE);
}

void RenderCube(unsigned int boxNormalMap, unsigned int boxTexture, glm::mat4 world) {
	//set texture channels
	glUseProgram(shaderProgram);

	//set channels world
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
	glUniform3fv(glGetUniformLocation(shaderProgram, "camPos"), 1, glm::value_ptr(camPos));

	glUniform1i(glGetUniformLocation(shaderProgram, "mainTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, boxNormalMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTexture);
}

unsigned int GeneratePlane(const char* heightmap, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID) {
	int width, height, channels;
	unsigned char* data = nullptr;
	if (heightmap != nullptr) {
		data = stbi_load(heightmap, &width, &height, &channels, comp);
		if (data) {
			glGenTextures(1, &heightmapID);
			glBindTexture(GL_TEXTURE_2D, heightmapID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	int stride = 8;
	float* vertices = new float[(width * height) * stride];
	unsigned int* indices = new unsigned int[(width - 1) * (height - 1) * 6];

	int index = 0;
	for (int i = 0; i < (width * height); i++) {

		int x = i % width;
		int z = i / width;

		vertices[index++] = x * xzScale;
		vertices[index++] = 0;
		vertices[index++] = z * xzScale;
		
		vertices[index++] = 0;
		vertices[index++] = 1;
		vertices[index++] = 0;
		
		vertices[index++] = x / (float)width;
		vertices[index++] = z / (float)height;
	}

	index = 0;
	for (int i = 0; i < (width - 1) * (height - 1); i++) {
		int x = i % (width - 1);
		int z = i / (width - 1); //widht - 1

		int vertex = z * width + x;

		indices[index++] = vertex;
		indices[index++] = vertex + width;
		indices[index++] = vertex + width + 1;

		indices[index++] = vertex;
		indices[index++] = vertex + width + 1;
		indices[index++] = vertex + 1;
	}

	unsigned int vertSize = (width * height) * stride * sizeof(float);
	indexCount = ((width - 1) * (height - 1) * 6);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// vertex information!
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, 0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	// uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	delete[] vertices;
	delete[] indices;

	stbi_image_free(data);

	return VAO;
}

void processInput(GLFWwindow*& window)
{
	// Exit the application if ESC key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	bool camChanged = false;

	if (keys[GLFW_KEY_W]) {
		camPos += camQuat * glm::vec3(0, 0, 1);
		camChanged = true;
	}
	if (keys[GLFW_KEY_S]) {
		camPos += camQuat * glm::vec3(0, 0, -1);
		camChanged = true;
	}
	if (keys[GLFW_KEY_A]) {
		camPos += camQuat * glm::vec3(1, 0, 0);
		camChanged = true;
	}
	if (keys[GLFW_KEY_D]) {
		camPos += camQuat * glm::vec3(-1, 0, 0);
		camChanged = true;
	}

	if (camChanged) {
		glm::vec3 camFor = camQuat * glm::vec3(0, 0, 1);
		glm::vec3 camUp = camQuat * glm::vec3(0, 1, 0);
		view = glm::lookAt(camPos, camPos + camFor, camUp);
	}
}

void CreateGeometry(unsigned int& VAO, unsigned int& EBO, int& size, int& numIndices)
{
	// need 24 vertices for normal/uv-mapped Cube
	float vertices[] = {
		// positions            //colors            // tex coords   // normals          //tangents      //bitangents
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f
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

	int stride = (3 + 3 + 2 + 3 + 3 + 3) * sizeof(float);
	//3 position // 3 colors //2 tex coords //3 normals
	size = sizeof(vertices) / stride;
	numIndices = sizeof(indices) / sizeof(int);

	// Generate vertex array object (VAO)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Generate vertex buffer object (VBO)
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Generate element buffer object (EBO)
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	//color to shader
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//uv to shader
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//normal to shader
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, stride, (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	//tangents to shader
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_TRUE, stride, (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

	//bitangents to shader
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_TRUE, stride, (void*)(14 * sizeof(float)));
	glEnableVertexAttribArray(5);
}

void CreateShaders()
{
	// Create shader program
	CreateProgram(shaderProgram, "shaders/defaultvertex.glsl", "shaders/defaultfragment.glsl");
	CreateProgram(skyProgram, "shaders/skyvertex.glsl", "shaders/skyfrag.glsl");
	CreateProgram(terrainProgram, "shaders/terrainvertex.glsl", "shaders/terrainfrag.glsl");
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

int init(GLFWwindow*& window, int width, int height) {

	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create main window
	window = glfwCreateWindow(width, height, "My Cool Shaders", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set current context
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float x = (float)xpos;
	float y = (float)ypos;

	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float dx = x - lastX;
	float dy = y - lastY;
	lastX = x;
	lastY = y;

	camYaw -= dx;
	camPitch = glm::clamp(camPitch + dy, -90.f, 90.f);
	if (camYaw > 180.0f) camYaw -= 360.0f;
	if (camYaw < -180.0f) camYaw += 360.0f;

	camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0)); //big oof
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	FOV -= static_cast<float>(yoffset);
	FOV = glm::clamp(FOV, 1.0f, 90.0f);
	projection = glm::perspective(glm::radians(FOV), screenWidth / (float)screenHeight, 0.1f, 100.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (action == GLFW_PRESS) {
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = false;
	}
}

glm::vec3 getRandCol() {
	// Usage
	glm::vec3 rainbowColors[] = {
		glm::vec3(1.0f, 0.0f, 0.0f),   // Red
		glm::vec3(1.0f, 0.5f, 0.0f),   // Orange
		glm::vec3(1.0f, 1.0f, 0.0f),   // Yellow
		glm::vec3(0.0f, 1.0f, 0.0f),   // Green
		glm::vec3(0.0f, 0.0f, 1.0f),   // Blue
		glm::vec3(0.75f, 0.0f, 0.75f)  // Purple
	};
	int numColors = sizeof(rainbowColors) / sizeof(rainbowColors[0]);

	double time = glfwGetTime();
	double colorIndex = fmod(time, numColors);
	int colorIndex1 = static_cast<int>(colorIndex) % numColors;
	int colorIndex2 = (colorIndex1 + 1) % numColors;
	double factor = colorIndex - floor(colorIndex);
	glm::vec3 interpolatedColor = glm::mix(rainbowColors[colorIndex1], rainbowColors[colorIndex2], factor);
	return interpolatedColor;
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
		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
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