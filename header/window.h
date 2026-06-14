#pragma once

#define NOMINMAX
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <memory>
#include <observer.h>
#include <filament_pch.h>

#include <Hotaru/Structure/hotaru_ent.h>
#include <Hotaru/Structure/window_context.h>

using namespace filament;
using namespace utils;
using namespace filament::gltfio;
using namespace filament::math;



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
	bool Render(WindowContext& context);
	/*
	* @PY IN create_game
	* ウィンドウを作成する。
	*/
	static void error_callback(int error, const char* description) {
		std::cerr << description << "\n";
	}
	void GenWindow(WindowContext& context);
	void Destroy();
	void SetTransform(HotaruENT& hotaruEnt);

	GLFWwindow* window;
};