#include "utils/common.h"

#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "utils/stb_image.h"

#include "render_base.h"

#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_SYNTHESIS_H

#ifdef _DEBUG
void GLCheckError(const std::string& file, int32_t line) {
    auto err = glGetError();
    switch (err) {
        case GL_NO_ERROR:
            break;
        case GL_INVALID_ENUM:
            std::cerr << "Invalid enum at " << file << ":" << line << std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cerr << "Invalid value at " << file << ":" << line << std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cerr << "Invalid operation at " << file << ":" << line << std::endl;
            break;
        case GL_STACK_OVERFLOW:
            std::cerr << "Stack overflow at " << file << ":" << line << std::endl;
            break;
        case GL_STACK_UNDERFLOW:
            std::cerr << "Stack underflow at " << file << ":" << line << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << "Out of memory at " << file << ":" << line << std::endl;
            break;
        case GL_TABLE_TOO_LARGE:
            std::cerr << "Table to large at " << file << ":" << line << std::endl;
            break;
        default:
            std::cerr << "Unknown error #" << err << " at " << file << ":" << line << std::endl;
            break;
    }
}
#endif

namespace base {
    
    Shader::~Shader() {
        if(vert_shader)
            glDeleteShader(vert_shader);
        if(frag_shader)
            glDeleteShader(frag_shader);
        if(program)
            glDeleteProgram(program);
    }
    
    bool Shader::LoadSource(const std::string& source) {
        int32_t vert_begin = (int32_t)source.find("#version");
        if(vert_begin == source.npos)
            return false;
        int32_t frag_begin = (int32_t)source.find("#version", vert_begin + 8);
        if(frag_begin == source.npos)
            return false;
        vert_shader = LoadShader(source.data() + vert_begin, frag_begin - vert_begin, GL_VERTEX_SHADER);
        if(!vert_shader)
            return false;
        frag_shader = LoadShader(source.data() + frag_begin, (int32_t)source.size() - frag_begin, GL_FRAGMENT_SHADER);
        if(!vert_shader)
            return false;
        if(Link())
            return false;
        return true;
    }
    
    int32_t Shader::LoadShader(const char* buffer, int32_t length, int32_t shader_type) {
        int32_t shader_id = glCreateShader(shader_type);
        glShaderSource(shader_id, 1, &buffer, &length);
        glCompileShader(shader_id);
        int32_t result;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE) {
            if(shader_type == GL_VERTEX_SHADER)
                std::cerr << "Vertex shader compilation failed!" << std::endl;
            else
                std::cerr << "Fragment shader compilation failed!" << std::endl;
            GLint logLen;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &logLen);
            if(logLen > 0) {
                char* log = new char[logLen];
                GLsizei written;
                glGetShaderInfoLog(shader_id, logLen, &written, log);
                std::cerr << "Shader log:" << log << std::endl;
                delete[] log;
            }
            glDeleteShader(shader_id);
            shader_id = 0;
        }
        return shader_id;
    }
    
    bool Shader::Link() {
        program = glCreateProgram();
        glAttachShader(program, vert_shader);
        glAttachShader(program, frag_shader);
        glLinkProgram(program);
        int32_t result;
        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if(result == GL_FALSE) {
            std::cerr << "Failed to link shader program!\n" << std::endl;
            GLint logLen;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
            if(logLen > 0) {
                char* log = new char[logLen];
                GLsizei written;
                glGetProgramInfoLog(program, logLen, &written, log);
                std::cerr << "Program log: " << log << std::endl;
                delete[] log;
            }
            glDeleteProgram(program);
            program = 0;
            return false;
        }
        return true;
    }
    
    bool Shader::Use() {
        if(program) {
            glUseProgram(program);
            return true;
        }
        return false;
    }
    
    void Shader::SetParam1i(const char* varname, const int32_t value) {
        auto loc = glGetUniformLocation(program, varname);
        if(loc >= 0)
            glUniform1i(loc, value);
    }
    
    void Shader::SetParam1f(const char* varname, const float value) {
        auto loc = glGetUniformLocation(program, varname);
        if(loc >= 0)
            glUniform1f(loc, value);
    }
    
    void Shader::SetParamMat4(const char* varname, const float m[]) {
        auto loc = glGetUniformLocation(program, varname);
        if(loc >= 0)
            glUniformMatrix4fv(loc, 1, GL_FALSE, m);
    }
    
    void Shader::Unload() {
        if(vert_shader)
            glDeleteShader(vert_shader);
        if(frag_shader)
            glDeleteShader(frag_shader);
        if(program)
            glDeleteProgram(program);
        vert_shader = 0;
        frag_shader = 0;
        program = 0;
    }
    
    void Shader::Unuse() {
        glUseProgram(0);
    }
    
    Image::~Image() {
        if(buffer != nullptr)
            delete[] buffer;
    }
    
    bool Image::LoadFile(const std::string &file) {
        uint8_t* data = stbi_load(file.c_str(), &width, &height, nullptr, 4);
        if(data == nullptr) {
            std::cout << stbi_failure_reason() << std::endl;
            return false;
        }
        if(buffer != nullptr)
            delete[] buffer;
        buffer = new uint8_t[width * height * 4];
        memcpy(buffer, data, width * height * 4);
        stbi_image_free(data);
        return true;
    }
    
    bool Image::LoadMemory(const uint8_t* mem, uint32_t sz) {
        uint8_t* data = stbi_load_from_memory(mem, sz, &width, &height, nullptr, 4);
        if(data == nullptr) {
            std::cout << stbi_failure_reason() << std::endl;
            return false;
        }
        if(buffer != nullptr)
            delete[] buffer;
        buffer = new uint8_t[width * height * 4];
        memcpy(buffer, data, width * height * 4);
        stbi_image_free(data);
        return true;
    }
    
    void Texture::Load(const uint8_t* data, int32_t x, int32_t y, int32_t filter_type) {
        if(texture_id)
            glDeleteTextures(1, &texture_id);
        tex_width = texlen(x);
        tex_height = texlen(y);
        img_width = x;
        img_height = y;
        glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type);
        if(data) {
            uint8_t* px = new uint8_t[tex_width * tex_height * 4];
            memset(px, 0, tex_width * tex_height * 4);
            for(int32_t h = 0; h < y; ++h)
                memcpy(&px[tex_width * 4 * h], &data[x * 4 * h], x * 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
            delete[] px;
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
    
    void Texture::Update(const uint8_t* data, int32_t offx, int32_t offy, int32_t width, int32_t height) {
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
    
    TextureInfo<float, 4> Texture::ConvTextureInfo(TextureInfo<int32_t, 4> tinfo) {
        TextureInfo<float, 4> ret;
        ret.vert[0] = ConvTexCoord(tinfo.vert[0]);
        ret.vert[1] = ConvTexCoord(tinfo.vert[1]);
        ret.vert[2] = ConvTexCoord(tinfo.vert[2]);
        ret.vert[3] = ConvTexCoord(tinfo.vert[3]);
        return ret;
    }
    
    rect<float> Texture::ConvTextureInfo(rect<int32_t> r) {
        if(!frame_tex)
            return rect<float>{(float)r.left / tex_width, (float)r.top / tex_height, (float)r.width / tex_width, (float)r.height / tex_height};
        else
            return rect<float>{(float)r.left / tex_width, ymax - (float)r.top / tex_height, (float)r.width / tex_width, -(float)r.height / tex_height};
    }
    
    TextureInfo<float, 4> Texture::GetTextureInfo(rect<int32_t> tinfo) {
        TextureInfo<float, 4> ret;
        ret.vert[0] = ConvTexCoord({tinfo.left, tinfo.top});
        ret.vert[1] = ConvTexCoord({tinfo.left + tinfo.width, tinfo.top});
        ret.vert[2] = ConvTexCoord({tinfo.left, tinfo.top + tinfo.height});
        ret.vert[3] = ConvTexCoord({tinfo.left + tinfo.width, tinfo.top + tinfo.height});
        return ret;
    }
    
    Font::Font() {
        FT_Init_FreeType(&library);
    }
    
    Font::~Font() {
        FT_Done_FreeType(library);
        char_tex.Unload();
    }
    
    bool Font::Load(const std::string &file, uint32_t sz, const std::string& style, vector2<int32_t> offset) {
        if (FT_New_Face(library, file.c_str(), 0, &face))
            return false;
        FT_Select_Charmap(face, FT_ENCODING_UNICODE);
        FT_Set_Pixel_Sizes(face, 0, sz);
        char_tex.Load(nullptr, 2048, 2048);
        tex_pos.x = 0;
        tex_pos.y = 0;
        font_size = sz;
        if(style.find("bold") != std::string::npos)
            is_bold = true;
        if(style.find("italic") != std::string::npos) {
            float lean = 0.4f;
            FT_Matrix matrix;
            matrix.xx = 0x10000;
            matrix.xy = lean * 0x10000;
            matrix.yx = 0;
            matrix.yy = 0x10000;
            FT_Set_Transform(face, &matrix, 0);
        }
        text_offset = offset;
        return true;
    }

    void Font::LoadEmoji(Image& emoji_img, vector2<int32_t> esize, vector2<int32_t> ecount, rect<int32_t> erect, int32_t ls) {
        emoji_img_size = v2i{emoji_img.GetWidth(), emoji_img.GetHeight()};
        char_tex.Update(emoji_img.GetRawData(), 0, 0, emoji_img_size.x, emoji_img_size.y);
        if(tex_pos.x < emoji_img_size.x)
            tex_pos.x = emoji_img_size.x;
        emoji_spacing = ls;
        for(int32_t y = 0; y < ecount.y; ++y) {
            for(int32_t x = 0; x < ecount.x; ++x) {
                int32_t idx = 0xe000 + y * ecount.x + x;
                auto& gl = glyphs[idx];
                gl.bounds = erect;
                gl.textureRect = rect<int32_t>{x * esize.x, y * esize.y, esize.x, esize.y};
                gl.advance = erect.width + 1;
                gl.loaded = true;                
            }
        }
    }
    
    void Font::Unload() {
        char_tex.Unload();
    }
	
    const FontGlyph& Font::GetGlyph(uint32_t ch) {
        auto& gl = glyphs[ch];
        if(gl.loaded)
            return gl;
        FT_UInt glyph_index = FT_Get_Char_Index(face, ch);
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        if(is_bold)
            FT_Outline_Embolden(&face->glyph->outline, 1 << 6);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        gl.bounds = {face->glyph->bitmap_left, -face->glyph->bitmap_top, (int32_t)face->glyph->bitmap.width, (int32_t)face->glyph->bitmap.rows};
        gl.advance = face->glyph->advance.x / 64.0f;
        if(2048 - tex_pos.x < gl.bounds.width) {
            tex_pos.x = 0;
            tex_pos.y += font_size;
        }
        gl.textureRect = {tex_pos.x, tex_pos.y, gl.bounds.width, gl.bounds.height};
        uint32_t* px = new uint32_t[gl.bounds.width * gl.bounds.height];
        uint8_t* buf = face->glyph->bitmap.buffer;
        for(int32_t i = 0; i < gl.bounds.width * gl.bounds.height; ++i)
            px[i] = (((uint32_t)buf[i]) << 24) | 0xffffff;
        char_tex.Update((const uint8_t*)px, tex_pos.x, tex_pos.y, gl.bounds.width, gl.bounds.height);
        tex_pos.x += gl.bounds.width;
        gl.loaded = true;
        delete[] px;
        return gl;
    }
}
