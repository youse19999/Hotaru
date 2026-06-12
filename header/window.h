#define NOMINMAX
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <memory>
#include <observer.h>
/**
 * @class Window
 * @brief ウィンドウの制御をする
 */
class Window
{
public:
	/*
	* @PY IN process_window
	ウィンドウプロシージャを処理する。
	*/
	bool ShouldClose();
	/*
	* @PY IN create_game
	* ウィンドウを作成する。
	*/
	static void error_callback(int error, const char* description) {
		std::cerr << description << "\n";
	}
	void GenWindow();
	~Window();

	GLFWwindow* window;
};