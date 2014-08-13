#ifndef _COMMAND_H_
#define _COMMAND_H_

template<typename T>
class CommandList {
public:
    virtual ~CommandList() {
        cmd_queue.clear();
    }
    
    virtual void PushCommand(std::shared_ptr<T> cmd) {
        cmd_mutex.lock();
        cmd_queue.push_back(cmd);
        cmd_mutex.unlock();
    }
    
    virtual std::shared_ptr<T> PullCommand() {
        cmd_queue.lock();
        if(cmd_queue.empty()) {
            cmd_mutex.unlock();
            return nullptr;
        }
        auto cmd = cmd_queue.front();
        cmd_queue.pop_front();
        cmd_mutex.unlock();
        return cmd;
    }
    
protected:
    std::mutex cmd_mutex;
    std::list<std::shared_ptr<T>> cmd_queue;
};

#endif
