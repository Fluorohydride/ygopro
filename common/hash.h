#ifndef _HASH32_SHA1_H
#define _HASH32_SHA1_H

#include <memory.h>
#include <string>

#define LEFT_MOST(n) ((n<<24)|(n>>24)|((n<<8)&0xff0000)|((n>>8)&0xff00))
#define LEFT_MOST64(n) ( (n<<56)|(n>>56)|((n&0xff00)<<40)|((n&0xff000000000000)>>40)|    \
        ((n&0xff0000)<<24)|((n&0xff0000000000)>>24)|((n&0xff00000000)>>8)|((n&0xff000000)<<8) )

// use sha1 algorithm
class Hash32_SHA1 {

public:
    Hash32_SHA1() {
        pos = 0;
        length = 0;
        result = 0;
        digest[0] = 0x67452301;
        digest[1] = 0xefcdab89;
        digest[2] = 0x98badcfe;
        digest[3] = 0x10325476;
        digest[4] = 0xc3d2e1f0;
    }

    explicit Hash32_SHA1(std::string& str) {
        pos = 0;
        length = 0;
        result = 0;
        digest[0] = 0x67452301;
        digest[1] = 0xefcdab89;
        digest[2] = 0x98badcfe;
        digest[3] = 0x10325476;
        digest[4] = 0xc3d2e1f0;
        Append(str.c_str(), str.length());
    }

    explicit Hash32_SHA1(const void* data, unsigned int size) {
        pos = 0;
        length = 0;
        result = 0;
        digest[0] = 0x67452301;
        digest[1] = 0xefcdab89;
        digest[2] = 0x98badcfe;
        digest[3] = 0x10325476;
        digest[4] = 0xc3d2e1f0;
        Append(data, size);
    }

    Hash32_SHA1& Append(const void* data, size_t size) {
        unsigned int lcpy;
        unsigned int pbuf = 0;
        unsigned char* buffer = (unsigned char*)data;
        length += size;
        while(pos + size >= 64){
            lcpy = 64 - pos;
            memcpy(&tblock[pos], &buffer[pbuf], lcpy);
            BlockTransform();
            pos = 0;
            size -= lcpy;
            pbuf += lcpy;
        }
        if(size)
            memcpy(&tblock[pos], &buffer[pbuf], size);
        pos += (unsigned int)length;
        return *this;
    }

    unsigned int GetHash() {
        if(result)
            return result;
        unsigned int out[5];
        memset(&tblock[pos], 0, 64 - pos);
        tblock[pos] = 0x80;
        if(pos >= 56){
            BlockTransform();
            memset(tblock, 0, 64);
        }
        length <<= 3;
        length = LEFT_MOST64(length);
        memcpy(&tblock[56], &length, 8);
        BlockTransform();
        out[0] = LEFT_MOST(digest[0]);
        out[1] = LEFT_MOST(digest[1]);
        out[2] = LEFT_MOST(digest[2]);
        out[3] = LEFT_MOST(digest[3]);
        out[4] = LEFT_MOST(digest[4]);
        result = out[0] ^ out[1] ^ out[2] ^ out[3] ^ out[4];
        return result;
    }

    void GetHash(unsigned int out[5]) {
        memset(&tblock[pos], 0, 64 - pos);
        tblock[pos] = 0x80;
        if(pos >= 56){
            BlockTransform();
            memset(tblock, 0, 64);
        }
        length <<= 3;
        length = LEFT_MOST64(length);
        memcpy(&tblock[56], &length, 8);
        BlockTransform();
        out[0] = LEFT_MOST(digest[0]);
        out[1] = LEFT_MOST(digest[1]);
        out[2] = LEFT_MOST(digest[2]);
        out[3] = LEFT_MOST(digest[3]);
        out[4] = LEFT_MOST(digest[4]);
    }

private:

    void BlockTransform()
    {
        unsigned int a = digest[0];
        unsigned int b = digest[1];
        unsigned int c = digest[2];
        unsigned int d = digest[3];
        unsigned int e = digest[4];
        unsigned int block[80];
        unsigned int* pb = (unsigned int *)(tblock);

        for(int i = 0; i < 16; ++i){
            block[i] = LEFT_MOST(pb[i]);
        }
        for(int i = 16; i < 80; ++i){
            unsigned int tmp = block[i - 3] ^ block[i - 8] ^ block[i - 14] ^ block[i - 16];
            block[i] = (tmp << 1) | (tmp >> 31);
        }

#define F(a,b,c,d,e,m) {e+=((a<<5)|(a>>27))+(d^(b&(c^d)))+m+0x5A827999;b=(b<<30)|(b>>2);}
        F (a, b, c, d, e, block[ 0] ); 
        F (e, a, b, c, d, block[ 1] ); 
        F (d, e, a, b, c, block[ 2] ); 
        F (c, d, e, a, b, block[ 3] ); 
        F (b, c, d, e, a, block[ 4] ); 
        F (a, b, c, d, e, block[ 5] ); 
        F (e, a, b, c, d, block[ 6] ); 
        F (d, e, a, b, c, block[ 7] ); 
        F (c, d, e, a, b, block[ 8] ); 
        F (b, c, d, e, a, block[ 9] ); 
        F (a, b, c, d, e, block[10] ); 
        F (e, a, b, c, d, block[11] ); 
        F (d, e, a, b, c, block[12] ); 
        F (c, d, e, a, b, block[13] ); 
        F (b, c, d, e, a, block[14] ); 
        F (a, b, c, d, e, block[15] ); 
        F (e, a, b, c, d, block[16] ); 
        F (d, e, a, b, c, block[17] ); 
        F (c, d, e, a, b, block[18] ); 
        F (b, c, d, e, a, block[19] ); 
#undef F

#define F(a,b,c,d,e,m) {e+=((a<<5)|(a>>27))+(b^c^d)+m+0x6ED9EBA1;b=(b<<30)|(b>>2);}
        F (a, b, c, d, e, block[20] ); 
        F (e, a, b, c, d, block[21] ); 
        F (d, e, a, b, c, block[22] ); 
        F (c, d, e, a, b, block[23] ); 
        F (b, c, d, e, a, block[24] ); 
        F (a, b, c, d, e, block[25] ); 
        F (e, a, b, c, d, block[26] ); 
        F (d, e, a, b, c, block[27] ); 
        F (c, d, e, a, b, block[28] ); 
        F (b, c, d, e, a, block[29] ); 
        F (a, b, c, d, e, block[30] ); 
        F (e, a, b, c, d, block[31] ); 
        F (d, e, a, b, c, block[32] ); 
        F (c, d, e, a, b, block[33] ); 
        F (b, c, d, e, a, block[34] ); 
        F (a, b, c, d, e, block[35] ); 
        F (e, a, b, c, d, block[36] ); 
        F (d, e, a, b, c, block[37] ); 
        F (c, d, e, a, b, block[38] ); 
        F (b, c, d, e, a, block[39] );  
#undef F

#define F(a,b,c,d,e,m) {e+=((a<<5)|(a>>27))+((b&c)|(d&(b|c)))+m+0x8F1BBCDC;b=(b<<30)|(b>>2);}
        F (a, b, c, d, e, block[40] ); 
        F (e, a, b, c, d, block[41] ); 
        F (d, e, a, b, c, block[42] ); 
        F (c, d, e, a, b, block[43] ); 
        F (b, c, d, e, a, block[44] ); 
        F (a, b, c, d, e, block[45] ); 
        F (e, a, b, c, d, block[46] ); 
        F (d, e, a, b, c, block[47] ); 
        F (c, d, e, a, b, block[48] ); 
        F (b, c, d, e, a, block[49] ); 
        F (a, b, c, d, e, block[50] ); 
        F (e, a, b, c, d, block[51] ); 
        F (d, e, a, b, c, block[52] ); 
        F (c, d, e, a, b, block[53] ); 
        F (b, c, d, e, a, block[54] ); 
        F (a, b, c, d, e, block[55] ); 
        F (e, a, b, c, d, block[56] ); 
        F (d, e, a, b, c, block[57] ); 
        F (c, d, e, a, b, block[58] ); 
        F (b, c, d, e, a, block[59] );  
#undef F

#define F(a,b,c,d,e,m) {e+=((a<<5)|(a>>27))+(b^c^d)+m+0xCA62C1D6;b=(b<<30)|(b>>2);}
        F (a, b, c, d, e, block[60] ); 
        F (e, a, b, c, d, block[61] ); 
        F (d, e, a, b, c, block[62] ); 
        F (c, d, e, a, b, block[63] ); 
        F (b, c, d, e, a, block[64] ); 
        F (a, b, c, d, e, block[65] ); 
        F (e, a, b, c, d, block[66] ); 
        F (d, e, a, b, c, block[67] ); 
        F (c, d, e, a, b, block[68] ); 
        F (b, c, d, e, a, block[69] ); 
        F (a, b, c, d, e, block[70] ); 
        F (e, a, b, c, d, block[71] ); 
        F (d, e, a, b, c, block[72] ); 
        F (c, d, e, a, b, block[73] ); 
        F (b, c, d, e, a, block[74] ); 
        F (a, b, c, d, e, block[75] ); 
        F (e, a, b, c, d, block[76] ); 
        F (d, e, a, b, c, block[77] ); 
        F (c, d, e, a, b, block[78] ); 
        F (b, c, d, e, a, block[79] ); 
#undef F

        digest[0] += a;
        digest[1] += b;
        digest[2] += c;
        digest[3] += d;
        digest[4] += e;
    }

    unsigned int digest[5];
    unsigned char tblock[64];
    unsigned int pos;
    unsigned long long length;
    unsigned int result;
};

#endif
