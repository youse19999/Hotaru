#define NOMINMAX
#include <window.h>
#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/View.h>
#include <filament/Scene.h>
#include <filament/Camera.h>
#include <filament/SwapChain.h>
#include <utils/EntityManager.h>
#include <filament/Viewport.h>

using namespace filament;
using namespace utils;

void* getNativeWindow(GLFWwindow* window) {
#if defined(GLFW_EXPOSE_NATIVE_WIN32)
    return reinterpret_cast<void*>(glfwGetWin32Window(window));
#elif defined(GLFW_EXPOSE_NATIVE_COCOA)
    return reinterpret_cast<void*>(glfwGetCocoaWindow(window));
#elif defined(GLFW_EXPOSE_NATIVE_X11)
    return reinterpret_cast<void*>(glfwGetX11Window(window));
#else
    return nullptr;
#endif
}

View* view;
Camera* camera;
Renderer* renderer;
SwapChain* swapChain;

void OnSize(HWND hwnd, UINT flag, int width, int height)
{

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/*
ウィンドウプロシージャを処理する。
*/
bool Window::ShouldClose()
{
    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    view->setViewport(filament::Viewport(0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height)));

    camera->setProjection(45.0, static_cast<double>(width) / height, 0.1, 100.0);

    if (renderer->beginFrame(swapChain)) {
        renderer->render(view);
        renderer->endFrame();
    }

    return glfwWindowShouldClose(window);
}

/*
ウィンドウを作成
*/
void Window::GenWindow()
{
    //glfwのエラーのコールバック設定
    glfwSetErrorCallback(error_callback);

    //GetLogSubjectのエラーのコールバック設定
    ObserverManager::getInstance().GetLogSubject().notify("INIT GLFW");

    //glfwの初期化
    if (!glfwInit()) {
        ObserverManager::getInstance().GetLogSubject().notify("ERROR IN GLFW_INIT");
        return;
    }

    //glfwのOPENGLなし
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    ObserverManager::getInstance().GetLogSubject().notify("CREATING GLFW WINDOW");
    //glfwウィンドウ作成
    window = glfwCreateWindow(800, 600, "WebGPU Triangle", nullptr, nullptr);
    if (!window) {
        //glfw停止
        ObserverManager::getInstance().GetLogSubject().notify("WINDOW WAS DEAD");
        glfwTerminate();
        return;
    }
    ObserverManager::getInstance().GetLogSubject().notify("WINDOW CREATED");

    Engine* engine = Engine::create(Engine::Backend::VULKAN);
    if (!engine) return;

    void* nativeWindow = getNativeWindow(window);
    swapChain = engine->createSwapChain(nativeWindow);

    renderer = engine->createRenderer();
    Scene* scene = engine->createScene();
    view = engine->createView();

    Entity cameraEntity = EntityManager::get().create();
    camera = engine->createCamera(cameraEntity);

    view->setCamera(camera);
    view->setScene(scene);
}

Window::~Window()
{

}