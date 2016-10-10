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
    class FieldSprite;
    class FloatingNumber;
    class FloatingSprite;
    class LogPanel;
    
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
        std::wstring ToString();
    };
    
    struct LocalPlayerData {
        std::array<std::shared_ptr<FieldBlock>, 17> field_blocks;
        std::vector<std::shared_ptr<FieldCard>> m_zone;
        std::vector<std::shared_ptr<FieldCard>> s_zone;
        std::vector<std::shared_ptr<FieldCard>> deck;
        std::vector<std::shared_ptr<FieldCard>> hand;
        std::vector<std::shared_ptr<FieldCard>> extra;
        std::vector<std::shared_ptr<FieldCard>> grave;
        std::vector<std::shared_ptr<FieldCard>> banished;
        std::array<std::shared_ptr<FloatingNumber>, 22> fixed_numbers;
        std::shared_ptr<FloatingSprite> lpbar;
        int32_t lp = 0;
        int32_t extra_faceup_count = 0;
        std::array<v2i, 4> lp_verts_rel;
        std::array<v2f, 4> lp_verts_prop;
        std::wstring name;
    };
    
    enum class FloatingNumberType {
        DeckCount = 0,
        GraveCount,
        BanishCount,
        ExtraCount,
        LScale,
        RScale,
        Star1,
        Star2,
        Star3,
        Star4,
        Star5,
        ATK1,
        ATK2,
        ATK3,
        ATK4,
        ATK5,
        DEF1,
        DEF2,
        DEF3,
        DEF4,
        DEF5,
        LP,
    };
    
    struct LocalChainInfo {
        bool solved = false;
        uint32_t code = 0;
        uint32_t desc = 0;
        FieldCard* chaining_card = nullptr;
        CardPosInfo triggering_pos;
        std::vector<FieldSprite*> chain_sprites;
    };
    
    struct LocalDuelData {
        bool deck_reversed = false;
        int32_t turn_count = 0;
        int32_t host_player = 0;
        uint32_t disabled_field = 0;
        bool last_chain = false;
        std::vector<LocalChainInfo> chains;
        std::vector<FieldSprite*> attack_sprite;
    };
    
    extern LocalDuelData g_duel;
    extern LocalPlayerData g_player[2];
    
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
        
        inline int32_t LocalPlayer(int32_t pid) { return (g_duel.host_player == 0) ? pid : (1 - pid); }
        inline int32_t LocalPosInfo(int32_t ipos) { return (g_duel.host_player == 0) ? ipos : ((ipos & 0xffffff00) | (1 - (ipos & 0xff))); }
        
        void ClearField();
        void SetLP(int32_t pid, int32_t lp);
        void AddChain(int32_t ct);
        void SetDisabledField(uint32_t fd_info);
        std::shared_ptr<FieldSprite> AddAttackSprite(v3f trans, glm::quat rot);
        void ClearAttackSprite();
        
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
        std::shared_ptr<LogPanel> log_panel;
        bool message_lock = false;
        CommandListMT<DuelMessage> messages;
        std::vector<uint8_t> last_operation_message;
    };
    
}

#endif
