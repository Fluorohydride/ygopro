#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

#include <string>

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
            uint32_t val = 0;
            if(current[0] < 0x80) {
                val = current[0];
                current += 1;
            } else if(current[0] < 0xc0) {
                val = ((uint32_t)current[0] & 0x3f) | ((uint32_t)current[1] << 6);
                current += 2;
            } else if(current[0] < 0xe0) {
                val = ((uint32_t)current[0] & 0x1f) | ((uint32_t)current[1] << 5) | ((uint32_t)current[2] << 13);
                current += 3;
            } else if(current[0] < 0xf0) {
                val = ((uint32_t)current[0] & 0x0f) | ((uint32_t)current[1] << 4) | ((uint32_t)current[2] << 12) | ((uint32_t)current[3] << 20);
                current += 4;
            } else {
                val = *((uint32_t*)(current + 1));
                current += 5;
            }
            return static_cast<T>(val);
        } else {
            T val = *((T*)current);
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
    
    PacketReader& operator >> (PacketObject& val) {
        val.ReadFromPacket(*this);
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
        *(uint16_t*)&buffer[0] = proto;
        current = &buffer[2];
    }
    
    template<typename T>
    PacketWriter& operator << (T inval) {
        if(std::is_integral<T>::value) {
            uint32_t val = static_cast<uint32_t>(inval);
            if(val < 0x80) {
                current[0] = val;
                current += 1;
            } else if(val < 0x4000) {
                current[0] = (val & 0x3f) | 0x80;
                current[1] = val >> 6;
                current += 2;
            } else if(val < 0x200000) {
                current[0] = (val & 0x1f) | 0xc0;
                current[1] = val >> 5;
                current[2] = val >> 13;
                current += 3;
            } else if(val < 0x10000000) {
                current[0] = (val & 0xf) | 0xe0;
                current[1] = val >> 4;
                current[2] = val >> 12;
                current[3] = val >> 20;
                current += 4;
            } else {
                current[0] = 0xff;
                *((uint32_t*)(current + 1)) = val;
                current += 5;
            }
        } else {
            *((T*)current) = inval;
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
