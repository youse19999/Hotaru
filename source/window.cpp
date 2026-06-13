#define NOMINMAX
#include <vector>
#include <fstream>
#include <window.h>
#include <filament_pch.h>
#include <filagui/ImGuiHelper.h>
#include <imgui.h>

using namespace filament;
using namespace utils;
using namespace filament::gltfio;
using namespace filament::math;

extern "C" {
    extern const uint8_t UBERARCHIVE_PACKAGE[];
}

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
Engine* engine;

filament::View* imguiView;
filagui::ImGuiHelper* imguiHelper;

float deltaTime = 1.0f / 60.0f;

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

    imguiView->setViewport({ 0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    
    view->setViewport(filament::Viewport(0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height)));

    camera->setProjection(45.0, static_cast<double>(width) / height, 0.1, 100.0);

    imguiHelper->setDisplaySize(width, height);

    imguiHelper->render(deltaTime, [](filament::Engine* engine, filament::View* view) {
        ImGui::Begin("Filament UI");
        ImGui::Text("Hello, Filament with ImGui!");
        if (ImGui::Button("Click Me")) {
        }
        ImGui::End();
    });

    if (renderer->beginFrame(swapChain)) {
        renderer->render(view);
        renderer->render(imguiView);
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

    engine = Engine::create(Engine::Backend::VULKAN);
    if (!engine) return;

    void* nativeWindow = getNativeWindow(window);
    swapChain = engine->createSwapChain(nativeWindow);

    renderer = engine->createRenderer();
    Scene* scene = engine->createScene();
    view = engine->createView();

    Entity cameraEntity = EntityManager::get().create();
    camera = engine->createCamera(cameraEntity);
    camera->setExposure(16.0f, 1 / 125.0f, 100.0f);

    view->setScene(scene);
    view->setCamera(camera);

    Entity lightEntity = EntityManager::get().create();

    filament::LightManager::Builder(filament::LightManager::Type::DIRECTIONAL)
        .color(filament::Color::toLinear(filament::sRGBColor(1.0f, 1.0f, 1.0f)))
        .intensity(100000.0f)
        .direction({ 0.0f, -1.0f, -1.0f })
        .castShadows(true)
        .build(*engine, lightEntity);

    scene->addEntity(lightEntity);

    /*
    glb読み込み
    */

    AssetConfiguration config;
    config.engine = engine;

    MaterialProvider* materialProvider = createUbershaderProvider(engine, UBERARCHIVE_DEFAULT_DATA, UBERARCHIVE_DEFAULT_SIZE);
    config.materials = materialProvider;

    AssetLoader* assetLoader = AssetLoader::create(config);

    std::ifstream file("gltf/Duck.glb", std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);

    file.read(reinterpret_cast<char*>(buffer.data()), size);

    /*
    glb終了
    */

    /*
    asset作成
    */
    FilamentAsset* asset = assetLoader->createAsset(buffer.data(), buffer.size());

    ResourceConfiguration resConfig;
    resConfig.engine = engine;
    resConfig.gltfPath = "gltf/";

    ResourceLoader resourceLoader(resConfig);

    filament::gltfio::TextureProvider* m_stbDecoder = filament::gltfio::createStbProvider(engine);
    filament::gltfio::TextureProvider* m_ktxDecoder = filament::gltfio::createKtx2Provider(engine);
    resourceLoader.addTextureProvider("image/png", m_stbDecoder);
    resourceLoader.addTextureProvider("image/jpeg", m_ktxDecoder);

    bool success = resourceLoader.loadResources(asset);

    scene->addEntities(asset->getEntities(), asset->getEntityCount());

    /*
    asset終了
    */

    Entity rootEntity = asset->getRoot();

    filament::TransformManager& tcm = engine->getTransformManager();

    auto instance = tcm.getInstance(rootEntity);
    if (instance) {
        filament::math::mat4f transform = filament::math::mat4f(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        transform *= transform.eulerYXZ(0, 0, 0);
        transform *= transform.lookAt(float3(0.0f, -1.0f, -6.0f), float3(0.0f, 1.0f, 1.0f), float3(0.0f, 1.0f, 0.0f));
        transform *= transform.scaling(float3(1.0, 1.0, 1.0f));
        tcm.setTransform(instance, transform);
    }
    auto box = asset->getBoundingBox();
    filament::math::float4 clearColor = filament::Color::toLinear(
        filament::math::float4(1.0f, 1.0f, 1.0f, 1.0f)
    );

    filament::Renderer::ClearOptions options;
    options.clearColor = clearColor;
    options.clear = true;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    imguiView = engine->createView();

    imguiHelper = new filagui::ImGuiHelper(engine, imguiView, "APT.ttf");

    renderer->setClearOptions(options);

    asset->getWireframe();                                                       
}

Window::~Window()
{

}