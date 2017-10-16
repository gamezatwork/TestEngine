#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <sstream>

#include "Reflection.h"
#include "Time\TimeManager.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <imgui.h>

#include <stb\stb_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static unsigned int shaderProgram;
unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int texID;

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
	
	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, texID);

	// Bind the vertex array (since you'll be drawing with it)
	glBindVertexArray(VAO);
	// Draw the array
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	
	// Draw with elements instead
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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


	

	// Next, set the OpenGL viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	// And set the callback for when the window is resized
	glfwSetFramebufferSizeCallback(window, Callback_FrameBufferSize);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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

	
	// FINALLY WE DRAW
	while (!glfwWindowShouldClose(window))
	{
		// Process the input
		ProcessInput(window);

		Time::TimeManager::Get().Update();

		glfwSetWindowTitle(window, std::string("FPS:" + std::to_string(Time::TimeManager::Get().GetFPS())).c_str());

		// Set the background colour
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Clear!
		glClear(GL_COLOR_BUFFER_BIT);

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