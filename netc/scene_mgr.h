#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "glbase.h"
#include "sungui.h"
#include "xml_config.h"

namespace ygopro
{
 
    class InputHandler {
    public:
        virtual bool Update() = 0;
        virtual void MouseMove(sgui::MouseMoveEvent evt) = 0;
        virtual void MouseButtonDown(sgui::MouseButtonEvent evt) = 0;
        virtual void MouseButtonUp(sgui::MouseButtonEvent evt) = 0;
        virtual void MouseWheel(sgui::MouseWheelEvent evt) = 0;
        virtual void KeyDown(sgui::KeyEvent evt) = 0;
        virtual void KeyUp(sgui::KeyEvent evt) = 0;

    };
    
    class Scene {
    public:
        virtual ~Scene() = default;
        virtual void Activate() = 0;
        virtual bool Update() = 0;
        virtual void Draw() = 0;
        virtual void SetSceneSize(v2i sz) = 0;
        virtual recti GetScreenshotClip() = 0;
        
        void SetInputHandler(std::shared_ptr<InputHandler> ih) {
            input_handler = ih;
        }
        
        std::shared_ptr<InputHandler> GetInputHandler() {
            return input_handler;
        }
        
    protected:
        std::shared_ptr<InputHandler> input_handler;
    };
    
    class SceneMgr : public Singleton<SceneMgr> {
    public:
        
        void Init(const std::wstring& layout);
        void Uninit();
        void InitDraw();
        bool Update();
        void Draw();
        inline double GetGameTime() { return now; }
        void SetFrameRate(double rate);
        void CheckFrameRate();
        void SetSceneSize(v2i sz);
        void SetScene(std::shared_ptr<Scene> sc);
        std::shared_ptr<InputHandler> GetInputHandler();
        void ScreenShot();
        std::shared_ptr<Scene> GetScene() { return current_scene; };
        void SetMousePosition(v2i pos) { mouse_pos = pos; }
        v2i GetMousePosition() { return mouse_pos; }
        rectf LayoutRectConfig(const std::string& name) { return rect_config[name]; }
        int LayoutIntConfig(const std::string& name) { return int_config[name]; }
        float LayoutFloatConfig(const std::string& name) { return float_config[name]; }
        
    protected:
        v2i scene_size;
        unsigned long long start_time = 0;
        double now = 0.0;
        std::shared_ptr<Scene> current_scene = nullptr;
        double frame_check = 0.0;
        double frame_time = 0.0;
        double frame_interval = 0.0;
        v2i mouse_pos = {0, 0};
        std::unordered_map<std::string, rectf> rect_config;
        std::unordered_map<std::string, int> int_config;
        std::unordered_map<std::string, float> float_config;
    };

	extern CommonConfig commonCfg;
    extern CommonConfig stringCfg;
}

#endif
