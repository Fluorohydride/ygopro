#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "utils/singleton.h"
#include "utils/action.h"

#include "render_util.h"

namespace ygopro
{

    class SceneHandler {
    public:
        virtual void BeginHandler() = 0;
        virtual bool UpdateHandler() = 0;
        virtual void MouseMove(int32_t x, int32_t y) = 0;
        virtual void MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y) = 0;
        virtual void MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y) = 0;
        virtual void MouseWheel(float deltax, float deltay) = 0;
        virtual void KeyDown(int32_t key, int32_t mods) = 0;
        virtual void KeyUp(int32_t key, int32_t mods) = 0;
    };
    
    class Scene {
    public:
        virtual ~Scene() = default;
        virtual void Activate() = 0;
        virtual void Terminate() = 0;
        virtual bool Update() = 0;
        virtual bool Draw() = 0;
        virtual void SetSceneSize(v2i sz) = 0;
        virtual recti GetScreenshotClip() = 0;
        virtual bool IsActive() { return is_active; }
        virtual void Exit() { is_active = false; }
        
        inline void SetSceneHandler(std::shared_ptr<SceneHandler> sh) { scene_handler = sh; }
        inline std::shared_ptr<SceneHandler> GetSceneHandler() { return scene_handler; }
        
    protected:
        std::shared_ptr<SceneHandler> scene_handler;
        bool is_active = true;
    };
    
    class SceneMgr : public base::FrameControler, public ActionMgr<int64_t>, public Singleton<SceneMgr> {
    public:
        
        void Init();
        void Uninit();
        bool Update();
        bool Draw();
        void SetSceneSize(v2i sz);
        inline v2i GetSceneSize() { return scene_size; }
        void ScreenShot();
        inline void SetMousePosition(v2i pos) { mouse_pos = pos; }
        inline v2i GetMousePosition() { return mouse_pos; }
        
        void SetScene(std::shared_ptr<Scene> sc);
        inline std::shared_ptr<Scene> GetScene() { return current_scene; };
        
    protected:
        v2i scene_size = {1, 1};
        v2i mouse_pos = {0, 0};
        std::shared_ptr<Scene> current_scene = nullptr;
        
    };

}

#endif
