#pragma once
#include <filament_pch.h>
enum HotaruENTFactoryType
{
	Light,
	Model,
};
struct HotaruENT
{
	std::vector<uint8_t> buffer;
	FilamentAsset* asset;
	std::string gltfPath;
	float3 position;
	float3 rotation;
	float3 scale;
	Entity entity;
	HotaruENTFactoryType factoryType;
	LightManager::Type lightType;
	float animationTime;
	bool destroyed;
};