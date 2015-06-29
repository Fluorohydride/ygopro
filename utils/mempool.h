#ifndef _MEM_POOL_H_
#define _MEM_POOL_H_

template<typename T>
class MempoolAllocator {
protected:
    struct ObjectNode {
        T value;
        ObjectNode* next;
    };
public:
    ~MempoolAllocator() { for(auto& iter : blocks) free(iter); }
    template<typename... TR>
    inline T* alloc(TR&&... params) {
        if(node_head == nullptr) {
            ObjectNode* new_block = static_cast<ObjectNode*>(malloc(sizeof(ObjectNode) * alloc_size));
            node_head = new_block;
            blocks.push_back(new_block);
            for(int32_t i = 0; i < alloc_size - 1; ++i) {
                ObjectNode* cur_obj = new_block++;
                cur_obj->next = new_block;
            }
            new_block->next = nullptr;
            alloc_size *= 2;
        }
        T* ptr = new(node_head) T(std::forward<TR>(params)...);
        node_head = node_head->next;
        return ptr;
    }
    inline void recycle(T *obj) {
        obj->~T();
        if(node_head == nullptr) {
            node_head = reinterpret_cast<ObjectNode*>(obj);
            node_head->next = nullptr;
        } else {
            ObjectNode* cur = reinterpret_cast<ObjectNode*>(obj);
            cur->next = node_head;
            node_head = cur;
        }
    }
protected:
    int32_t alloc_size = 32;
    ObjectNode* node_head = nullptr;
    std::vector<void*> blocks;
};

#endif
