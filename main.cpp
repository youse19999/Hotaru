#include <Windows.h>
#include "header/observer.h"
#include <console.h>
#include <window.h>
#include <gpu.h>
#include <command.h>
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

    //ここで所有権が移ってしまうよ
    context.entities["monkey"] = std::move(ent);
    context.entities["duck"] = std::move(ent2);
    context.entities["girl"] = std::move(ent3);

    window->GenWindow(context);

    bool loop = true;

    while (loop)
    {
        std::move(context.entities["duck"]).scale = float3(1, 1, 1);
        std::move(context.entities["duck"]).position -= float3(0, 0, 0.01f);
        loop = !window->Render(context);
    }
    window->Destroy();
}