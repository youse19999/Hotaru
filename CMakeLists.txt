cmake_minimum_required(VERSION 4.3)


include(FetchContent)

project(Hotaru CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
else()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PHYSX_DIR "physx")

include_directories("${PHYSX_DIR}/include")

string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPERCASE_BUILD_TYPE)

#filament
include(FetchContent)

FetchContent_Declare(
    filament
    GIT_REPOSITORY https://github.com/google/filament.git
    GIT_TAG        v1.51.0
)

set(FILAMENT_BUILD_FILAMAT OFF CACHE BOOL "" FORCE)
set(FILAMENT_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(filament)

set(PHYSX_LIB_DIR "${PHYSX_DIR}/lib")

link_directories(${FILAMENT_LIB_DIR})

link_directories(${PHYSX_LIB_DIR})

find_package(pybind11 CONFIG REQUIRED)
find_package(DirectX-Headers REQUIRED)
find_package(glfw3)
find_package(imgui REQUIRED)

add_executable(Hotaru  
main.cpp
header/observer.h
header/console.h 
source/console.cpp
header/window.h
source/window.cpp
header/gpu.h
header/webgpu.h
header/command.h
header/Hotaru/Structure/hotaru_ent.h
header/Hotaru/Structure/window_context.h)

target_include_directories(Hotaru PRIVATE header filagui/include)

target_link_libraries(Hotaru PUBLIC pybind11::module
    backend
    basis_transcoder
    bluegl
    bluevk
    camutils
    civetweb
    dracodec
    filabridge
    filaflat
    filamat
    filament-generatePrefilterMipmap
    filament-iblprefilter
    filament
    filameshio
    geometry
    getopt
    matdbg
    gltfio
    gltfio_core
    ibl-lite
    ibl
    image
    imagediff
    imageio-lite
    ktxreader
    matdbg
    matp
    meshoptimizer
    mikktspace
    perfetto
    shaders
    smol-v
    stb
    uberarchive
    uberzlib
    utils
    viewer
    utils
    glfw
    Shlwapi
    imgui::imgui
	carb
	ovphysx
)

target_precompile_headers(Hotaru PRIVATE "header/filament_pch.h")