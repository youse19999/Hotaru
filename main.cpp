#include <Windows.h>
#include "header/observer.h"
#include <console.h>
#include <window.h>
#include<gpu.h>
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
        filament::math::float4(1.0f, 0.0f, 0.0f, 1.0f)
    );

    HotaruENT ent;
    ent.gltfPath = "gltf/monkey.glb";
    HotaruENT ent2;
    ent2.gltfPath = "gltf/Duck.glb";
    HotaruENT ent3;
    ent3.gltfPath = "gltf/Girl.glb";

    ent2.position = float3(0, 0, -5);
    ent2.scale = float3(1,1,1);

    //ここで所有権が移ってしまうよ
    context.entities.push_back(std::move(ent));
    context.entities.push_back(std::move(ent2));
    context.entities.push_back(std::move(ent3));

    window->GenWindow(context);

    bool loop = true;

    while (loop)
    {
        loop = !window->Render(context);
    }
    window->Destroy();
}