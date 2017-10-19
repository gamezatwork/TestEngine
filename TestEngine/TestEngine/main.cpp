#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <sstream>

#include "Reflection.h"
#include "Time\TimeManager.h"
#include "Core\WindowManager.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb\stb_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static unsigned int shaderProgram;
unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int boxTexID;
unsigned int faceTexID;

// Rectangle Vertices
// Counter-clockwise
static float vertices[] = {
	// positions		// Colours		//Texture coordinates
	0.5f,  0.5f, 0.0f,	1.0f,0.0f,0.0f,	1.0f,1.0f,// top right
	0.5f, -0.5f, 0.0f,	0.0f,1.0f,0.0f,	1.0f,0.0f,// bottom right
	-0.5f, -0.5f, 0.0f,	0.0f,0.0f,1.0f,	0.0f,0.0f,// bottom left
	-0.5f,  0.5f, 0.0f,	1.0f,1.0f,0.0f,	0.0f,1.0f// top left
};
// This is to play with element buffers
static unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
};

// Cube vertices
static float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,   1.0f,1.0f,1.0f, 1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,   1.0f,1.0f,1.0f, 1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,   1.0f,1.0f,1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f, 0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,   1.0f,1.0f,1.0f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,   1.0f,1.0f,1.0f, 1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,   1.0f,1.0f,1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f, 0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f, 1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f, 1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f, 1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f, 0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,   1.0f,1.0f,1.0f, 1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,   1.0f,1.0f,1.0f, 1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,   1.0f,1.0f,1.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  1.0f,1.0f,1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,   1.0f,1.0f,1.0f, 1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,   1.0f,1.0f,1.0f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,   1.0f,1.0f,1.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f,1.0f,1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f,1.0f,1.0f, 0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

// Camera stuff
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


// Called when the window is resized
void Callback_FrameBufferSize(GLFWwindow* window, int newWidth, int newHeight)
{
	// Set up the glviewport again
	glViewport(0, 0, newWidth, newHeight);
}

// Process the input
void ProcessInput(GLFWwindow* window)
{
	// If it is the escape key, close
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		// Close the window
		glfwSetWindowShouldClose(window, true);
	}
}

// Call to grab the text data from a file
std::string GetTextFromFile(std::string fileName)
{
	std::string result;
	// Open it up with a ifstream
	std::ifstream is(fileName);
	if (is.is_open())
	{
		std::stringstream ss;
		ss << is.rdbuf();
		result = ss.str();
	}
	return result;
}

// The render loop part
void RenderLoop()
{
	// Make sure you are using the program
	glUseProgram(shaderProgram);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTexID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, faceTexID);

	// Bind the vertex array (since you'll be drawing with it)
	glBindVertexArray(VAO);
	// Draw the array
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	// Kay, stuff in the view mtx (since the camera not gonna move now) and the projection mtx (since that is not gonna change) first
	// The view matrix
	glm::mat4 viewMtx;
	viewMtx = glm::translate(viewMtx, glm::vec3(0.0f, 0.0f, -3.0f));
	unsigned int viewMtxLoc = glGetUniformLocation(shaderProgram, "viewMtx");
	glUniformMatrix4fv(viewMtxLoc, 1, GL_FALSE, glm::value_ptr(viewMtx));
	// The projection matrix
	glm::mat4 projMtx;
	projMtx = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	unsigned int projMtxLoc = glGetUniformLocation(shaderProgram, "projMtx");
	glUniformMatrix4fv(projMtxLoc, 1, GL_FALSE, glm::value_ptr(projMtx));

	// Randomly stuff in 10 cubes
	for (unsigned int i = 0; i < 10; ++i)
	{
		//float randX = (float)(rand() % 80 - 40) / 10.0f;
		//float randY = (float)(rand() % 100 - 50) / 10.0f;
		//float randZ = (float)(rand() % 300 - 150) / 10.0f;
		//glm::vec3 pos(randX, randY, randZ);
		glm::vec3 pos = cubePositions[i];
		// The model matrix
		glm::mat4 modelMtx;
		modelMtx = glm::translate(modelMtx, pos);
		modelMtx = glm::rotate(modelMtx, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));

		unsigned int modelMtxLoc = glGetUniformLocation(shaderProgram, "modelMtx");
		glUniformMatrix4fv(modelMtxLoc, 1, GL_FALSE, glm::value_ptr(modelMtx));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Draw with elements instead
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void LoadTexture(GLuint& texID, std::string resourcePath, bool isRGBA)
{
	// Generate the texture in OpenGL
	glGenTextures(1, &texID);
	// Bind it so that we can stuff the imageData in
	glBindTexture(GL_TEXTURE_2D, texID);
	// Set up the parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Load the image
	int imageWidth, imageHeight, imageColourChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imageData = stbi_load(resourcePath.c_str(), &imageWidth, &imageHeight, &imageColourChannels, 0);
	if (imageData)
	{
		// Then stuff it in
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, isRGBA?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error loading image!" << std::endl;
	}
	// Done with the image, can free it liao
	stbi_image_free(imageData);
}


int main()
{
	// Set up glfw
	// Starts up the glfw library
	
	glfwInit();
	// Hint to what versions you want the glfw to be for opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "THIS IS A WINDOW", nullptr, nullptr);
	// If it failed, kill the program
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Then set the window as the current context
	glfwMakeContextCurrent(window);

	// Set up GLAD by settling the opengl function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to intialize GLAD!" << std::endl;
		return -1;
	}
	

	//Core::WindowManager::Get().Init();


	// And set the callback for when the window is resized
	glfwSetFramebufferSizeCallback(window, Callback_FrameBufferSize);
	

	// Next, set the OpenGL viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	

	Reflection::Test::RunReflectionTest();
	Time::TimeManager::Get().Init();

	// Create the VBO and VAO and EBO
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	// Load the shaders
	
	// Create the shaders first
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Then attach the source codes
	std::string vertSource = GetTextFromFile("../TestEngine/Shaders/Basic.vert");
	const char* vertSourcePtr = vertSource.c_str();
	glShaderSource(vertexShader, 1, &vertSourcePtr, nullptr);
	std::string fragSource = GetTextFromFile("../TestEngine/Shaders/Basic.frag");
	const char* fragSourcePtr = fragSource.c_str();
	glShaderSource(fragmentShader, 1, &fragSourcePtr, nullptr);
	// Then attempt to compile them
	int success;
	char infoLog[512];
	
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "Error! Vertex shader compilation failed: \n" << infoLog << std::endl;
	}

	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "Error! Fragment shader compilation failed: \n" << infoLog << std::endl;
	}

	// Then put the program together
	// Create the program first
	shaderProgram = glCreateProgram();
	// Then attach and link
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// And mention the errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "Error! Program failed to link: \n" << infoLog << std::endl;
	}

	// After that, kill the old shaders, since they are already linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	// Init code for drawing the triangle, since we'll be using the VAO
	// Bind the vertex array object
	glBindVertexArray(VAO);
	// Copy the vertices over to the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// Then set the vertex attrib pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// Copy the indices over to the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Uncomment this to see in wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Next, the texture
	LoadTexture(boxTexID, "../Resources/Textures/container.jpg",false);
	LoadTexture(faceTexID, "../Resources/Textures/awesomeface.png", true);

	// Set the uniforms
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

	/*
	// Generate the texture in OpenGL
	glGenTextures(1, &texID);
	// Bind it so that we can stuff the imageData in
	glBindTexture(GL_TEXTURE_2D, texID);
	// Set up the parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Load the image
	int imageWidth, imageHeight, imageColourChannels;
	unsigned char* imageData = stbi_load("../Resources/Textures/container.jpg", &imageWidth, &imageHeight, &imageColourChannels, 0);
	if (imageData)
	{
		// Then stuff it in
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error loading image!" << std::endl;
	}
	// Done with the image, can free it liao
	stbi_image_free(imageData);
	*/
	
	// FINALLY WE DRAW
	while (!glfwWindowShouldClose(window))
	{
		// Process the input
		ProcessInput(window);

		Time::TimeManager::Get().Update();

		glfwSetWindowTitle(window, std::string("FPS:" + std::to_string(Time::TimeManager::Get().GetFPS())).c_str());

		// Set the background colour
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clear! (along with the depth buffer, since we are using it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// RENDER HERE
		RenderLoop();

		// swap the bufers
		glfwSwapBuffers(window);
		// Check for events
		glfwPollEvents();
	}

	// Ok, time to end
	glfwTerminate();

	return 0;
}