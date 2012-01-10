#ifndef BUFFERIO_H
#define BUFFERIO_H

class BufferIO {
public:
	inline static int ReadInt32(char*& p) {
		int ret = *(int*)p;
		p += 4;
		return ret;
	}
	inline static short ReadInt16(char*& p) {
		short ret = *(short*)p;
		p += 2;
		return ret;
	}
	inline static char ReadInt8(char*& p) {
		char ret = *(char*)p;
		p++;
		return ret;
	}
	inline static unsigned char ReadUInt8(char*& p) {
		unsigned char ret = *(unsigned char*)p;
		p++;
		return ret;
	}
	inline static void WriteInt32(char*& p, int val) {
		(*(int*)p) = val;
		p += 4;
	}
	inline static void WriteInt16(char*& p, short val) {
		(*(short*)p) = val;
		p += 2;
	}
	inline static void WriteInt8(char*& p, char val) {
		*p = val;
		p++;
	}
};

#endif //BUFFERIO_H
