#ifndef _GLBASE_H_
#define _GLBASE_H_

#define GLEW_STATIC
#include <GL/glew.h>
#ifdef __WXMAC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef _DEBUG
void GLCheckError(const std::string& file, int line);
#else
#define GLCheckError(x, y)
#endif

namespace glbase {
    
    template<typename T>
	struct vector2 {
		T x;
		T y;
        
		inline void operator += (const vector2<T>& op) {
			x += op.x;
			y += op.y;
		}
        
		inline void operator -= (const vector2<T>& op) {
			x -= op.x;
			y -= op.y;
		}
	};
    
	template<typename T>
	struct vector3 {
		T x;
		T y;
		T z;
        
		inline void operator += (const vector3<T>& op) {
			x += op.x;
			y += op.y;
			z += op.z;
		}
        
		inline void operator -= (const vector3<T>& op) {
			x -= op.x;
			y -= op.y;
			z -= op.z;
		}
        
        inline void operator = (const vector2<T>& op) {
            x = op.x;
            y = op.y;
            z = T();
        }
        
	};
    
    template<typename T>
	struct vector4 {
		T x;
		T y;
		T z;
        T w;
	};
    
    template<typename T>
    inline vector2<T> operator + (const vector2<T>& a, const vector2<T>& b) {
        return vector2<T>{a.x + b.x, a.y + b.y};
    }
    
    template<typename T>
    inline vector2<T> operator - (const vector2<T>& a, const vector2<T>& b) {
        return vector2<T>{a.x - b.x, a.y - b.y};
    }
    
    template<typename T, typename S>
    inline vector2<T> operator * (const vector2<T>& a, S scalar) {
        return vector2<T>{a.x * scalar, a.y * scalar};
    }
    
    template<typename T>
    inline T operator * (const vector2<T>& a, const vector2<T>& b) {
        return a.x * b.x + a.y * b.y;
    }
    
    template<typename T>
    inline vector3<T> operator ^ (const vector2<T>& a, const vector2<T>& b) {
        return vector3<T>{0, 0, a.x * b.y - a.y * b.x};
    }
    
    template<typename T>
    inline vector3<T> operator + (const vector3<T>& a, const vector3<T>& b) {
        return vector3<T>{a.x + b.x, a.y + b.y, a.z + b.z};
    }
    
    template<typename T>
    inline vector3<T> operator - (const vector3<T>& a, const vector3<T>& b) {
        return vector3<T>{a.x - b.x, a.y - b.y, a.z - b.z};
    }
    
    template<typename T>
    inline vector3<T> operator * (const vector3<T>& a, T scalar) {
        return vector3<T>{a.x * scalar, a.y * scalar, a.z * scalar};
    }
    
    template<typename T>
    inline T operator * (const vector3<T>& a, const vector3<T>& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    
    template<typename T>
    inline vector3<T> operator ^ (const vector3<T>& a, const vector3<T>& b) {
        return vector3<T>{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }
    
    template<typename T>
    struct rect {
        T left;
        T top;
        T width;
        T height;
    };
    
    struct quaternion;
    inline quaternion operator * (const quaternion& p, const quaternion& q);
    // q = w + xi + yj + zk
    struct quaternion {
        float w;
        float x;
        float y;
        float z;
        
        quaternion& Normalize() {
            float mag = sqrt(w * w + x * x + y * y + z * z);
            w /= mag;
            x /= mag;
            y /= mag;
            z /= mag;
            return *this;
        }
        
        // normalized
        quaternion& FromAxis(vector3<float> vec, float angle) {
            float mag = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
            float sinA = sinf(angle * 0.5f);
            x = (vec.x / mag * sinA);
            y = (vec.y / mag * sinA);
            z = (vec.z / mag * sinA);
            w = cosf(angle * 0.5f);
            return *this;
        }
        
        // radius
        quaternion& FromEular(vector3<float> eular) {
            float sinx = sinf(eular.x * 0.5f);
            float siny = sinf(eular.y * 0.5f);
            float sinz = sinf(eular.z * 0.5f);
            float cosx = cosf(eular.x * 0.5f);
            float cosy = cosf(eular.y * 0.5f);
            float cosz = cosf(eular.z * 0.5f);
            w = cosx * cosy * cosz + sinx * siny * sinz;
            x = sinx * cosy * cosz - cosx * siny * sinz;
            y = cosx * siny * cosz + sinx * cosy * sinz;
            z = cosx * cosy * sinz - sinx * siny * cosz;
            Normalize();
            return *this;
        }
        
        // quaternion should normalized
        quaternion GetInverse() {
            return quaternion{w, -x, -y, -z};
        }
        
        vector3<float> ApplyToVector(vector3<float> vec) {
            auto q = *this * quaternion{0.0f, vec.x, vec.y, vec.z} * GetInverse();
            return {q.x, q.y, q.z};
        }
        
        // apply all transforms
        vector3<float> ApplyToVector(vector3<float> vec, vector3<float> s, vector3<float> t) {
            auto q = *this * quaternion{0.0f, vec.x, vec.y, vec.z} * GetInverse();
            return {q.x * s.x + t.x, q.y * s.y + t.y, q.z * s.z + t.z};
        }
        
    };
    
    inline quaternion operator + (const quaternion& p, const quaternion& q) {
        return quaternion {p.w + q.w, p.x + q.x, p.y + q.y, p.z + q.z};
    }
    
    inline quaternion operator - (const quaternion& p, const quaternion& q) {
        return quaternion {p.w - q.w, p.x - q.x, p.y - q.y, p.z - q.z};
    }
    
    inline quaternion operator * (const quaternion& p, float s) {
        return quaternion {p.w * s, p.x * s, p.y * s, p.z * s};
    }
    
    // represent p * q
    inline quaternion operator * (const quaternion& p, const quaternion& q) {
        return quaternion {
            p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z,
            p.w * q.x + p.x * q.w + p.z * q.y - p.y * q.z,
            p.w * q.y + p.y * q.w + p.x * q.z - p.z * q.x,
            p.w * q.z + p.z * q.w + p.y * q.x - p.x * q.y
        };
    }
    
    inline quaternion QuaternionSlerp(const quaternion& p, const quaternion& q, float t) {
        float dot = p.w * q.w + p.x * q.x + p.y * q.y + p.z * q.z;
        bool neg = false;
        if(dot <= 0.0f) {
            dot = -dot;
            neg = true;
        }
        float theta = acosf(dot);
        float sinv = sinf(theta);
        float t1 = sinf((1.0f - t) * theta) / sinv;
        float t2 = sinf(t * theta) / sinv;
        if(!neg)
            return p * t1 + q * t2;
        else
            return p * t1 - q * t2;
    }
    
    struct v2ct {
        vector2<float> vertex = {0.0f, 0.0f};
        unsigned int color = 0xffffffff;
        vector2<float> texcoord = {0.0f, 0.0f};
        static const int color_offset = 8;
        static const int tex_offset = 12;
    };
    
    struct v3ct {
        vector3<float> vertex = {0.0f, 0.0f, 0.0f};
        unsigned int color = 0xffffffff;
        vector2<float> texcoord = {0.0f, 0.0f};
        static const int color_offset = 12;
        static const int tex_offset = 16;
    };
    
    struct v3cnt {
        vector3<float> vertex = {0.0f, 0.0f, 0.0f};
        unsigned int color = 0xffffffff;
        vector4<float> normal = {0.0f, 0.0f, 0.0f, 0.0f};
        vector2<float> texcoord = {0.0f, 0.0f};
        static const int color_offset = 12;
        static const int normal_offset = 16;
        static const int tex_offset = 32;
    };
    
    template<int TCOUNT>
	struct TextureInfo {
        vector2<float> vert[TCOUNT];
	};
    
    inline void FillVertex(v2ct* vt, vector2<float> pos, vector2<float> sz, TextureInfo<4>& ti, unsigned int cl = 0xffffffff) {
        vt[0].vertex = {pos.x, pos.y};
        vt[0].texcoord = ti.vert[0];
        vt[0].color = cl;
        vt[1].vertex = {pos.x + sz.x, pos.y};
        vt[1].texcoord = ti.vert[1];
        vt[1].color = cl;
        vt[2].vertex = {pos.x, pos.y + sz.y};
        vt[2].texcoord = ti.vert[2];
        vt[2].color = cl;
        vt[3].vertex = {pos.x + sz.x, pos.y + sz.y};
        vt[3].texcoord = ti.vert[3];
        vt[3].color = cl;
    };
    
    class Shader {
    public:
        ~Shader();
        bool LoadVertShader(const char* buffer);
        bool LoadFragShader(const char* buffer);
        bool Link();
        bool Use();
        void Unuse();
        void Unload();
        void SetParam1i(const char* varname, int value);
        
    protected:
        unsigned int vert_shader = 0;
        unsigned int frag_shader = 0;
        unsigned int program = 0;
    };
    
    class Image {
    public:
        ~Image();
        bool LoadFile(const std::string& file);
        bool LoadMemory(const unsigned char* mem, unsigned int sz);
        inline unsigned char* GetRawData() { return buffer; }
        inline int GetWidth() { return width; }
        inline int GetHeight() { return height; }
        
    protected:
        unsigned char* buffer = nullptr;
        int width = 0;
        int height = 0;
    };
    
    class Texture {
    public:
        
        inline unsigned int texlen(unsigned int len) {
			len = len - 1;
			len = len | (len >> 1);
			len = len | (len >> 2);
			len = len | (len >> 4);
			len = len | (len >> 8);
			len = len | (len >> 16);
			return len + 1;
		}
        
        void Load(const unsigned char* data, int x, int y);
        void Unload();
        void Update(const unsigned char* data, int offx, int offy, int width, int height);
        void Bind();
        inline unsigned int GetTextureId() { return texture_id; }
        TextureInfo<4> GetTextureInfo();
        inline int GetWidth() { return tex_width; }
        inline int GetHeight() { return tex_height; }
        inline int GetImgWidth() { return img_width; }
        inline int GetImgHeight() { return img_height; }
        inline vector2<int> GetSize() { return {tex_width, tex_height}; }
        
    protected:
        unsigned int texture_id = 0;
        int tex_width = 0;
        int tex_height = 0;
        int img_width = 0;
        int img_height = 0;
    };
    
    struct FontGlyph {
        bool loaded = false;
        rect<int> textureRect;
        rect<int> bounds;
        int advance = 0;
    };
    
    class Font {
    public:
        Font();
        ~Font();
        bool Load(const std::string& file, unsigned int sz);
        void Unload();
        const FontGlyph& GetGlyph(unsigned int ch);
        inline Texture& GetTexture() { return char_tex; }
        inline int GetFontSize() { return font_size; }
        
    protected:
        Texture char_tex;
        FontGlyph glyphs[0x10000];
        FT_Face face;
        FT_Library library;
        int font_size = 0;
        int tex_posx = 0;
        int tex_posy = 0;
    };
}

typedef glbase::vector2<int> v2i;
typedef glbase::vector2<float> v2f;
typedef glbase::vector2<double> v2d;
typedef glbase::vector3<int> v3i;
typedef glbase::vector3<float> v3f;
typedef glbase::vector3<double> v3d;
typedef glbase::rect<int> recti;
typedef glbase::rect<float> rectf;
typedef glbase::TextureInfo<4> ti4;

#endif
