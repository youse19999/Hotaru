cmake_minimum_required(VERSION 4.3)

include(FetchContent)

project(Hotaru CXX)
project(HotaruTest CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
else()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


set(CMAKE_CXX_STANDARD 23)

set(FILAMENT_DIR "L:/FuckSchool/Hotaru/filament")

include_directories("${FILAMENT_DIR}/include")

string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPERCASE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
set(FILAMENT_LIB_DIR "${FILAMENT_DIR}/lib/x86_64/mt")
else()
set(FILAMENT_LIB_DIR "${FILAMENT_DIR}/lib/x86_64/mtd")
endif()

link_directories(${FILAMENT_LIB_DIR})

find_package(pybind11 CONFIG REQUIRED)
find_package(DirectX-Headers REQUIRED)
find_package(glfw3)

pybind11_add_module(Hotaru  
main.cpp
header/observer.h
header/console.h 
source/console.cpp
header/window.h
source/window.cpp
header/gpu.h
header/webgpu.h
source/gpu.cpp)

add_executable(HotaruTest  
test.cpp
header/observer.h
header/console.h
source/console.cpp
header/window.h
source/window.cpp
header/gpu.h
header/webgpu.h
source/gpu.cpp)


target_include_directories(Hotaru PRIVATE header)

target_link_libraries(Hotaru PUBLIC pybind11::module
    glfw
    filament
    backend
    utils
    geometry
    smol-v
    filaflat
    image
    # WebGPU(Dawn)やVulkanなどのバックエンドを動かすために必要なOS標準ライブラリ
    gdi32
    user32
    opengl32
    shlwapi
    bluegl
    bluevk
    filabridge
    zstd
)

target_include_directories(HotaruTest PRIVATE header header/webgpu)

target_link_libraries(HotaruTest PUBLIC 
    glfw
    filament
    backend
    utils
    geometry
    smol-v
    filaflat
    image
    # WebGPU(Dawn)やVulkanなどのバックエンドを動かすために必要なOS標準ライブラリ
    gdi32
    user32
    opengl32
    shlwapi
    bluegl
    bluevk
    filabridge
    zstd
)