#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <JSLib/shader.h>


struct Vertex
{
	float x, y, z;
	float u, v;
};

Vertex vertices[4] = {
	//x    y    z    u    v
   { -1.0 , -1.0 , 0.0 , 0.0 , 0.0 }, //Bottom left
   { 1.0 , -1.0 , 0.0 , 1.0 , 0.0 }, //Bottom right
   { 1.0 , 1.0 , 0.0 , 1.0 , 1.0 },  //Top right
   { -1.0 , 1.0 , 0.0 , 0.0 , 1.0 }  //Top left
};


unsigned int indices[6] = {
	0 , 1 , 2 , //Triangle 1
	2 , 3 , 0  //Triangle 2
};


float skyTopColor[3] = {0.6f, 0.5f, 0.6f};
float skyBottomColor[3] = {0.8f, 0.3f, 0.0f};

float sunColor[3] = { 1.0f, 0.2f, 0.3f };
float sunRad = 0.2f;
float sunSpeed = 1.0f;

float hill1Color[3] = { 0.2f, 0.5f, 0.2f };
float hill2Color[3] = { 0.5f, 0.6f, 0.5f };

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;
bool showImGUIDemoWindow = true;


unsigned int createShader(GLenum shaderType, const char* sourceCode);
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);


int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Loads shaders from files
	JSLib::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");

	unsigned int vao = createVAO(vertices, 6, indices, 6);

	shader.use();
	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		shader.setFloat("iTime", (float)glfwGetTime());
		shader.setVec2("iResolution", SCREEN_WIDTH, SCREEN_HEIGHT);
		shader.setVec3("SkyTopColor", skyTopColor[0], skyTopColor[1], skyTopColor[2]);
		shader.setVec3("SkyBottomColor", skyBottomColor[0], skyBottomColor[1], skyBottomColor[2]);
		shader.setVec3("SunColor", sunColor[0], sunColor[1], sunColor[2]);
		shader.setFloat("SunRad", sunRad);
		shader.setFloat("SunSpeed", sunSpeed);
		shader.setVec3("Hill1Color", hill1Color[0], hill1Color[1], hill1Color[2]);
		shader.setVec3("Hill2Color", hill2Color[0], hill2Color[1], hill2Color[2]);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::ColorEdit3("Sky Top Color", skyTopColor);
			ImGui::ColorEdit3("Sky Bottom Color", skyBottomColor);
			ImGui::ColorEdit3("Sun Color", sunColor);
			ImGui::SliderFloat("Sun Radius", &sunRad, 0.0f, 2.0f);
			ImGui::SliderFloat("Sun Speed", &sunSpeed, 0.0f, 10.0f);
			ImGui::ColorEdit3("Hill 1 Color", hill1Color);
			ImGui::ColorEdit3("Hill 2 Color", hill2Color);
			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices)
{
	//VAO ID
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//EDO ID
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 5, vertexData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(0);

	//UV
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

