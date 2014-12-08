#ifndef _DUEL_SCENE_HANDLER_H_
#define _DUEL_SCENE_HANDLER_H_

#include "../scene_mgr.h"
#include "duel_command.h"

namespace ygopro
{
        
    class DuelScene;
    
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
    
    class DuelSceneHandler : public SceneHandler, public Timer<double> {
    public:
        DuelSceneHandler(std::shared_ptr<DuelScene> pscene);
        virtual bool UpdateEvent();
        virtual void BeginHandler();
        template<typename T>
        void SetProtoHandler(T ph) {
            proto_handler = std::static_pointer_cast<DuelProtoHandler>(ph);
        }
        int32_t SolveMessage(uint8_t msg_type, BufferUtil& reader);
        
    protected:
        std::weak_ptr<DuelScene> duel_scene;
        CommandList<DuelCmd> cur_commands;
        std::shared_ptr<DuelProtoHandler> proto_handler;
    };
    
}

#endif
