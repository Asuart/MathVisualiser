#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Formula.h"
#include <windows.h>
#include <time.h>


// CONFIG VALUES. Should be in separate class
u32 max_str_length = 255;
u32 screenHeight = 800;
u32 screenWidth = 800;
f64 scale = 10;

f32 ratio = (float)screenWidth / (float)screenHeight;
u32 pixelCount = screenHeight * screenWidth;
u32 pixelDataLength = pixelCount * 4;
u32 centerHeight = screenHeight / 2;
u32 centerWidth = screenWidth / 2;
f64 pixelWidth = scale / screenWidth;
f64 pixelHeight = scale / screenHeight;
bool shouldUpdate = true;

void UpdateConfigs() {
	ratio = (float)screenWidth / (float)screenHeight;
	pixelCount = screenHeight * screenWidth;
	pixelDataLength = pixelCount * 4;
	centerHeight = screenHeight / 2;
	centerWidth = screenWidth / 2;
	pixelWidth = scale / screenWidth;
	pixelHeight = scale / screenHeight;
	shouldUpdate = true;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scale -= yoffset;
	if (scale < 1) scale = 1;
	UpdateConfigs();
}

int main(int argc, char** argv)
{
	const char* path = "C:/Users/Asuart/source/repos/FormulaVisioliser/Debug/formula.txt";
	if (argc > 0) {
		char* path = argv[0];
	}

	srand(time(0));
	u8* screen = new u8[pixelDataLength];

	std::list<Formula> formules;

	std::ifstream reader = std::ifstream(path, std::ios::in);

	if (reader.is_open()) {
		s8* c_str = new s8[max_str_length]; // max str length is 250 symbols;
		while (!reader.eof()) {
			reader.getline(c_str, max_str_length);
			std::string str = std::string(c_str);
			formules.push_back(Formula(str));
		}
	}
	else exit(1);
	reader.close();
	


	auto DrawCoords = [screen]() {
		memset(screen, 0, pixelDataLength);
		u32* screen32 = (u32*)screen;
		u32 c_vertical = screenWidth / 2;
		u32 c_horizontal = screenHeight / 2;

		s32 v_count = scale;
		s32 h_count = scale;
		u32 v_size = screenWidth / v_count;
		u32 h_size = screenHeight / h_count;
		for (s32 i = (v_count / -2); i <= v_count / 2; i++) {
			for (u32 j = 0; j < screenHeight; j++) {
				screen32[c_vertical + i * v_size + j * screenWidth] = 0x22222222;
			}
		}
		for (s32 i = (h_count / -2); i <= h_count / 2; i++) {
			for (u32 j = 0; j < screenWidth; j++) {
				screen32[(c_horizontal + (i * h_size)) * screenWidth + j] = 0x22222222;
			}
		}
		for (u32 i = c_horizontal * screenWidth; i < (c_horizontal + 1) * screenWidth; i++) screen32[i] = 0xffffffff;
		for (u32 i = c_vertical; i < pixelCount; i += screenWidth) screen32[i] = 0xffffffff;
	};
	auto DrawFunction = [screen](Formula formula, u32 color) {
		u32* screen32 = (u32*)screen;

		for (s32 i = 0; i < screenWidth; i++) {
			f32 value = formula.DefineInX((i - ((s32)screenWidth / 2)) * pixelWidth);
			if (value < screenHeight * pixelHeight / 2 && value > screenHeight * pixelHeight / 2 * -1) {
				s32 height = (value + screenHeight * pixelHeight / 2) / pixelHeight;
				screen32[800 * height + i] = color;
			}
			
		}
	};
	
	if(!glfwInit()) exit(3);
	GLFWwindow* mainWindow = glfwCreateWindow(screenWidth, screenHeight, "Formula Visualizer", NULL, NULL);
	glfwMakeContextCurrent(mainWindow);
	glfwSetKeyCallback(mainWindow, key_callback);
	glfwSetScrollCallback(mainWindow, scroll_callback);

	glewInit();
	glClearColor(0, 0, 0, 1);
	while (!glfwWindowShouldClose(mainWindow)) {
		if (shouldUpdate) {
			shouldUpdate = false;
			glClear(GL_COLOR_BUFFER_BIT);

			DrawCoords();
			for (auto i = formules.begin(); i != formules.end(); i++) DrawFunction(*i, std::rand() | 0xf0f);
			glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, (void*)screen);

			glfwSwapBuffers(mainWindow);
		}
		glfwPollEvents();
	}
	return 0;
}

