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

#ifndef _DEBUG
#define _DEBUG
#endif

#ifdef _DEBUG
void GLCheckError(const std::string& file, int32_t line);
#else
#define GLCheckError(x, y)
#endif

namespace base {
    
    template<typename T>
	struct vector2 {
		T x;
		T y;
        inline void operator += (const vector2<T>& op) { x += op.x; y += op.y; }
		inline void operator -= (const vector2<T>& op) { x -= op.x; y -= op.y; }
        inline bool operator == (const vector2<T>& op) { return x == op.x && y == op.y; }
        inline bool operator != (const vector2<T>& op) { return x != op.x || y != op.y; }
	};
    
	template<typename T>
	struct vector3 {
		T x;
		T y;
		T z;
        inline void operator += (const vector3<T>& op) { x += op.x; y += op.y; z += op.z; }
        inline void operator -= (const vector3<T>& op) { x -= op.x; y -= op.y; z -= op.z; }
        inline bool operator == (const vector3<T>& op) { return x == op.x && y == op.y && z == op.z; }
        inline bool operator != (const vector3<T>& op) { return x != op.x || y != op.y || z != op.z; }
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
        return vector2<T>{(T)(a.x * scalar), (T)(a.y * scalar)};
    }
    
    template<typename T, typename S>
    inline vector2<T> interpolate (const vector2<T>& a, const vector2<T>& b, S s) {
        return vector2<T>{(T)(a.x * (1 - s) + b.x * s), (T)(a.y * (1 - s) + b.y * s)};
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
        return vector3<T>{(T)(a.x * scalar), (T)(a.y * scalar), (T)(a.z * scalar)};
    }
    
    template<typename T, typename S>
    inline vector3<T> interpolate (const vector3<T>& a, const vector3<T>& b, S s) {
        return vector3<T>{(T)(a.x * (1 - s) + b.x * s), (T)(a.y * (1 - s) + b.y * s), (T)(a.z * (1 - s) + b.z * s)};
    }
    
    template<typename T>
    struct rect {
        T left;
        T top;
        T width;
        T height;
    };
    
    template<typename T, int32_t TCOUNT>
    struct TextureInfo {
        vector2<T> vert[TCOUNT];
    };
    
    template<int32_t VC>
    struct VEC_TYPE_TRAITS { using vec_type = vector2<float>; };
    
    template<>
    struct VEC_TYPE_TRAITS<3> { using vec_type = vector3<float>; };
    
    template<int32_t VCOUNT>
    struct vtch {
        typename VEC_TYPE_TRAITS<VCOUNT>::vec_type vertex;
        vector2<float> texcoord = {0.0f, 0.0f};
        uint32_t color = 0xffffffff;
        uint32_t hcolor = 0;
        static const int32_t tex_offset = VCOUNT * 4;
        static const int32_t color_offset = VCOUNT * 4 + 8;
        static const int32_t hcolor_offset = VCOUNT * 4 + 12;
        
        static uint32_t InitVao(uint32_t vbo, uint32_t ibo) {
            uint32_t vao_id;
            glGenVertexArrays(1, &vao_id);
            glBindVertexArray(vao_id);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glVertexAttribPointer(0, VCOUNT, GL_FLOAT, GL_FALSE, sizeof(vtch), 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vtch), (const GLvoid*)vtch::tex_offset);
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vtch), (const GLvoid*)vtch::color_offset);
            glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vtch), (const GLvoid*)vtch::hcolor_offset);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBindVertexArray(0);
            return vao_id;
        }
        static void DrawBegin(uint32_t vbo, uint32_t ibo, uint32_t vao) {
            if(vao) {
                glBindVertexArray(vao);
            } else {
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glEnableClientState(GL_INDEX_ARRAY);
                glDisableClientState(GL_NORMAL_ARRAY);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glVertexPointer(VCOUNT, GL_FLOAT, sizeof(vtch), 0);
                glTexCoordPointer(2, GL_FLOAT, sizeof(vtch), (const GLvoid*)vtch::tex_offset);
                glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vtch), (const GLvoid*)vtch::color_offset);
                glSecondaryColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vtch), (const GLvoid*)vtch::hcolor_offset);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glIndexPointer(GL_UNSIGNED_SHORT, sizeof(GL_UNSIGNED_SHORT), 0);
            }
        }
        static void DrawEnd(uint32_t vbo, uint32_t ibo, uint32_t vao) {
            if(vao) {
                glBindVertexArray(0);
            } else {
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
                glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                glDisableClientState(GL_INDEX_ARRAY);
                glDisableClientState(GL_NORMAL_ARRAY);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }
        static inline void Fill(vtch* vt, vector2<float> pos, vector2<float> sz, TextureInfo<float, 4> ti, uint32_t cl = 0xffffffff, uint32_t hcl = 0) {
            vt[0].vertex = {pos.x, pos.y};
            vt[0].texcoord = ti.vert[0];
            vt[0].color = cl;
            vt[0].hcolor = hcl;
            vt[1].vertex = {pos.x + sz.x, pos.y};
            vt[1].texcoord = ti.vert[1];
            vt[1].color = cl;
            vt[1].hcolor = hcl;
            vt[2].vertex = {pos.x, pos.y + sz.y};
            vt[2].texcoord = ti.vert[2];
            vt[2].color = cl;
            vt[2].hcolor = hcl;
            vt[3].vertex = {pos.x + sz.x, pos.y + sz.y};
            vt[3].texcoord = ti.vert[3];
            vt[3].color = cl;
            vt[3].hcolor = hcl;
        }
        static inline void GenQuadIndex(int16_t* idx, int32_t count, int32_t base = 0) {
            static int16_t index[] = {0, 1, 2, 2, 1, 3};
            for(int32_t i = 0; i < count; ++i) {
                idx[i * 6 + 0] = base + index[0] + i * 4;
                idx[i * 6 + 1] = base + index[1] + i * 4;
                idx[i * 6 + 2] = base + index[2] + i * 4;
                idx[i * 6 + 3] = base + index[3] + i * 4;
                idx[i * 6 + 4] = base + index[4] + i * 4;
                idx[i * 6 + 5] = base + index[5] + i * 4;
            }
        }
    };

    class Shader {
        using param_cb = std::function<void(Shader*)>;
    public:
        ~Shader();
        bool LoadSource(const std::string& source);
        int32_t LoadShader(const char* buffer, int32_t length, int32_t shader_type);
        bool Link();
        bool Use();
        void Unload();
        inline void SetDefaultParamCallback(param_cb cb) { default_cb = cb; }
        inline param_cb GetDefaultParamCallback() { return default_cb; }
        void SetParam1i(const char* varname, const int32_t value);
        void SetParam1f(const char* varname, const float value);
        void SetParamMat4(const char* varname, const float m[]);
        
    public:
        static void Unuse();
        
    protected:
        uint32_t vert_shader = 0;
        uint32_t frag_shader = 0;
        uint32_t program = 0;
        param_cb default_cb;
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
        
        inline vector2<float> ConvTexCoord(vector2<int32_t> texpos) {
            if(!frame_tex)
                return vector2<float>{(float)texpos.x / tex_width, (float)texpos.y / tex_height};
            else
                return vector2<float>{(float)texpos.x / tex_width, ymax - (float)texpos.y / tex_height};
        }
        
        void Load(const uint8_t* data, int32_t x, int32_t y, int32_t filter_type = GL_LINEAR);
        void Unload();
        void Update(const uint8_t* data, int32_t offx, int32_t offy, int32_t width, int32_t height);
        void Bind();
        TextureInfo<float, 4> ConvTextureInfo(TextureInfo<int32_t, 4> tinfo);
        rect<float> ConvTextureInfo(rect<int32_t> tinfo);
        TextureInfo<float, 4> GetTextureInfo(rect<int32_t> tinfo);
        inline TextureInfo<float, 4> GetTextureInfo() { return GetTextureInfo({0, 0, img_width, img_height}); }
        inline uint32_t GetTextureId() { return texture_id; }
        inline int32_t GetWidth() { return tex_width; }
        inline int32_t GetHeight() { return tex_height; }
        inline int32_t GetImgWidth() { return img_width; }
        inline int32_t GetImgHeight() { return img_height; }
        inline vector2<int32_t> GetSize() { return {tex_width, tex_height}; }
        inline void SetFrameBufferTexture(bool f) { frame_tex = f; ymax = (float)img_height / tex_height; }
        inline bool IsFrameBufferTexture() { return frame_tex; }
        
    protected:
        uint32_t texture_id = 0;
        int32_t tex_width = 1;
        int32_t tex_height = 1;
        int32_t img_width = 0;
        int32_t img_height = 0;
        bool frame_tex = false;
        float ymax = 0.0f;
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
        bool Load(const std::string& file, uint32_t sz, const std::string& style, vector2<int32_t> offset);
        void LoadEmoji(Image& emoji_img, vector2<int32_t> esize, vector2<int32_t> ecount, rect<int32_t> erect, int32_t ls);
        void Unload();
        const FontGlyph& GetGlyph(uint32_t ch);
        inline Texture& GetTexture() { return char_tex; }
        inline int32_t GetFontSize() { return font_size; }
        inline int32_t GetLineSpacing(uint32_t ch) { return IsEmoji(ch) ? emoji_spacing : font_size; }
        inline int32_t IsEmoji(uint32_t ch) { return ch >= 0xe000 && ch < 0xefff; }
        inline vector2<int32_t> GetTextOffset() { return text_offset; }
        
    protected:
        Texture char_tex;
        FontGlyph glyphs[0x10000];
        FT_Face face;
        FT_Library library;
        bool is_bold = false;
        int32_t font_size = 0;
        int32_t emoji_spacing = 0;
        vector2<int32_t> tex_pos;
        vector2<int32_t> emoji_img_size;
        vector2<int32_t> text_offset;
    };
    
}

using v2i = base::vector2<int32_t>;
using v2f = base::vector2<float>;
using v3i = base::vector3<int32_t>;
using v3f = base::vector3<float>;
using recti = base::rect<int32_t>;
using rectf = base::rect<float>;
using texi4 = base::TextureInfo<int32_t, 4>;
using texf4 = base::TextureInfo<float, 4>;
using vt2 = base::vtch<2>;
using vt3 = base::vtch<3>;

#include "render_util.h"

#endif
