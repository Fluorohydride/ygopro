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

namespace glbase {
    
    struct VertexVT {
        float vertex[3] = {0.0f, 0.0f, 0.0f};
        float texcoord[2] = {0.0f, 0.0f};
        static const int tex_offset = 12;
    };
    
    struct VertexVCT {
        float vertex[3] = {0.0f, 0.0f, 0.0f};
        unsigned int color = 0xffffffff;
        float texcoord[2] = {0.0f, 0.0f};
        static const int color_offset = 12;
        static const int tex_offset = 16;
    };
    
    struct VertexVNT {
        float vertex[3] = {0.0f, 0.0f, 0.0f};
        float normal[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        float texcoord[2] = {0.0f, 0.0f};
        static const int normal_offset = 12;
        static const int tex_offset = 28;
    };
    
    struct VertexVNCT {
        float vertex[3] = {0.0f, 0.0f, 0.0f};
        float normal[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        unsigned int color = 0xffffffff;
        float texcoord[2] = {0.0f, 0.0f};
        static const int normal_offset = 12;
        static const int color_offset = 28;
        static const int tex_offset = 32;
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
        unsigned int GetTextureId() { return texture_id; }
        void Bind();
        int GetWidth() { return tex_width; }
        int GetHeight() { return tex_height; }
        
    protected:
        unsigned int texture_id = 0;
        int tex_width = 0;
        int tex_height = 0;
    };
    
}

#endif
