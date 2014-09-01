#ifndef _DUEL_SCENE_H_
#define _DUEL_SCENE_H_

#include "../common/tcpclient.h"

#include "glbase.h"
#include "scene_mgr.h"
#include "duel_field.h"

namespace ygopro
{
    
    struct DuelCommand;
    
    struct v3hct {
        v3f vertex = {0.0f, 0.0f, 0.0f};
        unsigned int color = 0xffffffff;
        unsigned int hcolor = 0;
        v2f texcoord = {0.0f, 0.0f};
        static const int color_offset = 12;
        static const int hcolor_offset = 16;
        static const int tex_offset = 20;
    };
    
    struct CameraParam {
        float fovy = 3.1415926f * 0.25f;
        float cnear = 0.1f;
        float cfar = 100.0f;
        float angle = 3.1415926f * 0.25f;
        float radius = 7.0f;
        float xoffset = 0.0f;
        float yoffset = 0.0f;
        float cameray = 0.0f;
        float cameraz = 0.0f;
        float scrx = 0.0f;
        float scry = 0.0f;
        glm::mat4 mvp;
        glm::quat hand_quat;
    };
    
    class DuelScene : public Scene, public TcpClientSeed {
    public:
        DuelScene();
        virtual ~DuelScene();
        virtual void Activate();
        virtual bool Update();
        virtual void Draw();
        virtual void SetSceneSize(v2i sz);
        virtual recti GetScreenshotClip();
        virtual void MouseMove(sgui::MouseMoveEvent evt);
        virtual void MouseButtonDown(sgui::MouseButtonEvent evt);
        virtual void MouseButtonUp(sgui::MouseButtonEvent evt);
        virtual void MouseWheel(sgui::MouseWheelEvent evt);
        virtual void KeyDown(sgui::KeyEvent evt);
        virtual void KeyUp(sgui::KeyEvent evt);
        
        virtual void OnConnected();
        virtual void OnConnectError();
        virtual void OnConnectTimeOut();
        virtual void OnDisconnected();
        virtual void HandlePacket(unsigned short proto, unsigned char data[], unsigned int size);
        
        void UpdateParams();
        void UpdateBackground();
        void UpdateField();
        void UpdateMisc();
        
        std::pair<int, int> GetHoverPos(int posx, int posy);
        
    protected:
        v2i scene_size = {0, 0};
        unsigned int index_buffer = 0;
        unsigned int card_index_buffer = 0;
        unsigned int field_buffer = 0;
        unsigned int back_buffer = 0;
        unsigned int card_buffer = 0;
        unsigned int misc_buffer = 0;
        unsigned int field_vao = 0;
        unsigned int back_vao = 0;
        unsigned int card_vao = 0;
        unsigned int misc_vao = 0;        
        bool update_bg = true;
        bool update_misc = true;
        CommandList<DuelCommand> duel_commands;
        glbase::Shader duel_shader;
        std::pair<int, int> hover_pos;
        std::pair<int, int> click_pos;
        std::weak_ptr<FieldObject> hover_obj;
        DuelField field;
        std::list<std::weak_ptr<FieldObject>> updating_blocks;
        CameraParam camera;
        bool btnDown[2] = {false};
        v2i btnPos[2];
    };
    
}

#endif
