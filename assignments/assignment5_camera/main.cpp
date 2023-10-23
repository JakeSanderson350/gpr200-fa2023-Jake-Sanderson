#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <JSLib/camera.h>
#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>


void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

void moveCamera(GLFWwindow* window, JSLib::Camera* camera, JSLib::CameraControls* controls, float deltaTime)
{
	//If right mouse is not held, release cursor and return early.
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) 
	{
		//Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}
	//GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Get screen mouse position this frame
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	//If we just started right clicking, set prevMouse values to current position.
	//This prevents a bug where the camera moves as soon as we click.
	if (controls->firstMouse) 
	{
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	//TODO: Get mouse position delta for this frame
	double mouseXDelta = mouseX - controls->prevMouseX;
	double mouseYDelta = mouseY - controls->prevMouseY;

	//double mouseXDelta = controls->prevMouseX - mouseX;
	//double mouseYDelta = controls->prevMouseY - mouseY;
	
	//TODO: Add to yaw and pitch
	controls->yaw += (mouseXDelta) * controls->mouseSensitivity;
	controls->pitch -= (mouseYDelta) * controls->mouseSensitivity;
	
	//TODO: Clamp pitch between -89 and 89 degrees
	if (controls->pitch < -89)
		controls->pitch = -89;
	else if (controls->pitch > 89)
		controls->pitch = 89;

	//Remember previous mouse position
	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	//Construct forward vector using yaw and pitch. Don't forget to convert to radians!
	ew::Vec3 forward = ew::Vec3(cos(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch)), 
								sin(ew::Radians(controls->pitch)), 
								sin(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch)));

	ew::Vec3 up = ew::Vec3(0, 1, 0);

	ew::Vec3 right = ew::Vec3(ew::Normalize(ew::Cross(forward, up)));

	up = ew::Vec3(ew::Normalize(ew::Cross(right, forward)));

	//Movement controls
	if (glfwGetKey(window, GLFW_KEY_W)) 
	{
		camera->position += forward * controls->moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) 
	{
		camera->position -= forward * controls->moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) 
	{
		camera->position += right * controls->moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) 
	{
		camera->position -= right * controls->moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) 
	{
		camera->position += up * controls->moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) 
	{
		camera->position -= up * controls->moveSpeed * deltaTime;
	}

	//Sprint function
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) 
	{
		controls->moveSpeed = 10.0;
	}
	else if (controls->moveSpeed != 5.0) 
	{
		controls->moveSpeed = 5.0;
	}

	//By setting target to a point in front of the camera along its forward direction, our 
	//LookAt will be updated accordingly when rendering.
	camera->target = camera->position + forward;
}

void resetCamera(JSLib::Camera* camera, JSLib::CameraControls* controls)
{
	camera->position = ew::Vec3(0, 0, 5);
	camera->target = ew::Vec3(0, 0, 0);
	camera->fov = 60;

	controls->yaw = -90;
	controls->pitch = 0;
}

void cameraInitialize(JSLib::Camera* camera)
{
	camera->position = ew::Vec3(0, 0, 5);
	camera->target = ew::Vec3(0, 0, 0);
	camera->fov = 60;

	camera->orthographic = true;
	camera->orthoSize = 6;

	camera->nearPlane = 0.1;
	camera->farPlane = 100;
}


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

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	JSLib::Camera cam;
	
	cameraInitialize(&cam);

	JSLib::CameraControls camControl;

	float prevTime = 0.0f;

	GLint viewportData[4];

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//Calculate and pass deltaTime
		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		moveCamera(window, &cam, &camControl, deltaTime);

		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Gets current viewport width and height
		glGetIntegerv(GL_VIEWPORT, viewportData);
		cam.aspectRatio = (float)viewportData[2] / (float)viewportData[3];

		//Set uniforms
		shader.use();

		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cam.ProjectionMatrix() * cam.ViewMatrix() * cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::Text("Camera");
			ImGui::DragFloat3("Position", &cam.position.x, 0.05f);
			ImGui::DragFloat3("Target", &cam.target.x, 0.05f);
			ImGui::DragFloat("FOV", &cam.fov, 1.0f);
			ImGui::Checkbox("Orthographic", &cam.orthographic);
			ImGui::DragFloat("Ortho Height", &cam.orthoSize, 0.05f);
			ImGui::DragFloat("Near Plane", &cam.nearPlane, 0.05f);
			ImGui::DragFloat("Far Plane", &cam.farPlane, 0.05f);
			ImGui::DragFloat("Yaw", &camControl.yaw, 0.05f);
			ImGui::DragFloat("Pitch", &camControl.pitch, 0.05f);
			
			if (ImGui::Button("Reset"))
			{
				resetCamera(&cam, &camControl);
			}

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
}

