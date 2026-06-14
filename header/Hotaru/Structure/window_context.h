
#include <filament_pch.h>

struct WindowContext {
	std::string window;
	int width;
	int height;
	filament::math::float4 color;
	std::vector<HotaruENT> entities;
};