#include <Windows.h>
#include <console.h>
#include <stdio.h>

/**
 * @class CreateConsole
 * @brief コンソールを作成する。
 */
void Console::CreateConsole()
{
    AllocConsole();
    SetConsoleOutputCP(CP_UTF8);
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
}

/**
 * @class Console
 * @brief 破棄されたと同時にコンソールを消す。
 */
Console::~Console()
{
    FreeConsole();
}