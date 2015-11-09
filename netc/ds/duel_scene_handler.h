#ifndef _DUEL_SCENE_HANDLER_H_
#define _DUEL_SCENE_HANDLER_H_

#include "utils/command.h"
#include "utils/bufferutil.h"

#include "../scene_mgr.h"

namespace ygopro
{
    
    class DuelScene;
    class FieldBlock;
    class FieldCard;
    class FloatingNumber;
    class InfoPanel;
    
    struct DuelMessage {
        uint8_t msg_type = 0;
        std::vector<uint8_t> msg_buffer;
    };
    
    struct CardPosInfo {
        CardPosInfo() {}
        CardPosInfo(int32_t inf) { info = inf; }
        CardPosInfo(int8_t c, int8_t l, int8_t s, int8_t ps) : controler(c), location(l), sequence(s), position(ps) {}
        
        union {
            struct {
                int8_t controler;
                int8_t location;
                int8_t sequence;
                union {
                    int8_t position;
                    int8_t subsequence;
                };
            };
            int32_t info;
        };
        
        bool operator == (CardPosInfo& p) { return info == p.info; }
    };
    
    extern bool deck_reversed;
    extern int32_t host_player;
    extern std::array<std::shared_ptr<FieldBlock>, 17> field_blocks[2];
    extern std::vector<std::shared_ptr<FieldCard>> m_zone[2];
    extern std::vector<std::shared_ptr<FieldCard>> s_zone[2];
    extern std::vector<std::shared_ptr<FieldCard>> deck[2];
    extern std::vector<std::shared_ptr<FieldCard>> hand[2];
    extern std::vector<std::shared_ptr<FieldCard>> extra[2];
    extern std::vector<std::shared_ptr<FieldCard>> grave[2];
    extern std::vector<std::shared_ptr<FieldCard>> banished[2];
    extern std::array<std::shared_ptr<FloatingNumber>, 23> fixed_numbers[2];
    
    class DuelSceneHandler : public SceneHandler {
    public:
        DuelSceneHandler(DuelScene* pscene);
        ~DuelSceneHandler();
        virtual void BeginHandler();
        virtual void EndHandler();
        virtual bool UpdateHandler();
        virtual void MouseMove(int32_t x, int32_t y);
        virtual void MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseWheel(float deltax, float deltay);
        virtual void KeyDown(int32_t key, int32_t mods);
        virtual void KeyUp(int32_t key, int32_t mods);
        
        void InitField();
        std::shared_ptr<FieldCard> AddCard(uint32_t code, CardPosInfo pos_info);
        std::shared_ptr<FieldCard> GetCard(CardPosInfo pos_info);
        std::shared_ptr<FieldCard> RemoveCard(CardPosInfo pos_info);
        void MoveCard(std::shared_ptr<FieldCard> pcard, CardPosInfo pos_info);
        void ChangePos(std::shared_ptr<FieldCard> pcard, int32_t pos);
        
        inline int32_t LocalPlayer(int32_t pid) { return (host_player == 0) ? pid : (1 - pid); }
        inline int32_t LocalPosInfo(int32_t ipos) { return (host_player == 0) ? ipos : ((ipos & 0xffffff00) | (1 - (ipos & 0xff))); }
        
        void ClearField();
        void InitHp(int32_t pid, int32_t hp);
        void AddChain(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs, int32_t tside, int32_t tzone, int32_t tseq);
        
        template<typename... ACTS>
        void PushMessageActions(ACTS... acts) {
            message_lock = true;
            auto ptr = std::make_shared<ActionSequence<int64_t>>(std::forward<ACTS>(acts)...);
            ptr->Push(std::make_shared<ActionCallback<int64_t>>([this](){ message_lock = false; }));
            SceneMgr::Get() << ptr;
        }

        void PushMessageActions(const std::vector<std::shared_ptr<Action<int64_t>>>& lst) {
            message_lock = true;
            auto ptr = std::make_shared<ActionSequence<int64_t>>();
            for(auto& act : lst)
                ptr->Push(act);
            ptr->Push(std::make_shared<ActionCallback<int64_t>>([this](){ message_lock = false; }));
            SceneMgr::Get() << ptr;
        }
        
        template<typename... ACTS>
        void PushMessageActionsNoLock(ACTS... acts) {
            auto ptr = std::make_shared<ActionSequence<int64_t>>(std::forward<ACTS>(acts)...);
            SceneMgr::Get() << ptr;
        }
        
        void PushMessageActionsNoLock(const std::vector<std::shared_ptr<Action<int64_t>>>& lst) {
            auto ptr = std::make_shared<ActionSequence<int64_t>>();
            for(auto& act : lst)
                ptr->Push(act);
            SceneMgr::Get() << ptr;
        }
        
        void Test(int32_t param);
        int32_t SolveMessage(uint8_t msg_type, BufferReader& reader);
        
    protected:
        DuelScene* duel_scene = nullptr;
        
        bool btnDown[2] = {false};
        v2i btnPos[2];
        v2i click_pos = {0, 0};
        int64_t show_info_time = 0;
        std::weak_ptr<FieldBlock> pre_block;
        std::weak_ptr<FieldCard> pre_card;
        std::shared_ptr<InfoPanel> info_panel;
        std::shared_ptr<LogPanel> log_panel;
        bool message_lock = false;
        CommandListMT<DuelMessage> messages;
    };
    
}

#endif
