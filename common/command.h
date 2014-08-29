#ifndef _COMMAND_H_
#define _COMMAND_H_

template<typename T>
class CommandList {
public:
    virtual ~CommandList() {
        cmd_queue.clear();
    }
    
    virtual void PushCommand(std::shared_ptr<T> cmd) {
        cmd_queue.push_back(cmd);
    }
    
    virtual void InsertCommand(std::shared_ptr<T> cmd) {
        sub_queue.push_back(cmd);
    }
    
    virtual std::shared_ptr<T> PullCommand() {
        if(cmd_queue.empty() && sub_queue.empty())
            return nullptr;
        if(!sub_queue.empty())
            cmd_queue.splice(cmd_queue.begin(), sub_queue, sub_queue.begin(), sub_queue.end());
        return cmd_queue.front();
    }
    
    virtual void PopCommand() {
        if(!cmd_queue.empty())
            cmd_queue.pop_front();
    }
    
    virtual bool IsEmpty() {
        return cmd_queue.empty() && sub_queue.empty();
    }
    
protected:
    std::list<std::shared_ptr<T>> cmd_queue;
    std::list<std::shared_ptr<T>> sub_queue;
};


template<typename T>
class CommandListMT {
public:
    virtual ~CommandListMT() {
        cmd_queue.clear();
    }
    
    virtual void PushCommand(std::shared_ptr<T> cmd) {
        std::lock_guard<std::mutex> lck(cmd_mutex);
        cmd_queue.push_back(cmd);
    }
    
    virtual void InsertCommand(std::shared_ptr<T> cmd) {
        std::lock_guard<std::mutex> lck(cmd_mutex);
        sub_queue.push_back(cmd);
    }
    
    virtual std::shared_ptr<T> PullCommand() {
        std::lock_guard<std::mutex> lck(cmd_mutex);
        if(cmd_queue.empty() && sub_queue.empty())
            return nullptr;
        return cmd_queue.front();
    }
    
    virtual void PopCommand() {
        std::lock_guard<std::mutex> lck(cmd_mutex);
        if(!cmd_queue.empty())
            cmd_queue.pop_front();
    }
    
    virtual bool IsEmpty() {
        std::lock_guard<std::mutex> lck(cmd_mutex);
        return cmd_queue.empty() && sub_queue.empty();
    }
    
protected:
    std::mutex cmd_mutex;
    std::list<std::shared_ptr<T>> cmd_queue;
    std::list<std::shared_ptr<T>> sub_queue;
};

#endif
