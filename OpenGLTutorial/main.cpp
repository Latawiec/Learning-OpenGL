#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Standard Library Includes
#include <vector>
#include <string_view>
#include <tuple>
#include <iostream>
#include <fstream>
#include <numeric>

#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Utils.hpp"
#include "Gizmo.hpp"

#ifndef SHADERS_SOURCE_DIR
#define SHADERS_SOURCE_DIR "INCORRECT SOURCE DIR"
#endif

#ifndef TEXTURES_SOURCE_DIR
#define TEXTURES_SOURCE_DIR "INCORRECT SOURCE DIR"
#endif

#ifndef MODELS_SOURCE_DIR
#define MODELS_SOURCE_DIR "INCORRECT SOURCE DIR"
#endif

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

Camera camera;
bool cursorEnabled = false;
void processInput(GLFWwindow* window) {
	const float cameraSpeed = 1.5f;
	auto updateFront = 0.f;
	auto updateLeft = 0.f;
	auto updateUp = 0.f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		updateFront += cameraSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		updateFront -= cameraSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		updateLeft -= cameraSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		updateLeft += cameraSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		updateUp -= cameraSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		updateUp += cameraSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		std::cout << std::boolalpha << cursorEnabled << '\n';
		glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		cursorEnabled = !cursorEnabled;
	}
	camera.updatePosition(updateFront, updateLeft, updateUp);
}

float lastMouseX;
float lastMouseY;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	const auto posX = static_cast<float>(xpos);
	const auto posY = static_cast<float>(ypos);

	if(firstMouse)
    {
        lastMouseX = posX;
        lastMouseY = posY;
        firstMouse = false;
    }
	const float rotationSpeed = 0.1f;
	float deltaX = posX - lastMouseX;
	float deltaY = posY - lastMouseY;
	lastMouseX = posX;
	lastMouseY = posY;

	camera.updateRotation(rotationSpeed * deltaY, rotationSpeed * deltaX);
}

float fov = 45.f;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	const float scrollSpeed = 1.5f;
	const float offset = scrollSpeed * static_cast<float>(yoffset);
	
	camera.updateFov(offset);
}

std::vector<Vertex> vertices = {
	// pos				   // normal		    // texcoord   
    {{-0.5f, -0.5f, -0.5f},   {0.0f,  0.0f, -1.0f}, 	{0.0f, 0.0f} },
    {{ 0.5f, -0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},     {1.0f, 0.0f} },
    {{ 0.5f,  0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},     {1.0f, 1.0f} },
    {{ 0.5f,  0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},     {1.0f, 1.0f} },
    {{-0.5f,  0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},     {0.0f, 1.0f} },
    {{-0.5f, -0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},     {0.0f, 0.0f} },
    {{-0.5f, -0.5f,  0.5f},   {0.0f,  0.0f, 1.0f}, 	    {0.0f, 0.0f} },
    {{ 0.5f, -0.5f,  0.5f},   {0.0f,  0.0f, 1.0f}, 	    {1.0f, 0.0f} },
    {{ 0.5f,  0.5f,  0.5f},   {0.0f,  0.0f, 1.0f}, 	    {1.0f, 1.0f} },
    {{ 0.5f,  0.5f,  0.5f},   {0.0f,  0.0f, 1.0f}, 	    {1.0f, 1.0f} },
    {{-0.5f,  0.5f,  0.5f},   {0.0f,  0.0f, 1.0f}, 	    {0.0f, 1.0f} },
    {{-0.5f, -0.5f,  0.5f},   {0.0f,  0.0f, 1.0f}, 	    {0.0f, 0.0f} },
    {{-0.5f,  0.5f,  0.5f},   {-1.0f,  0.0f,  0.0f},    {1.0f, 0.0f} },
    {{-0.5f,  0.5f, -0.5f},   {-1.0f,  0.0f,  0.0f},    {1.0f, 1.0f} },
    {{-0.5f, -0.5f, -0.5f},   {-1.0f,  0.0f,  0.0f},    {0.0f, 1.0f} },
    {{-0.5f, -0.5f, -0.5f},   {-1.0f,  0.0f,  0.0f},    {0.0f, 1.0f} },
    {{-0.5f, -0.5f,  0.5f},   {-1.0f,  0.0f,  0.0f},    {0.0f, 0.0f} },
    {{-0.5f,  0.5f,  0.5f},   {-1.0f,  0.0f,  0.0f},    {1.0f, 0.0f} },
    {{ 0.5f,  0.5f,  0.5f},   {1.0f,  0.0f,  0.0f},     {1.0f, 0.0f} },
    {{ 0.5f,  0.5f, -0.5f},   {1.0f,  0.0f,  0.0f},     {1.0f, 1.0f} },
    {{ 0.5f, -0.5f, -0.5f},   {1.0f,  0.0f,  0.0f},     {0.0f, 1.0f} },
    {{ 0.5f, -0.5f, -0.5f},   {1.0f,  0.0f,  0.0f},     {0.0f, 1.0f} },
    {{ 0.5f, -0.5f,  0.5f},   {1.0f,  0.0f,  0.0f},     {0.0f, 0.0f} },
    {{ 0.5f,  0.5f,  0.5f},   {1.0f,  0.0f,  0.0f},     {1.0f, 0.0f} },
    {{-0.5f, -0.5f, -0.5f},   {0.0f, -1.0f,  0.0f},     {0.0f, 1.0f} },
    {{ 0.5f, -0.5f, -0.5f},   {0.0f, -1.0f,  0.0f},     {1.0f, 1.0f} },
    {{ 0.5f, -0.5f,  0.5f},   {0.0f, -1.0f,  0.0f},     {1.0f, 0.0f} },
    {{ 0.5f, -0.5f,  0.5f},   {0.0f, -1.0f,  0.0f},     {1.0f, 0.0f} },
    {{-0.5f, -0.5f,  0.5f},   {0.0f, -1.0f,  0.0f},     {0.0f, 0.0f} },
    {{-0.5f, -0.5f, -0.5f},   {0.0f, -1.0f,  0.0f},     {0.0f, 1.0f} },
    {{-0.5f,  0.5f, -0.5f},   {0.0f,  1.0f,  0.0f},     {0.0f, 1.0f} },
    {{ 0.5f,  0.5f, -0.5f},   {0.0f,  1.0f,  0.0f},     {1.0f, 1.0f} },
    {{ 0.5f,  0.5f,  0.5f},   {0.0f,  1.0f,  0.0f},     {1.0f, 0.0f} },
    {{ 0.5f,  0.5f,  0.5f},   {0.0f,  1.0f,  0.0f},     {1.0f, 0.0f} },
    {{-0.5f,  0.5f,  0.5f},   {0.0f,  1.0f,  0.0f},     {0.0f, 0.0f} },
    {{-0.5f,  0.5f, -0.5f},   {0.0f,  1.0f,  0.0f},     {0.0f, 1.0f} }
};    

std::vector<unsigned int> indices(36);

int main() {
	std::iota(indices.begin(), indices.end(), 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, 800, 600);

	const auto vertexShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "phong.vert.glsl");
	const auto fragmentShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "phong.frag.glsl");

	auto shaderProgram = ShaderProgram(
		Shader<ShaderType::Vertex>(vertexShaderCode.c_str()),
		Shader<ShaderType::Fragment>(fragmentShaderCode.c_str())
	);

	const auto lightVertexShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "light.vert.glsl");
	const auto lightFragmentShaderCode = Utils::readFile(SHADERS_SOURCE_DIR "/" "light.frag.glsl");

	auto lightProgram = ShaderProgram(
		Shader<ShaderType::Vertex>(lightVertexShaderCode.c_str()),
		Shader<ShaderType::Fragment>(lightFragmentShaderCode.c_str())
	);

	// Setup light data
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);

	unsigned int lightVBO;
	glGenBuffers(1, &lightVBO);

	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Texture
	std::vector<Texture> textures = { 
		Texture(TEXTURES_SOURCE_DIR "/" "container2.png", TextureType::Diffuse),
		Texture(TEXTURES_SOURCE_DIR "/" "container2_specular.png", TextureType::Specular)
	};

	auto cube = Mesh(
		vertices,
		indices,
		textures
	);

	auto light = Mesh(
		vertices,
		indices,
		{}
	);

	auto house = Model(MODELS_SOURCE_DIR "/" "house.fbx");


    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    //create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 100, 100); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bool set = false;
	glEnable(GL_DEPTH_TEST);

	Gizmo gizmo;
	const glm::vec3 blue(0.f, 0.f, 1.f);
	const glm::vec3 red(1.f, 0.f, 0.f);
	glm::vec3 policeColor;
	float gizmoOffset[2] {};
	while(!glfwWindowShouldClose(window)) {
		glClearColor(.2f, .3f, .3f, 1.f);
		glViewport(0, 0, 800, 600);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// input
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		processInput(window);

		// runtime stuff...
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;  

		policeColor = glm::mix(blue, red, glm::sin(currentFrame*5.f));

		auto lightWorldTransform = glm::mat4(1.0f);
		lightWorldTransform = glm::rotate(lightWorldTransform, currentFrame, glm::vec3(0.f, 1.f, 0.f));
		lightWorldTransform = glm::translate(lightWorldTransform, glm::vec3(5.f, 0.f, 0.f));
		lightWorldTransform = glm::scale(lightWorldTransform, glm::vec3(0.05f));
		// rendering commands ...
		{
			lightProgram.use();
			lightProgram.set("lightColor", policeColor);

			lightProgram.set("model", lightWorldTransform);
			lightProgram.set("view", camera.getViewTransform());
			lightProgram.set("projection", camera.getProjectionTransform());
			light.Draw(lightProgram);

			auto up = glm::mat4(1.0f);
			up = glm::translate(up, glm::vec3(0.f, 1.f, 0.f));
			up = glm::scale(up, glm::vec3(0.05f));
			lightProgram.set("lightColor", glm::vec3(0.f, 1.f, 0.f));
			lightProgram.set("model", up);
			light.Draw(lightProgram);

			auto front = glm::mat4(1.0f);
			front = glm::translate(front, glm::vec3(0.f, 0.f, 1.f));
			front = glm::scale(front, glm::vec3(0.05f));
			lightProgram.set("lightColor", glm::vec3(0.f, 0.f, 1.f));
			lightProgram.set("model", front);
			light.Draw(lightProgram);

			auto lightTransform = glm::mat4(1.0f);
			lightTransform = glm::translate(lightTransform, glm::vec3(1.f, 0.f, 0.f));
			lightTransform = glm::scale(lightTransform, glm::vec3(0.05f));
			lightProgram.set("lightColor", glm::vec3(1.f, 0.f, 0.f));
			lightProgram.set("model", lightTransform);
			light.Draw(lightProgram);
			
		}

		{
			shaderProgram.use();
			shaderProgram.set("material.shininess", 32.0f);
			shaderProgram.set("spotLight.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
			shaderProgram.set("spotLight.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
			shaderProgram.set("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f)); 
			shaderProgram.set("spotLight.constant", 1.0f);
			shaderProgram.set("spotLight.linear", 0.09f);
			shaderProgram.set("spotLight.quadratic", 0.032f);
			shaderProgram.set("spotLight.position", camera.getPosition());
			shaderProgram.set("spotLight.direction", camera.getFront());
			shaderProgram.set("spotLight.cutoffStart", glm::cos(glm::radians(10.f)));
			shaderProgram.set("spotLight.cutoffEnd", glm::cos(glm::radians(11.f)));

			shaderProgram.set("pointLight.position", glm::vec3(lightWorldTransform * glm::vec4(0.f, 0.f, 0.f, 1.f)));
			shaderProgram.set("pointLight.ambient",  0.1f * policeColor);
			shaderProgram.set("pointLight.diffuse",  0.5f * policeColor); // darken diffuse light a bit
			shaderProgram.set("pointLight.specular", policeColor); 
			shaderProgram.set("pointLight.constant", 1.0f);
			shaderProgram.set("pointLight.linear", 0.09f);
			shaderProgram.set("pointLight.quadratic", 0.032f);

			shaderProgram.set("viewPos", camera.getPosition());
			auto model = glm::mat4(1.0f);
			// model = glm::scale(model, glm::vec3(0.01f));
			shaderProgram.set("model", model);
			shaderProgram.set("view", camera.getViewTransform());
			shaderProgram.set("projection", camera.getProjectionTransform());

			house.Draw(shaderProgram);
		}

		// Magic gizmo drawing.
		glViewport(gizmoOffset[0], gizmoOffset[1], 100, 100);
		gizmo.setDirection(camera.getFront());
		gizmo.draw();

		// glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// glViewport(0, 0, 100, 100);
		// glClearColor(0.f, 0.f, 0.f, 0.f);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		// glEnable(GL_DEPTH_TEST);
		// gizmo.setDirection(camera.getFront());
		// gizmo.draw();
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// render your GUI
		ImGui::Begin("Demo window");
		ImGui::Button("Button");
		ImGui::SliderFloat2("Gizmo Position", gizmoOffset, 0, 800);
		ImGui::End();
		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}