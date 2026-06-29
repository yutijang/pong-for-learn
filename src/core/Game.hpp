#pragma once

#include "resources/AssetManager.hpp"
#include "resources/AudioManager.hpp"
#include "save/SaveManager.hpp"
#include "scenes/IScene.hpp"
#include "scenes/SceneRequest.hpp"

#include <memory>

class Game {
  public:
    bool initializer();
    void run();
    void shutdown();

  private:
    // Scene phát yêu cầu, Game quyết định thực hiện
    void processSceneRequest();

    [[nodiscard]]
    std::unique_ptr<IScene> createScene(SceneId sceneId);

    AssetManager m_assetManager;
    SaveManager m_saveManager{"saves"};
    AudioManager m_audioManager{m_assetManager};
    std::unique_ptr<IScene> m_currentScene;

    bool m_running{true};
};
