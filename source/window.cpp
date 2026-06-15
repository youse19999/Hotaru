#define NOMINMAX
#include <vector>
#include <chrono>
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

float deltaTime = 0;
float gameTime = 0;

auto lastFrameTime = std::chrono::high_resolution_clock::now();

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
    /*
    デルタタイム計算
    */
    auto currentFrameTime = std::chrono::high_resolution_clock::now();

    deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();

    deltaTime = std::min(deltaTime, 0.1f);

    lastFrameTime = currentFrameTime;
    /*
    glfw処理
    */
    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    /*
    描画指定
    */

    imguiView->setViewport({ 0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    
    view->setViewport(filament::Viewport(0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height)));

    camera->setProjection(45.0, static_cast<double>(width) / height, 0.1, 100.0);

    /*
    最小化されてるか確認
    */
    int iconified = glfwGetWindowAttrib(window, GLFW_ICONIFIED);

    if (iconified == GLFW_TRUE) {
        return false;
    }

    imguiHelper->setDisplaySize(width, height);

    ImGuiIO& io = ImGui::GetIO();

    /*
    位置の設定の表示
    */

    imguiHelper->render(1.0f / 60.0, [&](filament::Engine* engine, filament::View* view) {
        ImGui::Begin("Entity List");
        for (auto& entity : context.entities)
        {
            RenderImGUIPos(entity.first, context);
        }
        ImGui::End();

        ImGui::Begin("DBG");
        ImGui::Text(std::format("X: {}, Y: {}, Z: {}", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z).c_str());
        ImGui::Text(std::format("dt: {}",deltaTime).c_str());
        ImGui::Text(std::format("entcnt: {}", engine->getEntityManager().getAliveEntities().size()).c_str());
        ImGui::End();
        GameProcess(context,true);
        ImGui::Begin("ANIM");
        ImGui::End();
    });

    if (renderer->beginFrame(swapChain)) {
        renderer->render(view);
        renderer->render(imguiView);
        renderer->endFrame();
    }

    gameTime += deltaTime;

    return glfwWindowShouldClose(window);
}
void Window::GameProcess(WindowContext& context,bool imGuiEnabled)
{
    //ゲーム内処理
    for (auto& entity : context.entities)
    {
        //初期化されてないなら
        if (entity.second.animationTime < -1)
        {
            entity.second.animationTime = 0;
        }
        SetTransform(entity.second);
        //アニメーション再生
        if (entity.second.factoryType == HotaruENTFactoryType::Model) {
            auto instance = entity.second.asset->getInstance();
            gltfio::Animator* animator = instance->getAnimator();

            if (animator->getAnimationCount() > 0) {
                animator->applyAnimation(1, entity.second.animationTime);
                animator->updateBoneMatrices();
                if (imGuiEnabled) {
                    ImGui::Text(std::format("{} t: {}", entity.first, entity.second.animationTime).c_str());
                }
            }

            entity.second.animationTime += deltaTime;
        }
    }
}
void Window::RenderImGUIPos(std::string entityID,WindowContext context)
{
    if (entity_windows.count(entityID) == 1)
    {
        ImGui::Checkbox(entityID.c_str(), &entity_windows[entityID]);
        if (entity_windows[entityID]) {
            std::string entity_label = entityID;
            ImGui::Text("position");
            ImGui::SliderFloat((entity_label + ("_p_x")).c_str(), &context.entities[entityID].position.x, -50.0f, 50.0f);
            ImGui::SliderFloat((entity_label + ("_p_y")).c_str(), &context.entities[entityID].position.y, -50.0f, 50.0f);
            ImGui::SliderFloat((entity_label + ("_p_z")).c_str(), &context.entities[entityID].position.z, -50.0f, 50.0f);
            ImGui::Text("rotation");
            ImGui::SliderFloat((entity_label + ("_r_x")).c_str(), &context.entities[entityID].rotation.x, -6.28f, 6.28f);
            ImGui::SliderFloat((entity_label + ("_r_y")).c_str(), &context.entities[entityID].rotation.y, -6.28f, 6.28f);
            ImGui::SliderFloat((entity_label + ("_r_z")).c_str(), &context.entities[entityID].rotation.z, -6.28f, 6.28f);
            ImGui::Text("scale");
            ImGui::SliderFloat((entity_label + ("_s_x")).c_str(), &context.entities[entityID].scale.x, -5.0f, 5.0f);
            ImGui::SliderFloat((entity_label + ("_s_y")).c_str(), &context.entities[entityID].scale.y, -5.0f, 5.0f);
            ImGui::SliderFloat((entity_label + ("_s_z")).c_str(), &context.entities[entityID].scale.z, -5.0f, 5.0f);
        }
    }
    else {
        entity_windows[entityID] = false;
    }
}
Camera* Window::GetCamera()
{
    return camera;
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
        return;
    }
    ObserverManager::getInstance().GetLogSubject().notify("WINDOW CREATED");

    engine = Engine::create(Engine::Backend::VULKAN);
    if (!engine) return;

    //ウィンドウ

    void* nativeWindow = getNativeWindow(window);
    swapChain = engine->createSwapChain(nativeWindow);

    renderer = engine->createRenderer();
}

void Window::GenEngine(WindowContext& context)
{

    //クリア

    filament::math::float4 clearColor = context.color;

    filament::Renderer::ClearOptions options;
    options.clearColor = clearColor;
    options.clear = true;

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

        //ファクトリ部分
        if (entity.second.factoryType == HotaruENTFactoryType::Light)
        {
            Entity lightEntity = EntityManager::get().create();

            //filament側で光を作る。
            auto result = filament::LightManager::Builder(entity.second.lightType)
                .position(float3(entity.second.position))
                .intensity(200000.0f, LightManager::EFFICIENCY_LED)//todo 調節可能にする。
                .falloff(500000.0f)
                .castShadows(true)//todo 調節可能にする。
                .color(filament::Color::toLinear(filament::sRGBColor(1.0f, 1.0f, 1.0f)))//todo 調節可能にする。
                .build(*engine, lightEntity);

            scene->addEntity(lightEntity);

            //エンティティの構造体のentityのnullを埋める。
            entity.second.entity = lightEntity;

            entities[entity.first] = (lightEntity);
        }
        if (entity.second.factoryType == HotaruENTFactoryType::Model)
        {
            //アセットローダーを取得 staticで軽量化
            static AssetLoader* assetLoader = AssetLoader::create(config);

            //ファイルを入力
            std::ifstream file(entity.second.gltfPath, std::ios::binary | std::ios::ate);//todo 例外処理がない
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            if (size == -1)
            {
                ObserverManager::getInstance().GetLogSubject().notify(entity.second.gltfPath + " IS NOT FOUND");
            }

            //ここで読み込み失敗してると、-1をアロケートして失敗する。
            std::vector<uint8_t> buffer(size);

            //データを読み込む。
            file.read(reinterpret_cast<char*>(buffer.data()), size);

            //assetを割り当て
            entity.second.asset = assetLoader->createAsset(buffer.data(), buffer.size());\

            //リソースの位置を決定。
            ResourceConfiguration resConfig;
            resConfig.engine = engine;
            resConfig.gltfPath = entity.second.gltfPath.c_str();//構造体から読み込み

            ResourceLoader resourceLoader(resConfig);

            //テクスチャを読み込み
            /*
            ・png
            ・jpeg
            に対応。
            */
            filament::gltfio::TextureProvider* m_stbDecoder = filament::gltfio::createStbProvider(engine);
            filament::gltfio::TextureProvider* m_ktxDecoder = filament::gltfio::createKtx2Provider(engine);
            resourceLoader.addTextureProvider("image/png", m_stbDecoder);
            resourceLoader.addTextureProvider("image/jpeg", m_ktxDecoder);

            //リソースを読み込み
            bool success = resourceLoader.loadResources(entity.second.asset);

            //Entityを追加。
            scene->addEntities(entity.second.asset->getEntities(), entity.second.asset->getEntityCount());

            //トランスフォームを設定
            SetTransform(entity.second);

            //
            entities[entity.first] = entity.second.asset->getRoot();
        }
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

    TransformManager::Instance instance;
    /*
    ファクトリごとに受け取り方が違う
    */
    if (hotaruEnt.factoryType == HotaruENTFactoryType::Model)
    {
        instance = tcm.getInstance(hotaruEnt.asset->getRoot());
    }
    if (hotaruEnt.factoryType == HotaruENTFactoryType::Light)
    {
        instance = tcm.getInstance(hotaruEnt.entity);
    }
    //tcmがない場合
    if (!instance) {
        tcm.create(hotaruEnt.entity);
    }
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
トランスフォーム用の統一した処理
*/
void Window::SetTransform(HotaruENT& hotaruEnt,Entity entity)
{
    static filament::TransformManager& tcm = engine->getTransformManager();

    auto instance = tcm.getInstance(entity);

    if (instance) {
        filament::math::mat4f transform = filament::math::mat4f(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        transform *= transform.lookAt(float3(hotaruEnt.position.x, hotaruEnt.position.y, hotaruEnt.position.z), hotaruEnt.position + float3(0, 1, 0), float3(0.0f, 1.0f, 0.0f));
        transform *= transform.eulerYXZ(hotaruEnt.rotation.x, hotaruEnt.rotation.y, hotaruEnt.rotation.z);
        transform *= transform.scaling(hotaruEnt.scale);
        tcm.setTransform(instance, transform);
    }
}
std::vector<utils::Entity> Window::GetChildren(Entity entity)
{
    auto& tcm = engine->getTransformManager();
    size_t childCount = tcm.getChildCount(tcm.getInstance(entity));
    std::vector<utils::Entity> children(childCount);
    tcm.getChildren(tcm.getInstance(entity), children.data(), childCount);
    return children;
}
/*
再帰的に子オブジェクトを消す。
*/
void Window::RemoveChildrenEntity(Entity entity)
{
    auto& rcm = engine->getRenderableManager();
    for (auto child : GetChildren(entity))
    {
        scene->remove(child);
        auto& rcm = engine->getRenderableManager();
        rcm.destroy(child);
        utils::EntityManager::get().destroy(child);
        RemoveChildrenEntity(child);
    }
}
/*
エンティティを削除
*/
void Window::DestroyEntity(Entity entity)
{
    RemoveChildrenEntity(entity);
}
/*
解放処理
*/
void Window::Destroy()
{
    for(auto entity : entities) {
        //ちゃんと解放されるようになりました。
        RemoveChildrenEntity(entity.second);
    }
    engine->destroy(view);
    engine->destroy(renderer);
    engine->destroy(scene);
    engine->destroy(swapChain);

    glfwTerminate();
}