#ifndef _DUEL_SCENE_HANDLER_H_
#define _DUEL_SCENE_HANDLER_H_

#include "utils/command.h"
#include "utils/bufferutil.h"

#include "../scene_mgr.h"

namespace ygopro
{
    
    class DuelCmd;
    class DuelScene;
    class FieldBlock;
    class FieldCard;
    
    class DuelProtoHandler {
    public:
        virtual void BeginProto() = 0;
        virtual void EndProto() = 0;
        virtual bool IsEnd() = 0;
        virtual void ProcessMsg(uint32_t sz);
        virtual int32_t MessageToCmd(uint8_t msg_type, BufferUtil& reader);
        
        template<typename T>
        void PushCommand(T cmd) {
            std::unique_lock<std::mutex> lck(cmd_mtx);
            if(cmd == nullptr)
                return;
            cur_command = std::static_pointer_cast<DuelCmd>(cmd);
            cmd_notifier.wait(lck);
        }
        
        std::shared_ptr<DuelCmd> GetCommand() {
            std::unique_lock<std::mutex> lck(cmd_mtx);
            return cur_command;
        }
        
        void EndCommand() {
            std::unique_lock<std::mutex> lck(cmd_mtx);
            if(cur_command == nullptr)
                return;
            cur_command = nullptr;
            cmd_notifier.notify_one();
        }
        
    protected:
        std::vector<uint8_t> msg_buffer;
        std::shared_ptr<DuelCmd> cur_command;
        std::mutex cmd_mtx;
        std::condition_variable cmd_notifier;
    };
    
    class DuelSceneHandler : public SceneHandler {
    public:
        DuelSceneHandler(std::shared_ptr<DuelScene> pscene);
        virtual void BeginHandler();
        virtual bool UpdateHandler();
        virtual void MouseMove(int32_t x, int32_t y);
        virtual void MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseWheel(float deltax, float deltay);
        virtual void KeyDown(int32_t key, int32_t mods);
        virtual void KeyUp(int32_t key, int32_t mods);
        
        template<typename T>
        void SetProtoHandler(T ph) {
            proto_handler = std::static_pointer_cast<DuelProtoHandler>(ph);
        }
        int32_t SolveMessage(uint8_t msg_type, BufferUtil& reader);
        
    protected:
        std::weak_ptr<DuelScene> duel_scene;
        CommandList<DuelCmd> cur_commands;
        std::shared_ptr<DuelProtoHandler> proto_handler;
        
        bool btnDown[2] = {false};
        v2i btnPos[2];
        std::weak_ptr<FieldBlock> pre_block;
        std::weak_ptr<FieldCard> pre_card;
        v2i click_pos = {0, 0};
    };
    
}

#endif
