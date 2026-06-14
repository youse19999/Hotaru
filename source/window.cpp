#define NOMINMAX
#include <vector>
#include <fstream>
#include <window.h>
#include <filagui/ImGuiHelper.h>
#include <imgui.h>
#include <iostream>
#include <string>
#include <command.h>

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
Scene* scene;

std::unordered_map<std::string, bool> entity_windows;

std::unordered_map<std::string, Entity> entities;

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
レンダリングをする。
*/
bool Window::Render(WindowContext& context)
{
    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    imguiView->setViewport({ 0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    
    view->setViewport(filament::Viewport(0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height)));

    camera->setProjection(45.0, static_cast<double>(width) / height, 0.1, 100.0);

    imguiHelper->setDisplaySize(width, height);

    /*
    位置の設定の表示
    */

    imguiHelper->render(deltaTime, [&](filament::Engine* engine, filament::View* view) {
        ImGui::Begin("Filament UI");
        for (auto& entity : context.entities)
        {
            if (entity_windows.count(entity.second.gltfPath) == 1)
            {
                ImGui::Checkbox(entity.second.gltfPath.c_str(), &entity_windows[entity.second.gltfPath.c_str()]);
                if (entity_windows[entity.second.gltfPath.c_str()]) {
                    std::string entity_label = entity.second.gltfPath;
                    ImGui::Text("position");
                    ImGui::SliderFloat((entity_label + ("_p_x")).c_str(), &entity.second.position.x, -50.0f, 50.0f);
                    ImGui::SliderFloat((entity_label + ("_p_y")).c_str(), &entity.second.position.y, -50.0f, 50.0f);
                    ImGui::SliderFloat((entity_label + ("_p_z")).c_str(), &entity.second.position.z, -50.0f, 50.0f);
                    ImGui::Text("rotation");
                    ImGui::SliderFloat((entity_label + ("_r_x")).c_str(), &entity.second.rotation.x, -6.28f, 6.28f);
                    ImGui::SliderFloat((entity_label + ("_r_y")).c_str(), &entity.second.rotation.y, -6.28f, 6.28f);
                    ImGui::SliderFloat((entity_label + ("_r_z")).c_str(), &entity.second.rotation.z, -6.28f, 6.28f);
                    ImGui::Text("scale");
                    ImGui::SliderFloat((entity_label + ("_s_x")).c_str(), &entity.second.scale.x, -5.0f, 5.0f);
                    ImGui::SliderFloat((entity_label + ("_s_y")).c_str(), &entity.second.scale.y, -5.0f, 5.0f);
                    ImGui::SliderFloat((entity_label + ("_s_z")).c_str(), &entity.second.scale.z, -5.0f, 5.0f);
                }
            }
            else {
                entity_windows[entity.second.gltfPath] = false;
            }
        }
        ImGui::End();
    });

    if (renderer->beginFrame(swapChain)) {
        renderer->render(view);
        renderer->render(imguiView);
        renderer->endFrame();
    }

    for (auto entity : context.entities)
    {
        SetTransform(entity.second);
    }

    return glfwWindowShouldClose(window);
}



//マウスの移動のコールバック
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)xpos, (float)ypos);
}

// マウスボタンの更新コールバック
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (button >= 0 && button < ImGuiMouseButton_COUNT) {
        io.MouseDown[button] = (action == GLFW_PRESS);
    }
}
/*
ウィンドウを作成
*/
void Window::GenWindow(WindowContext& context)
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
    window = glfwCreateWindow(context.width, context.height, context.window.c_str(), nullptr, nullptr);
    if (!window) {
        //glfw停止
        ObserverManager::getInstance().GetLogSubject().notify("WINDOW WAS DEAD");
        glfwTerminate();
        return;
    }
    ObserverManager::getInstance().GetLogSubject().notify("WINDOW CREATED");

    engine = Engine::create(Engine::Backend::VULKAN);
    if (!engine) return;


    //クリア

    filament::math::float4 clearColor = context.color;

    filament::Renderer::ClearOptions options;
    options.clearColor = clearColor;
    options.clear = true;

    //ウィンドウ

    void* nativeWindow = getNativeWindow(window);
    swapChain = engine->createSwapChain(nativeWindow);

    renderer = engine->createRenderer();

    renderer->setClearOptions(options);

    //シーン・レンダラー

    scene = engine->createScene();
    view = engine->createView();

    /*
    カメラ
    */

    Entity cameraEntity = EntityManager::get().create();
    camera = engine->createCamera(cameraEntity);
    camera->setExposure(16.0f, 1 / 125.0f, 100.0f);

    view->setScene(scene);
    view->setCamera(camera);

    entities["camera"] = (cameraEntity);

    Entity lightEntity = EntityManager::get().create();

    filament::LightManager::Builder(filament::LightManager::Type::DIRECTIONAL)
        .color(filament::Color::toLinear(filament::sRGBColor(1.0f, 1.0f, 1.0f)))
        .intensity(100000.0f)
        .direction({ 0.0f, -1.0f, -1.0f })
        .castShadows(true)
        .build(*engine, lightEntity);

    scene->addEntity(lightEntity);

    entities["light"] = (lightEntity);

    /*
    glb読み込み
    */

    AssetConfiguration config;
    config.engine = engine;

    MaterialProvider* materialProvider = createUbershaderProvider(engine, UBERARCHIVE_DEFAULT_DATA, UBERARCHIVE_DEFAULT_SIZE);
    config.materials = materialProvider;

    /*
    glb終了
    */

    /*
    asset作成
    */
    /*
    ファイルアセット部
    */

    for (auto& entity : context.entities) {
        AssetLoader* assetLoader = AssetLoader::create(config);

        std::ifstream file(entity.second.gltfPath, std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(size);

        file.read(reinterpret_cast<char*>(buffer.data()), size);

        entity.second.asset = assetLoader->createAsset(buffer.data(), buffer.size());

        ResourceConfiguration resConfig;
        resConfig.engine = engine;
        resConfig.gltfPath = entity.second.gltfPath.c_str();

        ResourceLoader resourceLoader(resConfig);

        filament::gltfio::TextureProvider* m_stbDecoder = filament::gltfio::createStbProvider(engine);
        filament::gltfio::TextureProvider* m_ktxDecoder = filament::gltfio::createKtx2Provider(engine);
        resourceLoader.addTextureProvider("image/png", m_stbDecoder);
        resourceLoader.addTextureProvider("image/jpeg", m_ktxDecoder);

        bool success = resourceLoader.loadResources(entity.second.asset);

        scene->addEntities(entity.second.asset->getEntities(), entity.second.asset->getEntityCount());

        SetTransform(entity.second);

        entities[entity.first] = entity.second.asset->getRoot();
    }
    /*
    asset終了
    */

    //imguiの作成

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    imguiView = engine->createView();

    imguiHelper = new filagui::ImGuiHelper(engine, imguiView, "APT.ttf");

    //IOの設定

    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}
void Window::SetTransform(HotaruENT& hotaruEnt)
{
    static filament::TransformManager& tcm = engine->getTransformManager();

    auto instance = tcm.getInstance(hotaruEnt.asset->getRoot());

    if (instance) {
        filament::math::mat4f transform = filament::math::mat4f(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        transform *= transform.lookAt(float3(hotaruEnt.position.x, hotaruEnt.position.y, hotaruEnt.position.z), hotaruEnt.position+float3(0,1,0), float3(0.0f, 1.0f, 0.0f));
        transform *= transform.eulerYXZ(hotaruEnt.rotation.x, hotaruEnt.rotation.y, hotaruEnt.rotation.z);
        transform *= transform.scaling(hotaruEnt.scale);
        tcm.setTransform(instance, transform);
    }
}
/*

View* view;
Camera* camera;
Renderer* renderer;
SwapChain* swapChain;
Engine* engine;

filament::View* imguiView;
filagui::ImGuiHelper* imguiHelper;
*/
void Window::Destroy()
{
    for(auto entity : entities) {
        scene->remove(entity.second);
        auto& rcm = engine->getRenderableManager();
        rcm.destroy(entity.second);
        utils::EntityManager::get().destroy(entity.second);
    }
    engine->destroy(view);
    engine->destroy(renderer);
    engine->destroy(scene);
    engine->destroy(swapChain);
}