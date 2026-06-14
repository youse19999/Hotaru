#include <Windows.h>
#include "header/observer.h"
#include <window.h>
std::unique_ptr<Console> console;
std::unique_ptr<Window> window;
std::unique_ptr<HotaruGPU> gpu;
Subject<std::string> subject;

//テスト用
int main()
{
    window = std::make_unique<Window>();
    console->CreateConsole();

    WindowContext context;
    context.window = "Hello";
    context.width = 800;
    context.height = 600;
    context.color = filament::Color::toLinear(
        filament::math::float4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    /*
    ワールド生成
    */
    HotaruENT monkey;
    monkey.scale = float3(1, 1, 1);
    monkey.position = float3(1, 0, 0);
    monkey.gltfPath = "gltf/monkey.glb";
    monkey.factoryType = HotaruENTFactoryType::Model;

    HotaruENT girl;
    girl.scale = float3(1, 1, 1);
    girl.position = float3(-2, 5, -2);
    girl.rotation = float3(0, 0, 0);
    girl.gltfPath = "gltf/girl_anim.glb";
    girl.factoryType = HotaruENTFactoryType::Model;

    HotaruENT camera;
    camera.scale = float3(1, 1, 1);
    camera.position = float3(0, -5, 0);
    camera.gltfPath = "gltf/monkey.glb";
    camera.factoryType = HotaruENTFactoryType::Model;

    HotaruENT light;
    light.scale = float3(1, 1, 2);
    light.position = float3(-5,0,0);
    light.lightType = LightManager::Type::POINT;
    light.factoryType = HotaruENTFactoryType::Light;

    HotaruENT ground;
    ground.gltfPath = "gltf/ground.glb";
    ground.position = float3(0, 0, -3);
    ground.scale = float3(1, 1, 1);
    ground.factoryType = HotaruENTFactoryType::Model;

    /*
    所有権移動＋登録
    */

    context.entities["monkey"] = std::move(monkey);
    context.entities["ground"] = std::move(ground);
    context.entities["camera"] = std::move(camera);
    context.entities["light"] = std::move(light);
    context.entities["girl"] = std::move(girl);

    window->GenWindow(context);

    bool loop = true;

    while (loop)
    {
        window->SetTransform(context.entities["camera"], window->GetCamera()->getEntity());
        loop = !window->Render(context);
    }
    window->Destroy();
}