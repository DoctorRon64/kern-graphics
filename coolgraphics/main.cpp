#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "util/model.h"
#include "util/mesh.h"

//loading textures models and meshes
#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"

//glm opensource
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

struct FrameBuffer {
	unsigned int Id = 0;
	unsigned int color1 = 0, color2 = 0, depth = 0;

	FrameBuffer() {}
};

FrameBuffer screenBuffer;

//Forward Declarations
void processInput(GLFWwindow*& window, float deltaTime);
int init(GLFWwindow*& window, int width, int height);

//Create Stuff
void CreateShader(unsigned int& programId, const char* vertex, const char* fragment);
void CreateGeometry(unsigned int& VAO, unsigned int& EBO, int& size, int& numIndices);
unsigned int loadTexture(const char* path, int comp = 0);
void RenderModel(Model* model, glm::mat4 view, glm::mat4 projection, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int clipDir = 0);
void RenderCube(glm::mat4 view, glm::mat4 projection);
void RenderSkybox(glm::mat4 view, glm::mat4 projection);
void RenderTerrain(glm::mat4 view, glm::mat4 projection, glm::vec3 pos ,int clipDir = 0);
unsigned int GeneratePlane(const char* heightmap, GLenum format, int comp, unsigned char*& data, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID);
void setupRescources();

//postFX
void createFrameBuffer(int width, int height, unsigned int& frameBufferId, unsigned int& colorBufferId, unsigned int& depthBufferId);
void createSceneBuffer(int width, int height, unsigned int& frameBufferId, unsigned int& colorBufferId, unsigned int& colorBufferId2, unsigned int& depthBufferId);
void renderToBuffer(FrameBuffer To, FrameBuffer From, unsigned int shader);
void renderQuad();

//waterRendering
void renderInvertedScene(glm::mat4 projection, FrameBuffer targetBuffer);
void renderWaterPlane(glm::mat4 projection, glm::mat4 view, FrameBuffer target, FrameBuffer InvertSource);

//util
void CalculateDeltaTime();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void loadFile(const char* fileName, char*& output);
void InfoShaderLog(unsigned int shaderId, int success, char* infoLog);

/////////////////////////////////////////////////////////////////////////
//Variables
/////////////////////////////////////////////////////////////////////////

// Program ID
unsigned int shaderProgram, skyProgram, terrainProgram, chromabbProgram, blitProgram, waterProgram, modelProgram;
unsigned int screenWidth = 1200;
unsigned int screenHeight = 800;

glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -0.5f, -0.5f));
glm::vec3 camPos = glm::vec3(900, 500, 900);
glm::vec3 camUp;
glm::vec3 camFor;

//boxData
unsigned int boxVAO, boxEBO, boxTextureId, boxNormalId;
int boxSize, boxIndexCount;
static float FOV = 65.0f;

// time 
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float lastX, lastY;
float camYaw, camPitch;
glm::quat camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0));
bool keys[1024];

//Terrain Data
unsigned int terrainVAO, terrainIndexCount, heightMapId, heightNormalId;
unsigned char* heightMapTexture;
unsigned int dirt, sand, grass, rock, snow;
float terrainHeight = 100.0f;
float terrainWidth = 5.0f;

Model* Guitar;
Model* aaronHead;
Model* boat;
Model* lighthouse;
Model* japantemple;
Model* japanman;

unsigned int defaultAttach[1] = { GL_COLOR_ATTACHMENT0 };
unsigned int sceneAttach[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

float waterHeight = 25.0f;
unsigned int waterNormalId;

/// <summary>////////////////
/// Functions/
/// </summary>////////////////

int main()
{
	GLFWwindow* window;
	int setup = init(window, screenWidth, screenHeight);
	if (setup != 0) return setup;

	// Create viewport
	glViewport(0, 0, screenWidth, screenHeight);

	setupRescources();

	/*chromatic abberation*/ 
	FrameBuffer PostProcess1, PostProcess2, scene;
	createFrameBuffer(screenWidth, screenHeight, PostProcess1.Id, PostProcess1.color1, PostProcess1.depth);
	createFrameBuffer(screenWidth, screenHeight, PostProcess2.Id, PostProcess2.color1, PostProcess2.depth);
	createSceneBuffer(screenWidth, screenHeight, scene.Id, scene.color1, scene.color2, scene.depth);

	glm::mat4 view;
	glm::mat4 projection;
	view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	projection = glm::perspective(glm::radians(FOV), screenWidth / (float)screenHeight, 0.1f, 5000.0f);

	// Game render loop
	while (!glfwWindowShouldClose(window)) {
		CalculateDeltaTime();
		processInput(window, deltaTime);
		view = glm::lookAt(camPos, camPos + camFor, camUp);

		//render scene
		glBindFramebuffer(GL_FRAMEBUFFER, scene.Id);
		glDrawBuffers(2, sceneAttach);
			glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderSkybox(view, projection);
			RenderTerrain(view, projection, glm::vec3(0,0,0));
			
			RenderModel(aaronHead, view, projection, glm::vec3(0, 500, 1000), glm::vec3(0, glfwGetTime() * 2,0), glm::vec3(200,200,200));
			RenderModel(boat, view, projection, glm::vec3(1300, 10.5, 200), glm::vec3(0,-90,0), glm::vec3(.2, .2, .2));
			RenderModel(lighthouse, view, projection, glm::vec3(1050, 30, 290), glm::vec3(0, 90, 0), glm::vec3(3, 3, 3));
			RenderModel(japantemple, view, projection, glm::vec3(1000, 55, 1250), glm::vec3(0, -180, 0), glm::vec3(10, 10, 10));
			RenderModel(japanman, view, projection, glm::vec3(1050, 60, 1050), glm::vec3(0, 0, 0), glm::vec3(5, 5, 5));

			RenderCube(view, projection);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		renderInvertedScene(projection ,PostProcess1);
		renderWaterPlane(projection, view, scene, PostProcess1);

		//post processing
		renderToBuffer(PostProcess1, scene, chromabbProgram);
		
		//blit to screen
		renderToBuffer(screenBuffer, PostProcess1, blitProgram);

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void setupRescources() {
	//texture geometry stuff
	boxTextureId = loadTexture("textures/cardbox.jpg");
	boxNormalId = loadTexture("textures/cardbox_normal.png");
	CreateGeometry(boxVAO, boxEBO, boxSize, boxIndexCount);

	terrainVAO = GeneratePlane("textures/terrainheightmap.png", GL_RGBA, 4, heightMapTexture, 100.0f, 5.0f, terrainIndexCount, heightMapId);
	heightNormalId = loadTexture("textures/terrain_normalmap.png");

	waterNormalId = loadTexture("textures/water_normalmap.jpg");

	dirt = loadTexture("textures/terrain/dirt.jpg");
	sand = loadTexture("textures/terrain/sand.jpg");
	grass = loadTexture("textures/terrain/grass.png", 4);
	rock = loadTexture("textures/terrain/rock.jpg");
	snow = loadTexture("textures/terrain/snow.jpg");

	aaronHead = new Model("models/aaron/aaron_model.obj");
	boat = new Model("models/boat/Boat.obj");
	lighthouse = new Model("models/lighthouse/LightHouse.obj");
	Guitar = new Model("models/backpack/backpack.obj");
	japantemple = new Model("models/japanese_temple/Japanese_Temple.obj");
	japanman = new Model("models/man/man.obj");
	
	CreateShader(shaderProgram, "shaders/defaultvertex.glsl", "shaders/defaultfragment.glsl");
	CreateShader(skyProgram, "shaders/skyvertex.glsl", "shaders/skyfrag.glsl");
	CreateShader(terrainProgram, "shaders/terrainvertex.glsl", "shaders/terrainfrag.glsl");
	CreateShader(chromabbProgram, "shaders/pp/chrabb_vert.glsl", "shaders/pp/chrabb_frag.glsl");
	CreateShader(blitProgram, "shaders/pp/img_vert.glsl", "shaders/pp/img_frag.glsl");
	CreateShader(modelProgram, "shaders/modelvert.glsl", "shaders/modelfrag.glsl");
	CreateShader(waterProgram, "shaders/waterVert.glsl", "shaders/waterFrag.glsl");

	glUseProgram(modelProgram);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_diffuse1"), 0);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_specular1"), 1);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_normal1"), 2);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_roughness1"), 3);
	glUniform1i(glGetUniformLocation(modelProgram, "texture_ao1"), 4);
	
	glUseProgram(blitProgram);
	glUseProgram(chromabbProgram);
}

void renderInvertedScene(glm::mat4 projection, FrameBuffer targetBuffer)
{
	//create inverted viewMatrix
	glm::vec3 invertPos = camPos;
	invertPos.y =  -invertPos.y + waterHeight * 2;
	glm::vec3 invertUp = glm::reflect(camUp, glm::vec3(0, 1, 0));
	glm::vec3 invertTarget = camPos + camFor;
	invertTarget.y = -invertTarget.y + waterHeight * 2;

	glm::mat4 invertView = glm::lookAt(invertPos, invertTarget, invertUp);

	glBindFramebuffer(GL_FRAMEBUFFER, targetBuffer.Id);
	glDrawBuffers(1, defaultAttach);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 temp = camPos;
	camPos = invertPos;

	int clipdir = -1;

	RenderSkybox(invertView, projection);
	RenderModel(boat, invertView, projection, glm::vec3(1300, 10.5, 200), glm::vec3(0, -90, 0), glm::vec3(.2, .2, .2), clipdir);
	RenderModel(lighthouse, invertView, projection, glm::vec3(1050, 30, 290), glm::vec3(0, 90, 0), glm::vec3(3, 3, 3), clipdir);

	RenderTerrain(invertView, projection, glm::vec3(0, 0, 0), clipdir);

	camPos = temp;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// <summary>
/// RENDERING STUFFFFFFFFFFFFF
/// </summary>
void RenderModel(Model* model, glm::mat4 view, glm::mat4 projection, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int clipDir)
{
	//glEnable(GL_BLEND);
	//alpha
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//additive blend
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
	//multiply blend
	//glBlendFunc(GL_DST_COLOR, GL_ZERO);

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(modelProgram);

	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, pos);
	world = glm::scale(world, scale);
	world = world * glm::toMat4(glm::quat(rot));

	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(modelProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform1f(glGetUniformLocation(modelProgram, "waterHeight"), waterHeight);
	glUniform1i(glGetUniformLocation(modelProgram, "clipDir"), clipDir);

	glUniform3fv(glGetUniformLocation(modelProgram, "lightDir"), 1, glm::value_ptr(lightDir));
	glUniform3fv(glGetUniformLocation(modelProgram, "camPos"), 1, glm::value_ptr(camPos));

	model->Draw(modelProgram);

	//glDisable(GL_BLEND);
}

void RenderSkybox(glm::mat4 view, glm::mat4 projection) {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH);

	glUseProgram(skyProgram);

	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, camPos);
	world = glm::scale(world, glm::vec3(100, 100, 100));

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

void RenderTerrain(glm::mat4 view, glm::mat4 projection,glm::vec3 pos ,int clipDir) {
	glUseProgram(terrainProgram);

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, pos);

	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	double time = glfwGetTime();
	lightDir = glm::normalize(glm::vec3(glm::sin(time / 10), -.5, glm::cos(time / 10)));
	glUniform3fv(glGetUniformLocation(terrainProgram, "lightDir"), 1, glm::value_ptr(lightDir));
	glUniform3fv(glGetUniformLocation(terrainProgram, "camPos"), 1, glm::value_ptr(camPos));
	glUniform1f(glGetUniformLocation(terrainProgram, "waterHeight"), waterHeight);
	glUniform1i(glGetUniformLocation(terrainProgram, "clipDir"), clipDir);

	glUniform1i(glGetUniformLocation(terrainProgram, "mainTexture"), 0);
	glUniform1i(glGetUniformLocation(terrainProgram, "normalMap"), 1);

	glUniform1i(glGetUniformLocation(terrainProgram, "dirt"), 2);
	glUniform1i(glGetUniformLocation(terrainProgram, "sand"), 3);
	glUniform1i(glGetUniformLocation(terrainProgram, "grass"), 4);
	glUniform1i(glGetUniformLocation(terrainProgram, "rock"), 5);
	glUniform1i(glGetUniformLocation(terrainProgram, "snow"), 6);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMapId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightNormalId);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dirt);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sand);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, grass);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rock);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, snow);

	// Bind vertex array and draw
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);

	glDisable(GL_DEPTH);
	glDisable(GL_CULL_FACE);
}

void renderWaterPlane(glm::mat4 projection, glm::mat4 view, FrameBuffer scene, FrameBuffer invert)
{
	glUseProgram(waterProgram);

	//buffer
	glBindFramebuffer(GL_FRAMEBUFFER, scene.Id);
	glDrawBuffers(2, sceneAttach);

	//worldmatrix opbouwen
	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, glm::vec3(0, waterHeight , 0));

	//settings
	glUniformMatrix4fv(glGetUniformLocation(waterProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(waterProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(waterProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3fv(glGetUniformLocation(waterProgram, "lightDir"), 1, glm::value_ptr(lightDir));
	glUniform3fv(glGetUniformLocation(waterProgram, "camPos"), 1, glm::value_ptr(camPos));
	glUniform1f(glGetUniformLocation(waterProgram, "waterHeight"), waterHeight);
	glUniform1f(glGetUniformLocation(waterProgram, "time"), (float)glfwGetTime());

	glUniform1i(glGetUniformLocation(waterProgram, "color"), 0);
	glUniform1i(glGetUniformLocation(waterProgram, "depth"), 1);
	glUniform1i(glGetUniformLocation(waterProgram, "invert"), 2);
	glUniform1i(glGetUniformLocation(waterProgram, "normalMap"), 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene.color1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, scene.color2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, invert.color1);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, waterNormalId);

	//renderplane
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderCube(glm::mat4 view, glm::mat4 projection) {
	//set texture channels
	glEnable(GL_DEPTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(shaderProgram);

	glm::vec3 size = glm::vec3(10.0f, 10.0f, 10.0f);
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::mat4 world = glm::mat4(1.0f);
	float degrees = 0;

	world = glm::translate(world, position);
	world = glm::scale(world, size);
	world = glm::rotate(world, glm::radians(degrees), glm::vec3(0, 1, 0));

	//set channels world
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
	glUniform3fv(glGetUniformLocation(shaderProgram, "camPos"), 1, glm::value_ptr(camPos));

	glUniform1i(glGetUniformLocation(shaderProgram, "mainTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, boxNormalId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTextureId);

	glBindVertexArray(boxVAO);
	glDrawElements(GL_TRIANGLES, boxIndexCount, GL_UNSIGNED_INT, 0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH);
}

unsigned int GeneratePlane(const char* heightmap, GLenum format, int comp, unsigned char*& data, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID) {
	int width = 0, height = 0, channels = 0;
	data = nullptr;

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
		else {
			std::cerr << "Failed to load heightmap: " << stbi_failure_reason() << std::endl;
			return 0;
		}
	}
	else {
		std::cerr << "Heightmap is null." << std::endl;
		return 0;
	}

	int stride = 8;
	float* vertices = new float[(width * height) * stride];
	unsigned int* indices = new unsigned int[(width - 1) * (height - 1) * 6];

	int index = 0;
	for (int i = 0; i < (width * height); i++) {

		int x = i % width;
		int z = i / width;

		float texHeight = (float)data[i * comp];

		vertices[index++] = static_cast<float>(x) * xzScale;
		vertices[index++] = (texHeight / 255.0f) * hScale;
		vertices[index++] = static_cast<float>(z) * xzScale;

		vertices[index++] = 0;
		vertices[index++] = 1;
		vertices[index++] = 0;

		vertices[index++] = static_cast<float>(x) / (float)width;
		vertices[index++] = static_cast<float>(z) / (float)height;
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
	indexCount = static_cast<unsigned int>((width - 1) * (height - 1) * 6);

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

	//stbi_image_free(data);

	return VAO;
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

void CreateShader(unsigned int& programId, const char* vertex, const char* fragment) {

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
	window = glfwCreateWindow(width, height, "Godot", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Set current context
	glfwSetCursorPosCallback(window, mouse_callback);
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

void CalculateDeltaTime() {
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void processInput(GLFWwindow*& window, float deltaTime)
{
	// Exit the application if ESC key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	static double cursorX = -1, cursorY = -1, lastCursorX, lastCursorY;
	static float Yaw, Pitch;
	static bool firstMouse = true;
	static float sensitivity = 100.0f * deltaTime;
	static float camYaw = 0.0f, camPitch = 0.0f;

	if (cursorX == -1) {
		glfwGetCursorPos(window, &cursorX, &cursorY);
	}
	lastCursorX = cursorX;
	lastCursorY = cursorY;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	glm::vec2 mouseDelta(cursorX - lastCursorX, cursorY - lastCursorY);

	Yaw -= mouseDelta.x * sensitivity;
	Pitch += mouseDelta.y * sensitivity;

	if (Pitch < -90.0f) Pitch = -90.0f;
	else if (Pitch > 90.0f) Pitch = 90.0f;
	if (Yaw < -180.0f) Yaw += 360.0f;
	else if (Yaw > 180.0f) Yaw -= 360.0f;

	camYaw -= mouseDelta.x;
	camPitch = glm::clamp(camPitch + mouseDelta.y, -90.f, 90.f);
	if (camYaw > 180.0f) camYaw -= 360.0f;
	if (camYaw < -180.0f) camYaw += 360.0f;

	glm::vec3 euler(glm::radians(camPitch), glm::radians(camYaw), 0);
	glm::quat q(euler);
	glm::vec3 translation(0, 0, 0);

	float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 300.0f : 100.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) translation.z += speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) translation.z -= speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) translation.x += speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) translation.x -= speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) translation.y += speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) translation.y -= speed * deltaTime;

	camPos += q * translation;
	camUp = q * glm::vec3(0, 1, 0);
	camFor = q * glm::vec3(0, 0, 1);
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

	glm::vec3 camFor = camQuat * glm::vec3(0, 0, 1);
	glm::vec3 camUp = camQuat * glm::vec3(0, 1, 0);
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

unsigned int loadTexture(const char* path, int comp)
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
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, comp);
	if (data) {
		if (comp != 0) { nrChannels = comp; }

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

void createFrameBuffer(int width, int height, unsigned int& frameBufferId, unsigned int& colorBufferId, unsigned int& depthBufferId) {
	//generate frame buffer
	glGenFramebuffers(1, &frameBufferId);

	//generate color buffer
	glGenTextures(1, &colorBufferId);
	glBindTexture(GL_TEXTURE_2D, colorBufferId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	//generate depthbuffer
	glGenRenderbuffers(1, &depthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	//binding buffers
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferId, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "framebuffer not found" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void createSceneBuffer(int width, int height, unsigned int& frameBufferId, unsigned int& colorBufferId, unsigned int& colorBufferId2, unsigned int& depthBufferId) {
	// generate frame buffer
	glGenFramebuffers(1, &frameBufferId);

	// generate color buffer
	glGenTextures(1, &colorBufferId);
	glBindTexture(GL_TEXTURE_2D, colorBufferId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	// generate color buffer
	glGenTextures(1, &colorBufferId2);
	glBindTexture(GL_TEXTURE_2D, colorBufferId2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	// generate depth buffer
	glGenRenderbuffers(1, &depthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	// attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorBufferId2, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderToBuffer(FrameBuffer To, FrameBuffer From, unsigned int shader) {
	glBindFramebuffer(GL_FRAMEBUFFER, To.Id);

	glUseProgram(shader);

	// Rendering
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, From.color1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, From.color2);

	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

