#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <JSLib/procGen.h>
//Terrain
#include <JSLib/terrain.h>
//
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;

struct AppSettings {
	const char* shadingModeNames[6] = { "Solid Color","Normals","UVs","Texture","Lit","Texture Lit"};
	int shadingModeIndex;

	ew::Vec3 bgColor = ew::Vec3(0.1f);
	ew::Vec3 shapeColor = ew::Vec3(1.0f);
	bool wireframe = true;
	bool drawAsPoints = false;
	bool backFaceCulling = true;

	//Euler angles (degrees)
	ew::Vec3 lightRotation = ew::Vec3(0, 0, 0);
}appSettings;

ew::Camera camera;
ew::CameraController cameraController;


int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);
	glPointSize(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");

//Terrain
	unsigned int snowTexture = ew::loadTexture("assets/textures/snow_color.jpg",GL_REPEAT,GL_LINEAR);
	unsigned int grassTexture = ew::loadTexture("assets/textures/grass_color.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int rockTexture = ew::loadTexture("assets/textures/rock_color.jpg", GL_REPEAT, GL_LINEAR);

	//Create terrain mesh
	char* terrainFilename= "assets/heightmaps/heightmap02.jpg";
	ew::Mesh terrainMesh1(JSLib::createTerrain("assets/heightmaps/heightmap01.jpg"));
	ew::Mesh terrainMesh2(JSLib::createTerrain("assets/heightmaps/heightmap02.jpg"));
	ew::Mesh terrainMesh3(JSLib::createTerrain("assets/heightmaps/heightmap03.jpg"));

	//Initialize transforms
	ew::Transform terrainTransform;
	terrainTransform.position = ew::Vec3(0.0f, 0.0f, 0.0f);

	//Initialize UI uniforms
	int heightmapNum = 1;

	float terMinY, terMaxY;

	float HBTrange1 = 0.15f;
	float HBTrange2 = 0.3f;
	float HBTrange3 = 0.65f;
	float HBTrange4 = 0.85f;
//

	resetCamera(camera,cameraController);

	//Camera stuff
	camera.farPlane = 200.0f;
	camera.position.y = 75.0f;
	camera.target.y = 75.0f;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		cameraController.Move(window, &camera, deltaTime);

		//Render
		glClearColor(appSettings.bgColor.x, appSettings.bgColor.y, appSettings.bgColor.z,1.0f);

		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		shader.use();

	//Terrain
		terMinY = abs(terrainTransform.position.y);
		terMaxY = terrainTransform.position.y + (64.0f * terrainTransform.scale.y);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, snowTexture);
		shader.setInt("_TextureSnow", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		shader.setInt("_TextureGrass", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, rockTexture);
		shader.setInt("_TextureRock", 2);
	//

		shader.setInt("_Mode", appSettings.shadingModeIndex);
		shader.setVec3("_Color", appSettings.shapeColor);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Euler angels to forward vector
		ew::Vec3 lightRot = appSettings.lightRotation * ew::DEG2RAD;
		ew::Vec3 lightF = ew::Vec3(sinf(lightRot.y) * cosf(lightRot.x), sinf(lightRot.x), -cosf(lightRot.y) * cosf(lightRot.x));
		shader.setVec3("_LightDir", lightF);

	//Terrain
		//UI uniforms
		shader.setFloat("_terMinY", terMinY);
		shader.setFloat("_terMaxY", terMaxY);

		shader.setFloat("_HBTrange1", HBTrange1);
		shader.setFloat("_HBTrange2", HBTrange2);
		shader.setFloat("_HBTrange3", HBTrange3);
		shader.setFloat("_HBTrange4", HBTrange4);

		//Draw terrain
		shader.setMat4("_Model", terrainTransform.getModelMatrix());
		switch (heightmapNum)
		{
		case 1:
			terrainMesh1.draw((ew::DrawMode)appSettings.drawAsPoints);
			break;
		case 2: 
			terrainMesh2.draw((ew::DrawMode)appSettings.drawAsPoints);
			break;
		case 3:
			terrainMesh3.draw((ew::DrawMode)appSettings.drawAsPoints);
			break;
		default:
			terrainMesh1.draw((ew::DrawMode)appSettings.drawAsPoints);
			break;
		}
	//

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			ImGui::ColorEdit3("BG color", &appSettings.bgColor.x);
			ImGui::ColorEdit3("Shape color", &appSettings.shapeColor.x);
			ImGui::Combo("Shading mode", &appSettings.shadingModeIndex, appSettings.shadingModeNames, IM_ARRAYSIZE(appSettings.shadingModeNames));
			if (appSettings.shadingModeIndex > 3) {
				ImGui::DragFloat3("Light Rotation", &appSettings.lightRotation.x, 1.0f);
			}
			ImGui::Checkbox("Draw as points", &appSettings.drawAsPoints);
			if (ImGui::Checkbox("Wireframe", &appSettings.wireframe)) {
				glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);
			}
			if (ImGui::Checkbox("Back-face culling", &appSettings.backFaceCulling)) {
				if (appSettings.backFaceCulling)
					glEnable(GL_CULL_FACE);
				else
					glDisable(GL_CULL_FACE);
			}

		//Terrain UI
			if (ImGui::CollapsingHeader("Terrain"))
			{
				ImGui::DragInt("Heightmap", &heightmapNum, 1.0f, 1, 3);
				ImGui::DragFloat3("Position", &terrainTransform.position.x, 0.1f);
				ImGui::DragFloat3("Scale", &terrainTransform.scale.x, 0.1f);

				if (ImGui::CollapsingHeader("Height Based Texturing Ranges"))
				{
					ImGui::DragFloat("Range 1", &HBTrange1, 0.05f, 0.0f, 1.0f);
					ImGui::DragFloat("Range 2", &HBTrange2, 0.05f, 0.0f, 1.0f);
					ImGui::DragFloat("Range 3", &HBTrange3, 0.05f, 0.0f, 1.0f);
					ImGui::DragFloat("Range 4", &HBTrange4, 0.05f, 0.0f, 1.0f);
				}
			}
		//

			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 3);
	camera.target = ew::Vec3(0);
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


