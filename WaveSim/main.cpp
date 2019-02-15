//OpenGL includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Shader class
#include "ShaderClass.h"

#include <iostream>
#include <math.h>
#include <chrono>
#include <thread>

//For handling changes in window size
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
//For handling inputs
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

//Settings
const unsigned int WindowWidth = 800;
const unsigned int WindowHeight = 800;

//Simulation parameters
const int N = 50;			//Number of divisions along x axis for displaying sine wave
const float Np = 5000;		//Divisions to split platform into when performing calculations

//Constants
const float g = 9.81f;			//Acceleration due to gravity in ms-2
const float Pp = 70.0f;			//Density of platform in kgm-3
const float Pw = 997.0f;		//Density of water in kgm-3
const float Spv = 1.0f;			//Mass per unit area of solar panels in kgm-2
const float pi = 3.141592654f;	//Pi

//Wave parameters
const float L = 5.0f;					//Half width of wave displayed (1.0f is half the window) in m divided by 3.35
const float K = pi/L;					//Wavenumber of the wave in m-1 multiplied by 3.35
const float V = 0.5f * sqrt(9.81 * L / pi);	//Velocity of the wave in ms-1 divided by 3.35
const float W = V * K;					//Frequency of the wave in ms-1 divided by 3.35
const float A = 0.2f;					//Amplitude of the wave in m divided by 3.35

//Platform parameters
const float width = 1.5f;		//Width of platform in m divided by 3.35
const float breadth = 1.5f;		//Breadth of platform in m divided by 3.35
const float depth = 0.12f;		//Depth of platform in m divided by 3.35
const float Apv = 1.0f;			//Fractional surface area covered in solar panels
const float dx = width / Np;	//Small change in distance when numerically integrating along the platform to find the force and couple in m divided by 3.35
// Mass of the platform and solar panels / kg
const float mass = Pp * width * breadth	* depth * 3.35 * 3.35 * 3.35 + Spv * Apv * width * breadth * 3.35 * 3.35;
//Moment of inertia about the horizontal axis (out of the plane of the screen) of the platform and solar panels:
const float Ix = mass * (width * width + depth  * depth) * 3.35 * 3.35/12 + (Spv * Apv *width * breadth * 3.35 * 3.35) * ((width * width * 3.35 * 3.35)/12 + (depth * depth *3.35 * 3.35)/4);
//Initial height, set so as to minimise undamped (as of yet) transient oscillations.
const float initialHeight = A + depth / 5;

int main() {
	//Setup glfw:
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Make a window:
	GLFWwindow* window = glfwCreateWindow(800, 600, "WaveSim", NULL, NULL);
	//Check for errors:
	if (window == NULL) {
		std::cout << "Failed to create a GLFW window" << std::endl;
		glfwTerminate();
		return -2;
	}
	//Set context and size callback
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	//Load OpenGL function pointers with glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD" << std::endl;
		return -1;
	}

	//Shader program, 
	Shader aShader("vertex.vs", "fragment.fs");

	//Initial vertex data, units: m divided by 3.35
	float wavevertices[3 * N];
	int waveindices[2 * N];
	for (int i = 0; i < N; i++) {
		float x = -L + 2 * i * L / N;
		wavevertices[3 * i] = x;
		wavevertices[3 * i + 1] = 0.0f;
		wavevertices[3 * i + 2] = 0.0f;
		waveindices[2 * i] = i;
		waveindices[2 * i + 1] = i + 1;
	}

	float platvertices[12] = {
		-width / 2, -depth / 2 + initialHeight, 0.0f,
		-width / 2, depth / 2 + initialHeight, 0.0f,
		width / 2, -depth / 2 + initialHeight, 0.0f,
		width / 2, depth / 2 + initialHeight, 0.0f
	};
	int platindices[6] = {
		0, 1, 2,
		1, 2, 3
	};

	//Centre of mass of the platform, and angle of platform to horizontal
	float platCoM[3] = { 0.0f, initialHeight, 0.0f };
	float theta = 0.0f;
	//Buffer and array objects
	unsigned int VBO, VAO, EBO, VBOP, EBOP, VAOP;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAOP);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBOP);
	glGenBuffers(1, &EBOP);

	//VAO for the wave
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wavevertices), wavevertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(waveindices), waveindices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//VAO for the platform
	glBindVertexArray(VAOP);
	glBindBuffer(GL_ARRAY_BUFFER, VBOP);
	glBufferData(GL_ARRAY_BUFFER, sizeof(platvertices), platvertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOP);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(platindices), platindices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Initialise time and change in time variables
	float t = 0.0f;
	float deltat = 0.001f;
	float oldt = 0.0f;

	//Velocities to use in iteration
	float oldvel = 0.0f;
	float newvel = 0.0f;
	float oldangvel = 0.0f;
	float newangvel = 0.0f;
	//Render loop:
	while (!glfwWindowShouldClose(window)) {
		
		//Get the time
		//deltat = (glfwGetTime() - t);
		t += deltat;
		if (t > glfwGetTime()) {
			int waitfor = round((t - glfwGetTime())*1000000);
			std::this_thread::sleep_for(std::chrono::microseconds(waitfor));
		}
		if (t < glfwGetTime()) {
			glfwSetTime(t);
		}

		//Move the wave
		for (int i = 0; i < N; i++) {
			wavevertices[3 * i + 1] = A * cos(K * wavevertices[3 * i] - W*t);
		}

		//Calculate the force and couple on the platform
		float force = -mass * g;
		float couple = 0.0f;
		for (int i = 0; i < Np + 1; i++) {
			float x = (platCoM[0] - width / 2 + i*dx) * 3.35f;	//Iterate along the platform, parallel to its bottom face
			float horizpos = (platCoM[0] + (-width / 2 + i * dx) * cos(theta))*3.35f;
			float hplat = (platCoM[1] - (depth / 2)*cos(theta) + x*sin(theta)) * 3.35f;
			float hwater = (A * cos(K * horizpos - W * t)) * 3.35f;
			if (hplat < hwater) {
				force += Pw * dx * (hwater - hplat) * breadth * 3.35f * g/ cos(theta); //Add dF
				couple += x * Pw * dx * (hwater - hplat) * breadth * 3.35f * g; //Add dF * cos(theta) * x
				
			}
		}
		
		//Calculate vertical acceleration
		float acc = force / mass;
		newvel = oldvel + acc * deltat;

		std::cout << t << std::endl;

		//Calculate angular acceleration
		float angaccx = couple / Ix;
		newangvel = oldangvel + angaccx * deltat;
		theta += newangvel * deltat;

		//Move (update vertex positions)
		platCoM[1] += newvel * deltat / 3.35f;
		platvertices[0] = platCoM[0] - (width / 2) * cos(theta) + (depth/2) * sin(theta);
		platvertices[1] = platCoM[1] - (width / 2) * sin(theta) - (depth / 2)*cos(theta);
		platvertices[3] = platCoM[0] - (width / 2) * cos(theta) - (depth / 2) * sin(theta);
		platvertices[4] = platCoM[1] - (width / 2) * sin(theta) + (depth / 2)*cos(theta);
		platvertices[6] = platCoM[0] + (width / 2) * cos(theta) + (depth / 2) * sin(theta);
		platvertices[7] = platCoM[1] + (width / 2) * sin(theta) - (depth / 2)*cos(theta);
		platvertices[9] = platCoM[0] + (width / 2) * cos(theta) - (depth / 2) * sin(theta);
		platvertices[10] = platCoM[1] + (width / 2) * sin(theta) + (depth / 2)*cos(theta);


		//Reset old velocities
		oldvel = newvel;
		oldangvel = newangvel;
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(wavevertices), wavevertices, GL_STATIC_DRAW);
		//Process inputs

		processInput(window);

		//Render everything
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		aShader.use();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_LINES, 2 * N - 1, GL_UNSIGNED_INT, 0);


		glBindVertexArray(VAOP);

		glBindBuffer(GL_ARRAY_BUFFER, VBOP);
		glBufferData(GL_ARRAY_BUFFER, sizeof(platvertices), platvertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOP);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Swap buffers, poll input/output events:
		glfwSwapBuffers(window);
		glfwPollEvents();

		//std::cout << deltat << std::endl;
	}
	//De-allocate resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//Terminate glfw to clear all glfw resources
	glfwTerminate();

	return 0;
}