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
    window->GenWindow();
    gpu->CreateGPU();
    while (true)
    {
        window->ShouldClose();
    }
}