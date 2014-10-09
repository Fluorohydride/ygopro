#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

#include <string>

class PacketReader;
class PacketWriter;

class PacketStream {
public:
    virtual void SendPacket(unsigned short proto, void* buffer, unsigned short len) {}
    virtual void SendPacket(PacketWriter& pkt) {}
};

class PacketObject {
public:
    virtual void ReadFromPacket(PacketReader& _reader) {}
    virtual void WriteToPacket(PacketWriter& _writer) {}
};

class PacketReader {
    
public:
    PacketReader(void* buffer, unsigned int buf_size) {
        ptr = (unsigned char*) buffer;
        current = ptr;
        end = ptr + buf_size;
    }
    
    unsigned int ReadUInt32() {
        unsigned int val = 0;
        if(current[0] < 0x80) {
            val = current[0];
            current += 1;
        } else if(current[0] < 0xc0) {
            val = ((unsigned int)current[0] & 0x3f) | ((unsigned int)current[1] << 6);
            current += 2;
        } else if(current[0] < 0xe0) {
            val = ((unsigned int)current[0] & 0x1f) | ((unsigned int)current[1] << 5) | ((unsigned int)current[2] << 13);
            current += 3;
        } else if(current[0] < 0xf0) {
            val = ((unsigned int)current[0] & 0x0f) | ((unsigned int)current[1] << 4) | ((unsigned int)current[2] << 12) | ((unsigned int)current[3] << 20);
            current += 4;
        } else {
            val = *((unsigned int*)(current + 1));
            current += 5;
        }
        return val;
    }
    
    int ReadInt32() {
        return static_cast<int>(ReadUInt32());
    }
    
    float ReadFloat() {
        float val = *((float*)current);
        current += sizeof(float);
        return val;
    }
    
    double ReadDouble() {
        double val = *((double*)current);
        current += sizeof(double);
        return val;
    }
    
    PacketReader& operator >> (unsigned int& val) {
        val = ReadUInt32();
        return *this;
    }
    
    PacketReader& operator >> (int& val) {
        val = ReadInt32();
        return *this;
    }
    
    PacketReader& operator >> (float& val) {
        val = *((float*)current);
        current += sizeof(float);
        return *this;
    }
    
    PacketReader& operator >> (double& val) {
        val = *((double*)current);
        current += sizeof(double);
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
    unsigned char* ptr;
    unsigned char* current;
    unsigned char* end;
};

class PacketWriter {
    
public:
    PacketWriter(unsigned short proto) {
        *(unsigned short*)&buffer[0] = proto;
        current = &buffer[2];
    }
    
    PacketWriter& operator << (unsigned int val) {
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
            *((unsigned int*)(current + 1)) = val;
            current += 5;
        }
        return *this;
    }
    
    PacketWriter& operator << (int val) {
        unsigned int raw = (unsigned int)val;
        return this->operator << (raw);
    }
    
    PacketWriter& operator << (float val) {
        *((float*)current) = val;
        current += sizeof(float);
        return *this;
    }
    
    PacketWriter& operator << (double val) {
        *((double*)current) = val;
        current += sizeof(double);
        return *this;
    }
    
    PacketWriter& operator << (const std::string& str) {
        memcpy(current, str.c_str(), str.length() + 1);
        current += str.length() + 1;
        return *this;
    }
    
    PacketWriter& operator << (std::pair<char*, unsigned int>& buf) {
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
    
    unsigned char* PacketBuffer() {
        return buffer;
    }
    
    unsigned int PacketSize() {
        return current - buffer;
    }
    
private:
    unsigned char buffer[2048];
    unsigned char* current;
    
};

#endif
