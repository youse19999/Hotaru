#pragma once
#include <filament_pch.h>
#include <unordered_map>

struct WindowContext {
	std::string window;
	int width;
	int height;
	filament::math::float4 color;
	std::unordered_map<std::string, HotaruENT> entities;
};