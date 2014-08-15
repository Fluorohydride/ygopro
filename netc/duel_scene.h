#ifndef _DUEL_SCENE_H_
#define _DUEL_SCENE_H_

#include "../common/tcpclient.h"

#include "scene_mgr.h"
#include "gui_extra.h"
#include "duel_command.h"

namespace ygopro
{
    
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
        
    protected:
        v2i scene_size = {0, 0};
        unsigned int index_buffer = 0;
        unsigned int field_buffer = 0;
        unsigned int back_buffer = 0;
        unsigned int card_buffer = 0;
        unsigned int misc_buffer = 0;
        double waiting_time = 0.0;
        CommandList<DuelCommand> duel_commands;
        std::function<void()> current_cb;
        
    };
    
}

#endif
