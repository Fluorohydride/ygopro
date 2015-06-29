#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

#include "common.h"

class PacketReader;
class PacketWriter;

class PacketStream {
public:
    virtual void SendPacket(uint16_t proto, void* buffer, size_t sz) {}
    virtual void SendPacket(PacketWriter& pkt) {}
};

class PacketObject {
public:
    virtual void ReadFromPacket(PacketReader& _reader) {}
    virtual void WriteToPacket(PacketWriter& _writer) {}
};

class PacketReader {
    
public:
    PacketReader(void* buffer, size_t buf_size) {
        ptr = (uint8_t*) buffer;
        current = ptr;
        end = ptr + buf_size;
    }
    
    template<typename T>
    T Read() {
        if(std::is_integral<T>::value) {
            uint64_t val = 0;
            uint32_t shift = 0;
            while(current[0] & 0x80) {
                val |= (static_cast<uint64_t>(current[0]) & 0x7f) << shift;
                shift += 7;
                current++;
            }
            val |= static_cast<uint64_t>(current[0]) << shift;
            current++;
            return static_cast<T>(val);
        } else {
            T val;
            memcpy(&val, current, sizeof(T));
            current += sizeof(T);
            return val;
        }
    }
    
    template<typename T>
    PacketReader& operator >> (T& val) {
        val = Read<T>();
        return *this;
    }
    
    PacketReader& operator >> (std::string& val) {
        unsigned char* base = current;
        while((current < end) && *current)
            current++;
        if(current != base) {
            *current = 0;
            val = (char*)base;
        }
        current++;
        return *this;
    }
    
    PacketReader& operator >> (PacketObject& obj) {
        obj.ReadFromPacket(*this);
        return *this;
    }
    
    PacketReader& operator >> (PacketObject* obj) {
        obj->ReadFromPacket(*this);
        return *this;
    }
    
    inline bool IsEnd() {
        return current >= end;
    }
    
private:
    uint8_t* ptr = nullptr;
    uint8_t* current = nullptr;
    uint8_t* end = nullptr;
};

class PacketWriter {
    
public:
    PacketWriter(uint16_t proto) {
        buffer[0] = proto & 0xff;
        buffer[1] = proto >> 8;
        current = &buffer[2];
    }
    
    template<typename T>
    PacketWriter& operator << (T inval) {
        if(std::is_integral<T>::value) {
            uint64_t val = static_cast<uint64_t>(inval);
            while(val > 0x7f) {
                current[0] = (val & 0x7f) | 0x80;
                val >>= 7;
                current++;
            }
            current[0] = val;
            current++;
        } else {
            memcpy(current, &inval, sizeof(T));
            current += sizeof(T);
        }
        return *this;
    }
    
    PacketWriter& operator << (const std::string& str) {
        memcpy(current, str.c_str(), str.length() + 1);
        current += str.length() + 1;
        return *this;
    }
    
    PacketWriter& operator << (std::pair<char*, uint32_t>& buf) {
        memcpy(current, buf.first, buf.second);
        current += buf.second;
        return *this;
    }
    
    PacketWriter& operator << (PacketObject& obj) {
        obj.WriteToPacket(*this);
        return *this;
    }
    
    PacketWriter& operator << (PacketObject* obj) {
        obj->WriteToPacket(*this);
        return *this;
    }
    
    PacketWriter& operator >> (PacketStream& ns) {
        ns.SendPacket(*this);
        return *this;
    }
    
    PacketWriter& operator >> (PacketStream* ns) {
        ns->SendPacket(*this);
        return *this;
    }
    
    uint8_t* PacketBuffer() {
        return buffer;
    }
    
    size_t PacketSize() {
        return current - buffer;
    }
    
private:
    uint8_t buffer[2048];
    uint8_t* current;
    
};

class ConnectionHandler {
public:
    // handle a single packet
    virtual void HandlePacket(uint16_t proto, uint8_t data[], size_t sz) {};
    // events
    virtual void OnConnected() {};
    virtual void OnDisconnected() {};
    virtual void OnConnectError() {};
    virtual void OnConnectTimeOut() {};
};

#endif
