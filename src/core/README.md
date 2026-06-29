Game
 └─ IScene
        ├─ MenuScene
        ├─ SettingsScene
        ├─ GameplayScene
        └─ CreditsScene

GameplayScene
 └─ ILevel
        ├─ WorldMapLevel
        ├─ DungeonLevel
        ├─ CaveLevel
        └─ Level1

* Quy tắc ownership của asset group:
Game
    không load asset

Scene
    load/unload asset group

Level
    không biết AssetManager

Entity
    không biết AssetManager

Các scene chịu trách nhiệm load/unload asset groupId trong OnEnter/onExit
scene: Menu, Settings, Gameplay, GameOver, Credits
Gameplay (GameplayScene) chịu trách nhiệm load/unload level group tại OnEnter/onExit
