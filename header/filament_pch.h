/**
 * @class Window
 * @brief フィラメントライブラリのpch
 * https://wiki.codeblocks.org/index.php/Precompiled_headers
 */

#include <shlwapi.h>
#include <math/mat3.h>
#include <math/vec3.h>
#include <math/quat.h>
#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/View.h>
#include <filament/Scene.h>
#include <filament/Camera.h>
#include <filament/SwapChain.h>
#include <filament/TransformManager.h>
#include <filament/LightManager.h>
#include <gltfio/AssetLoader.h>
#include <gltfio/ResourceLoader.h>
#include <gltfio/TextureProvider.h>
#include <gltfio/MaterialProvider.h>
#include <gltfio/materials/uberarchive.h>
#include <utils/EntityManager.h>
#include <filament/Viewport.h>