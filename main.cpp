#include <Windows.h>
#include "header/observer.h"
#include <console.h>
#include <window.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

//一個しか存在できないからuniqueptr
std::unique_ptr<Console> console;
std::unique_ptr<Window> window;
Subject<std::string> subject;

bool ShouldClose(){
    return window->ShouldClose();
}
void CreateConsole() {
    console->CreateConsole();
    subject.notify("CONSOLE CRAETED");
}
void GenWindow() {
    window = std::make_unique<Window>();
    window->GenWindow();
    subject.notify("WINDOW CRAETED");
}
//PYTHONのバインド
PYBIND11_MODULE(Hotaru, m) {
    m.def("should_close", &ShouldClose, "");
    m.def("create_console", &CreateConsole, "");
    m.def("create_window", &GenWindow, "");
}