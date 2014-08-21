#ifndef _DUEL_SCENE_H_
#define _DUEL_SCENE_H_

#include "../common/tcpclient.h"

#include "scene_mgr.h"

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
        virtual void KeyDown(sgui::KeyEvent evt);
        virtual void KeyUp(sgui::KeyEvent evt);
        
        virtual void OnConnected();
        virtual void OnConnectError();
        virtual void OnConnectTimeOut();
        virtual void OnDisconnected();
        virtual void HandlePacket(unsigned short proto, unsigned char data[], unsigned int size);
        
        void UpdateBackground();
        void UpdateField();
        void UpdateMisc();
        
    protected:
        v2i scene_size = {0, 0};
        unsigned int index_buffer = 0;
        unsigned int field_buffer = 0;
        unsigned int back_buffer = 0;
        unsigned int card_buffer = 0;
        unsigned int misc_buffer = 0;
        unsigned int field_vao = 0;
        unsigned int back_vao = 0;
        unsigned int card_vao = 0;
        unsigned int misc_vao = 0;
        bool update_bg = true;
        bool update_field = true;
        double waiting_time = 0.0;
        CommandList<DuelCommand> duel_commands;
        std::function<void()> current_cb;
        glbase::Shader duel_shader;
    };
    
}

#endif
