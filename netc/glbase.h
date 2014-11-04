#ifndef _GLBASE_H_
#define _GLBASE_H_

#define GLEW_STATIC
#include <GL/glew.h>
#ifdef __MACH__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef _DEBUG
void GLCheckError(const std::string& file, int32_t line);
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
    inline vector3<T> operator + (const vector3<T>& a, const vector3<T>& b) {
        return vector3<T>{a.x + b.x, a.y + b.y, a.z + b.z};
    }
    
    template<typename T>
    inline vector3<T> operator - (const vector3<T>& a, const vector3<T>& b) {
        return vector3<T>{a.x - b.x, a.y - b.y, a.z - b.z};
    }
    
    template<typename T, typename S>
    inline vector3<T> operator * (const vector3<T>& a, S scalar) {
        return vector3<T>{a.x * scalar, a.y * scalar, a.z * scalar};
    }
    
    template<typename T>
    struct rect {
        T left;
        T top;
        T width;
        T height;
    };
    
    struct v2ct {
        vector2<float> vertex = {0.0f, 0.0f};
        uint32_t color = 0xffffffff;
        vector2<float> texcoord = {0.0f, 0.0f};
        static const int32_t color_offset = 8;
        static const int32_t tex_offset = 12;
    };
    
    struct v3ct {
        vector3<float> vertex = {0.0f, 0.0f, 0.0f};
        uint32_t color = 0xffffffff;
        vector2<float> texcoord = {0.0f, 0.0f};
        static const int32_t color_offset = 12;
        static const int32_t tex_offset = 16;
    };
    
    struct v3hct {
        vector3<float> vertex = {0.0f, 0.0f, 0.0f};
        uint32_t color = 0xffffffff;
        uint32_t hcolor = 0;
        vector2<float> texcoord = {0.0f, 0.0f};
        static const int32_t color_offset = 12;
        static const int32_t hcolor_offset = 16;
        static const int32_t tex_offset = 20;
    };
    
    struct v3cnt {
        vector3<float> vertex = {0.0f, 0.0f, 0.0f};
        uint32_t color = 0xffffffff;
        vector3<float> normal = {0.0f, 0.0f, 0.0f};
        vector2<float> texcoord = {0.0f, 0.0f};
        static const int32_t color_offset = 12;
        static const int32_t normal_offset = 16;
        static const int32_t tex_offset = 32;
    };
    
    template<int32_t TCOUNT>
	struct TextureInfo {
        vector2<float> vert[TCOUNT];
	};
    
    inline void FillVertex(v2ct* vt, vector2<float> pos, vector2<float> sz, TextureInfo<4>& ti, uint32_t cl = 0xffffffff) {
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
        void Unload();
        void SetParam1i(const char* varname, const int32_t value);
        void SetParamMat4(const char* varname, const float m[]);
        
    public:
        static Shader& GetDefaultShader(bool reload = false);
        static void Unuse();
        
    protected:
        uint32_t vert_shader = 0;
        uint32_t frag_shader = 0;
        uint32_t program = 0;
    };
    
    class Image {
    public:
        ~Image();
        bool LoadFile(const std::string& file);
        bool LoadMemory(const uint8_t* mem, uint32_t sz);
        inline uint8_t* GetRawData() { return buffer; }
        inline int32_t GetWidth() { return width; }
        inline int32_t GetHeight() { return height; }
        
    protected:
        uint8_t* buffer = nullptr;
        int32_t width = 0;
        int32_t height = 0;
    };
    
    class Texture {
    public:
        
        inline uint32_t texlen(uint32_t len) {
			len = len - 1;
			len = len | (len >> 1);
			len = len | (len >> 2);
			len = len | (len >> 4);
			len = len | (len >> 8);
			len = len | (len >> 16);
			return len + 1;
		}
        
        void Load(const uint8_t* data, int32_t x, int32_t y);
        void Unload();
        void Update(const uint8_t* data, int32_t offx, int32_t offy, int32_t width, int32_t height);
        void Bind();
        inline uint32_t GetTextureId() { return texture_id; }
        TextureInfo<4> GetTextureInfo();
        inline int32_t GetWidth() { return tex_width; }
        inline int32_t GetHeight() { return tex_height; }
        inline int32_t GetImgWidth() { return img_width; }
        inline int32_t GetImgHeight() { return img_height; }
        inline vector2<int32_t> GetSize() { return {tex_width, tex_height}; }
        
    protected:
        uint32_t texture_id = 0;
        int32_t tex_width = 0;
        int32_t tex_height = 0;
        int32_t img_width = 0;
        int32_t img_height = 0;
    };
    
    struct FontGlyph {
        bool loaded = false;
        rect<int32_t> textureRect;
        rect<int32_t> bounds;
        int32_t advance = 0;
    };
    
    class Font {
    public:
        Font();
        ~Font();
        bool Load(const std::string& file, uint32_t sz);
        void Unload();
        const FontGlyph& GetGlyph(uint32_t ch);
        inline Texture& GetTexture() { return char_tex; }
        inline int32_t GetFontSize() { return font_size; }
        
    protected:
        Texture char_tex;
        FontGlyph glyphs[0x10000];
        FT_Face face;
        FT_Library library;
        int32_t font_size = 0;
        int32_t tex_posx = 0;
        int32_t tex_posy = 0;
    };
}

typedef glbase::vector2<int32_t> v2i;
typedef glbase::vector2<float> v2f;
typedef glbase::vector3<int32_t> v3i;
typedef glbase::vector3<float> v3f;
typedef glbase::rect<int32_t> recti;
typedef glbase::rect<float> rectf;
typedef glbase::TextureInfo<4> ti4;

#endif
