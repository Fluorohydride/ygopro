#ifndef _SGUI_H_
#define _SGUI_H_

#include "utils/singleton.h"
#include "utils/textfile.h"
#include "utils/convert.h"
#include "utils/jaweson.h"
#include "utils/timer.h"
#include "utils/clock.h"

#include "render_base.h"

#include <GLFW/glfw3.h>

namespace sgui
{
    
    // ===== Delegate Implement =====
    // OT: Object Type
    // ST: Sender Type
    // ET: Event Type
    
    template<typename ST, typename... ET>
    class SGEventHandler {
        using function_type = std::function<bool (ST&, ET...)>;
        using function_ptr = std::shared_ptr<function_type>;
    public:
        template<typename OT>
        void Bind(OT* obj, bool (OT::*fun)(ST&, ET...), void* object = nullptr) {
            auto lambda = [obj, fun](ST& sender, ET... evt)->bool { return (obj->*fun)(sender, std::forward<ET>(evt)...); };
            auto ptr = std::make_shared<function_type>(lambda);
            operate_ptrs.push_back(std::make_pair(ptr, object));
            if(!lock)
                delegate_ptrs.push_back(ptr);
            else need_sync = true;
        }
        
        template<typename FT>
        void Bind(FT f, void* object = nullptr) {
            auto ptr = std::make_shared<function_type>(f);
            operate_ptrs.push_back(std::make_pair(ptr, object));
            if(!lock)
                delegate_ptrs.push_back(ptr);
            else need_sync = true;
        }
        
        template<typename FT>
        inline void operator += (FT f) { Bind(f, nullptr); }
        
        void Reset() {
            if(operate_ptrs.empty())
                return;
            operate_ptrs.clear();
            if(!lock)
                delegate_ptrs.clear();
            else need_sync = true;
        }
        
        void Remove(void* object) {
            auto check_obj = [object](std::pair<function_ptr, void*>& p)->bool { return p.second == object; };
            auto enditer = std::remove_if(operate_ptrs.begin(), operate_ptrs.end(), check_obj);
            if(enditer == operate_ptrs.end())
                return;
            operate_ptrs.resize(enditer - operate_ptrs.begin());
            need_sync = true;
            if(!lock)
                SyncPtrs();
        }
        
        bool Trigger(ST& sender, ET... evt) {
            if(delegate_ptrs.size() == 0)
                return false;
            bool ret = false;
            lock = true;
            for(auto& ptr : delegate_ptrs)
                ret = ptr->operator()(sender, std::forward<ET>(evt)...) || ret;
            lock = false;
            SyncPtrs();
            return ret;
        }
        
    protected:
        void SyncPtrs() {
            if(need_sync) {
                delegate_ptrs.resize(operate_ptrs.size());
                for(size_t i = 0; i < operate_ptrs.size(); ++i)
                    delegate_ptrs[i] = operate_ptrs[i].first;
                need_sync = false;
            }
        }
        
        bool lock = false;
        bool need_sync = false;
        std::vector<function_ptr> delegate_ptrs;
        std::vector<std::pair<function_ptr, void*>> operate_ptrs;
    };
    
    // ===== Delegate Implement End =====
    
    class RegionObject {
    public:
        struct AreaAttr {
            v2i relative = {0, 0};
            v2f proportion = {0.0f, 0.0f};
            v2i absolute = {0, 0};
        };
        
        class RegionModifier {
        public:
            RegionModifier(RegionObject* obj) : object(obj) {}
            inline RegionModifier& PosRX(int32_t x) { object->area_pos.relative.x = x; pos_changed = true; return *this; }
            inline RegionModifier& PosRY(int32_t y) { object->area_pos.relative.y = y; pos_changed = true; return *this; }
            inline RegionModifier& PosPX(float x) { object->area_pos.proportion.x = x; pos_changed = true; return *this; }
            inline RegionModifier& PosPY(float y) { object->area_pos.proportion.y = y; pos_changed = true; return *this; }
            inline RegionModifier& SizeRX(int32_t x) { object->area_size.relative.x = x; size_changed = true; return *this; }
            inline RegionModifier& SizeRY(int32_t y) { object->area_size.relative.y = y; size_changed = true; return *this; }
            inline RegionModifier& SizePX(float x) { object->area_size.proportion.x = x; size_changed = true; return *this; }
            inline RegionModifier& SizePY(float y) { object->area_size.proportion.y = y; size_changed = true; return *this; }
            inline RegionModifier& AlignX(float x) { object->self_factor.x = x; pos_changed = true; return *this; }
            inline RegionModifier& AlignY(float y) { object->self_factor.y = y; pos_changed = true; return *this; }
            void End() { object->OnPositionSizeChange(pos_changed, size_changed); }
            
        protected:
            RegionObject* object = nullptr;
            bool pos_changed = false;
            bool size_changed = false;
        };
        
    public:
        void SetPositionSize(v2i pos, v2i sz, v2f ppos = {0.0f, 0.0f}, v2f psz = {0.0f, 0.0f}, v2f af = {0.0f, 0.0f}) {
            area_pos.relative = pos;
            area_pos.proportion = ppos;
            area_size.relative = sz;
            area_size.proportion = psz;
            self_factor = af;
            OnPositionSizeChange(true, true);
        }
        void SetPosition(v2i pos, v2f ppos = {0.0f, 0.0f}, v2f af = {0.0f, 0.0f}) {
            area_pos.relative = pos;
            area_pos.proportion = ppos;
            self_factor = af;
            OnPositionSizeChange(true, false);
        }
        void SetSize(v2i sz, v2f psz = {0.0f, 0.0f}) {
            area_size.relative = sz;
            area_size.proportion = psz;
            OnPositionSizeChange(false, true);
        }
        void SetPositionSizeR(v2i pos, v2i sz) {
            area_pos.relative = pos;
            area_size.relative = sz;
            OnPositionSizeChange(true, true);
        }
        void SetPositionR(v2i pos) {
            area_pos.relative = pos;
            OnPositionSizeChange(true, false);
        }
        void SetSizeR(v2i sz) {
            area_size.relative = sz;
            OnPositionSizeChange(false, true);
        }
        void SetPositionSizeP(v2f pos, v2f sz, v2f af) {
            area_pos.proportion = pos;
            area_size.proportion = sz;
            self_factor = af;
            OnPositionSizeChange(true, true);
        }
        void SetPositionP(v2f pos) {
            area_pos.proportion = pos;
            OnPositionSizeChange(true, false);
        }
        void SetSizeP(v2f sz) {
            area_size.proportion = sz;
            OnPositionSizeChange(false, true);
        }
        void SetAlignFactor(v2f af) {
            self_factor = af;
            OnPositionSizeChange(true, false);
        }
        
        inline v2i DotFactor(v2i v, v2f f) { return v2i{(int32_t)(v.x * f.x), (int32_t)(v.y * f.y)}; }
        inline v2i GetAbsolutePosition() { return area_pos.absolute; }
        inline v2i GetAbsoluteSize() { return area_size.absolute; }
        inline void SetContainer(RegionObject* ct) { container = ct; }
        inline const AreaAttr& GetPosAttr() { return area_pos; }
        inline const AreaAttr& GetSizeAttr() { return area_size; }
        inline const v2f& GetAlignFactor() { return self_factor; }
        inline RegionModifier BeginModify() { return RegionModifier(this); }
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto pre_pos = area_pos.absolute;
            auto pre_size = area_size.absolute;
            if(container) {
                auto offset = container->GetAbsolutePosition();
                auto pprop = container->GetAbsoluteSize();
                if(re_size)
                    area_size.absolute = area_size.relative + DotFactor(pprop, area_size.proportion);
                if(re_pos)
                    area_pos.absolute = area_pos.relative + offset + DotFactor(pprop, area_pos.proportion) + DotFactor(area_size.absolute, self_factor);
            } else {
                if(re_size)
                    area_size.absolute = area_size.relative;
                if(re_pos)
                    area_pos.absolute = area_pos.relative + DotFactor(area_size.absolute, self_factor);
            }
            return std::make_pair(pre_pos != area_pos.absolute, pre_size != area_size.absolute);
        }

    protected:
        RegionObject* container = nullptr;
        v2f self_factor = {0.0f, 0.0f};
        AreaAttr area_pos;
        AreaAttr area_size;
    };
    
    // == basic ui elements =
    
    class UIComponent : public RegionObject, public base::RenderUnit<vt2> {
    public:
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto ret = RegionObject::OnPositionSizeChange(re_pos, re_size);
            if(ret.first || ret.second)
                SetUpdate();
            return ret;
        }
        
        inline v2f ConvScreenCoord(v2i pos) { return (static_cast<base::RenderObject2DLayout*>(manager))->ConvScreenCoord(pos);}
        inline void SetColor(uint32_t cl)   { if(color == cl) return; color = cl; SetUpdate(); }
        
    protected:
        uint32_t color = 0xffffffff;
    };
    
    struct UIVertex {
        v2i offset = {0, 0};
        v2f prop = {0.0f, 0.0f};
        v2f texcoord = {0, 0};
        uint32_t color = 0xffffffff;
    };
    
    template<int32_t VERT_COUNT>
    struct UIVertexArray {
        inline UIVertexArray& BuildSprite(recti o, rectf p, rectf t, uint32_t c) {
            SpriteOffset(o); SpriteProp(p); SpriteTexcoord(t); SpriteColor(c); return *this;
        }
        inline UIVertexArray& BuildSprite(recti o, rectf p, texf4 t, uint32_t c) {
            SpriteOffset(o); SpriteProp(p); SpriteTexcoord(t); SpriteColor(c); return *this;
        }
        
        inline UIVertexArray& SpriteOffset(recti pos) {
            static_assert(VERT_COUNT >= 4, "");
            verts[0].offset = {pos.left, pos.top};
            verts[1].offset = {pos.left + pos.width, pos.top};
            verts[2].offset = {pos.left, pos.top + pos.height};
            verts[3].offset = {pos.left + pos.width, pos.top + pos.height};
            return *this;
        }
        
        inline UIVertexArray& SpriteProp(rectf prop) {
            static_assert(VERT_COUNT >= 4, "");
            verts[0].prop = {prop.left, prop.top};
            verts[1].prop = {prop.left + prop.width, prop.top};
            verts[2].prop = {prop.left, prop.top + prop.height};
            verts[3].prop = {prop.left + prop.width, prop.top + prop.height};
            return *this;
        }
        
        inline UIVertexArray& SpriteTexcoord(rectf tex) {
            static_assert(VERT_COUNT >= 4, "");
            verts[0].texcoord = {tex.left, tex.top};
            verts[1].texcoord = {tex.left + tex.width, tex.top};
            verts[2].texcoord = {tex.left, tex.top + tex.height};
            verts[3].texcoord = {tex.left + tex.width, tex.top + tex.height};
            return *this;
        }
        
        inline UIVertexArray& SpriteTexcoord(texf4 tex) {
            static_assert(VERT_COUNT >= 4, "");
            verts[0].texcoord = tex.vert[0];
            verts[1].texcoord = tex.vert[1];
            verts[2].texcoord = tex.vert[2];
            verts[3].texcoord = tex.vert[3];
            return *this;
        }
        
        inline UIVertexArray& SpriteColor(uint32_t cl) {
            static_assert(VERT_COUNT >= 4, "");
            verts[0].color = verts[1].color = verts[2].color = verts[3].color = cl;
            return *this;
        }
        
        inline UIVertex* Ptr() { return verts; }
        
        UIVertex verts[VERT_COUNT];
    };
    
    class UIVertexList : public UIComponent {
    public:
        
        virtual bool CheckAvailable() { return texture != nullptr; }
        virtual int32_t GetTextureId() { return texture ? texture->GetTextureId() : 0; }
        
        inline void SetTexture(base::Texture* tex) { texture = tex; SetRedraw(true); }
        inline void SetPoints(std::vector<UIVertex> pts) {
            size_t pre_size = points.size();
            points = std::move(pts);
            if(pre_size == points.size())
                SetUpdate();
            else
                SetRedraw(true);
        }
        inline v2f CalUIVertex(UIVertex& v) { return ConvScreenCoord(area_pos.absolute + v.offset + DotFactor(area_size.absolute, v.prop)); }
        
    protected:
        std::vector<UIVertex> points;
        base::Texture* texture = nullptr;
    };
    
    template<int32_t PRIMITIVE_TYPE>
    class UIPrimitiveList : public UIVertexList {
        virtual int32_t GetPrimitiveType() { return PRIMITIVE_TYPE; }
        virtual void RefreshVertices() {
            vertices.resize(points.size());
            indices.resize(points.size());
            for(size_t i = 0; i < points.size(); ++i) {
                vertices[i].vertex = CalUIVertex(points[i]);
                vertices[i].texcoord = texture->ConvTexCoord(points[i].texcoord);
                vertices[i].color = points[i].color;
                indices[i] = vert_index + i;
            }
        }
    };
    
    class UIConvexRegion : public UIVertexList {
    public:
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        
        virtual void RefreshVertices() {
            size_t vsize = points.size();
            if(vsize < 3) {
                vertices.clear();
                indices.clear();
            } else {
                vertices.resize(vsize);
                indices.resize(vsize * 3 - 6);
                for(size_t i = 0; i < vsize; ++i) {
                    vertices[i].vertex = CalUIVertex(points[i]);
                    vertices[i].texcoord = points[i].texcoord;
                    vertices[i].color = points[i].color;
                }
                for(size_t i = 0; i < vsize - 2; ++i) {
                    indices[i * 3] = vert_index;
                    indices[i * 3 + 1] = vert_index + i + 1;
                    indices[i * 3 + 2] = vert_index + i + 2;
                }
            }
        }
    };
    
    class UISpriteList : public UIVertexList {
    public:
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        
        virtual void RefreshVertices() {
            static const int16_t quad_idx[] = {0, 1, 2, 2, 1, 3};
            for(size_t i = 0; i < points.size(); ++i) {
                vertices[i].vertex = CalUIVertex(points[i]);
                vertices[i].texcoord = points[i].texcoord;
                vertices[i].color = points[i].color;
            }
            size_t pt_size = points.size() / 4;
            for(size_t i = 0; i < pt_size; ++i) {
                for(size_t j = 0; j < 6; ++j)
                    indices[i * 6 + j] = vert_index + i * 4 + quad_idx[j];
            }
            for(size_t i = pt_size * 6; i < indices.size(); ++i)
                indices[i] = 0;
        }
        
        void SetCapacity(int32_t sz, bool reserve_points = false) {
            if(sz == capacity)
                return;
            capacity = sz;
            vertices.resize(capacity * 4);
            indices.resize(capacity * 6);
            if(reserve_points)
                points.resize(capacity * 4);
            SetRedraw(true);
        }
        
        void Clear() {
            points.clear();
            SetUpdate();
        }
        
        void AddSprite(UIVertex* v) {
            CheckCapacity(1);
            points.insert(points.end(), v, v + 4);
        }
        
        void SetSprite(UIVertex* v, int32_t index) {
            if(index * 4 >= points.size())
                return;
            memcpy(&points[index * 4], v, sizeof(UIVertex) * 4);
            SetUpdate();
        }
        
        void SetSpriteColor(uint32_t color, int32_t index) {
            if(index * 4 >= points.size())
                return;
            for(int32_t i = 0; i < 4; ++i)
                points[index * 4 + i].color = color;
            SetUpdate();
        }
        
        void CheckCapacity(int32_t inc) {
            auto cur_size = points.size() / 4;
            if(cur_size + inc > capacity) {
                while(cur_size + inc > capacity) {
                    if(cur_size == 0)
                        capacity = 8;
                    else
                        capacity = capacity * 1.5;
                }
                vertices.resize(capacity * 4);
                indices.resize(capacity * 6);
                SetRedraw(true);
            } else
                SetUpdate();
        }
        
        inline int32_t GetCapacity() { return capacity; }
        
    protected:
        int32_t capacity = 0;
    };
    
    class UISpriteBase : public UIComponent {
    public:
        virtual bool CheckAvailable() { return texture != nullptr; }
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        virtual int32_t GetTextureId() { return texture ? texture->GetTextureId() : 0; }
        
        inline void SetTexture(base::Texture* tex) { if(texture == tex) return; texture = tex; SetRedraw(true); }
        inline void SetTextureRect(recti rct) { tex_rect = rct; SetUpdate(); }
        
    protected:
        recti tex_rect = {0, 0, 0, 0};
        base::Texture* texture = nullptr;
    };
    
    class UISprite : public UISpriteBase {
    public:
        virtual void RefreshVertices() {
            vertices.resize(4);
            indices.resize(6);
            FillQuad(&vertices[0], &indices[0]);
        }
        
        void FillQuad(vt2* v, int16_t* idx) {
            static const int16_t quad_idx[] = {0, 1, 2, 2, 1, 3};
            v[0].vertex = ConvScreenCoord({area_pos.absolute.x, area_pos.absolute.y});
            v[1].vertex = ConvScreenCoord({area_pos.absolute.x + area_size.absolute.x, area_pos.absolute.y});
            v[2].vertex = ConvScreenCoord({area_pos.absolute.x, area_pos.absolute.y + area_size.absolute.y});
            v[3].vertex = ConvScreenCoord({area_pos.absolute.x + area_size.absolute.x, area_pos.absolute.y + area_size.absolute.y});
            if(direct_texcoord) {
                v[0].texcoord = texcoords.vert[0];
                v[1].texcoord = texcoords.vert[1];
                v[2].texcoord = texcoords.vert[2];
                v[3].texcoord = texcoords.vert[3];
            } else {
                v[0].texcoord = texture->ConvTexCoord({tex_rect.left, tex_rect.top});
                v[1].texcoord = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top});
                v[2].texcoord = texture->ConvTexCoord({tex_rect.left, tex_rect.top + tex_rect.height});
                v[3].texcoord = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top + tex_rect.height});
            }
            for(int16_t i = 0; i < 6; ++i)
                idx[i] = vert_index + quad_idx[i];
            for(int16_t i = 0; i < 4; ++i)
                v[i].color = color;
        }
        
        inline void SetDirectCoord(bool d) { direct_texcoord = d; SetUpdate(); }
        inline void SetTexcoords(texf4 tex) { texcoords = tex; SetUpdate(); }
        
    protected:
        texf4 texcoords;
        bool direct_texcoord = false;
    };
    
    class UISprite9 : public UISpriteBase {
    public:
        virtual void RefreshVertices() {
            vertices.resize(20);
            indices.resize(60);
            FillQuad9(&vertices[0], &indices[0]);
        }
        
        void FillQuad9(vt2* v, int16_t* idx) {
            static const int16_t quad9_idx[] = {    16,17,18,18,17,19,
                0, 1, 4, 4, 1, 5, 1, 2, 5, 5, 2, 6, 2, 3, 6, 6, 3, 7,
                4, 5, 8, 8, 5, 9, 5, 6, 9, 9, 6, 10,6, 7, 10,10,7, 11,
                8, 9, 12,12,9, 13,9, 10,13,13,10,14,10,11,14,14,11,15
            };
            v2f verts[4], texcoord[4];
            verts[0] = ConvScreenCoord(area_pos.absolute);
            verts[1] = ConvScreenCoord(area_pos.absolute + v2i{border.left, border.top});
            verts[2] = ConvScreenCoord(area_pos.absolute + area_size.absolute - v2i{border.width, border.height});
            verts[3] = ConvScreenCoord(area_pos.absolute + area_size.absolute);
            texcoord[0] = texture->ConvTexCoord({tex_rect.left, tex_rect.top});
            texcoord[1] = texture->ConvTexCoord({tex_rect.left + border_tex.left, tex_rect.top + border_tex.top});
            texcoord[2] = texture->ConvTexCoord({tex_rect.left + tex_rect.width - border_tex.width, tex_rect.top + tex_rect.height - border_tex.height});
            texcoord[3] = texture->ConvTexCoord({tex_rect.left + tex_rect.width, tex_rect.top + tex_rect.height});
            for(int32_t i = 0; i < 4; ++i) {
                for(int32_t j = 0; j < 4; ++j) {
                    v[i * 4 + j].vertex = v2f{verts[j].x, verts[i].y};
                    v[i * 4 + j].texcoord = v2f{texcoord[j].x, texcoord[i].y};
                }
            }
            v[16].vertex = ConvScreenCoord(area_pos.absolute + v2i{back.left, back.top});
            v[17].vertex = ConvScreenCoord(area_pos.absolute + v2i{area_size.absolute.x - back.width, back.top});
            v[18].vertex = ConvScreenCoord(area_pos.absolute + v2i{back.left, area_size.absolute.y - back.height});
            v[19].vertex = ConvScreenCoord(area_pos.absolute + area_size.absolute - v2i{back.width, back.height});
            v[16].texcoord = texture->ConvTexCoord({back_tex.left, back_tex.top});
            v[17].texcoord = texture->ConvTexCoord({back_tex.left + back_tex.width, back_tex.top});
            v[18].texcoord = texture->ConvTexCoord({back_tex.left, back_tex.top + back_tex.height});
            v[19].texcoord = texture->ConvTexCoord({back_tex.left + back_tex.width, back_tex.top + back_tex.height});
            for(int32_t i = 0; i < 60; ++i)
                idx[i] = vert_index + quad9_idx[i];
            for(int16_t i = 0; i < 20; ++i)
                v[i].color = color;
        }
        
        inline void SetBackRect(recti bk, recti bkt) { back = bk; back_tex = bkt; SetUpdate(); }
        inline void SetBorderRect(recti bd, recti bdt) {border = bd; border_tex = bdt; SetUpdate();}
        
    protected:
        recti border = {0, 0, 0, 0};
        recti border_tex = {0, 0, 0, 0};
        recti back = {0, 0, 0, 0};
        recti back_tex = {0, 0, 0, 0};
    };
    
    template<bool USE> struct UISpriteTraits { using sprite_type = UISprite; };
    template<> struct UISpriteTraits<true> { using sprite_type = UISprite9; };
    template<bool USE> using UISpriteType = typename UISpriteTraits<USE>::sprite_type;
    
    class UIText : public UIComponent {
    public:
        virtual bool CheckAvailable() { return text_font != nullptr; }
        virtual int32_t GetPrimitiveType() { return GL_TRIANGLES; }
        virtual int32_t GetTextureId() { return text_font ? text_font->GetTexture().GetTextureId() : 0; }
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto pre_pos = area_pos.absolute;
            if(re_pos) {
                if(container) {
                    auto offset = container->GetAbsolutePosition();
                    auto pprop = container->GetAbsoluteSize();
                    area_pos.absolute = area_pos.relative + offset + DotFactor(pprop, area_pos.proportion) + DotFactor(area_size.absolute, self_factor);
                } else
                    area_pos.absolute = area_pos.relative + DotFactor(area_size.absolute, self_factor);
                if(pre_pos != area_pos.absolute)
                    SetUpdate();
            }
            return std::make_pair(pre_pos != area_pos.absolute, false);
        }
        
        virtual void RefreshVertices() {
            static const int16_t quad_idx[] = {0, 1, 2, 2, 1, 3};
            advance.x = 0;
            advance.y = 0;
            int32_t line_height = 0;
            int32_t actual_size = 0;
            for(size_t i = 0; i < texts.length(); ++i) {
                auto ch = texts[i];
                if(ch < L' ') {
                    if(ch == L'\n') {
                        advance.x = 0;
                        advance.y += line_height + line_spacing;
                        line_height = 0;
                    }
                    continue;
                }
                auto color = colors[i];
                auto& gl = text_font->GetGlyph(ch);
                if((uint32_t)(advance.x + gl.advance) > max_width) {
                    advance.x = 0;
                    advance.y += line_height + line_spacing;
                    line_height = 0;
                }
                auto final_offset = area_pos.absolute + advance + offset;
                auto v = &vertices[actual_size * 4];
                auto idx = &indices[actual_size * 6];
                v[0].vertex = ConvScreenCoord(v2i{gl.bounds.left, gl.bounds.top} + final_offset);
                v[1].vertex = ConvScreenCoord(v2i{gl.bounds.left + gl.bounds.width, gl.bounds.top} + final_offset);
                v[2].vertex = ConvScreenCoord(v2i{gl.bounds.left, gl.bounds.top + gl.bounds.height} + final_offset);
                v[3].vertex = ConvScreenCoord(v2i{gl.bounds.left + gl.bounds.width, gl.bounds.top + gl.bounds.height} + final_offset);
                v[0].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left, gl.textureRect.top});
                v[1].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left + gl.textureRect.width, gl.textureRect.top});
                v[2].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left, gl.textureRect.top + gl.textureRect.height});
                v[3].texcoord = text_font->GetTexture().ConvTexCoord({gl.textureRect.left + gl.textureRect.width, gl.textureRect.top + gl.textureRect.height});
                for(int16_t j = 0; j < 6; ++j)
                    idx[j] = vert_index + actual_size * 4 + quad_idx[j];
                if(!text_font->IsEmoji(ch))
                    for(int16_t j = 0; j < 4; ++j)
                        v[j].color = color;
                advance.x += gl.advance;
                line_height = std::max(text_font->GetLineSpacing(ch), line_height);
                actual_size++;
            }
            for(int32_t i = vert_size; i < vert_cap; ++i) {
                auto idx = &indices[i * 6];
                for(auto j = 0; j < 6; ++j)
                    idx[j] = 0;
            }
        }
        
        void EvaluateSize() {
            area_size.absolute = v2i{0, 0};
            evaluate_info = v2i{0, 0};
            text_pos.clear();
            EvaluateSize(L"");
        }
        
        void EvaluateSize(const std::wstring& txt) {
            auto pre_size = area_size.absolute;
            for(auto ch : txt) {
                text_pos.push_back(v2i{evaluate_info.x, area_size.absolute.y - evaluate_info.y});
                if(ch < L' ') {
                    if(ch == L'\n') {
                        if(evaluate_info.x > area_size.absolute.x)
                            area_size.absolute.x = evaluate_info.x;
                        area_size.absolute.y += line_spacing;
                        if(evaluate_info.x == 0)
                            area_size.absolute.y += text_font->GetFontSize();
                        evaluate_info = v2i{0, 0};
                    }
                    continue;
                }
                auto& gl = text_font->GetGlyph(ch);
                if((max_width != 0xffffffff) && ((uint32_t)(evaluate_info.x + gl.advance) > max_width)) {
                    if(evaluate_info.x > area_size.absolute.x)
                        area_size.absolute.x = evaluate_info.x;
                    area_size.absolute.y += line_spacing;
                    evaluate_info = v2i{0, 0};
                }
                evaluate_info.x += gl.advance;
                if(text_font->GetLineSpacing(ch) > evaluate_info.y) {
                    area_size.absolute.y += text_font->GetLineSpacing(ch) - evaluate_info.y;
                    evaluate_info.y = text_font->GetLineSpacing(ch);
                }
            }
            last_text_pos = {evaluate_info.x, area_size.absolute.y - evaluate_info.y};
            if(evaluate_info.x > area_size.absolute.x)
                area_size.absolute.x = evaluate_info.x;
            if(area_size.absolute != pre_size) {
                OnPositionSizeChange(true, true);
                if(size_cb)
                    size_cb(area_size.absolute);
            }
        }
        
        inline void SetFont(base::Font* ft) { if(!ft || ft == text_font) return; text_font = ft; EvaluateSize(); SetRedraw(true); }
        inline void SetMaxWidth(int32_t mw) { if(max_width == mw) return; max_width = mw; EvaluateSize(); SetUpdate(); }
        inline void SetLinespacing(int32_t ls) { if(line_spacing == ls) return; line_spacing = ls; EvaluateSize(); SetUpdate(); }
        inline void SetText(const std::wstring& t, uint32_t cl) { Clear(); AppendText(t, cl); }
        inline void Clear() { texts.clear(); colors.clear(); vert_size = 0; EvaluateSize(); SetUpdate(); }
        inline const std::wstring& GetText() { return texts; }
        inline void SetSizeChangeCallback(std::function<void(v2i)> cb) { size_cb = cb;}
        inline int32_t GetFontSize() { return text_font ? text_font->GetFontSize() : 0; }
        inline v2i GetTextPosition(int32_t index) { return (index >= 0 && index < text_pos.size()) ? text_pos[index] : last_text_pos; }

        void SetCapacity(int32_t cap) {
            if(vert_cap >= cap)
                return;
            vertices.resize(cap * 4);
            indices.resize(cap * 6);
            vert_cap = cap;
            SetRedraw(false);
        }
        
        void InsertText(int32_t start, const std::wstring& txt, uint32_t cl) {
            if(start >= (int32_t)texts.size())
                start = (int32_t)texts.size();
            auto new_texts = std::move(texts);
            auto new_color = std::move(colors);
            new_texts.insert(new_texts.begin() + start, txt.begin(), txt.end());
            new_color.insert(new_color.begin() + start, txt.length(), cl);
            vert_size = 0;
            EvaluateSize();
            AppendText(new_texts, 0);
            colors = std::move(new_color);
        }
        
        void RemoveText(int32_t start, int32_t len) {
            if(start >= texts.size() || len <= 0)
                return;
            if(start + len > texts.size())
                len = (int32_t)texts.size() - start;
            auto new_texts = std::move(texts);
            auto new_color = std::move(colors);
            new_texts.erase(new_texts.begin() + start, new_texts.begin() + start + len);
            new_color.erase(new_color.begin() + start, new_color.begin() + start + len);
            vert_size = 0;
            EvaluateSize();
            AppendText(new_texts, 0);
            colors = std::move(new_color);
        }
        
        void ReplaceText(int32_t start, int32_t len, const std::wstring& txt, uint32_t cl) {
            if(start >= texts.size() || len <= 0)
                return;
            if(start + len > texts.size())
                len = (int32_t)texts.size() - start;
            auto new_texts = std::move(texts);
            auto new_color = std::move(colors);
            new_texts.erase(new_texts.begin() + start, new_texts.begin() + start + len);
            new_color.erase(new_color.begin() + start, new_color.begin() + start + len);
            new_texts.insert(new_texts.begin() + start, txt.begin(), txt.end());
            new_color.insert(new_color.begin() + start, txt.length(), cl);
            vert_size = 0;
            EvaluateSize();
            AppendText(new_texts, 0);
            colors = std::move(new_color);
        }
        
        void AppendText(const std::wstring& txt, uint32_t cl) {
            int32_t app_size = 0;
            for(auto& ch : txt) {
                if(ch >= ' ')
                    app_size++;
            }
            auto redraw = false;
            if(vert_size + app_size > vert_cap) {
                while(vert_size + app_size > vert_cap) {
                    if(vert_cap == 0)
                        vert_cap = 8;
                    else
                        vert_cap *= 2;
                }
                vertices.resize(vert_cap * 4);
                indices.resize(vert_cap * 6);
                redraw = true;
            }
            texts.append(txt);
            colors.insert(colors.end(), txt.length(), cl);
            vert_size += app_size;
            EvaluateSize(txt);
            if(redraw)
                SetRedraw(true);
            else
                SetUpdate();
        }
        
        int32_t CheckHitPositionSingleLine(int32_t x) {
            for(size_t i = 0; i < texts.size(); ++i)
                if(text_pos[i].x > x)
                    return (i > 0) ? ((int32_t)i - 1) : 0;
            if(last_text_pos.x > x)
                return (int32_t)texts.size() - 1;
            else
                return (int32_t)texts.size();;
        }
        
        using format_callback = std::function<void(UIText* ui, const std::wstring& type, const std::wstring& value, uint32_t color)>;
        void PushStringWithFormat(const std::wstring& str, uint32_t cur_color, format_callback unknown_format_cb = nullptr) {
            std::wstring match_str = str;
            std::wregex re(L"\\[ *(\\w+) *: *(\\w+) *\\]");
            std::wsmatch m;
            while(std::regex_search(match_str, m, re)) {
                AppendText(m.prefix(), cur_color);
                match_str = m.suffix();
                std::wstring type = m[1].str();
                if(type == L"c") {
                    uint32_t agbr = To<uint32_t>(m[2].str());
                    cur_color = ((agbr >> 24) & 0xff) | (((agbr >> 8) & 0xff00)) | ((agbr << 8) & 0xff0000) | ((agbr << 24) & 0xff000000);
                } else if(type == L"m") {
                    std::wstring em;
                    em.push_back(0xe000 + To<uint32_t>(m[2].str()));
                    AppendText(em, cur_color);
                } else if(unknown_format_cb) {
                    unknown_format_cb(this, type, m[2].str(), cur_color);
                }
            }
            AppendText(match_str, cur_color);
        }
        
    protected:
        base::Font* text_font = nullptr;
        int32_t vert_cap = 0;
        int32_t vert_size = 0;
        uint32_t max_width = 0xffffffff;
        uint32_t back_color = 0x40404040;
        int32_t line_spacing = 0;
        v2i offset = {0, 0};
        v2i advance = {0, 0};
        v2i evaluate_info = {0, 0};
        v2i last_text_pos = {0, 0};
        std::wstring texts;
        std::vector<uint32_t> colors;
        std::vector<v2i> text_pos;
        std::function<void(v2i)> size_cb;
    };

    // Json helper
    
    class SGJsonUtil {
    public:
        static v2i ConvertV2i(jaweson::JsonNode<>& node, int32_t index) {
            v2i ret = {0, 0};
            if(node.is_array()) {
                ret.x = (int32_t)node[index].to_integer();
                ret.y = (int32_t)node[index + 1].to_integer();
            }
            return ret;
        }
        
        static v2f ConvertV2f(jaweson::JsonNode<>& node, int32_t index) {
            v2f ret = {0.0f, 0.0f};
            if(node.is_array()) {
                ret.x = node[index].to_double();
                ret.y = node[index + 1].to_double();
            }
            return ret;
        }
        
        static recti ConvertRecti(jaweson::JsonNode<>& node) {
            recti ret = {0, 0, 0, 0};
            if(node.is_array()) {
                ret.left = (int32_t)node[0].to_integer();
                ret.top = (int32_t)node[1].to_integer();
                ret.width = (int32_t)node[2].to_integer();
                ret.height = (int32_t)node[3].to_integer();
            }
            return ret;
        }
        
        static rectf ConvertRectf(jaweson::JsonNode<>& node) {
            rectf ret = {0, 0, 0, 0};
            if(node.is_array()) {
                ret.left = (float)node[0].to_double();
                ret.top = (float)node[1].to_double();
                ret.width = (float)node[2].to_double();
                ret.height = (float)node[3].to_double();
            }
            return ret;
        }
        
        static uint32_t ConvertRGBA(jaweson::JsonNode<>& node) {
            std::string rgba = node.to_string();
            uint32_t agbr = To<uint32_t>(rgba);
            return ((agbr >> 24) & 0xff) | (((agbr >> 8) & 0xff00)) | ((agbr << 8) & 0xff0000) | ((agbr << 24) & 0xff000000);
        }
        
        static void SetUIPositionSize(jaweson::JsonNode<>& node, RegionObject* obj, v2i offset) {
            v2i pos_relative = SGJsonUtil::ConvertV2i(node, 0);
            v2i sz_relative = SGJsonUtil::ConvertV2i(node, 2);
            v2f pos_prop = SGJsonUtil::ConvertV2f(node, 4);
            v2f sz_prop = SGJsonUtil::ConvertV2f(node, 6);
            v2f self_factor = SGJsonUtil::ConvertV2f(node, 8);
            obj->SetPositionSize(pos_relative + offset, sz_relative, pos_prop, sz_prop, self_factor);
        }
    };
    
    //
    class SGClickableObject {
    public:
        SGEventHandler<SGClickableObject, int32_t, int32_t> event_drag_begin;
        SGEventHandler<SGClickableObject, int32_t, int32_t> event_drag_end;
        SGEventHandler<SGClickableObject, int32_t, int32_t, int32_t, int32_t> event_drag_update;
        
    public:
        virtual ~SGClickableObject() {}
        
        virtual bool OnDragBegin(int32_t x, int32_t y) = 0;
        virtual bool OnDragEnd(int32_t x, int32_t y) = 0;
        virtual bool OnDragUpdate(int32_t sx, int32_t sy, int32_t dx, int32_t dy) = 0;
        
        inline std::shared_ptr<SGClickableObject> GetDragTarget() { return drag_target.lock(); }
        inline void SetDragTarget(std::shared_ptr<SGClickableObject> target) { drag_target = target; }
        
    protected:
        std::weak_ptr<SGClickableObject> drag_target;
    };
    
    class SGClickingMgr : public Singleton<SGClickingMgr> {
    public:
        bool DragBegin(std::shared_ptr<SGClickableObject> dr, int32_t x, int32_t y) {
            if(!draging_object.expired())
                draging_object.lock()->OnDragEnd(x, y);
            draging_object = dr;
            start_point = {x, y};
            diff_value = {0, 0};
            if(dr)
                return dr->OnDragBegin(x, y);
            return false;
        }
        
        bool DragEnd(int32_t x, int32_t y) {
            if(!draging_object.expired()) {
                auto ret = draging_object.lock()->OnDragEnd(x, y);
                draging_object.reset();
                return ret;
            }
            return false;
        }
        
        bool DragUpdate(int32_t dx, int32_t dy) {
            if(!draging_object.expired()) {
                auto ret = draging_object.lock()->OnDragUpdate(start_point.x, start_point.y, dx, dy);
                start_point = v2i{dx, dy};
                return ret;
            }
            return false;
        }
        
        inline v2i& GetDiffValue() { return diff_value; }
        inline void CancelDrag() { draging_object.reset(); }
        inline std::shared_ptr<SGClickableObject> GetClickingObject() { return clicking_object.lock(); }
        inline void SetClickingObject(std::shared_ptr<SGClickableObject> target) { clicking_object = target; }

    public:
        std::weak_ptr<SGClickableObject> clicking_object;
        std::weak_ptr<SGClickableObject> draging_object;
        v2i start_point = {0, 0};
        v2i diff_value = {0, 0};
    };
    
    class SGWidget;
    
    class SGWidgetParent {
    public:
        virtual void RemoveChild(SGWidget* child) = 0;
        virtual void SetFocusWidget(SGWidget* child) = 0;
    };
    
    class SGGUIRenderer : public Singleton<SGGUIRenderer> {
    public:
        void SetRenderer(base::RenderObject2DLayout* r) { render_object = r; }
        base::RenderObject2DLayout* GetRenderer() { return render_object; }
    protected:
        base::RenderObject2DLayout* render_object = nullptr;
    };
    
    class SGWidget : public RegionObject, public SGClickableObject, public std::enable_shared_from_this<SGWidget> {
        friend class SGWidgetContainer;
    public:
        SGEventHandler<SGWidget, int32_t, int32_t> event_mouse_move;
        SGEventHandler<SGWidget> event_mouse_enter;
        SGEventHandler<SGWidget> event_mouse_leave;
        SGEventHandler<SGWidget, int32_t, int32_t, int32_t, int32_t> event_mouse_down;
        SGEventHandler<SGWidget, int32_t, int32_t, int32_t, int32_t> event_mouse_up;
        SGEventHandler<SGWidget, float, float> event_mouse_wheel;
        SGEventHandler<SGWidget, int32_t, int32_t> event_key_down;
        SGEventHandler<SGWidget, int32_t, int32_t> event_key_up;
        SGEventHandler<SGWidget, uint32_t> event_text_enter;
        SGEventHandler<SGWidget> event_get_focus;
        SGEventHandler<SGWidget> event_lose_focus;
        SGEventHandler<SGWidget> event_on_destroy;
        SGEventHandler<SGWidget> event_click;
        
    public:
        SGWidget() {}
        SGWidget(const SGWidget&) = delete;
        virtual ~SGWidget() {
            auto r = SGGUIRenderer::Get().GetRenderer();
            for(auto comp : ui_components)
                r->DeleteObject(comp);
            event_on_destroy.Trigger(*this);
        }
        
        template<typename T>
        inline std::shared_ptr<T> CastPtr() { return std::dynamic_pointer_cast<T>(shared_from_this()); }
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto ret = RegionObject::OnPositionSizeChange(re_pos, re_size);
            if(ret.first || ret.second) {
                for(auto comp : ui_components)
                    comp->OnPositionSizeChange(ret.first || ret.second, ret.second);
            }
            return ret;
        }
        
        virtual bool OnMouseMove(int32_t x, int32_t y) { return event_mouse_move.Trigger(*this, x, y) || is_entity; }
        virtual bool OnMouseEnter() { return event_mouse_enter.Trigger(*this) || is_entity; }
        virtual bool OnMouseLeave() { return event_mouse_leave.Trigger(*this) || is_entity; }
        virtual bool OnMouseDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
            auto ptr = shared_from_this();
            auto ret = is_entity;
            ret = event_mouse_down.Trigger(*this, button, mods, x, y) || ret;
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                SGClickingMgr::Get().SetClickingObject(ptr);
                auto dr = GetDragTarget();
                if(dr)
                    ret = SGClickingMgr::Get().DragBegin(dr, x, y) || ret;
            }
            return ret;
        }
        virtual bool OnMouseUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
            auto ptr = shared_from_this();
            auto ret = is_entity;
            ret = event_mouse_up.Trigger(*this, button, mods, x, y) || ret;
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                if(SGClickingMgr::Get().GetClickingObject().get() == this)
                    event_click.Trigger(*this);
            }
            return ret;
        }
        virtual bool OnMouseWheel(float deltax, float deltay) { return event_mouse_wheel.Trigger(*this, deltax, deltay) || is_entity; }
        virtual bool OnKeyDown(int32_t key, int32_t mods) { return event_key_down.Trigger(*this, key, mods) || is_entity; }
        virtual bool OnKeyUp(int32_t key, int32_t mods) { return event_key_up.Trigger(*this, key, mods) || is_entity; }
        virtual bool OnTextEnter(uint32_t unichar) { return event_text_enter.Trigger(*this, unichar) || is_entity; }
        virtual bool OnGetFocus() { return event_get_focus.Trigger(*this) || is_entity; }
        virtual bool OnLoseFocus() { return event_lose_focus.Trigger(*this) || is_entity; }
        virtual bool OnDestroy() { return event_on_destroy.Trigger(*this) || is_entity; }
        virtual bool OnDragBegin(int32_t x, int32_t y) { return event_drag_begin.Trigger(*this, x, y) || is_entity; }
        virtual bool OnDragEnd(int32_t x, int32_t y) { return event_drag_end.Trigger(*this, x, y) || is_entity; }
        virtual bool OnDragUpdate(int32_t sx, int32_t sy, int32_t dx, int32_t dy) { return event_drag_update.Trigger(*this, sx, sy, dx, dy) || is_entity; }
        
    public:
        inline bool IsVisible() { return is_visible; }
        inline void SetVisible(bool vis) { if(vis != is_visible) { is_visible = vis; SetRedraw(); } }
        inline bool IsEntity() { return is_entity; }
        inline bool AllowFocus() { return allow_focus; }
        inline void SetAllowFocus(bool foc) { allow_focus = foc; }
        inline void SetColor(uint32_t cl) { for(auto& ui : ui_components) ui->SetColor(cl); }
        inline void SetCustomValue(intptr_t val) { custom_value = val; }
        inline intptr_t GetCustomValue() { return custom_value; }
        
        virtual bool CheckInside(int32_t x, int32_t y) {
            return x >= area_pos.absolute.x && x <= area_pos.absolute.x + area_size.absolute.x
                && y >= area_pos.absolute.y && y <= area_pos.absolute.y + area_size.absolute.y;
        }
        
        virtual bool CheckInRect(recti rct) {
            return !(area_pos.absolute.x > rct.left + rct.width) && !(area_pos.absolute.x + area_size.absolute.x < rct.left)
                && !(area_pos.absolute.y > rct.top + rct.height) && !(area_pos.absolute.y + area_size.absolute.y < rct.top);
        }
        
        virtual void SetFocus() {
            if(!parent.expired())
                parent.lock()->SetFocusWidget(this);
        }
        
        virtual void RemoveFromParent() {
            if(!parent.expired())
                parent.lock()->RemoveChild(this);
        }
        
        virtual void SetRedraw() {
            SGGUIRenderer::Get().GetRenderer()->RequestRedraw();
        }
        
        virtual void SetUpdate() {
            SGGUIRenderer::Get().GetRenderer()->RequestUpdate();
        }
        
        virtual void InitUIComponents() = 0;
        virtual void PushUIComponents() { if(is_visible) for(auto comp : ui_components) comp->PushVertices(); }
        
        void SetName(std::string nm) { name = std::move(nm); }
        virtual SGWidget* FindWidget(const std::string& nm) { return name == nm ? this : nullptr; }
        
        template<typename T>
        inline T* FindWidgetAs(const std::string& nm) { return dynamic_cast<T*>(FindWidget(nm)); }
        
    protected:
        bool is_visible = true;
        bool is_entity = true;
        bool allow_focus = true;
        intptr_t custom_value = 0;
        std::weak_ptr<SGWidgetParent> parent;
        std::vector<UIComponent*> ui_components;
        std::string name;
    };
    
    class SGWidgetContainer : public SGWidget, public SGWidgetParent {
    public:
        virtual ~SGWidgetContainer() {}
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto ret = SGWidget::OnPositionSizeChange(re_pos, re_size);
            if(ret.first || ret.second) {
                for(auto& child : children)
                    child->OnPositionSizeChange(ret.first || ret.second, ret.second);
            }
            return ret;
        }
        
        template<typename WIDGET_TYPE, typename... TR>
        WIDGET_TYPE* NewChild(TR... tr) {
            static_assert(std::is_base_of<SGWidget, WIDGET_TYPE>::value, "widget type should be subclass of SGWidget.");
            auto ptr = std::make_shared<WIDGET_TYPE>(std::forward<TR>(tr)...);
            AddChild(ptr);
            return ptr.get();
        }
        
        virtual void AddChild(std::shared_ptr<SGWidget> child) {
            child->parent = std::static_pointer_cast<SGWidgetParent>(std::static_pointer_cast<SGWidgetContainer>(shared_from_this()));
            child->SetContainer(this);
            child->InitUIComponents();
            children.push_back(child);
            SetRedraw();
        }
        
        virtual void RemoveChild(SGWidget* child) {
            for(auto iter = children.begin(); iter != children.end(); ++iter) {
                if((*iter).get() == child) {
                    children.erase(iter);
                    SetRedraw();
                    return;
                }
            }
        }
        
        virtual void SetFocusWidget(SGWidget* child) {
            SetFocus();
            if((child->parent.lock().get() == this) && (focus_widget.lock().get() != child)) {
                focus_widget = child->shared_from_this();
                child->OnGetFocus();
            }
        }

        //  events
        virtual bool OnMouseMove(int32_t x, int32_t y) {
            auto ptr = shared_from_this();
            bool ret = event_mouse_move.Trigger(*this, x, y) || is_entity;
            auto nhoving = GetHovingWidget(x, y);
            auto choving = hoving_widget.lock();
            if(nhoving) {
                if(choving) {
                    if(choving == nhoving)
                        ret = choving->OnMouseMove(x, y) || ret;
                    else {
                        choving->OnMouseLeave();
                        ret = nhoving->OnMouseEnter() || ret;
                        ret = nhoving->OnMouseMove(x, y) || ret;
                    }
                } else {
                    ret = nhoving->OnMouseEnter() || ret;
                    ret = nhoving->OnMouseMove(x, y) || ret;
                }
            } else {
                if(choving)
                    choving->OnMouseLeave();
            }
            hoving_widget = nhoving;
            return ret;
        }
        
        virtual bool OnMouseEnter() {
            auto ptr = shared_from_this();
            bool ret = event_mouse_enter.Trigger(*this) || is_entity;
            hoving_widget.reset();
            return ret;
        }
        
        virtual bool OnMouseLeave() {
            auto ptr = shared_from_this();
            bool ret = event_mouse_leave.Trigger(*this) || is_entity;
            if(!hoving_widget.expired())
                hoving_widget.lock()->OnMouseLeave();
            hoving_widget.reset();
            return ret;
        }
        
        virtual bool OnMouseDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
            auto ptr = shared_from_this();
            bool ret = event_mouse_down.Trigger(*this, button, mods,x, y) || is_entity;
            auto choving = hoving_widget.lock();
            if(choving) {
                auto fwidget = focus_widget.lock();
                if(choving != fwidget && choving->AllowFocus()) {
                    if(fwidget)
                        fwidget->OnLoseFocus();
                    choving->OnGetFocus();
                    focus_widget = hoving_widget;
                }
                return choving->OnMouseDown(button, mods, x, y) || ret;
            } else {                
                if(button == GLFW_MOUSE_BUTTON_LEFT) {
                    SGClickingMgr::Get().SetClickingObject(ptr);
                    auto dr = GetDragTarget();
                    if(dr)
                        ret = SGClickingMgr::Get().DragBegin(dr, x, y) || ret;
                }
                return ret;
            }
        }
        
        virtual bool OnMouseUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
            auto ptr = shared_from_this();
            bool ret = event_mouse_up.Trigger(*this, button, mods,x, y) || is_entity;
            if(!hoving_widget.expired())
                ret = hoving_widget.lock()->OnMouseUp(button, mods, x, y) || ret;
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                if(SGClickingMgr::Get().GetClickingObject().get() == this)
                    event_click.Trigger(*this);
            }
            return ret;
        }
        
        virtual bool OnMouseWheel(float deltax, float deltay) {
            auto ptr = shared_from_this();
            bool ret = event_mouse_wheel.Trigger(*this, deltax, deltay) || is_entity;
            if(!hoving_widget.expired())
                ret = hoving_widget.lock()->OnMouseWheel(deltax, deltay) || ret;
            return ret;
        }
        
        virtual bool OnKeyDown(int32_t key, int32_t mods) {
            auto ptr = shared_from_this();
            bool ret = event_key_down.Trigger(*this, key, mods) || is_entity;
            if(!focus_widget.expired())
                ret = focus_widget.lock()->OnKeyDown(key, mods) || ret;
            return ret;
        }
        
        virtual bool OnKeyUp(int32_t key, int32_t mods) {
            auto ptr = shared_from_this();
            bool ret = event_key_up.Trigger(*this, key, mods) || is_entity;
            if(!focus_widget.expired())
                ret = focus_widget.lock()->OnKeyUp(key, mods) || ret;
            return ret;
        }
        
        virtual bool OnTextEnter(uint32_t unichar) {
            auto ptr = shared_from_this();
            bool ret = event_text_enter.Trigger(*this, unichar) || is_entity;
            if(!focus_widget.expired())
                ret = focus_widget.lock()->OnTextEnter(unichar) || ret;
            return ret;
        }
        
        virtual bool OnLoseFocus() {
            auto ptr = shared_from_this();
            bool ret = event_lose_focus.Trigger(*this) || is_entity;
            if(!focus_widget.expired())
                ret = focus_widget.lock()->OnLoseFocus() || ret;
            focus_widget.reset();
            return ret;
        }
        
        virtual void InitUIComponents() {}
        virtual void PushUIComponents() {
            auto renderer = SGGUIRenderer::Get().GetRenderer();
            auto shader = renderer->GetShader();
            alpha_cmd = renderer->PushCommand<base::RenderCmdBeginGlobalAlpha>(shader, container_alpha);
            SGWidget::PushUIComponents();
            if(is_visible)
                for(auto& child : children)
                    child->PushUIComponents();
            renderer->PushCommand<base::RenderCmdEndGlobalAlpha>(shader);
        }
        
        virtual SGWidget* FindWidget(const std::string& nm) {
            if(name == nm)
                return this;
            for(auto& iter : children) {
                auto ret = iter->FindWidget(nm);
                if(ret)
                    return ret;
            }
            return nullptr;
        }
        
        virtual std::shared_ptr<SGWidget> GetHovingWidget(int32_t x, int32_t y) {
            for(auto iter = children.rbegin(); iter != children.rend(); ++iter) {
                if((*iter)->IsVisible() && (*iter)->AllowFocus() && (*iter)->CheckInside(x, y))
                    return (*iter);
            }
            return nullptr;
        }
        
        void SetContainerAlpha(float f) {
            container_alpha = f;
            if(!alpha_cmd.expired())
                alpha_cmd.lock()->global_alpha = container_alpha;
            SetUpdate();
        }
        
        inline SGWidget* CurrentHovingWidget() { return hoving_widget.lock().get(); }
        inline SGWidget* CurrentFocusWidget() { return focus_widget.lock().get(); }
        
    protected:
        float container_alpha = 1.0f;
        std::weak_ptr<SGWidget> hoving_widget;
        std::weak_ptr<SGWidget> focus_widget;
        std::vector<std::shared_ptr<SGWidget>> children;
        std::weak_ptr<base::RenderCmdBeginGlobalAlpha<vt2>> alpha_cmd;
    };
    
    class SGGUIRoot : public SGWidgetContainer, public base::RenderObject2DLayout, public SysClock, public Timer<uint64_t> {
    public:
        static SGGUIRoot& GetSingleton() {
            static std::shared_ptr<SGGUIRoot> ptr;
            if(ptr == nullptr)
                ptr = std::make_shared<SGGUIRoot>();
            return *ptr;
        }
        
        bool Init(const std::string& conf_file, v2i scr_size, bool use_vao) {
            is_entity = false;
            InitGLState(use_vao);
            SetScreenSize(scr_size);
            InitSysClock();
            InitTimer(0);
            TextFile txt(conf_file);
            config_root = std::make_shared<jaweson::JsonRoot<>>();
            if(!config_root->parse(txt.Data(), txt.Length())) {
                auto err = config_root->get_error_info();
                std::cout << "Error loading config file: " << std::get<1>(err) << " at position " << std::get<0>(err) << std::endl;
                return false;
            }
            std::string tex_file = (*config_root)["gui_texture"].to_string();
            base::Image img;
            img.LoadFile(tex_file);
            gui_texture = std::make_shared<base::Texture>();
            gui_texture->Load(img.GetRawData(), img.GetWidth(), img.GetHeight());
            auto& font_node = (*config_root)["fonts"];
            auto& emoji_node = (*config_root)["emoji"];
            base::Image emoji;
            emoji.LoadFile(emoji_node["texture"].to_string());
            auto sz = SGJsonUtil::ConvertV2i(emoji_node["size"], 0);
            auto ct = SGJsonUtil::ConvertV2i(emoji_node["count"], 0);
            font_node.for_each([this, &emoji, &sz, &ct](const std::string& name, jaweson::JsonNode<>& info) {
                std::string font_file = info[0].to_string();
                int32_t font_size = (int32_t)info[1].to_integer();
                std::string style = info[2].to_string();
                auto off = SGJsonUtil::ConvertV2i(info, 3);
                auto ft = std::make_shared<base::Font>();
                if(ft->Load(font_file, font_size, style, off)) {
                    ft->LoadEmoji(emoji, sz, ct, {off.x, -off.y, font_size, font_size}, 0);
                    gui_fonts[name] = ft;
                }
            });
            SGGUIRenderer::Get().SetRenderer(this);
            return true;
        }
        
        void Uninit() { ClearChilds(); }
        void ClearChilds() { children.clear(); }
        
        virtual void SetScreenSize(v2i sz) {
            RenderObject2DLayout::SetScreenSize(sz);
            area_size.absolute = sz;
            for(auto& child : children)
                child->OnPositionSizeChange(true, true);
        }
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) { return std::make_pair(false, false); }
        
        virtual void PushVerticesAll() {
            base::RenderObject2DLayout::PushVerticesAll();
            PushUIComponents();
        }
        
        virtual bool PrepareRender() {
            UpdateSysClock();
            UpdateTimer(GetSysClock());
            return base::RenderObject2DLayout::PrepareRender();
        }
        
        bool InjectMouseEnterEvent() {
            inside_scene = true;
            return OnMouseEnter();
        }
        
        bool InjectMouseLeaveEvent() {
            inside_scene = false;
            return OnMouseLeave();
        }
        
        bool InjectMouseMoveEvent(int32_t x, int32_t y) {
            if(!inside_scene)
                OnMouseEnter();
            SGClickingMgr::Get().DragUpdate(x, y);
            mouse_pos = {x, y};
            return OnMouseMove(x, y);
        }
        
        bool InjectMouseButtonDownEvent(int32_t button, int32_t mods, int32_t x, int32_t y) {
            if(!inside_scene) {
                InjectMouseEnterEvent();
                InjectMouseMoveEvent(x, y);
            }
            if(!hoving_widget.expired()) {
                auto hwidget = hoving_widget.lock();
                if(hwidget != children.back()) {
                    for(auto iter = children.begin(); iter != children.end(); ++iter) {
                        if(*iter == hwidget) {
                            children.erase(iter);
                            children.push_back(hwidget);
                            SetRedraw();
                            break;
                        }
                    }
                }
                while(popup_objects.size() > 0) {
                    auto ptr = popup_objects.back().lock();
                    if(ptr == nullptr) {
                        popup_objects.pop_back();
                        continue;
                    } else if(hwidget != ptr) {
                        ptr->RemoveFromParent();
                        popup_objects.pop_back();
                        continue;
                    } else
                        break;
                }
            } else {
                for(auto& pop : popup_objects) {
                    auto ptr = pop.lock();
                    if(ptr != nullptr)
                        ptr->RemoveFromParent();
                }
                popup_objects.clear();
            }
            return OnMouseDown(button, mods, x, y);
        }
        
        bool InjectMouseButtonUpEvent(int32_t button, int32_t mods, int32_t x, int32_t y) {
            auto ret = OnMouseUp(button, mods, x, y);
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                SGClickingMgr::Get().DragEnd(x, y);
                SGClickingMgr::Get().SetClickingObject(nullptr);
            }
            return ret;
        }
        
        bool InjectMouseWheelEvent(float deltax, float deltay) {
            return OnMouseWheel(deltax, deltay);
        }
        
        bool InjectKeyDownEvent(int32_t key, int32_t mods) {
            return OnKeyDown(key, mods);
        }
        
        bool InjectKeyUpEvent(int32_t key, int32_t mods) {
            return OnKeyUp(key, mods);
        }
        
        bool InjectCharEvent(uint32_t unichar) {
            return OnTextEnter(unichar);
        }
        
        inline base::Texture* GetGuiTexture() { return gui_texture.get(); };
        inline jaweson::JsonRoot<>& GetConfig() { return *config_root; }
        inline void PopupObject(std::shared_ptr<SGWidget> obj) { popup_objects.push_back(obj); }
        inline void PopupCancel(std::shared_ptr<SGWidget> obj) {
            for(auto iter = popup_objects.begin(); iter != popup_objects.end(); ) {
                auto ptr = (*iter).lock();
                if(ptr == obj)
                    popup_objects.erase(iter++);
                else
                    iter++;
            }
        }
        inline base::Font* GetGuiFont(const std::string& name) {
            auto iter = gui_fonts.find(name);
            if(iter == gui_fonts.end())
                return nullptr;
            return iter->second.get();
        }
        inline v2i GetMousePosition() { return mouse_pos; }
        inline recti ConvertScissorRect(recti rc) {
            return recti{
                rc.left,
                screen_size.y - (rc.top + rc.height),
                rc.width,
                rc.height,
            };
        }
        
        inline void SetClipboardCallback(std::function<std::string()> g, std::function<void(const std::string&)> s) {
            clipboard_getcb = g;
            clipboard_setcb = s;
        }
        
        inline std::string GetClipboardString() {
            if(clipboard_getcb)
                return std::move(clipboard_getcb());
            return "";
        }
        
        inline void SetClipboardString(const std::string& str) {
            if(clipboard_setcb)
                clipboard_setcb(str);
        }
        
    protected:
        bool inside_scene = true;
        v2i mouse_pos = {0, 0};
        std::shared_ptr<base::Texture> gui_texture;
        std::shared_ptr<jaweson::JsonRoot<>> config_root;
        std::list<std::weak_ptr<SGWidget>> popup_objects;
        std::map<std::string, std::shared_ptr<base::Font>> gui_fonts;
        std::function<std::string()> clipboard_getcb;
        std::function<void(const std::string&)> clipboard_setcb;
    };
    
    template<typename UI_TYPE>
    class SGCommonUIWidget : public SGWidget {
    public:
        ~SGCommonUIWidget() {
            SGGUIRoot::GetSingleton().DeleteObject(common_ui);
        }
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto ret = SGWidget::OnPositionSizeChange(re_pos, re_size);
            if(ret.first || ret.second)
                common_ui->OnPositionSizeChange(ret.first || ret.second, ret.second);
            return ret;
        }
        
        virtual void InitUIComponents() {
            common_ui = SGGUIRoot::GetSingleton().NewObject<UI_TYPE>();
            common_ui->SetContainer(this);
        }
        
        virtual void PushUIComponents() {
            SGWidget::PushUIComponents();
            if(is_visible)
                common_ui->PushVertices();
        }
        
        virtual void InitUIComponentsExtra(jaweson::JsonNode<>&) = 0;
        
    protected:
        UI_TYPE* common_ui = nullptr;
    };
    
    template<>
    class SGCommonUIWidget<void> : public SGWidget {
    public:
        virtual void InitUIComponents() {}
        virtual void InitUIComponentsExtra(jaweson::JsonNode<>&) {}
    protected:
        UIComponent* common_ui = nullptr;
    };
    
    class SGCommonUISprite: public SGCommonUIWidget<UISprite> {
    public:
        virtual void InitUIComponentsExtra(jaweson::JsonNode<>& node) {
            SGJsonUtil::SetUIPositionSize(node["sprite_offset"], common_ui, {0, 0});
        }
        inline UISprite* GetSpriteUI() { return common_ui; }
        void SetAnimation(const std::vector<recti>& rects, int32_t interval) {
            if(rects.empty() || interval <= 0)
                return;
            auto ver = SGGUIRoot::GetSingleton().GetSysClock();
            animation_ver = ver;
            std::weak_ptr<SGCommonUISprite> thisptr = std::static_pointer_cast<SGCommonUISprite>(shared_from_this());
            auto rect_vec = std::make_shared<std::vector<recti>>(rects);
            int32_t index = 0;
            SGGUIRoot::GetSingleton().RegisterTimerEvent([interval, ver, thisptr, rect_vec, index]() mutable ->int64_t {
                if(thisptr.expired())
                    return 0;
                auto ptr = thisptr.lock();
                if(ptr->animation_ver != ver)
                    return 0;
                ptr->GetSpriteUI()->SetTextureRect(rect_vec->at(index));
                index++;
                if(index >= rect_vec->size())
                    index = 0;
                return interval;
            }, 0);
        }
        
    protected:
        int64_t animation_ver = 0;
    };
    
    class SGCommonUISpriteList: public SGCommonUIWidget<UISpriteList> {
    public:
        virtual void InitUIComponentsExtra(jaweson::JsonNode<>& node) {
            common_ui->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
        }
        inline UISpriteList* GetSpriteUI() { return common_ui; }
    };
    
    class SGCommonUIText : public SGCommonUIWidget<UIText> {
    public:
        virtual void InitUIComponentsExtra(jaweson::JsonNode<>& node) {
            auto ft = SGGUIRoot::GetSingleton().GetGuiFont(node["font"].to_string());
            common_ui->SetFont(ft);
            common_ui->SetCapacity(16);
            SGJsonUtil::SetUIPositionSize(node["text_offset"], common_ui, ft->GetTextOffset());
        }
        inline UIText* GetTextUI() { return common_ui; }
    };
    
    class SGCommonUIRegion: public SGCommonUIWidget<UIConvexRegion> {
    public:
        virtual void InitUIComponentsExtra(jaweson::JsonNode<>& node) {
            SGJsonUtil::SetUIPositionSize(node["region_offset"], common_ui, {0, 0});
        }
        inline UIConvexRegion* GetConvex() { return common_ui; }
    };
    
    class SGAutoSizeTextWidget : public SGCommonUIText {
    public:
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto pre_pos = area_pos.absolute;
            auto pre_size = area_size.absolute;
            area_size.absolute = GetAutoSize();
            if(re_pos) {
                if(container) {
                    auto offset = container->GetAbsolutePosition();
                    auto pprop = container->GetAbsoluteSize();
                    area_pos.absolute = area_pos.relative + offset + DotFactor(pprop, area_pos.proportion) + DotFactor(area_size.absolute, self_factor);
                } else
                    area_pos.absolute = area_pos.relative + DotFactor(area_size.absolute, self_factor);
            }
            auto ret = std::make_pair(pre_pos != area_pos.absolute, pre_size != area_size.absolute);
            if(ret.first || ret.second) {
                for(auto comp : ui_components)
                    comp->OnPositionSizeChange(ret.first || ret.second, ret.second);
                common_ui->OnPositionSizeChange(ret.first || ret.second, ret.second);
            }
            return ret;
        }
        
        virtual void InitUIComponents() {
            SGCommonUIText::InitUIComponents();
            common_ui->SetSizeChangeCallback([this](v2i new_sz) {
                OnPositionSizeChange(true, true);
            });
        }
        
        virtual v2i GetAutoSize() = 0;
    };
    
    class SGLabel : public SGAutoSizeTextWidget {
    public:
        SGLabel() { allow_focus = false; }
        
        virtual void InitUIComponents() {
            SGAutoSizeTextWidget::InitUIComponents();
            auto& text_node = SGGUIRoot::GetSingleton().GetConfig()["label"];
            InitUIComponentsExtra(text_node);
        }
        
        virtual v2i GetAutoSize() {
            return common_ui->GetAbsoluteSize();
        }
        
    };
    
    class SGImage : public SGCommonUISprite {
    public:
        SGImage() { allow_focus = false; }
        
        virtual void InitUIComponents() {
            SGCommonUISprite::InitUIComponents();
            auto& image_node = SGGUIRoot::GetSingleton().GetConfig()["image"];
            InitUIComponentsExtra(image_node);
        }
        
    };
    
    class SGImageList : public SGCommonUISpriteList {
    public:
        SGImageList() { allow_focus = false; }
        
        virtual void InitUIComponents() {
            SGCommonUISpriteList::InitUIComponents();
            auto& image_node = SGGUIRoot::GetSingleton().GetConfig()["imagelist"];
            InitUIComponentsExtra(image_node);
        }
    };
    
    const static int32_t STATUS_NORMAL = 0;
    const static int32_t STATUS_HOVING = 1;
    const static int32_t STATUS_DOWN = 2;
    const static int32_t STATUS_PUSHED = 3;
    
    template<typename UI_WIDGET_TYPE, bool USE_SPRITE9>
    class SGButton : public UI_WIDGET_TYPE {
    public:
        SGButton(bool push_button = false) : is_push_button(push_button) {}
        
        virtual void InitUIComponents() {
            UI_WIDGET_TYPE::InitUIComponents();
            UISpriteType<USE_SPRITE9>* button_surface = SGGUIRoot::GetSingleton().NewObject<UISpriteType<USE_SPRITE9>>();
            this->ui_components.push_back(button_surface);
            button_surface->SetContainer(this);
            button_surface->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
            button_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            auto& button_node = SGGUIRoot::GetSingleton().GetConfig()["button"];
            SetStyle(button_node);
            this->InitUIComponentsExtra(button_node);
            if(this->common_ui)
                common_ui_offset = this->common_ui->GetPosAttr().relative;
        }
        
        virtual bool OnMouseEnter() {
            if(!is_push_button) {
                if((status == STATUS_DOWN) && SGClickingMgr::Get().GetClickingObject().get() == this) {
                    static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_DOWN]);
                    if(this->common_ui)
                        this->common_ui->SetPositionR(common_ui_offset + press_offset);
                } else {
                    status = STATUS_HOVING;
                    static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_HOVING]);
                }
            } else {
                if(status == STATUS_NORMAL) {
                    if(SGClickingMgr::Get().GetClickingObject().get() == this) {
                        static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_DOWN]);
                        if(this->common_ui)
                            this->common_ui->SetPositionR(common_ui_offset + press_offset);
                        status = STATUS_DOWN;
                    } else {
                        static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_HOVING]);
                        status = STATUS_HOVING;
                    }
                }
            }
            UI_WIDGET_TYPE::OnMouseEnter();
            return true;
        }
        
        virtual bool OnMouseLeave() {
            if(!is_push_button) {
                static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_NORMAL]);
                if((status == STATUS_DOWN) && this->common_ui)
                    this->common_ui->SetPositionR(common_ui_offset);
            } else {
                if(status != STATUS_PUSHED) {
                    static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_NORMAL]);
                    if((status == STATUS_DOWN) && this->common_ui)
                        this->common_ui->SetPositionR(common_ui_offset);
                    status = STATUS_NORMAL;
                }
            }
            UI_WIDGET_TYPE::OnMouseLeave();
            return true;
        }
        
        virtual bool OnMouseDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                if(!is_push_button) {
                    if(status != STATUS_DOWN) {
                        static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_DOWN]);
                        if(this->common_ui)
                            this->common_ui->SetPositionR(common_ui_offset + press_offset);
                        status = STATUS_DOWN;
                    }
                } else {
                    if(status != STATUS_PUSHED) {
                        static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_DOWN]);
                        if(this->common_ui)
                            this->common_ui->SetPositionR(common_ui_offset + press_offset);
                        status = STATUS_DOWN;
                    }
                }
            }
            UI_WIDGET_TYPE::OnMouseDown(button, mods, x, y);
            return true;
        }
        
        virtual bool OnMouseUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                if(!is_push_button) {
                    if(status != STATUS_HOVING) {
                        static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_HOVING]);
                        if(this->common_ui)
                            this->common_ui->SetPositionR(common_ui_offset);
                        status = STATUS_HOVING;
                    }
                } else {
                    if(status == STATUS_DOWN) {
                        status = STATUS_PUSHED;
                    } else if(status == STATUS_PUSHED) {
                        status = STATUS_HOVING;
                        static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_HOVING]);
                        if(this->common_ui)
                            this->common_ui->SetPositionR(common_ui_offset);
                    }
                }
            }
            UI_WIDGET_TYPE::OnMouseUp(button, mods, x, y);
            return true;
        }
        
        void SetStyle(jaweson::JsonNode<>& button_node) {
            auto button_surface = static_cast<UISprite*>(this->ui_components[0]);
            style[0] = SGJsonUtil::ConvertRecti(button_node["normal_tex"]);
            style[1] = SGJsonUtil::ConvertRecti(button_node["hover_tex"]);
            style[2] = SGJsonUtil::ConvertRecti(button_node["down_tex"]);
            press_offset = SGJsonUtil::ConvertV2i(button_node["press_offset"], 0);
            auto cl = SGJsonUtil::ConvertRGBA(button_node["color"]);
            auto styleindex = (status <= STATUS_DOWN) ? status : STATUS_DOWN;
            button_surface->SetColor(cl);
            button_surface->SetTextureRect(style[styleindex]);
            if(USE_SPRITE9) {
                auto button_surface9 = static_cast<UISprite9*>(this->ui_components[0]);
                auto border_rect = SGJsonUtil::ConvertRecti(button_node["border"]);
                auto back_border = SGJsonUtil::ConvertRecti(button_node["back_border"]);
                auto back_rect = SGJsonUtil::ConvertRecti(button_node["back_tex"]);
                button_surface9->SetBackRect(back_border, back_rect);
                button_surface9->SetBorderRect(border_rect, border_rect);
            }
        }
        
        void SetSimpleStyle(jaweson::JsonNode<>& button_node) {
            auto cl = SGJsonUtil::ConvertRGBA(button_node["color"]);
            style[0] = SGJsonUtil::ConvertRecti(button_node["normal_tex"]);
            style[1] = SGJsonUtil::ConvertRecti(button_node["hover_tex"]);
            style[2] = SGJsonUtil::ConvertRecti(button_node["down_tex"]);
            auto styleindex = (status <= STATUS_DOWN) ? status : STATUS_DOWN;
            static_cast<UISprite*>(this->ui_components[0])->SetColor(cl);
            static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[styleindex]);
        }
        
        inline bool IsPushed() { return status == STATUS_PUSHED; }
        void SetPushed(bool pushed) {
            if(!is_push_button || ((status == STATUS_PUSHED) == pushed))
                return;
            if(pushed) {
                status = STATUS_PUSHED;
                static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_PUSHED]);
                if(this->common_ui)
                    this->common_ui->SetPositionR(common_ui_offset + press_offset);
            } else {
                status = STATUS_NORMAL;
                static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(style[STATUS_NORMAL]);
                if(this->common_ui)
                    this->common_ui->SetPositionR(common_ui_offset);
            }
        }
        
    protected:
        v2i press_offset = {0, 0};
        v2i common_ui_offset = {0, 0};
        bool is_push_button = false;
        int8_t status = 0;
        recti style[3];
    };
    
    using SGSimpleButton = SGButton<SGCommonUIWidget<void>, false>;
    using SGTextButton = SGButton<SGCommonUIText, true>;
    using SGImageButton = SGButton<SGCommonUISprite, true>;
    using SGTextureButton = SGButton<SGCommonUIRegion, true>;
    
    template<bool USE_SPRITE9 = false>
    class SGCheckBox : public SGAutoSizeTextWidget {
    public:
        SGEventHandler<SGWidget, bool> event_check_change;
        
    public:
        SGCheckBox(bool chk = false) : checked(chk) {}
        
        virtual void InitUIComponents() {
            SGAutoSizeTextWidget::InitUIComponents();
            UISpriteType<USE_SPRITE9>* cb_surface = SGGUIRoot::GetSingleton().NewObject<UISpriteType<USE_SPRITE9>>();
            auto& cb_node = SGGUIRoot::GetSingleton().GetConfig()["checkbox"];
            ui_components.push_back(cb_surface);
            cb_surface->SetContainer(this);
            cb_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            InitUIComponentsExtra(cb_node);
            SetStyle(cb_node);
        }
        
        void SetStyle(jaweson::JsonNode<>& cb_node) {
            auto cb_surface = static_cast<UISprite*>(this->ui_components[0]);
            style[0] = SGJsonUtil::ConvertRecti(cb_node["normal1"]);
            style[1] = SGJsonUtil::ConvertRecti(cb_node["hover1"]);
            style[2] = SGJsonUtil::ConvertRecti(cb_node["down1"]);
            style[3] = SGJsonUtil::ConvertRecti(cb_node["normal2"]);
            style[4] = SGJsonUtil::ConvertRecti(cb_node["hover2"]);
            style[5] = SGJsonUtil::ConvertRecti(cb_node["down2"]);
            auto cl = SGJsonUtil::ConvertRGBA(cb_node["color"]);
            auto& offset_node = cb_node["button_offset"];
            image_offset = SGJsonUtil::ConvertV2i(offset_node, 0);
            image_size = SGJsonUtil::ConvertV2i(offset_node, 2);
            auto ftsz = common_ui->GetFontSize();
            if(image_size.y < ftsz)
                image_size = v2i{ftsz, ftsz};
            auto styleindex = status + (checked ? 3 : 0);
            cb_surface->SetColor(cl);
            cb_surface->SetPositionSize(v2i{image_offset.x, 0}, image_size, {0.0f, 0.5f}, {0.0f, 0.0f}, {0.0f, -0.5f});
            cb_surface->SetTextureRect(style[styleindex]);
            if(USE_SPRITE9) {
                auto cb_surface9 = static_cast<UISprite9*>(this->ui_components[0]);
                auto border_rect = SGJsonUtil::ConvertRecti(cb_node["border"]);
                auto back_border = SGJsonUtil::ConvertRecti(cb_node["back_border"]);
                auto back_rect = SGJsonUtil::ConvertRecti(cb_node["back_tex"]);
                cb_surface9->SetBackRect(back_border, back_rect);
                cb_surface9->SetBorderRect(border_rect, border_rect);
            }
        }
        
        virtual v2i GetAutoSize() {
            v2i sz = common_ui->GetAbsoluteSize();
            int32_t ht = std::max(sz.y, image_size.y);
            return v2i{image_offset.x + image_offset.y + ht + sz.x, ht};
        }
        
        virtual bool OnMouseEnter() {
            if((status == STATUS_DOWN) && SGClickingMgr::Get().GetClickingObject().get() == this) {
                static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(style[STATUS_DOWN + (checked ? 3 : 0)]);
            } else {
                status = STATUS_HOVING;
                static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(style[STATUS_HOVING + (checked ? 3 : 0)]);
            }
            SGAutoSizeTextWidget::OnMouseEnter();
            return true;
        }
        
        virtual bool OnMouseLeave() {
            static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(style[STATUS_NORMAL + (checked ? 3 : 0)]);
            SGAutoSizeTextWidget::OnMouseLeave();
            return true;
        }
        
        virtual bool OnMouseDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                if(status != STATUS_DOWN) {
                    static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(style[STATUS_DOWN + (checked ? 3 : 0)]);
                    status = STATUS_DOWN;
                }
            }
            SGAutoSizeTextWidget::OnMouseDown(button, mods, x, y);
            return true;
        }
        
        virtual bool OnMouseUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                if(status != STATUS_HOVING) {
                    status = STATUS_HOVING;
                    SetChecked(!checked);
                }
            }
            SGAutoSizeTextWidget::OnMouseUp(button, mods, x, y);
            return true;
        }
        
        inline bool IsChecked() { return checked; }
        
        virtual void SetChecked(bool chk) {
            if(checked == chk)
                return;
            checked = chk;
            static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(style[status + (checked ? 3 : 0)]);
            event_check_change.Trigger(*this, checked);
        }
        
    protected:
        bool checked = false;
        int8_t status = 0;
        recti style[6];
        v2i image_offset = {0, 0};
        v2i image_size = {0, 0};
    };
    
    template<bool USE_SPRITE9 = false>
    class SGRadio : public SGCheckBox<USE_SPRITE9> {
    public:
        using SGCheckBox<USE_SPRITE9>::SGCheckBox;
        ~SGRadio() { DetachGroup(); }
        
        virtual void InitUIComponents() {
            SGAutoSizeTextWidget::InitUIComponents();
            UISpriteType<USE_SPRITE9>* rdo_surface = SGGUIRoot::GetSingleton().NewObject<UISpriteType<USE_SPRITE9>>();
            auto& rdo_node = SGGUIRoot::GetSingleton().GetConfig()["radio"];
            this->ui_components.push_back(rdo_surface);
            rdo_surface->SetContainer(this);
            rdo_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            this->InitUIComponentsExtra(rdo_node);
            this->SetStyle(rdo_node);
            next_group_member = this;
        }
        
        void AttackGroup(SGRadio* target) {
            if(target == this)
                return;
            if(next_group_member != this)
                DetachGroup();
            next_group_member = target->next_group_member;
            target->next_group_member = this;
        }
        
        void DetachGroup() {
            auto next = next_group_member;
            if(next == this)
                return;
            while(next->next_group_member != this)
                next = next->next_group_member;
            next->next_group_member = next_group_member;
            next_group_member = this;
        }
        
        SGRadio* GetCheckedTarget() {
            auto current = this;
            do {
                if(current->checked)
                    return current;
                current = current->next_group_member;
            } while (current != this);
            return nullptr;
        }
        
        virtual bool OnMouseUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                if(this->status != STATUS_HOVING) {
                    this->status = STATUS_HOVING;
                    if(!this->checked)
                        SetChecked(true);
                    else
                        static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(this->style[this->status + (this->checked ? 3 : 0)]);
                }
            }
            SGAutoSizeTextWidget::OnMouseUp(button, mods, x, y);
            return true;
        }
        
        virtual void SetChecked(bool chk) {
            if(this->checked == chk)
                return;
            if(chk) {
                auto pre = GetCheckedTarget();
                if(pre != nullptr && pre != this)
                    pre->SetChecked(false);
            }
            this->checked = chk;
            static_cast<UISprite*>(this->ui_components[0])->SetTextureRect(this->style[this->status + (this->checked ? 3 : 0)]);
            this->event_check_change.Trigger(*this, chk);
        }
        
    protected:
        SGRadio* next_group_member = nullptr;
    };
    
    template<bool USE_SPRITE9 = true>
    class SGScrollBar : public SGWidget {
    public:
        SGEventHandler<SGWidget, float> event_value_change;
        
    public:
        SGScrollBar(bool hori) : is_horizontal(hori) {}
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto pre_pos = area_pos.absolute;
            auto pre_size = area_size.absolute;
            if(container) {
                auto offset = container->GetAbsolutePosition();
                auto pprop = container->GetAbsoluteSize();
                if(re_size)
                    area_size.absolute = area_size.relative + DotFactor(pprop, area_size.proportion);
                if(!is_horizontal)
                    area_size.absolute.x = fix_size;
                else
                    area_size.absolute.y = fix_size;
                if(re_pos)
                    area_pos.absolute = area_pos.relative + offset + DotFactor(pprop, area_pos.proportion) + DotFactor(area_size.absolute, self_factor);
            } else {
                if(re_size)
                    area_size.absolute = area_size.relative;
                if(!is_horizontal)
                    area_size.absolute.x = fix_size;
                else
                    area_size.absolute.y = fix_size;
                if(re_pos)
                    area_pos.absolute = area_pos.relative + DotFactor(area_size.absolute, self_factor);
            }
            auto ret = std::make_pair(pre_pos != area_pos.absolute, pre_size != area_size.absolute);
            if(ret.first || ret.second) {
                for(auto comp : ui_components)
                    comp->OnPositionSizeChange(ret.first || ret.second, ret.second);
            }
            if(is_horizontal)
                pos_bound = {area_pos.absolute.x, area_pos.absolute.x + area_size.absolute.x - slength};
            else
                pos_bound = {area_pos.absolute.y, area_pos.absolute.y + area_size.absolute.y - slength};
            if(re_size) {
                current_pos = current_value * (pos_bound.y - pos_bound.x);
                if(is_horizontal)
                    static_cast<UISprite*>(this->ui_components[1])->SetPositionR({current_pos, 0});
                else
                    static_cast<UISprite*>(this->ui_components[1])->SetPositionR({0, current_pos});
            }
            return ret;
        }
        
        virtual void InitUIComponents() {
            UISpriteType<USE_SPRITE9>* slot_surface = SGGUIRoot::GetSingleton().NewObject<UISpriteType<USE_SPRITE9>>();
            UISpriteType<USE_SPRITE9>* slider_surface = SGGUIRoot::GetSingleton().NewObject<UISpriteType<USE_SPRITE9>>();
            ui_components.push_back(slot_surface);
            ui_components.push_back(slider_surface);
            slot_surface->SetContainer(this);
            slider_surface->SetContainer(this);
            slot_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            slider_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            auto& sc_node = SGGUIRoot::GetSingleton().GetConfig()["scroll"];
            SetStyle(sc_node);
            SetDragTarget(shared_from_this());
        }
        
        void SetStyle(jaweson::JsonNode<>& sc_node) {
            if(is_horizontal) {
                style[0] = SGJsonUtil::ConvertRecti(sc_node["sliderh1"]);
                style[1] = SGJsonUtil::ConvertRecti(sc_node["sliderh2"]);
                style[2] = SGJsonUtil::ConvertRecti(sc_node["sliderh3"]);
            } else {
                style[0] = SGJsonUtil::ConvertRecti(sc_node["sliderv1"]);
                style[1] = SGJsonUtil::ConvertRecti(sc_node["sliderv2"]);
                style[2] = SGJsonUtil::ConvertRecti(sc_node["sliderv3"]);
            }
            auto cl = SGJsonUtil::ConvertRGBA(sc_node["color"]);
            v2i slider_sz = SGJsonUtil::ConvertV2i(sc_node["slider_size"], 0);
            slength = is_horizontal ? slider_sz.x : slider_sz.y;
            v2i back_sz = SGJsonUtil::ConvertV2i(sc_node["back_size"], 0);
            fix_size = is_horizontal ? back_sz.y : back_sz.x;
            auto back_tex = SGJsonUtil::ConvertRecti(sc_node[is_horizontal ? "backh_tex" : "backv_tex"]);
            UISprite* slot_surface = static_cast<UISprite*>(this->ui_components[0]);
            UISprite* slider_surface = static_cast<UISprite*>(this->ui_components[1]);
            slot_surface->SetColor(cl);
            slot_surface->SetTextureRect(back_tex);
            slot_surface->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
            slider_surface->SetColor(cl);
            slider_surface->SetTextureRect(style[0]);
            if(is_horizontal)
                slider_surface->SetPositionSize({0, 0}, {slength, 0}, {0.0f, 0.0f}, {0.0f, 1.0f});
            else
                slider_surface->SetPositionSize({0, 0}, {0, slength}, {0.0f, 0.0f}, {1.0f, 0.0f});
            if(USE_SPRITE9) {
                auto slot_surface9 = static_cast<UISprite9*>(this->ui_components[0]);
                auto slider_surface9 = static_cast<UISprite9*>(this->ui_components[1]);
                auto border_rect = SGJsonUtil::ConvertRecti(sc_node["border"]);
                auto back_border = SGJsonUtil::ConvertRecti(sc_node["back_border"]);
                auto back_rect = SGJsonUtil::ConvertRecti(sc_node["back_tex"]);
                slot_surface9->SetBackRect(back_border, back_rect);
                slot_surface9->SetBorderRect(border_rect, border_rect);
                slider_surface9->SetBackRect(back_border, back_rect);
                slider_surface9->SetBorderRect(border_rect, border_rect);
            }
        }
        
        virtual bool OnDragBegin(int32_t x, int32_t y) {
            if(slength == 0 || !is_enabled) {
                SGClickingMgr::Get().CancelDrag();
                return true;
            }
            if(status == STATUS_HOVING) {
                is_draging = true;
                static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(style[STATUS_DOWN]);
            } else {
                if(is_horizontal) {
                    if(x < pos_bound.x + current_pos)
                        AddValue(-0.1f);
                    else
                        AddValue(0.1f);
                } else {
                    if(y < pos_bound.x + current_pos)
                        AddValue(-0.1f);
                    else
                        AddValue(0.1f);
                }
                SGClickingMgr::Get().CancelDrag();
                OnMouseMove(x, y);
            }
            return true;
        }
        
        virtual bool OnDragEnd(int32_t x, int32_t y) {
            is_draging = false;
            static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(style[status]);
            return true;
        }
        
        virtual bool OnDragUpdate(int32_t sx, int32_t sy, int32_t dx, int32_t dy) {
            if(slength == 0 || !is_enabled)
                return true;
            int32_t prepos = current_pos;
            auto& diff = SGClickingMgr::Get().GetDiffValue();
            if(is_horizontal)
                current_pos += dx - sx + diff.x;
            else
                current_pos += dy - sy + diff.x;
            diff.x = 0;
            if(current_pos < 0) {
                diff.x = current_pos;
                current_pos = 0;
            }
            if(current_pos > pos_bound.y - pos_bound.x) {
                diff.x = current_pos - (pos_bound.y - pos_bound.x);
                current_pos = pos_bound.y - pos_bound.x;
            }
            if(prepos != current_pos) {
                current_value = (float)current_pos / (pos_bound.y - pos_bound.x);
                if(is_horizontal)
                    static_cast<UISprite*>(this->ui_components[1])->SetPositionR({current_pos, 0});
                else
                    static_cast<UISprite*>(this->ui_components[1])->SetPositionR({0, current_pos});
                event_value_change.Trigger(*this, current_value);
            }
            return true;
        }
        
        virtual bool OnMouseMove(int32_t x, int32_t y) {
            CheckMousePosition(x, y);
            return SGWidget::OnMouseMove(x, y);
        }
        
        virtual bool OnMouseLeave() {
            if(!is_draging && status != STATUS_NORMAL) {
                status = STATUS_NORMAL;
                static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(style[STATUS_NORMAL]);
            }
            SGWidget::OnMouseLeave();
            return true;
        }
        
        virtual bool OnMouseWheel(float deltax, float deltay) {
            if(slength == 0 || !is_enabled)
                return true;
            AddValue(deltay / 50.0f);
            auto mpos = SGGUIRoot::GetSingleton().GetMousePosition();
            CheckMousePosition(mpos.x, mpos.y);
            return SGWidget::OnMouseWheel(deltax, deltay);
        }
        
        void CheckMousePosition(int32_t x, int32_t y) {
            if(slength == 0 || !is_enabled)
                return;
            auto pre_status = status;
            if(is_horizontal) {
                if(y >= area_pos.absolute.y && y <= area_pos.absolute.y + area_size.absolute.y) {
                    if(x >= area_pos.absolute.x + current_pos && x <= area_pos.absolute.x + current_pos + slength)
                        status = STATUS_HOVING;
                    else
                        status = STATUS_NORMAL;
                }
            } else {
                if(x >= area_pos.absolute.x && x <= area_pos.absolute.x + area_size.absolute.x) {
                    if(y >= area_pos.absolute.y + current_pos && y <= area_pos.absolute.y + current_pos + slength)
                        status = STATUS_HOVING;
                    else
                        status = STATUS_NORMAL;
                }
            }
            if(!is_draging && pre_status != status)
                static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(style[status]);
        }
        
        void SetValue(float cur, bool trigger = true) {
            int32_t prepos = current_pos;
            current_pos = cur * (pos_bound.y - pos_bound.x);
            if(current_pos < 0)
                current_pos = 0;
            if(current_pos > pos_bound.y - pos_bound.x)
                current_pos = pos_bound.y - pos_bound.x;
            if(prepos != current_pos) {
                auto slider_surface = static_cast<UISprite*>(this->ui_components[1]);
                current_value = (float)current_pos / (pos_bound.y - pos_bound.x);
                if(is_horizontal)
                    slider_surface->SetPositionR({current_pos, 0});
                else
                    slider_surface->SetPositionR({0, current_pos});
                if(trigger)
                    event_value_change.Trigger(*this, current_value);
            }
        }
        
        inline void AddValue(float val) { SetValue((float)current_pos / (pos_bound.y - pos_bound.x) + val); }
        inline float GetCurrentValue() { return current_value; }
        inline void SetEnabled(bool e) { if(is_enabled == e) return; is_enabled = e; this->ui_components[1]->SetVisible(e); }
        
    protected:
        bool is_horizontal = false;
        bool is_enabled = true;
        bool is_draging = false;
        int32_t status = 0;
        int32_t fix_size = 0;
        int32_t slength = 0;
        int32_t current_pos = 0;
        float current_value = 0.0f;
        v2i pos_bound = {0, 0};
        recti style[3];
    };
    
    class SGPanel : public SGWidgetContainer {
    public:
        SGPanel(bool entity = true) { is_entity = entity; }
        
        virtual bool OnDragUpdate(int32_t sx, int32_t sy, int32_t dx, int32_t dy) {
            auto pos = area_pos.absolute + v2i{dx - sx, dy - sy};
            SetPosition(pos);
            return true;
        }
        
        virtual void InitUIComponents() {
            SGWidgetContainer::InitUIComponents();
            if(is_entity) {
                UISprite9* panel_surface = SGGUIRoot::GetSingleton().NewObject<UISprite9>();
                ui_components.push_back(panel_surface);
                panel_surface->SetContainer(this);
                panel_surface->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
                panel_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
                auto& panel_node = SGGUIRoot::GetSingleton().GetConfig()["panel"];
                SetStyle(panel_node);
            }
        }
        
        void SetStyle(jaweson::JsonNode<>& panel_node) {
            if(is_entity) {
                auto panel_surface = static_cast<UISprite9*>(this->ui_components[0]);
                auto cl = SGJsonUtil::ConvertRGBA(panel_node["color"]);
                auto border_rect = SGJsonUtil::ConvertRecti(panel_node["border"]);
                auto border_tex = SGJsonUtil::ConvertRecti(panel_node["border_tex"]);
                auto back_border = SGJsonUtil::ConvertRecti(panel_node["back_border"]);
                auto back_rect = SGJsonUtil::ConvertRecti(panel_node["back_tex"]);
                auto client_tex = SGJsonUtil::ConvertRecti(panel_node["client_area"]);
                panel_surface->SetColor(cl);
                panel_surface->SetTextureRect(client_tex);
                panel_surface->SetBackRect(back_border, back_rect);
                panel_surface->SetBorderRect(border_rect, border_tex);
            }
        }
        
        inline void SetDragable(bool d) { SetDragTarget((d && is_entity) ? shared_from_this() : nullptr); }
    };
    
    class SGWindow : public SGPanel {
    public:
        SGWindow() : SGPanel(true) {}
        
        virtual bool OnDragBegin(int32_t x, int32_t y) {
            drag_status = 0;
            if(allow_resize
               && x >= area_pos.absolute.x + area_size.absolute.x - border.width
               && y >= area_pos.absolute.y + area_size.absolute.y - border.height)
                drag_status = 1;
            return true;
        }
        
        virtual bool OnDragUpdate(int32_t sx, int32_t sy, int32_t dx, int32_t dy) {
            if(drag_status == 0) {
                auto pos = area_pos.absolute + v2i{dx - sx, dy - sy};
                SetPosition(pos);
            } else {
                auto& diff_size = SGClickingMgr::Get().GetDiffValue();
                auto sz = area_size.absolute + v2i{dx - sx, dy - sy} + diff_size;
                if(sz.x < min_size.x) {
                    diff_size.x = sz.x - min_size.x;
                    sz.x = min_size.x;
                } else
                    diff_size.x = 0;
                if(sz.y < min_size.y) {
                    diff_size.y = sz.y - min_size.y;
                    sz.y = min_size.y;
                } else
                    diff_size.y = 0;
                SetSize(sz);
            }
            return true;
        }
        
        virtual void InitUIComponents() {
            SGWidgetContainer::InitUIComponents();
            UISprite9* panel_surface = SGGUIRoot::GetSingleton().NewObject<UISprite9>();
            ui_components.push_back(panel_surface);
            panel_surface->SetContainer(this);
            panel_surface->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
            panel_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            auto& window_node = SGGUIRoot::GetSingleton().GetConfig()["window"];
            SetStyle(window_node);
            UIText* caption = SGGUIRoot::GetSingleton().NewObject<UIText>();
            ui_components.push_back(caption);
            caption->SetContainer(this);
            auto ft = SGGUIRoot::GetSingleton().GetGuiFont(window_node["font"].to_string());
            caption->SetFont(ft);
            caption->SetCapacity(16);
            SGJsonUtil::SetUIPositionSize(window_node["text_offset"], caption, ft->GetTextOffset());
            
            SGSimpleButton* button = NewChild<SGSimpleButton>();
            button->SetSimpleStyle(window_node["close_button_style"]);
            SGJsonUtil::SetUIPositionSize(window_node["close_button_offset"], button, {0, 0});
            button->event_click += [this](SGWidget& sender)->bool {
                RemoveFromParent();
                return true;
            };
            min_size = SGJsonUtil::ConvertV2i(window_node["minimum_size"], 0);
            border = SGJsonUtil::ConvertRecti(window_node["border"]);
            SetDragable(true);
        }
        
        UIText* GetCaption() { return static_cast<UIText*>(this->ui_components[1]); }
        inline void SetCloseButtonVisible(bool v) { return this->children[0]->SetVisible(v); }
        inline void SetMinSize(v2i msz) { min_size = msz; }
        inline void SetResizable(bool r) { allow_resize = r; }
        
    protected:
        int8_t drag_status = 0;
        bool allow_resize = true;
        v2i min_size = {0, 0};
        recti border = {0, 0, 0, 0};
    };
    
    class SGScrollArea : public SGWidgetContainer {
    public:
        virtual bool CheckInside(int32_t x, int32_t y) {
            return x >= view_pos.x && x <= view_pos.x + view_size.x && y >= view_pos.y && y <= view_pos.y + view_size.y;
        }
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto pre_act_pos = area_pos.absolute;
            auto pre_act_size = area_size.absolute;
            area_pos.absolute = view_pos;
            area_size.absolute = view_size;
            auto ret = SGWidget::OnPositionSizeChange(re_pos, re_size);
            bool view_pos_change = view_pos != area_pos.absolute;
            bool view_size_change = view_size != area_size.absolute;
            if(ret.first || view_size_change) {
                children[0]->OnPositionSizeChange(ret.first || view_size_change, view_size_change);
                children[1]->OnPositionSizeChange(ret.first || view_size_change, view_size_change);
            }
            view_pos = area_pos.absolute;
            view_size = area_size.absolute;
            area_size.absolute = scroll_size;
            area_pos.absolute -= view_offset;
            bool act_pos_change = pre_act_pos != area_pos.absolute;
            bool act_size_change = pre_act_size != area_size.absolute;
            if(act_pos_change || act_size_change) {
                for(size_t i = 2; i < children.size(); ++i)
                    children[i]->OnPositionSizeChange(act_pos_change || act_size_change, act_size_change);
            }
            if(view_size_change)
                CheckViewSize();
            if(!cmd.expired() && (view_pos_change || view_size_change)) {
                recti clip_region = {view_pos.x, view_pos.y, view_size.x, view_size.y};
                cmd.lock()->scissor_rect = SGGUIRoot::GetSingleton().ConvertScissorRect(clip_region);
            }
            return ret;
        }
        
        virtual void AddChild(std::shared_ptr<SGWidget> child) {
            SGWidgetContainer::AddChild(child);
            need_refresh_widgets = true;
        }
        
        virtual void RemoveChild(SGWidget* child) {
            SGWidgetContainer::RemoveChild(child);
            RefreshVisibleWidgets();
        }
        
        virtual void InitUIComponents() {
            auto scrh = NewChild<SGScrollBar<>>(true);
            auto scrv = NewChild<SGScrollBar<>>(false);
            scrh->SetPositionSize({0, 0}, {0, 0}, {0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f});
            scrv->SetPositionSize({0, 0}, {0, 0}, {1.0f, 0.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f});
            scrh->SetSizeR({-scrv->GetAbsoluteSize().x, 0});
            scrh->event_value_change += [this](SGWidget& sender, float val)->bool {
                int32_t x = val * (scroll_size.x - view_size.x);
                ChangeViewOffset({x, view_offset.y});
                return true;
            };
            scrv->event_value_change += [this](SGWidget& sender, float val)->bool {
                int32_t y = val * (scroll_size.y - view_size.y);
                ChangeViewOffset({view_offset.x, y});
                return true;
            };
        }
        
        virtual void PushUIComponents() {
            if(!is_visible)
                return;
            auto shader = SGGUIRoot::GetSingleton().GetShader();
            alpha_cmd = SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdBeginGlobalAlpha>(shader, container_alpha);
            recti clip_region = {view_pos.x, view_pos.y, view_size.x, view_size.y};
            auto scissor_rect = SGGUIRoot::GetSingleton().ConvertScissorRect(clip_region);
            cmd = SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdBeginScissor>(scissor_rect);
            if(need_refresh_widgets)
                RefreshVisibleWidgets();
            for(auto iter : visible_widgets)
                iter->PushUIComponents();
            SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdEndScissor>();
            children[0]->PushUIComponents();
            children[1]->PushUIComponents();
            SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdEndGlobalAlpha>(shader);
        }
        
        virtual std::shared_ptr<SGWidget> GetHovingWidget(int32_t x, int32_t y) {
            for(size_t i = 0; i < 2; ++i)
                if(children[i]->IsVisible() && children[i]->CheckInside(x, y))
                    return children[i];
            for(size_t i = children.size() - 1; i > 1; --i)
                if(children[i]->IsVisible() && children[i]->AllowFocus() && children[i]->CheckInside(x, y))
                    return children[i];
            return nullptr;
        }
        
        bool RefreshVisibleWidgets() {
            bool widget_changed = false;
            int32_t index = 0;
            for(size_t i = 2; i < children.size(); ++i) {
                auto ptr = children[i].get();
                if(ptr->CheckInRect({view_pos.x, view_pos.y, view_size.x, view_size.y})) {
                    if(widget_changed)
                        visible_widgets.push_back(ptr);
                    else {
                        if(index < visible_widgets.size() && ptr == visible_widgets[index])
                            index++;
                        else {
                            visible_widgets.resize(index);
                            visible_widgets.push_back(ptr);
                            widget_changed = true;
                        }
                    }
                }
            }
            need_refresh_widgets = false;
            return widget_changed;
        }
        
        void SetScrollSize(v2i ssize) {
            if(ssize == scroll_size)
                return;
            scroll_size = ssize;
            OnPositionSizeChange(false, true);
            CheckViewSize();
        }
        
        void ChangeViewOffset(v2i off) {
            if(off.x < 0) off.x = 0;
            if(off.y < 0) off.y = 0;
            if(scroll_size.x >= view_size.x && off.x > scroll_size.x - view_size.x)
                off.x = scroll_size.x - view_size.x;
            if(scroll_size.y >= view_size.y && off.y > scroll_size.y - view_size.y)
                off.y = scroll_size.y - view_size.y;
            if(off == view_offset)
                return;
            view_offset = off;
            OnPositionSizeChange(true, false);
            if(RefreshVisibleWidgets())
                SetRedraw();
        }
        
        void CheckViewSize() {
            if(view_size.x >= scroll_size.x) {
                if(children[0]->IsVisible()) {
                    children[0]->SetVisible(false);
                    view_offset.x = 0;
                    static_cast<SGScrollBar<>*>(children[0].get())->SetValue(0.0f, false);
                }
            } else {
                if(!children[0]->IsVisible())
                    children[0]->SetVisible(true);
                if(view_offset.x > scroll_size.x - view_size.x)
                    view_offset.x = scroll_size.x - view_size.x;
                static_cast<SGScrollBar<>*>(children[0].get())->SetValue((float)view_offset.x / (scroll_size.x - view_size.x), false);
            }
            if(view_size.y >= scroll_size.y) {
                if(children[1]->IsVisible()) {
                    children[1]->SetVisible(false);
                    static_cast<SGScrollBar<>*>(children[1].get())->SetValue(0.0, false);
                }
            } else {
                if(!children[1]->IsVisible())
                    children[1]->SetVisible(true);
                if(view_offset.y > scroll_size.y - view_size.y)
                    view_offset.y = scroll_size.y - view_size.y;
                static_cast<SGScrollBar<>*>(children[1].get())->SetValue((float)view_offset.y / (scroll_size.y - view_size.y), false);
            }
            if(RefreshVisibleWidgets())
                SetRedraw();
        }
        
        inline v2i GetViewSize() { return view_size; }
        inline v2i GetViewOffset() { return view_offset; }
        inline SGScrollBar<>* GetScrollBar(bool hori) { return static_cast<SGScrollBar<>*>(children[hori ? 0 : 1].get()); }
        
    protected:
        v2i scroll_size = {0, 0};
        v2i view_pos = {0, 0};
        v2i view_size = {0, 0};
        v2i view_offset = {0, 0};
        std::weak_ptr<base::RenderCmdBeginScissor<vt2>> cmd;
        std::vector<SGWidget*> visible_widgets;
        bool need_refresh_widgets = false;
    };
    
    class SGItemListWidget {
    public:
        SGEventHandler<SGWidget, int32_t> event_sel_change;
        
        virtual void AddItem(const std::wstring& str, uint32_t cl, int32_t cvalue) = 0;
        virtual void RemoveItem(int32_t index) = 0;
        virtual int32_t GetItemCount() = 0;
        virtual const std::wstring& GetItemText(int32_t index) = 0;
        virtual int32_t GetItemCustomValue(int32_t) = 0;
        virtual void ClearItems() = 0;
        virtual void SetSelection(int32_t index, bool trigger) = 0;
        inline int32_t GetSelection() { return selection; }
        
    protected:
        int32_t selection = -1;
    };
    
    class SGListBox : public SGWidgetContainer, public SGItemListWidget {
    public:
        SGEventHandler<SGWidget, int32_t> event_item_double_click;
        
    public:
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto ret = SGWidgetContainer::OnPositionSizeChange(re_pos, re_size);
            if(!cmd.expired() && (ret.first || ret.second)) {
                recti clip_region = {
                    area_pos.absolute.x + bounds.left,
                    area_pos.absolute.y + bounds.top,
                    area_size.absolute.x - bounds.left - bounds.width,
                    area_size.absolute.y - bounds.top - bounds.height
                };
                cmd.lock()->scissor_rect = SGGUIRoot::GetSingleton().ConvertScissorRect(clip_region);
            }
            if(ret.second) {
                int32_t new_offset = item_offset;
                auto sc = static_cast<SGScrollBar<>*>(children[0].get());
                int32_t item_height_all = (int32_t)(ui_components.size() - 2) * item_height;
                if(item_height_all > area_size.absolute.y - bounds.top - bounds.height) {
                    if(!sc->IsVisible()) {
                        sc->SetVisible(true);
                        sc->SetValue(0.0f);
                    }
                    new_offset = (item_height_all - (area_size.absolute.y - bounds.top - bounds.height)) * sc->GetCurrentValue();
                } else {
                    if(sc->IsVisible()) {
                        sc->SetVisible(false);
                        sc->SetValue(0.0f, false);
                    }
                    new_offset = 0;
                }
                UISpriteList* back_surface = static_cast<UISpriteList*>(ui_components[1]);
                auto item_count = (area_size.absolute.y - bounds.top - bounds.height) / item_height + 2;
                if(item_count != back_surface->GetCapacity()) {
                    if(item_count > back_surface->GetCapacity()) {
                        back_surface->SetCapacity(item_count, true);
                        UpdateBackTex();
                        SetRedraw();
                    }
                }
                ChangeOffset(new_offset);
            }
            return ret;
        }
        
        virtual void InitUIComponents() {
            SGWidgetContainer::InitUIComponents();
            UISprite9* panel_surface = SGGUIRoot::GetSingleton().NewObject<UISprite9>();
            ui_components.push_back(panel_surface);
            panel_surface->SetContainer(this);
            panel_surface->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
            panel_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            UISpriteList* back_surface = SGGUIRoot::GetSingleton().NewObject<UISpriteList>();
            ui_components.push_back(back_surface);
            back_surface->SetContainer(this);
            back_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            auto scr = NewChild<SGScrollBar<>>(false);
            auto& panel_node = SGGUIRoot::GetSingleton().GetConfig()["listbox"];
            SetStyle(panel_node);
            scr->event_value_change += [this](SGWidget& sender, float val)->bool {
                auto item_height_all = (int32_t)(ui_components.size() - 2) * item_height;
                if(item_height_all <= area_size.absolute.y - bounds.top - bounds.height)
                    return true;
                ChangeOffset((item_height_all - (area_size.absolute.y - bounds.top - bounds.height)) * val);
                return true;
            };
            int64_t last_click_time = 0;
            int32_t index = -1;
            this->event_click += [this, index, last_click_time](SGWidget& sender) mutable ->bool {
                int64_t tm = SGGUIRoot::GetSingleton().GetSysClock();
                if(selection == index && tm - last_click_time < 300) {
                    event_item_double_click.Trigger(sender, selection);
                    index = -1;
                } else
                    index = selection;
                last_click_time = tm;
                return true;
            };
            back_surface->SetPositionSize({bounds.left, bounds.top}, {-bounds.left - bounds.width, 0}, {0.0, 0.0}, {1.0, 0.0});
        }
        
        virtual void PushUIComponents() {
            if(!is_visible)
                return;
            auto shader = SGGUIRoot::GetSingleton().GetShader();
            alpha_cmd = SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdBeginGlobalAlpha>(shader, container_alpha);
            ui_components[0]->PushVertices();
            recti clip_region = {
                area_pos.absolute.x + bounds.left,
                area_pos.absolute.y + bounds.top,
                area_size.absolute.x - bounds.left - bounds.width,
                area_size.absolute.y - bounds.top - bounds.height
            };
            auto scissor_rect = SGGUIRoot::GetSingleton().ConvertScissorRect(clip_region);
            cmd = SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdBeginScissor>(scissor_rect);
            ui_components[1]->PushVertices();
            auto item_count = (area_size.absolute.y - bounds.top - bounds.height) / item_height + 2;
            for(auto i = 0; i < item_count; ++i) {
                if(item_begin + i + 2 >= ui_components.size())
                    break;
                ui_components[item_begin + i + 2]->PushVertices();
            }
            SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdEndScissor>();
            children[0]->PushUIComponents();
            SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdEndGlobalAlpha>(shader);
        }
        
        virtual bool OnMouseEnter() {
            static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(client_tex[1]);
            return SGWidgetContainer::OnMouseEnter();
        }
        
        virtual bool OnMouseLeave() {
            static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(client_tex[0]);
            return SGWidgetContainer::OnMouseLeave();
        }
        
        virtual bool OnMouseDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
            if((button == GLFW_MOUSE_BUTTON_LEFT) && hoving_widget.expired()) {
                auto sel = CheckItemIndex(y);
                if(sel >= 0)
                    SetSelection(sel);
            }
            SGWidgetContainer::OnMouseDown(button, mods, x, y);
            return true;
        }
        
        virtual bool OnMouseWheel(float deltax, float deltay) {
            return static_cast<SGScrollBar<>*>(children[0].get())->OnMouseWheel(deltax, deltay);
        }
        
        void SetStyle(jaweson::JsonNode<>& lb_node) {
            auto panel_surface = static_cast<UISprite9*>(this->ui_components[0]);
            auto cl = SGJsonUtil::ConvertRGBA(lb_node["color"]);
            auto border_rect = SGJsonUtil::ConvertRecti(lb_node["border"]);
            auto border_tex = SGJsonUtil::ConvertRecti(lb_node["border_tex"]);
            auto back_border = SGJsonUtil::ConvertRecti(lb_node["back_border"]);
            auto back_rect = SGJsonUtil::ConvertRecti(lb_node["back_tex"]);
            client_tex[0] = SGJsonUtil::ConvertRecti(lb_node["client_normal"]);
            client_tex[1] = SGJsonUtil::ConvertRecti(lb_node["client_hover"]);
            panel_surface->SetColor(cl);
            panel_surface->SetTextureRect(client_tex[0]);
            panel_surface->SetBackRect(back_border, back_rect);
            panel_surface->SetBorderRect(border_rect, border_tex);
            SGScrollBar<>* scroll = static_cast<SGScrollBar<>*>(children[0].get());
            SGJsonUtil::SetUIPositionSize(lb_node["scroll_area"], scroll, {0, 0});
            item_font = SGGUIRoot::GetSingleton().GetGuiFont(lb_node["font"].to_string());
            auto& offset_node = lb_node["text_offset"];
            item_pos_rel = SGJsonUtil::ConvertV2i(offset_node, 0) + item_font->GetTextOffset();
            item_size_rel = SGJsonUtil::ConvertV2i(offset_node, 2);
            item_pos_pro = SGJsonUtil::ConvertV2f(offset_node, 4);
            item_size_pro = SGJsonUtil::ConvertV2f(offset_node, 6);
            item_self_factor = SGJsonUtil::ConvertV2f(offset_node, 8);
            item_height = (int32_t)lb_node["item_height"].to_integer();
            bounds = SGJsonUtil::ConvertRecti(lb_node["text_area"]);
            sel_tex = SGJsonUtil::ConvertRecti(lb_node["sel_tex"]);
            color[0] = SGJsonUtil::ConvertRGBA(lb_node["item_bcolor1"]);
            color[1] = SGJsonUtil::ConvertRGBA(lb_node["item_bcolor2"]);
            color[2] = SGJsonUtil::ConvertRGBA(lb_node["sel_bcolor"]);
        }
        
        virtual void AddItem(const std::wstring& str, uint32_t cl, int32_t cvalue  = 0) {
            UIText* item_surface = SGGUIRoot::GetSingleton().NewObject<UIText>();
            item_surface->SetContainer(this);
            item_surface->SetCapacity(16);
            item_surface->SetFont(item_font);
            item_surface->PushStringWithFormat(str, cl);
            item_surface->SetPositionSize(item_pos_rel, item_size_rel, item_pos_pro, item_size_pro, item_self_factor);
            item_surface->SetPositionR(item_pos_rel + v2i{bounds.left, bounds.top - item_offset + (int32_t)(ui_components.size() - 2) * item_height});
            ui_components.push_back(item_surface);
            SetRedraw();
            if((int32_t)(ui_components.size() - 2) * item_height > area_size.absolute.y - bounds.top - bounds.height) {
                auto sc = static_cast<SGScrollBar<>*>(children[0].get());
                sc->SetValue(0.0f);
                sc->SetVisible(true);
            }
            custom_value.push_back(cvalue);
        }
        
        virtual void RemoveItem(int32_t index) {
            if(index >= (int32_t)ui_components.size() - 2)
                return;
            auto rm = ui_components[index + 2];
            SGGUIRoot::GetSingleton().DeleteObject(rm);
            ui_components.erase(ui_components.begin() + (index + 2));
            for(size_t i = index + 2; i < ui_components.size(); ++i)
                ui_components[i]->SetPositionR(item_pos_rel + v2i{bounds.left, bounds.top - item_offset + (int32_t)(i - 2) * item_height});
            SetRedraw();
            if((int32_t)(ui_components.size() - 2) * item_height <= area_size.absolute.y - bounds.top - bounds.height) {
                auto sc = static_cast<SGScrollBar<>*>(children[0].get());
                sc->SetVisible(false);
                sc->SetValue(0.0f);
            }
            custom_value.erase(custom_value.begin() + index);
        }
        
        virtual void ClearItems() {
            ui_components.erase(ui_components.begin() + 2, ui_components.end());
            custom_value.clear();
            SetRedraw();
            auto sc = static_cast<SGScrollBar<>*>(children[0].get());
            sc->SetVisible(false);
            sc->SetValue(0.0f);
            SetSelection(-1);
        }
        
        virtual int32_t GetItemCount() {
            return (int32_t)ui_components.size() - 2;
        }
        
        virtual const std::wstring& GetItemText(int32_t index) {
            static std::wstring empty_val = L"";
            if(index >= (int32_t)ui_components.size() - 2)
                return empty_val;
            return static_cast<UIText*>(ui_components[index + 2])->GetText();
        }
        
        virtual int32_t GetItemCustomValue(int32_t index) {
            if(index < 0 || index >= (int32_t)ui_components.size() - 2)
                return 0;
            return custom_value[index];
        }
        
        void ChangeOffset(int32_t offset) {
            if(item_offset == offset)
                return;
            item_offset = offset;
            auto item_count = (area_size.absolute.y - bounds.top - bounds.height) / item_height + 2;
            auto begin_index = offset / item_height;
            if(begin_index != item_begin) {
                item_begin = begin_index;
                UpdateBackTex();
                SetRedraw();
            } else {
                auto back_offset = bounds.top + item_begin * item_height - item_offset;
                ui_components[1]->SetPositionSizeR({bounds.left, back_offset}, {-bounds.left - bounds.width, item_count * item_height});
            }
            RefreshItemPosition();
        }
        
        int32_t CheckItemIndex(int32_t y) {
            if(!hoving_widget.expired())
                return -2;
            int32_t index = (y - area_pos.absolute.y - bounds.top + item_offset) / item_height;
            if(index >= ui_components.size() - 2)
                index = -1;
            return index;
        }
        
        void RefreshItemPosition() {
            for(size_t i = 2; i < ui_components.size(); ++i) {
                ui_components[i]->SetPositionR(item_pos_rel + v2i{bounds.left, bounds.top - item_offset + (int32_t)(i - 2) * item_height});
            }
        }
        
        void UpdateBackTex() {
            UIVertexArray<4> v;
            auto back_surface = static_cast<UISpriteList*>(ui_components[1]);
            auto sel_texf = SGGUIRoot::GetSingleton().GetGuiTexture()->ConvTextureInfo(sel_tex);
            for(int32_t i = 0; i < back_surface->GetCapacity(); ++i) {
                auto item_color = ((i + item_begin) == selection) ? color[2] : ((i + item_begin) % 2) ? color[0] : color[1];
                v.BuildSprite({0, item_height * i, 0, item_height}, {0.0f, 0.0f, 1.0f, 0.0f}, sel_texf, item_color);
                back_surface->SetSprite(v.Ptr(), i);
            }
            auto back_offset = bounds.top + item_begin * item_height - item_offset;
            auto item_count = (area_size.absolute.y - bounds.top - bounds.height) / item_height + 1;
            back_surface->SetPositionSizeR({bounds.left, back_offset}, {-bounds.left - bounds.width, item_count * item_height});
        }
        
        virtual void SetSelection(int32_t sel, bool trigger = true) {
            if(selection == sel)
                return;
            auto back_surface = static_cast<UISpriteList*>(ui_components[1]);
            if(selection >= 0)
                back_surface->SetSpriteColor(color[(selection % 2) ? 0 : 1], selection - item_begin);
            if(sel >= 0)
                back_surface->SetSpriteColor(color[2], sel - item_begin);
            selection = (sel >= 0) ? sel : -1;
            if(sel >= 0 && trigger)
                event_sel_change.Trigger(*this, selection);
        }
        
        void SetBeginItem(int32_t index) {
            auto item_height_all = (int32_t)(ui_components.size() - 2) * item_height;
            if(item_height_all <= area_size.absolute.y - bounds.top - bounds.height)
                return;
            auto max_offset = item_height_all - (area_size.absolute.y - bounds.top - bounds.height);
            auto offset = index * item_height;
            if(offset > max_offset)
                offset = max_offset;
            static_cast<SGScrollBar<>*>(children[0].get())->SetValue((float)offset / max_offset);
        }
        
        void SetItemBackColor(uint32_t bcolor1, uint32_t bcolor2, uint32_t bcolor_sel) {
            color[0] = bcolor1;
            color[1] = bcolor2;
            color[2] = bcolor_sel;
            UpdateBackTex();
        }
        
        UIText* GetItemUI(int32_t idx) {
            if(idx < 0 || idx + 2 >= ui_components.size())
                return nullptr;
            return static_cast<UIText*>(ui_components[idx + 2]);
        }
        
    protected:
        recti bounds = {0, 0, 0, 0};
        recti sel_tex = {0, 0, 0, 0};
        recti client_tex[2];
        base::Font* item_font = nullptr;
        int32_t item_height = 0;
        int32_t item_offset = 0;
        int32_t item_begin = 0;
        int32_t color[3];
        v2i item_pos_rel = {0, 0};
        v2f item_pos_pro = {0, 0};
        v2i item_size_rel = {0, 0};
        v2f item_size_pro = {0, 0};
        v2f item_self_factor = {0, 0};
        std::weak_ptr<base::RenderCmdBeginScissor<vt2>> cmd;
        std::vector<int32_t> custom_value;
    };
    
    class SGComboBox : public SGCommonUIText, public SGItemListWidget {
    public:
        virtual void InitUIComponents() {
            SGCommonUIText::InitUIComponents();
            UISprite9* back_surface = SGGUIRoot::GetSingleton().NewObject<UISprite9>();
            ui_components.push_back(back_surface);
            back_surface->SetContainer(this);
            back_surface->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
            back_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            UISprite* button_surface = SGGUIRoot::GetSingleton().NewObject<UISprite>();
            ui_components.push_back(button_surface);
            button_surface->SetContainer(this);
            button_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            auto& combo_node = SGGUIRoot::GetSingleton().GetConfig()["combobox"];
            SetStyle(combo_node);
            InitUIComponentsExtra(combo_node);
        }
        
        void SetStyle(jaweson::JsonNode<>& cb_node) {
            auto back_surface = static_cast<UISprite9*>(this->ui_components[0]);
            auto button_surface = static_cast<UISprite*>(this->ui_components[1]);
            auto border_rect = SGJsonUtil::ConvertRecti(cb_node["border"]);
            auto border_tex = SGJsonUtil::ConvertRecti(cb_node["border_tex"]);
            auto back_border = SGJsonUtil::ConvertRecti(cb_node["back_border"]);
            auto back_rect = SGJsonUtil::ConvertRecti(cb_node["back_tex"]);
            client_tex[0] = SGJsonUtil::ConvertRecti(cb_node["client_normal"]);
            client_tex[1] = SGJsonUtil::ConvertRecti(cb_node["client_hover"]);
            back_surface->SetColor(SGJsonUtil::ConvertRGBA(cb_node["color"]));
            back_surface->SetTextureRect(client_tex[0]);
            back_surface->SetBackRect(back_border, back_rect);
            back_surface->SetBorderRect(border_rect, border_tex);
            button_style[0] = SGJsonUtil::ConvertRecti(cb_node["button_normal"]);
            button_style[1] = SGJsonUtil::ConvertRecti(cb_node["button_hover"]);
            button_style[2] = SGJsonUtil::ConvertRecti(cb_node["button_down"]);
            button_surface->SetColor(SGJsonUtil::ConvertRGBA(cb_node["button_color"]));
            button_surface->SetTextureRect(button_style[status]);
            SGJsonUtil::SetUIPositionSize(cb_node["button_offset"], button_surface, {0, 0});
            drop_offset = (int32_t)cb_node["drop_offset"].to_integer();
            drop_height = (int32_t)cb_node["drop_height"].to_integer();
        }
        
        virtual bool OnMouseEnter() {
            static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(client_tex[STATUS_HOVING]);
            if(status != STATUS_DOWN)
                static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(button_style[STATUS_HOVING]);
            hover = true;
            return SGCommonUIText::OnMouseEnter();
        }
        
        virtual bool OnMouseLeave() {
            static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(client_tex[STATUS_NORMAL]);
            if(status != STATUS_DOWN)
                static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(button_style[STATUS_NORMAL]);
            hover = false;
            return SGCommonUIText::OnMouseLeave();
        }
        
        virtual bool OnMouseDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                if(status == STATUS_DOWN) {
                    
                } else if(SGGUIRoot::GetSingleton().GetSysClock() != reset_time) {
                    static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(button_style[STATUS_DOWN]);
                    status = STATUS_DOWN;
                    auto ptr = SGGUIRoot::GetSingleton().NewChild<SGListBox>();
                    SGGUIRoot::GetSingleton().PopupObject(ptr->shared_from_this());
                    for(size_t i = 0; i < items.size(); ++i)
                        ptr->AddItem(items[i], color[i]);
                    ptr->SetPositionSize(area_pos.absolute + v2i{0, area_size.absolute.y + drop_offset}, {area_size.absolute.x, drop_height});
                    ptr->SetSelection(selection);
                    ptr->SetBeginItem(selection);
                    std::weak_ptr<SGComboBox> combo_ptr = std::static_pointer_cast<SGComboBox>(shared_from_this());
                    ptr->event_on_destroy += [combo_ptr](SGWidget& sender)->bool {
                        if(!combo_ptr.expired())
                            combo_ptr.lock()->ResetStatus();
                        return true;
                    };
                    ptr->event_mouse_move += [](SGWidget& sender, int32_t x, int32_t y)->bool {
                        auto& lb = static_cast<SGListBox&>(sender);
                        auto index = lb.CheckItemIndex(y);
                        if(index >= 0)
                            lb.SetSelection(index);
                        return true;
                    };
                    ptr->event_mouse_down += [combo_ptr](SGWidget& sender, int32_t button, int32_t mods, int32_t x, int32_t y)->bool {
                        if(button == GLFW_MOUSE_BUTTON_LEFT) {
                            auto& lb = static_cast<SGListBox&>(sender);
                            auto index = lb.CheckItemIndex(y);
                            if(index >= 0 && !combo_ptr.expired()) {
                                combo_ptr.lock()->SetSelection(index);
                                sender.RemoveFromParent();
                            }
                        }
                        return true;
                    };
                }
            }
            SGCommonUIText::OnMouseDown(button, mods, x, y);
            return true;
        }
        
        void ResetStatus() {
            if(status == STATUS_DOWN) {
                status = hover ? STATUS_HOVING : STATUS_NORMAL;
                static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(button_style[status]);
            }
            reset_time = SGGUIRoot::GetSingleton().GetSysClock();
        }
        
        virtual void AddItem(const std::wstring& it, uint32_t cl, int32_t cvalue = 0) {
            items.push_back(it);
            color.push_back(cl);
            custom_value.push_back(cvalue);
        }
        
        virtual void RemoveItem(int32_t index) {
            if(index < 0 || index >= items.size())
                return;
            items.erase(items.begin() + index);
            color.erase(color.begin() + index);
            if(selection == index)
                SetSelection(-1);
        }
        
        virtual int32_t GetItemCount() {
            return (int32_t)items.size();
        }
        
        virtual const std::wstring& GetItemText(int32_t index) {
            static std::wstring empty_val = L"";
            if(index >= items.size())
                return empty_val;
            return items[index];
        }
        
        virtual int32_t GetItemCustomValue(int32_t index) {
            if(index >= custom_value.size())
                return 0;
            return custom_value[index];
        }
        
        virtual void ClearItems() {
            items.clear();
            color.clear();
            custom_value.clear();
            SetSelection(-1);
        }
        
        virtual void SetSelection(int32_t index, bool trigger = true) {
            if((selection == index) || (index >= items.size()))
                return;
            if(index < 0) {
                if(selection >= 0) {
                    selection = -1;
                    common_ui->Clear();
                }
            } else {
                if(selection != index) {
                    selection = index;
                    common_ui->SetText(items[index], color[index]);
                }
            }
            if(trigger)
                event_sel_change.Trigger(*this, index);
        }
        
    protected:
        recti client_tex[2];
        recti button_style[3];
        bool hover = false;
        int8_t status = 0;
        int32_t drop_offset = 0;
        int32_t drop_height = 100;
        int64_t reset_time = 0;
        std::vector<std::wstring> items;
        std::vector<uint32_t> color;
        std::vector<int32_t> custom_value;
    };
    
    class SGTabControl : public SGWidget {
    public:
        SGEventHandler<SGWidget, int32_t> event_tab_change;
        
    protected:
        class SGTab : public SGWidgetContainer {
        public:
            SGTab(SGTabControl* ctrl, UIText* t) : control(ctrl), title(t) {}
            virtual void RemoveFromParent() {}
            virtual void SetFocusWidget(SGWidget* child) {
                control->SetFocus();
                SGWidgetContainer::SetFocusWidget(child);
            }
            inline UIText* GetTitle() { return title; }
            
        protected:
            SGTabControl* control = nullptr;
            UIText* title = nullptr;
        };
        
    public:
        virtual ~SGTabControl() {
            for(auto bk : title_back)
                SGGUIRoot::GetSingleton().DeleteObject(bk);
            for(auto tt : title_text)
                SGGUIRoot::GetSingleton().DeleteObject(tt);
        }
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto ret = SGWidget::OnPositionSizeChange(re_pos, re_size);
            if(ret.first || ret.second) {
                for(auto& sp : title_back)
                    sp->OnPositionSizeChange(ret.first || ret.second, ret.second);
                for(auto& txt : title_text)
                    txt->OnPositionSizeChange(ret.first || ret.second, ret.second);
                for(auto& tab : tabs)
                    tab->OnPositionSizeChange(ret.first || ret.second, ret.second);
            }
            if(ret.second)
                RefreshTitle();
            return ret;
        }
        
        virtual void InitUIComponents() {
            UISprite9* back_surface = SGGUIRoot::GetSingleton().NewObject<UISprite9>();
            ui_components.push_back(back_surface);
            back_surface->SetContainer(this);
            back_surface->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
            back_surface->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            auto& tab_node = SGGUIRoot::GetSingleton().GetConfig()["tab"];
            SetStyle(tab_node);
        }
        
        void SetStyle(jaweson::JsonNode<>& tab_node) {
            auto back_surface = static_cast<UISprite9*>(this->ui_components[0]);
            sprite_style[2] = SGJsonUtil::ConvertRecti(tab_node["border"]);
            sprite_style[3] = SGJsonUtil::ConvertRecti(tab_node["border_tex"]);
            sprite_style[0] = SGJsonUtil::ConvertRecti(tab_node["back_border"]);
            sprite_style[1] = SGJsonUtil::ConvertRecti(tab_node["back_tex"]);
            tab_color = SGJsonUtil::ConvertRGBA(tab_node["color"]);
            back_surface->SetColor(tab_color);
            back_surface->SetTextureRect(SGJsonUtil::ConvertRecti(tab_node["client_area"]));
            back_surface->SetBackRect(sprite_style[0], sprite_style[1]);
            back_surface->SetBorderRect(sprite_style[2], sprite_style[3]);
            tab_style[0] = SGJsonUtil::ConvertRecti(tab_node["tab_normal"]);
            tab_style[1] = SGJsonUtil::ConvertRecti(tab_node["tab_hover"]);
            tab_style[2] = SGJsonUtil::ConvertRecti(tab_node["tab_down"]);
            tab_offset = (int32_t)tab_node["tab_offset"].to_integer();
            title_indent = (int32_t)tab_node["title_indent"].to_integer();
            title_height = (int32_t)tab_node["title_height"].to_integer();
            title_rblank = (int32_t)tab_node["title_rblank"].to_integer();
            back_surface->SetPositionSize({0, tab_offset}, {0, -tab_offset}, {0.0f, 0.0f}, {1.0f, 1.0f});
            text_font = SGGUIRoot::GetSingleton().GetGuiFont(tab_node["font"].to_string());
            text_offset = SGJsonUtil::ConvertV2i(tab_node["text_offset"], 0) + text_font->GetTextOffset();
        }
        
        virtual void PushUIComponents() {
            if(is_visible) {
                for(auto comp : ui_components)
                    comp->PushVertices();
                for(size_t i = 0; i < title_back.size(); ++i)
                    if(i != current_tab)
                        title_back[i]->PushVertices();
                if(current_tab >= 0)
                    title_back[current_tab]->PushVertices();
                for(auto& txt : title_text)
                    txt->PushVertices();
                if(current_tab >= 0)
                    tabs[current_tab]->PushUIComponents();
            }
        }
        
        virtual SGWidget* FindWidget(const std::string& nm) {
            if(nm == name)
                return this;
            for(auto& iter : tabs) {
                auto ret = iter->FindWidget(nm);
                if(ret)
                    return ret;
            }
            return nullptr;
        }
        
        SGTab* AddTab(const std::wstring title, uint32_t cl) {
            UISprite9* sp = SGGUIRoot::GetSingleton().NewObject<UISprite9>();
            sp->SetContainer(this);
            sp->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            sp->SetColor(tab_color);
            if(tabs.size() == 0) {
                sp->SetTextureRect(tab_style[STATUS_DOWN]);
                current_tab = 0;
            } else
                sp->SetTextureRect(tab_style[STATUS_NORMAL]);
            sp->SetBackRect(sprite_style[0], sprite_style[1]);
            sp->SetBorderRect(sprite_style[2], sprite_style[3]);
            title_back.push_back(sp);
            UIText* tt = SGGUIRoot::GetSingleton().NewObject<UIText>();
            tt->SetContainer(this);
            tt->SetFont(text_font);
            tt->SetCapacity(16);
            tt->SetText(title, cl);
            title_text.push_back(tt);
            auto ptr = std::make_shared<SGTab>(this, tt);
            ptr->SetContainer(this);
            ptr->InitUIComponents();
            ptr->SetPositionSize({0, tab_offset}, {0, -tab_offset}, {0.0f, 0.0f}, {1.0f, 1.0f});
            tabs.push_back(ptr);
            SetRedraw();
            RefreshTitle();
            return ptr.get();
        }
        
        SGTab* GetTab(int32_t index) {
            if(index >= tabs.size())
                return nullptr;
            return tabs[index].get();
        }
        
        void RemoveTab(int32_t index) {
            if(index < 0 || index >= tabs.size())
                return;
            SGGUIRoot::GetSingleton().DeleteObject(title_back[index]);
            SGGUIRoot::GetSingleton().DeleteObject(title_text[index]);
            title_back.erase(title_back.begin() + index);
            title_text.erase(title_text.begin() + index);
            tabs.erase(tabs.begin() + index);
            SetRedraw();
        }
        
        inline int32_t GetTabCount() { return (int32_t)tabs.size(); }
        
        //  events
        virtual bool OnMouseMove(int32_t x, int32_t y) {
            auto ptr = shared_from_this();
            bool ret = event_mouse_move.Trigger(*this, x, y) || is_entity;
            auto hoving = y >= area_pos.absolute.y + tab_offset;
            if(hoving) {
                if(on_tab) {
                    if(current_tab >= 0)
                        ret = tabs[current_tab]->OnMouseMove(x, y) || ret;
                } else {
                    if(hoving_title != current_tab && hoving_title >= 0)
                        title_back[hoving_title]->SetTextureRect(tab_style[STATUS_NORMAL]);
                    hoving_title = -1;
                    if(current_tab >= 0) {
                        ret = tabs[current_tab]->OnMouseEnter() || ret;
                        ret = tabs[current_tab]->OnMouseMove(x, y) || ret;
                    }
                }
            } else {
                if(current_tab >= 0)
                    tabs[current_tab]->OnMouseLeave();
                int32_t tab_size = (int32_t)tabs.size();
                if(tab_size > 0) {
                    int32_t ht = (x - area_pos.absolute.x) * tab_size / (area_size.absolute.x - title_rblank);
                    if(ht >= tabs.size())
                        ht = -1;
                    if(ht != hoving_title) {
                        if(hoving_title != current_tab && hoving_title >= 0)
                            title_back[hoving_title]->SetTextureRect(tab_style[STATUS_NORMAL]);
                        if(ht != current_tab && ht >= 0)
                            title_back[ht]->SetTextureRect(tab_style[STATUS_HOVING]);
                        hoving_title = ht;
                    }
                }
            }
            on_tab = hoving;
            return ret;
        }
        
        virtual bool OnMouseEnter() {
            auto ptr = shared_from_this();
            bool ret = event_mouse_enter.Trigger(*this) || is_entity;
            on_tab = false;
            return ret;
        }
        
        virtual bool OnMouseLeave() {
            auto ptr = shared_from_this();
            bool ret = event_mouse_leave.Trigger(*this) || is_entity;
            if(current_tab >= 0)
                tabs[current_tab]->OnMouseLeave();
            on_tab = false;
            return ret;
        }
        
        virtual bool OnMouseDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
            auto ptr = shared_from_this();
            event_mouse_down.Trigger(*this, button, mods,x, y);
            if(on_tab) {
                if(current_tab >= 0)
                    tabs[current_tab]->OnMouseDown(button, mods, x, y);
            } else {
                if(hoving_title >= 0 && hoving_title != current_tab) {
                    SetCurrentTab(hoving_title);
                    SetRedraw();
                }
                if(button == GLFW_MOUSE_BUTTON_LEFT) {
                    SGClickingMgr::Get().SetClickingObject(ptr);
                    auto dr = GetDragTarget();
                    if(dr)
                        SGClickingMgr::Get().DragBegin(dr, x, y);
                }
            }
            return true;
        }
        
        virtual bool OnMouseUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
            auto ptr = shared_from_this();
            event_mouse_up.Trigger(*this, button, mods,x, y);
            if(current_tab >= 0)
                tabs[current_tab]->OnMouseUp(button, mods, x, y);
            return true;
        }
        
        virtual bool OnMouseWheel(float deltax, float deltay) {
            auto ptr = shared_from_this();
            bool ret = event_mouse_wheel.Trigger(*this, deltax, deltay) || is_entity;
            if(current_tab >= 0)
                ret = tabs[current_tab]->OnMouseWheel(deltax, deltay) || ret;
            return ret;
        }
        
        virtual bool OnKeyDown(int32_t key, int32_t mods) {
            auto ptr = shared_from_this();
            bool ret = event_key_down.Trigger(*this, key, mods) || is_entity;
            if(current_tab >= 0)
                ret = tabs[current_tab]->OnKeyDown(key, mods) || ret;
            return ret;
        }
        
        virtual bool OnKeyUp(int32_t key, int32_t mods) {
            auto ptr = shared_from_this();
            bool ret = event_key_up.Trigger(*this, key, mods) || is_entity;
            if(current_tab >= 0)
                ret = tabs[current_tab]->OnKeyUp(key, mods) || ret;
            return ret;
        }
        
        virtual bool OnTextEnter(uint32_t unichar) {
            auto ptr = shared_from_this();
            bool ret = event_text_enter.Trigger(*this, unichar) || is_entity;
            if(current_tab >= 0)
                ret = tabs[current_tab]->OnTextEnter(unichar) || ret;
            return ret;
        }
        
        virtual bool OnLoseFocus() {
            auto ptr = shared_from_this();
            bool ret = event_lose_focus.Trigger(*this) || is_entity;
            if(current_tab >= 0)
                ret = tabs[current_tab]->OnLoseFocus() || ret;
            on_tab = false;
            return ret;
        }
        
        void SetCurrentTab(int32_t index) {
            if(index >= tabs.size() || index == current_tab || index < 0)
                return;
            if(current_tab >= 0) {
                if(hoving_title == current_tab)
                    title_back[current_tab]->SetTextureRect(tab_style[STATUS_HOVING]);
                else
                    title_back[current_tab]->SetTextureRect(tab_style[STATUS_NORMAL]);
            }
            title_back[index]->SetTextureRect(tab_style[STATUS_DOWN]);
            current_tab = index;
            SetRedraw();
            event_tab_change.Trigger(*this, current_tab);
        }
        
        void RefreshTitle() {
            int32_t tab_size = (int32_t)tabs.size();
            if(tab_size > 0) {
                int32_t l = (area_size.absolute.x - title_rblank - title_indent) / tab_size + title_indent;
                for(int32_t i = 0; i < tab_size; ++i) {
                    int32_t left = i * (l - title_indent);
                    title_back[i]->SetPositionSize({left, 0}, {l, title_height});
                    title_text[i]->SetPosition(v2i{left + l / 2, 0} + text_offset, {0.0f, 0.0f}, {-0.5f, 0.0f});
                }
            }
        }
        
    protected:
        bool on_tab = false;
        int32_t hoving_title = -1;
        int32_t current_tab = -1;
        int32_t tab_offset = 0;
        int32_t title_height = 0;
        int32_t title_indent = 0;
        int32_t title_rblank = 0;
        uint32_t tab_color = 0xffffffff;
        recti tab_style[3];
        recti sprite_style[4];
        std::vector<std::shared_ptr<SGTab>> tabs;
        std::vector<UISprite9*> title_back;
        std::vector<UIText*> title_text;
        base::Font* text_font = nullptr;
        v2i text_offset = {0, 0};
    };
    
    class SGTextEdit : public SGWidget {
    public:
        SGEventHandler<SGWidget> event_entered;
        
        virtual std::pair<bool, bool> OnPositionSizeChange(bool re_pos, bool re_size) {
            auto ret = SGWidget::OnPositionSizeChange(re_pos, re_size);
            if(!cmd.expired() && (ret.first || ret.second)) {
                recti clip_region = {
                    area_pos.absolute.x + text_area.left,
                    area_pos.absolute.y + text_area.top,
                    area_size.absolute.x - text_area.left - text_area.width,
                    area_size.absolute.y - text_area.top - text_area.height
                };
                cmd.lock()->scissor_rect = SGGUIRoot::GetSingleton().ConvertScissorRect(clip_region);
            }
            return ret;
        }
        
        virtual void InitUIComponents() {
            UISprite9* sp = SGGUIRoot::GetSingleton().NewObject<UISprite9>();
            sp->SetContainer(this);
            sp->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            ui_components.push_back(sp);
            UISprite* sel = SGGUIRoot::GetSingleton().NewObject<UISprite>();
            sel->SetContainer(this);
            sel->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            ui_components.push_back(sel);
            UIText* txt = SGGUIRoot::GetSingleton().NewObject<UIText>();
            txt->SetContainer(this);
            txt->SetCapacity(16);
            ui_components.push_back(txt);
            UISprite* cur = SGGUIRoot::GetSingleton().NewObject<UISprite>();
            cur->SetContainer(this);
            cur->SetTexture(SGGUIRoot::GetSingleton().GetGuiTexture());
            ui_components.push_back(cur);
            auto& eb_node = SGGUIRoot::GetSingleton().GetConfig()["editbox"];
            SetStyle(eb_node);
            SetDragTarget(shared_from_this());
            txt->SetSizeChangeCallback([this](v2i new_sz) {
                auto display_width = area_size.absolute.x - text_area.left - text_area.width;
                if(new_sz.x > display_width - max_cursor_offset)
                    max_display_offset = new_sz.x - display_width + max_cursor_offset;
                else
                    max_display_offset = 0;
                if(display_offset > max_display_offset)
                    ChangeDisplayOffset(max_display_offset);
            });
            cur->SetPositionR({text_area.left + text_offset.x - display_offset, 0});
        }
        
        virtual void PushUIComponents() {
            if(!is_visible)
                return;
            ui_components[0]->PushVertices();
            recti clip_region = {
                area_pos.absolute.x + text_area.left,
                area_pos.absolute.y + text_area.top,
                area_size.absolute.x - text_area.left - text_area.width,
                area_size.absolute.y - text_area.top - text_area.height
            };
            auto scissor_rect = SGGUIRoot::GetSingleton().ConvertScissorRect(clip_region);
            cmd = SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdBeginScissor>(scissor_rect);
            ui_components[1]->PushVertices();
            ui_components[2]->PushVertices();
            ui_components[3]->PushVertices();
            SGGUIRoot::GetSingleton().PushCommand<base::RenderCmdEndScissor>();
        }
        
        void SetStyle(jaweson::JsonNode<>& eb_node) {
            auto back_surface = static_cast<UISprite9*>(this->ui_components[0]);
            auto border = SGJsonUtil::ConvertRecti(eb_node["border"]);
            auto border_tex = SGJsonUtil::ConvertRecti(eb_node["border_tex"]);
            auto back_border = SGJsonUtil::ConvertRecti(eb_node["back_border"]);
            auto back_tex = SGJsonUtil::ConvertRecti(eb_node["back_tex"]);
            auto color = SGJsonUtil::ConvertRGBA(eb_node["color"]);
            back_surface->SetColor(color);
            back_surface->SetBackRect(back_border, back_tex);
            back_surface->SetBorderRect(border, border_tex);
            client_style[0] = SGJsonUtil::ConvertRecti(eb_node["client_normal"]);
            client_style[1] = SGJsonUtil::ConvertRecti(eb_node["client_hover"]);
            back_surface->SetTextureRect(client_style[0]);
            back_surface->SetPositionSize({0, 0}, {0, 0}, {0.0f, 0.0f}, {1.0f, 1.0f});
            text_area = SGJsonUtil::ConvertRecti(eb_node["text_area"]);
            auto txt = static_cast<UIText*>(this->ui_components[2]);
            auto text_font = SGGUIRoot::GetSingleton().GetGuiFont(eb_node["font"].to_string());
            text_offset = SGJsonUtil::ConvertV2i(eb_node["text_offset"], 0) + text_font->GetTextOffset();
            txt->SetFont(text_font);
            txt->SetPosition({text_offset.x + text_area.left, text_offset.y}, {0.0, 0.5}, {0.0, -0.5});
            sel_tex = SGJsonUtil::ConvertRecti(eb_node["sel_tex"]);
            static_cast<UISprite*>(this->ui_components[1])->SetTextureRect(sel_tex);
            static_cast<UISprite*>(this->ui_components[1])->SetColor(0x80000000);
            cursor_tex = SGJsonUtil::ConvertRecti(eb_node["cursor_tex"]);
            static_cast<UISprite*>(this->ui_components[3])->SetTextureRect(cursor_tex);
            static_cast<UISprite*>(this->ui_components[3])->SetSize({cursor_tex.width, 0}, {0.0, 1.0});
            static_cast<UISprite*>(this->ui_components[3])->SetColor(0x0);
            max_cursor_offset = (int32_t)eb_node["max_cursor_offset"].to_integer();
        }
        
        void ChangeCursorStatus() {
            if(selection.x != selection.y || selection.x < 0)
                return;
            cursor_show = !cursor_show;
            uint32_t color = cursor_show ? 0xff000000 : 0x0;
            ui_components[3]->SetColor(color);
        }
        
        void SetCursorPos(int32_t index) {
            if(selection.x == selection.y && selection.x == index)
                return;
            auto txt = static_cast<UIText*>(this->ui_components[2]);
            auto cursor = static_cast<UISprite*>(this->ui_components[3]);
            auto pos = txt->GetTextPosition(index);
            selection.x = selection.y = index;
            cursor->SetPositionR({pos.x + text_area.left + text_offset.x - display_offset, 0});
            if(pos.x + text_area.left - display_offset + max_cursor_offset > area_size.absolute.x - text_area.width)
                ChangeDisplayOffset(pos.x + text_area.left + max_cursor_offset - (area_size.absolute.x - text_area.width));
            if(pos.x < display_offset)
                ChangeDisplayOffset(pos.x);
            ui_components[1]->SetColor(0x0);
            ResetCursorTimer();
        }
        
        void SetSelectedText(int32_t start, int32_t end = -1) {
            auto txt = static_cast<UIText*>(this->ui_components[2]);
            if(start < 0)
                start = 0;
            if(end < 0 || end > txt->GetText().size())
                end = (int32_t)txt->GetText().size();
            if(start == end) {
                SetCursorPos(start);
                return;
            }
            if(start > end)
                std::swap(start, end);
            if(start == selection.x && end == selection.y)
                return;
            auto pstart = txt->GetTextPosition(start);
            auto pend = txt->GetTextPosition(end);
            selection = {start, end};
            ui_components[3]->SetColor(0x0);
            ui_components[1]->SetColor(0x80000000);
            ui_components[1]->SetPositionSize({pstart.x + text_area.left + text_offset.x - display_offset, 0}, {pend.x - pstart.x, 0}, {0, 0}, {0.0, 1.0});
        }
        
        inline std::wstring GetSelectedText() {
            return std::move(static_cast<UIText*>(this->ui_components[2])->GetText().substr(selection.x, selection.y - selection.x));
        }
        
        inline void Clear() {
            static_cast<UIText*>(this->ui_components[2])->Clear();
            if(timer_version)
                SetCursorPos(0);
        }
        
        inline UIText* GetTextUI() { return static_cast<UIText*>(ui_components[2]); }
        
        virtual bool OnMouseEnter() {
            static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(client_style[STATUS_HOVING]);
            return SGWidget::OnMouseEnter();
        }
        
        virtual bool OnMouseLeave() {
            static_cast<UISprite9*>(this->ui_components[0])->SetTextureRect(client_style[STATUS_NORMAL]);
            return SGWidget::OnMouseLeave();
        }
        
        virtual bool OnKeyDown(int32_t key, int32_t mods) {
            auto txt = static_cast<UIText*>(ui_components[2]);
            switch(key) {
                case GLFW_KEY_LEFT:
                    if(selection.x == selection.y) {
                        if(selection.x > 0)
                             SetCursorPos(selection.x - 1);
                    } else
                        SetCursorPos(selection.x);
                    break;
                case GLFW_KEY_RIGHT:
                    if(selection.x == selection.y) {
                        if(selection.x < txt->GetText().size())
                            SetCursorPos(selection.x + 1);
                    } else
                        SetCursorPos(selection.y);
                    break;
                case GLFW_KEY_BACKSPACE:
                    if(read_only)
                        break;
                    if(selection.x == selection.y) {
                        if(selection.x > 0) {
                            txt->RemoveText(selection.x - 1, 1);
                            SetCursorPos(selection.x - 1);
                        }
                    } else {
                        txt->RemoveText(selection.x, selection.y - selection.x);
                        SetCursorPos(selection.x);
                    }
                    break;
                case GLFW_KEY_DELETE:
                    if(read_only)
                        break;
                    if(selection.x == selection.y) {
                        if(selection.x < txt->GetText().size())
                            txt->RemoveText(selection.x, 1);
                    } else {
                        txt->RemoveText(selection.x, selection.y - selection.x);
                        SetCursorPos(selection.x);
                    }
                case GLFW_KEY_C:
                case GLFW_KEY_X: {
                    if(mods & GLFW_MOD_SUPER) {
                        if(selection.x != selection.y) {
                            std::wstring str = txt->GetText().substr(selection.x, selection.y - selection.x);
                            std::string clipstr = To<std::string>(str);
                            SGGUIRoot::GetSingleton().SetClipboardString(clipstr);
                            if(key == GLFW_KEY_X && !read_only) {
                                txt->RemoveText(selection.x, selection.y - selection.x);
                                SetCursorPos(selection.x);
                            }
                        }
                    }
                    break;
                }
                case GLFW_KEY_V: {
                    if(read_only)
                        break;
                    if(mods & GLFW_MOD_SUPER) {
                        std::string str = SGGUIRoot::GetSingleton().GetClipboardString();
                        std::wstring wstr = To<std::wstring>(str);
                        wstr.resize(std::remove_if(wstr.begin(), wstr.end(), [](wchar_t ch)->bool { return ch < L' '; }) - wstr.begin());
                        if(!wstr.empty()) {
                            if(selection.x == selection.y)
                                txt->InsertText(selection.x, wstr, default_text_color);
                            else
                                txt->ReplaceText(selection.x, selection.y - selection.x, wstr, default_text_color);
                            SetCursorPos(selection.x + (int32_t)wstr.length());
                        }
                    }
                    break;
                }
                case GLFW_KEY_ENTER: {
                    if(read_only)
                        break;
                    if(mods & GLFW_MOD_SUPER)
                        event_entered.Trigger(*this);
                    break;
                }
                default:
                    break;
            }
            return SGWidget::OnKeyDown(key, mods);
        }

        virtual bool OnTextEnter(uint32_t unichar) {
            if(read_only || unichar < L' ')
                return SGWidget::OnTextEnter(unichar);
            auto txt = static_cast<UIText*>(this->ui_components[2]);
            std::wstring str;
            str.push_back((wchar_t)unichar);
            if(selection.x == selection.y)
                txt->InsertText(selection.x, str, default_text_color);
            else
                txt->ReplaceText(selection.x, selection.y - selection.x, str, default_text_color);
            SetCursorPos(selection.x + 1);
            return SGWidget::OnTextEnter(unichar);
        }
        
        virtual bool OnGetFocus() {
            ResetCursorTimer();
            return SGWidget::OnGetFocus();
        }
        
        virtual bool OnLoseFocus() {
            timer_version = 0;
            cursor_show = false;
            ui_components[3]->SetColor(0x0);
            return SGWidget::OnLoseFocus();
        }
        
        virtual bool OnDragBegin(int32_t x, int32_t y) {
            auto txt = static_cast<UIText*>(this->ui_components[2]);
            auto xoff = x - (area_pos.absolute.x + text_area.left + text_offset.x - display_offset);
            int32_t beg = txt->CheckHitPositionSingleLine(xoff);
            SetCursorPos(beg);
            SGClickingMgr::Get().GetDiffValue().x = beg;
            auto last_check = SGGUIRoot::GetSingleton().GetSysClock();
            SGGUIRoot::GetSingleton().RegisterTimerEvent([this, last_check]() mutable ->uint64_t {
                if(SGClickingMgr::Get().GetClickingObject().get() != this)
                    return 0;
                auto now = SGGUIRoot::GetSingleton().GetSysClock();
                if(now - last_check > 50) {
                    last_check += 50;
                    CheckDragOffset();
                }
                return 1;
            }, 1);
            return SGWidget::OnDragBegin(x, y);
        }
        
        virtual bool OnDragUpdate(int32_t sx, int32_t sy, int32_t ex, int32_t ey) {
            CheckDragPosition(ex);
            return SGWidget::OnDragUpdate(sx, sy, ex, ey);
        }
        
        void CheckDragOffset() {
            auto ex = SGGUIRoot::GetSingleton().GetMousePosition().x;
            if(ex + max_cursor_offset > area_pos.absolute.x + area_size.absolute.x - text_area.width) {
                auto dif = ex + max_cursor_offset - (area_pos.absolute.x + area_size.absolute.x - text_area.width);
                ChangeDisplayOffset(display_offset + dif);
                CheckDragPosition(ex);
            } else if(ex < area_pos.absolute.x + text_area.left) {
                auto dif = area_pos.absolute.x + text_area.left - ex;
                ChangeDisplayOffset(display_offset - dif);
                CheckDragPosition(ex);
            }
        }
        
        void CheckDragPosition(int32_t ex) {
            auto txt = static_cast<UIText*>(this->ui_components[2]);
            auto xoff = ex - (area_pos.absolute.x + text_area.left + text_offset.x - display_offset);
            int32_t end = txt->CheckHitPositionSingleLine(xoff);
            int32_t beg = SGClickingMgr::Get().GetDiffValue().x;
            if(end >= beg) {
                SetSelectedText(beg, end + 1);
            } else {
                SetSelectedText(end, beg);
            }
        }
        
        void ResetCursorTimer() {
            uint32_t timever = (uint32_t)(SGGUIRoot::GetSingleton().GetSysClock());
            if(timer_version == timever)
                return;
            timer_version = timever;
            std::weak_ptr<SGTextEdit> thiseb = std::static_pointer_cast<SGTextEdit>(shared_from_this());
            SGGUIRoot::GetSingleton().RegisterTimerEvent([thiseb, timever]()->uint64_t {
                if(thiseb.expired())
                    return 0;
                auto eb = thiseb.lock();
                if(eb->timer_version != timever)
                    return 0;
                eb->ChangeCursorStatus();
                return 500;
            }, 500);
            cursor_show = true;
            ui_components[3]->SetColor(0xff000000);
        }
        
        void ChangeDisplayOffset(int32_t off) {
            if(off < 0)
                off = 0;
            if(off > max_display_offset)
                off = max_display_offset;
            if(display_offset == off)
                return;
            display_offset = off;
            auto txt = static_cast<UIText*>(this->ui_components[2]);
            if(selection.x == selection.y) {
                auto pos = txt->GetTextPosition(selection.x);
                ui_components[3]->SetPositionR({pos.x + text_area.left + text_offset.x - display_offset, 0});
            } else {
                auto pstart = txt->GetTextPosition(selection.x);
                ui_components[1]->SetPositionR({pstart.x + text_area.left + text_offset.x - display_offset, 0});
            }
            txt->SetPositionR({text_offset.x + text_area.left - display_offset, text_offset.y});
        }
        
        inline void SetReadonly(bool r) { read_only = r; }
        inline void SetDefaultTextColor(uint32_t cl) { default_text_color = cl; }
        
    protected:
        bool read_only = false;
        bool cursor_show = false;
        int32_t display_offset = 0;
        int32_t max_display_offset = 0;
        int32_t cursor_pos = 0;
        int32_t max_cursor_offset = 0;
        uint32_t default_text_color = 0xffffffff;
        uint32_t timer_version = 0;
        v2i selection = {0, 0};
        v2i sel_offset = {0, 0};
        v2i text_offset = {0, 0};
        recti client_style[2];
        recti sel_tex = {0, 0, 0, 0};
        recti cursor_tex = {0, 0, 0, 0};
        recti text_area = {0, 0, 0, 0};
        std::weak_ptr<base::RenderCmdBeginScissor<vt2>> cmd;
    };
    
}

#endif
