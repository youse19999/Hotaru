
#include <filament_pch.h>
struct HotaruENT
{
	std::vector<uint8_t> buffer;
	FilamentAsset* asset;
	std::string gltfPath;
	float3 position;
	float3 rotation;
	float3 scale;
};