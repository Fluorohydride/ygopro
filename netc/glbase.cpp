#include <cstring>
#include <fstream>
#include <iostream>
#include "glbase.h"

#define STBI_HEADER_FILE_ONLY
#include "../buildin/stb_image.c"
#include "../buildin/stb_image_write.h"

#ifdef _DEBUG
void GLCheckError(const std::string& file, int line) {
    auto err = glGetError();
    switch (err) {
        case GL_NO_ERROR:
            break;
        case GL_INVALID_ENUM:
            std::cout << "Invalid enum at " << file << ":" << line << std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cout << "Invalid value at " << file << ":" << line << std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cout << "Invalid operation at " << file << ":" << line << std::endl;
            break;
        case GL_STACK_OVERFLOW:
            std::cout << "Stack overflow at " << file << ":" << line << std::endl;
            break;
        case GL_STACK_UNDERFLOW:
            std::cout << "Stack underflow at " << file << ":" << line << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cout << "Out of memory at " << file << ":" << line << std::endl;
            break;
        case GL_TABLE_TOO_LARGE:
            std::cout << "Table to large at " << file << ":" << line << std::endl;
            break;
        default:
            std::cout << "Unknown error #" << err << " at " << file << ":" << line << std::endl;
            break;
    }
}
#endif

namespace glbase {
    
    Image::~Image() {
        if(buffer != nullptr)
            delete buffer;
    }
    
    bool Image::LoadFile(const std::string &file) {
        unsigned char* data = stbi_load(file.c_str(), &width, &height, nullptr, 4);
        if(data == nullptr) {
            std::cout << stbi_failure_reason() << std::endl;
            return false;
        }
        if(buffer != nullptr)
            delete buffer;
        buffer = new unsigned char[width * height * 4];
        memcpy(buffer, data, width * height * 4);
        stbi_image_free(data);
        return true;
    }
    
    bool Image::LoadMemory(const unsigned char* mem, unsigned int sz) {
        unsigned char* data = stbi_load_from_memory(mem, sz, &width, &height, nullptr, 4);
        if(data == nullptr) {
            std::cout << stbi_failure_reason() << std::endl;
            return false;
        }
        if(buffer != nullptr)
            delete buffer;
        buffer = new unsigned char[width * height * 4];
        memcpy(buffer, data, width * height * 4);
        stbi_image_free(data);
        return true;
    }
    
    void Texture::Load(const unsigned char* data, int x, int y) {
        if(texture_id)
            glDeleteTextures(1, &texture_id);
        tex_width = texlen(x);
        tex_height = texlen(y);
        img_width = x;
        img_height = y;
        glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if(data) {
            unsigned char* px = new unsigned char[tex_width * tex_height * 4];
            memset(px, 0, tex_width * tex_height * 4);
            for(int h = 0; h < y; ++h)
                memcpy(&px[tex_width * 4 * h], &data[x * 4 * h], x * 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
            delete px;
        } else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void Texture::Unload() {
        if(!texture_id)
            return;
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
    
    void Texture::Update(const unsigned char* data, int offx, int offy, int width, int height) {
        if(!texture_id)
            return;
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, offx, offy, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void Texture::Bind() {
        if(texture_id)
            glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    
    TextureInfo<4> Texture::GetTextureInfo() {
        static TextureInfo<4> ti;
        if(!texture_id)
            return ti;
        TextureInfo<4> ret;
        ret.vert[0] = {0.0f, 0.0f};
        ret.vert[1] = {(float)img_width / tex_width, 0.0f};
        ret.vert[2] = {0.0f, (float)img_height / tex_height};
        ret.vert[3] = {(float)img_width / tex_width, (float)img_height / tex_height};
        return ret;
    }
    
    Font::Font() {
        FT_Init_FreeType(&library);
    }
    
    Font::~Font() {
        FT_Done_FreeType(library);
        char_tex.Unload();
    }
    
    bool Font::Load(const std::string &file, unsigned int sz) {
        if (FT_New_Face(library, file.c_str(), 0, &face))
            return false;
        FT_Select_Charmap(face, FT_ENCODING_UNICODE);
        FT_Set_Pixel_Sizes(face, 0, sz);
        char_tex.Load(nullptr, 2048, 2048);
        tex_posx = 0;
        tex_posy = 0;
        font_size = sz;
        return true;
    }
    
    const FontGlyph& Font::GetGlyph(unsigned int ch) {
        auto& gl = glyphs[ch];
        if(gl.loaded)
            return gl;
        FT_UInt glyph_index = FT_Get_Char_Index(face, ch);
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        gl.bounds = {face->glyph->bitmap_left, -face->glyph->bitmap_top, face->glyph->bitmap.width, face->glyph->bitmap.rows};
        gl.advance = face->glyph->advance.x / 64.0f;
        if(2048 - tex_posx < gl.bounds.width) {
            tex_posx = 0;
            tex_posy += font_size;
        }
        gl.textureRect = {tex_posx, tex_posy, gl.bounds.width, gl.bounds.height};
        unsigned int* px = new unsigned int[gl.bounds.width * gl.bounds.height];
        unsigned char* buf = face->glyph->bitmap.buffer;
        for(int i = 0; i < gl.bounds.width * gl.bounds.height; ++i)
            px[i] = (((unsigned int)buf[i]) << 24) | 0xffffff;
        char_tex.Update((const unsigned char*)px, tex_posx, tex_posy, gl.bounds.width, gl.bounds.height);
        tex_posx += gl.bounds.width;
        gl.loaded = true;
        delete px;
        return gl;
    }
}
