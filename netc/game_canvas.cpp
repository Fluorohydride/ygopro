#include "wx/image.h"
#include "wx/clipbrd.h"
#include "wx/time.h"
#include "image_mgr.h"
#include "game_canvas.h"
#include "card_data.h"
#include "deck_data.h"

namespace ygopro
{
/*
    wxGameCanvas::wxGameCanvas(wxFrame* parent, int id, int* args) : wxGLCanvas(parent, id, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
        glcontext = new wxGLContext(this);
        Bind(wxEVT_SIZE, &wxGameCanvas::EventResized, this);
        Bind(wxEVT_PAINT, &wxGameCanvas::EventRender, this);
        Bind(wxEVT_MOTION, &wxGameCanvas::EventMouseMoved, this);
        Bind(wxEVT_LEFT_DOWN, &wxGameCanvas::EventMouseLDown, this);
        Bind(wxEVT_LEFT_UP, &wxGameCanvas::EventMouseLUp, this);
        Bind(wxEVT_RIGHT_DOWN, &wxGameCanvas::EventMouseRDown, this);
        Bind(wxEVT_RIGHT_UP, &wxGameCanvas::EventMouseRUp, this);
        Bind(wxEVT_LEAVE_WINDOW, &wxGameCanvas::EventMouseLeftWindow, this);
        hover_timer.SetOwner(this);
        Bind(wxEVT_TIMER, &wxGameCanvas::OnHoverTimer, this);
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        t_buildbg = &imageMgr.textures["buildbg"];
        t_deckbg = &imageMgr.textures["deckbg"];
        t_font = &imageMgr.textures["number"];
        t_hmask = &imageMgr.textures["hmask"];
        t_limits[0] = &imageMgr.textures["limit0"];
        t_limits[1] = &imageMgr.textures["limit1"];
        t_limits[2] = &imageMgr.textures["limit2"];
        t_ocg = &imageMgr.textures["ocg"];
        t_tcg = &imageMgr.textures["tcg"];
    }

    wxGameCanvas::~wxGameCanvas() {
        delete glcontext;
    }

    void wxGameCanvas::ClearDeck() {
        wxGLCanvas::SetCurrent(*glcontext);
        current_deck.Clear();
        imageMgr.UnloadAllCardTexture();
    }

    void wxGameCanvas::SaveScreenshot(const wxString& name, bool clipboard) {
        wxGLCanvas::SetCurrent(*glcontext);
        imageMgr.InitTextures();
        DrawScene();
        unsigned char* image_buff = new unsigned char[glwidth * glheight * 4];
        unsigned char* rgb_buff = new unsigned char[glwidth * glheight * 3];
        glReadPixels(0, 0, glwidth, glheight, GL_RGBA, GL_UNSIGNED_BYTE, image_buff);
        for(unsigned int h = 0; h < glheight; ++h) {
            for(unsigned int w = 0; w < glwidth; ++w) {
                unsigned int ch = glheight - 1 - h;
                rgb_buff[h * glwidth * 3 + w * 3 + 0] = image_buff[ch * glwidth * 4 + w * 4 + 0];
                rgb_buff[h * glwidth * 3 + w * 3 + 1] = image_buff[ch * glwidth * 4 + w * 4 + 1];
                rgb_buff[h * glwidth * 3 + w * 3 + 2] = image_buff[ch * glwidth * 4 + w * 4 + 2];
            }
        }
        wxImage* img = new wxImage(glwidth, glheight, rgb_buff, true);
        if(clipboard) {
            wxBitmap bmp(*img);
            wxTheClipboard->Open();
            wxTheClipboard->SetData(new wxBitmapDataObject(bmp));
            wxTheClipboard->Close();
        } else
            img->SaveFile(name, wxBITMAP_TYPE_PNG);
        delete img;
        delete[] rgb_buff;
        delete[] image_buff;
    }

    void wxGameCanvas::EventResized(wxSizeEvent& evt) {
        glwidth = evt.GetSize().GetWidth();
        glheight = evt.GetSize().GetHeight();
        glViewport(0, 0, glwidth, glheight);
        Refresh();
    }

    void wxGameCanvas::EventRender(wxPaintEvent& evt) {
        if(!IsShown())
            return;
        wxGLCanvas::SetCurrent(*glcontext);
        wxPaintDC(this);
        imageMgr.InitTextures();
        DrawScene();
        SwapBuffers();
    }

    void wxGameCanvas::EventMouseMoved(wxMouseEvent& evt) {
        static wxPoint pre(0, 0);
        wxPoint pt = evt.GetPosition();
        float fx = pt.x / (float)glwidth * 2.0f - 1.0f;
        float fy = -pt.y / (float)glheight * 2.0f + 1.0f;
        short pre_field = hover_field;
        short pre_index = hover_index;
        unsigned int pre_code = hover_code;
        hover_field = 0;
        hover_index = 0;
        hover_code = 0;
        mouse_field = 0;
        float wd = 0.2f * glheight / glwidth;
        float ht = 0.29f;
        if(fx >= -0.85f && fx <= 0.95f) {
            if(fy <= 0.92f && fy >= -0.28) { //main
                mouse_field = 1;
                int main_size = (int)current_deck.main_deck.size();
                if(main_size) {
                    int line_size = 18.0f / 11 / wd;
                    if(main_size > line_size * 4)
                        line_size = (main_size - 1) / 4 + 1;
                    if(line_size < 10)
                        line_size = 10;
                    float dx = (1.8f - wd) / (line_size - 1);
                    if(dx > wd * 11.0f / 10.0f)
                        dx = wd * 11.0f / 10.0f;
                    float dy = 0.3f;
                    int xindex = (fx + 0.85f) / dx;
                    int yindex = (0.92f - fy) / dy;
                    if(xindex < 0)
                        xindex = 0;
                    if(yindex < 0)
                        yindex = 0;
                    if(xindex >= line_size)
                        xindex = line_size - 1;
                    if(yindex > 3)
                        yindex = 3;
                    float sx = -0.85 + dx * xindex;
                    float sy = 0.92 - dy * yindex;
                    hover_index = (short)(yindex * line_size + xindex);
                    if(hover_index < main_size && fx - sx <= wd && sy - fy <= ht) {
                        hover_field = 1;
                        hover_code = std::get<0>(current_deck.main_deck[hover_index])->code;
                    }
                }
            } else if(fy <= -0.31f && fy >= -0.60f) { //extra
                mouse_field = 2;
                int extra_size = (int)current_deck.extra_deck.size();
                if(extra_size) {
                    float dx = (1.8f - wd) / (extra_size - 1);
                    if(dx > wd * 11.0f / 10.0f)
                        dx = wd * 11.0f / 10.0f;
                    hover_index = (fx + 0.85f) / dx;
                    float sx = -0.85 + dx * hover_index;
                    if(hover_index < extra_size && fx - sx <= wd) {
                        hover_field = 2;
                        hover_code = std::get<0>(current_deck.extra_deck[hover_index])->code;
                    }
                }
            } else if(fy <= -0.64f && fy >= -0.93f) { //side
                mouse_field = 3;
                int side_size = (int)current_deck.side_deck.size();
                if(side_size) {
                    float dx = (1.8f - wd) / (side_size - 1);
                    if(dx > wd * 11.0f / 10.0f)
                        dx = wd * 11.0f / 10.0f;
                    hover_index = (fx + 0.85f) / dx;
                    float sx = -0.85 + dx * hover_index;
                    if(hover_index < side_size && fx - sx <= wd) {
                        hover_field = 3;
                        hover_code = std::get<0>(current_deck.side_deck[hover_index])->code;
                    }
                }
            }
        }
        click_time = 0;
        mousex = evt.GetX() * 2.0 / glwidth - 1.0;
        mousey = 1.0 - evt.GetY() * 2.0 / glheight;
        if(t_draging)
            Refresh();
        if(pre_field == hover_field && pre_index == hover_index)
            return;
        if(click_field) {
            if(click_field == 1) {
                draging_code = std::get<0>(current_deck.main_deck[click_index])->code;
                t_draging = std::get<1>(current_deck.main_deck[click_index]);
            } else if(click_field == 2) {
                draging_code = std::get<0>(current_deck.extra_deck[click_index])->code;
                t_draging = std::get<1>(current_deck.extra_deck[click_index]);
            } else {
                draging_code = std::get<0>(current_deck.side_deck[click_index])->code;
                t_draging = std::get<1>(current_deck.side_deck[click_index]);
            }
            current_deck.RemoveCard(click_field, click_index);
            click_field = 0;
            mainFrame->StopViewRegulation();
        } else {
            if(pre_code != hover_code && hover_code != 0) {
                int delay = (int)commonCfg["hover_info_delay"];
                if(delay == 0)
                    mainFrame->SetCardInfo(hover_code);
                else {
                    hover_timer.Stop();
                    hover_timer.SetClientData((void*)(long)hover_code);
                    hover_timer.StartOnce(delay);
                }
            }
        }
        Refresh();
    }

    void wxGameCanvas::EventMouseLDown(wxMouseEvent& evt) {
        click_field = hover_field;
        click_index = hover_index;
        click_field_pre = click_field;
        if(click_field)
            Refresh();
    }
    
    void wxGameCanvas::EventMouseLUp(wxMouseEvent& evt) {
        if(click_field) {
            click_field = 0;
            click_index = 0;
            Refresh();
        }
        if(draging_code) {
            if(mouse_field != 0) {
                if(!current_deck.InsertCard(draging_code, mouse_field, hover_index))
                    current_deck.InsertCard(draging_code, click_field_pre);
                EventMouseMoved(evt);
            }
            draging_code = 0;
            t_draging = nullptr;
            Refresh();
        }
        if(click_time == 0)
            click_time = wxGetUTCTimeMillis();
        else {
            auto now = wxGetUTCTimeMillis();
            long dif = (now - click_time).ToLong();
            if(dif < 300) {
                if(hover_field == 1) {
                    if(current_deck.InsertCard(std::get<0>(current_deck.main_deck[hover_index])->code, 1, -1, true, true)) {
                        Refresh();
                        EventMouseMoved(evt);
                    }
                } else if(hover_field == 2) {
                    if(current_deck.InsertCard(std::get<0>(current_deck.extra_deck[hover_index])->code, 2, -1, true, true)) {
                        Refresh();
                        EventMouseMoved(evt);
                    }
                } else if(hover_field == 3) {
                    if(current_deck.InsertCard(std::get<0>(current_deck.side_deck[hover_index])->code, 3, -1, true, true)) {
                        Refresh();
                        EventMouseMoved(evt);
                    }
                }
                click_time = 0;
            } else
                click_time = now;
        }
    }
    
    void wxGameCanvas::EventMouseRDown(wxMouseEvent& evt) {
    }
    
    void wxGameCanvas::EventMouseRUp(wxMouseEvent& evt) {
        if(click_field) {
            click_field = 0;
            click_index = 0;
            Refresh();
        }
        if(draging_code) {
            draging_code = 0;
            t_draging = nullptr;
            Refresh();
        } else if(hover_field) {
            current_deck.RemoveCard(hover_field, hover_index);
            EventMouseMoved(evt);
            mainFrame->StopViewRegulation();
            Refresh();
        }
    }
    
    void wxGameCanvas::EventMouseLeftWindow(wxMouseEvent& evt) {
        if(hover_field != 0) {
            hover_field = 0;
            hover_index = 0;
            hover_code = 0;
            Refresh();
        }
    }
    
    void wxGameCanvas::OnHoverTimer(wxTimerEvent& evt) {
        unsigned int code = (unsigned int)(long)evt.GetTimer().GetClientData();
        if(code == hover_code)
            mainFrame->SetCardInfo(code);
    }

    void wxGameCanvas::DrawString(const char* str, int size, unsigned int color, float lx, float ly, float rx, float ry, bool limit) {
        glBindTexture(GL_TEXTURE_2D, t_font->tex());
        glBegin(GL_QUADS);
        glColor4ui((color >> 24) & 0xff, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
        if(limit) {
            float dx = (rx - lx) / size;
            for(int i = 0; i < size; ++i) {
                TextureInfo& ti = imageMgr.text_texture[str[i] - '*'];
                glColor4ui((color >> 24) & 0xff, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
                glTexCoord2f(ti.lx, ti.ly); glVertex2f(lx, ly);
                glTexCoord2f(ti.lx, ti.ry); glVertex2f(lx, ry);
                glTexCoord2f(ti.rx, ti.ry); glVertex2f(lx + dx * i, ry);
                glTexCoord2f(ti.rx, ti.ly); glVertex2f(lx + dx * i, ly);
            }
        } else {
            float dx = rx - ry;
            for(int i = 0; i < size; ++i) {
                TextureInfo& ti = imageMgr.text_texture[str[i] - '*'];
                glTexCoord2f(ti.lx, ti.ly); glVertex2f(lx, ly);
                glTexCoord2f(ti.lx, ti.ry); glVertex2f(lx, ry);
                glTexCoord2f(ti.rx, ti.ry); glVertex2f(lx + dx * i, ry);
                glTexCoord2f(ti.rx, ti.ly); glVertex2f(lx + dx * i, ly);
            }
        }
        glEnd();
    }

    void wxGameCanvas::DrawNumber(int number, unsigned int color, float lx, float ly, float rx, float ry) {
        glBindTexture(GL_TEXTURE_2D, t_font->tex());
        glBegin(GL_QUADS);
        float dx = (rx - lx) / 2;
        float offset = 0.0035f;
        TextureInfo& ti1 = imageMgr.text_texture[(number == 0 ? '-' : (number / 10 % 10 + '0')) - '*'];
        TextureInfo& ti2 = imageMgr.text_texture[(number == 0 ? '-' : (number % 10 + '0')) - '*'];

        glColor3f(0.1f, 0.1f, 0.0f);
        glTexCoord2f(ti1.lx, ti1.ly); glVertex2f(lx + offset, ly - offset);
        glTexCoord2f(ti1.lx, ti1.ry); glVertex2f(lx + offset, ry - offset);
        glTexCoord2f(ti1.rx, ti1.ry); glVertex2f(lx + dx + offset, ry - offset);
        glTexCoord2f(ti1.rx, ti1.ly); glVertex2f(lx + dx + offset, ly - offset);
        glTexCoord2f(ti2.lx, ti2.ly); glVertex2f(lx + dx + offset, ly - offset);
        glTexCoord2f(ti2.lx, ti2.ry); glVertex2f(lx + dx + offset, ry - offset);
        glTexCoord2f(ti2.rx, ti2.ry); glVertex2f(rx + offset, ry - offset);
        glTexCoord2f(ti2.rx, ti2.ly); glVertex2f(rx + offset, ly - offset);

        glColor3f(((color >> 24) & 0xff) / 255.0f, ((color >> 16) & 0xff) / 255.0f, ((color >> 8) & 0xff) / 255.0f);
        glTexCoord2f(ti1.lx, ti1.ly); glVertex2f(lx, ly);
        glTexCoord2f(ti1.lx, ti1.ry); glVertex2f(lx, ry);
        glTexCoord2f(ti1.rx, ti1.ry); glVertex2f(lx + dx, ry);
        glTexCoord2f(ti1.rx, ti1.ly); glVertex2f(lx + dx, ly);
        glTexCoord2f(ti2.lx, ti2.ly); glVertex2f(lx + dx, ly);
        glTexCoord2f(ti2.lx, ti2.ry); glVertex2f(lx + dx, ry);
        glTexCoord2f(ti2.rx, ti2.ry); glVertex2f(rx, ry);
        glTexCoord2f(ti2.rx, ti2.ly); glVertex2f(rx, ly);
        glEnd();
    }

    void wxGameCanvas::DrawCard(TextureInfo* ti, float lx, float ly, float rx, float ry, bool hl, int limit, float ix, float iy, int pool) {
        glBindTexture(GL_TEXTURE_2D, ti->tex());
        glBegin(GL_QUADS);
        {
            glTexCoord2f(ti->lx, ti->ly); glVertex2f(lx, ly);
            glTexCoord2f(ti->lx, ti->ry); glVertex2f(lx, ry);
            glTexCoord2f(ti->rx, ti->ry); glVertex2f(rx, ry);
            glTexCoord2f(ti->rx, ti->ly); glVertex2f(rx, ly);
        }
        glEnd();
        if(hl) {
            glBindTexture(GL_TEXTURE_2D, t_hmask->tex());
            glBegin(GL_QUADS);
            glTexCoord2f(t_hmask->lx, t_hmask->ly); glVertex2f(lx, ly);
            glTexCoord2f(t_hmask->lx, t_hmask->ry); glVertex2f(lx, ry);
            glTexCoord2f(t_hmask->rx, t_hmask->ry); glVertex2f(rx, ry);
            glTexCoord2f(t_hmask->rx, t_hmask->ly); glVertex2f(rx, ly);
            glEnd();
        }
        if(limit < 3) {
            glBindTexture(GL_TEXTURE_2D, t_limits[limit]->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ly); glVertex2f(lx - 0.01f, ly + 0.01f);
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ry); glVertex2f(lx - 0.01f, ly + 0.01f - iy);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ry); glVertex2f(lx - 0.01f + ix, ly + 0.01f - iy);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ly); glVertex2f(lx - 0.01f + ix, ly + 0.01f);
            }
            glEnd();
        }
        if(show_exclusive && ( pool == 1 || pool == 2)) {
            TextureInfo* ti = (pool == 1) ? t_ocg : t_tcg;
            float px = (lx + rx) / 2.0 - ix * 0.75f;
            glBindTexture(GL_TEXTURE_2D, ti->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(ti->lx, ti->ly); glVertex2f(px, ry + iy * 0.75f - 0.01f);
                glTexCoord2f(ti->lx, ti->ry); glVertex2f(px, ry - 0.01f);
                glTexCoord2f(ti->rx, ti->ry); glVertex2f(px + ix * 2.0f * 0.75f, ry - 0.01f);
                glTexCoord2f(ti->rx, ti->ly); glVertex2f(px + ix * 2.0f * 0.75f, ry + iy * 0.75f - 0.01f);
            }
            glEnd();
        }
    }
    
    void wxGameCanvas::DrawScene() {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        
        glShadeModel(GL_SMOOTH);

        //draw background
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, t_buildbg->tex());
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        {
            glTexCoord2f(t_buildbg->lx, t_buildbg->ly); glVertex2f(-1.0f, 1.0f);
            glTexCoord2f(t_buildbg->lx, t_buildbg->ry); glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(t_buildbg->rx, t_buildbg->ry); glVertex2f(1.0f, -1.0f);
            glTexCoord2f(t_buildbg->rx, t_buildbg->ly); glVertex2f(1.0f, 1.0f);
        }
        glEnd();
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, t_deckbg->tex());
        glBegin(GL_QUADS);
        {
            glTexCoord2f(t_deckbg->lx, t_deckbg->ly); glVertex2f(-1.0f, 1.0f);
            glTexCoord2f(t_deckbg->lx, t_deckbg->ry); glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(t_deckbg->rx, t_deckbg->ry); glVertex2f(1.0f, -1.0f);
            glTexCoord2f(t_deckbg->rx, t_deckbg->ly); glVertex2f(1.0f, 1.0f);
        }
        glEnd();
        size_t main_size = current_deck.main_deck.size();
        float wd = 0.2f * glheight / glwidth;
        float ht = 0.29f;
        float ix = 0.08f * glheight / glwidth;
        float iy = 0.08f;
        size_t line_size = 18.0f / 11 / wd;
        if(main_size > line_size * 4)
            line_size = (main_size - 1) / 4 + 1;
        if(line_size < 10)
            line_size = 10;
        float sx = -0.85f, sy = 0.92f;
        float dx = (1.8f - wd) / (line_size - 1);
        if(dx > wd * 11.0f / 10.0f)
            dx = wd * 11.0f / 10.0f;
        float dy = 0.3f;
        for(size_t i = 0; i < main_size; ++i) {
            CardData* cd = std::get<0>(current_deck.main_deck[i]);
            CardTextureInfo* cti = std::get<1>(current_deck.main_deck[i]);
            int limit = std::get<2>(current_deck.main_deck[i]);
            if(cti == nullptr) {
                cti = &imageMgr.GetCardTexture(cd->code);
                std::get<1>(current_deck.main_deck[i]) = cti;
            }
            TextureInfo* ti = &cti->ti;
            size_t lx = i % line_size;
            size_t ly = i / line_size;
            if(click_field == 1 && click_index == i)
                DrawCard(ti, sx + lx * dx, sy - ly * dy + 0.05f, sx + lx * dx + wd, sy - ly * dy - ht + 0.05f, hover_field == 1 && hover_index == i, limit, ix, iy, cd->pool);
            else
                DrawCard(ti, sx + lx * dx, sy - ly * dy, sx + lx * dx + wd, sy - ly * dy - ht, hover_field == 1 && hover_index == i, limit, ix, iy, cd->pool);
        }
        size_t extra_size = current_deck.extra_deck.size();
        sx = -0.85f;
        sy = -0.31f;
        dx = (1.8f - wd) / (extra_size - 1);
        if(dx > wd * 11.0f / 10.0f)
            dx = wd * 11.0f / 10.0f;
        for(size_t i = 0; i < extra_size; ++i) {
            CardData* cd = std::get<0>(current_deck.extra_deck[i]);
            CardTextureInfo* cti = std::get<1>(current_deck.extra_deck[i]);
            int limit = std::get<2>(current_deck.extra_deck[i]);
            if(cti == nullptr) {
                cti = &imageMgr.GetCardTexture(cd->code);
                std::get<1>(current_deck.extra_deck[i]) = cti;
            }
            TextureInfo* ti = &cti->ti;
            if(click_field == 2 && click_index == i)
                DrawCard(ti, sx + i * dx, sy + 0.05f, sx + i * dx + wd, sy - ht + 0.05f, hover_field == 2 && hover_index == i, limit, ix, iy, cd->pool);
            else
                DrawCard(ti, sx + i * dx, sy, sx + i * dx + wd, sy - ht, hover_field == 2 && hover_index == i, limit, ix, iy, cd->pool);
        }
        size_t side_size = current_deck.side_deck.size();
        sx = -0.85f;
        sy = -0.64f;
        dx = (1.8f - wd) / (side_size - 1);
        if(dx > wd * 11.0f / 10.0f)
            dx = wd * 11.0f / 10.0f;
        for(size_t i = 0; i < side_size; ++i) {
            CardData* cd = std::get<0>(current_deck.side_deck[i]);
            CardTextureInfo* cti = std::get<1>(current_deck.side_deck[i]);
            int limit = std::get<2>(current_deck.side_deck[i]);
            if(cti == nullptr) {
                cti = &imageMgr.GetCardTexture(cd->code);
                std::get<1>(current_deck.side_deck[i]) = cti;
            }
            TextureInfo* ti = &cti->ti;
            if(click_field == 3 && click_index == i)
                DrawCard(ti, sx + i * dx, sy + 0.05f, sx + i * dx + wd, sy - ht + 0.05f, hover_field == 3 && hover_index == i, limit, ix, iy, cd->pool);
            else
                DrawCard(ti, sx + i * dx, sy, sx + i * dx + wd, sy - ht, hover_field == 3 && hover_index == i, limit, ix, iy, cd->pool);
        } 
        DrawNumber(current_deck.mcount, 0xffffff00, -0.931f, 0.913f, -0.868f, 0.850f);
        DrawNumber(current_deck.scount, 0xffffff00, -0.931f, 0.825f, -0.868f, 0.762f);
        DrawNumber(current_deck.tcount, 0xffffff00, -0.931f, 0.737f, -0.868f, 0.674f);
        DrawNumber(current_deck.syncount, 0xffffff00, -0.931f, 0.611f, -0.868f, 0.548f);
        DrawNumber(current_deck.xyzcount, 0xffffff00, -0.931f, 0.523f, -0.868f, 0.460f);
        DrawNumber(current_deck.fuscount, 0xffffff00, -0.931f, 0.435f, -0.868f, 0.372f);
        DrawNumber((int)current_deck.main_deck.size(), 0xffffff00, -0.931f, -0.003f, -0.868f, -0.066f);
        DrawNumber((int)current_deck.extra_deck.size(), 0xffffff00, -0.931f, -0.313f, -0.868f, -0.376f);
        DrawNumber((int)current_deck.side_deck.size(), 0xffffff00, -0.931f, -0.646f, -0.868f, -0.709f);
        
        if(t_draging)
            DrawCard(&t_draging->ti, mousex - wd / 2, mousey + ht / 2, mousex + wd / 2, mousey - ht / 2, false, 3, 0, 0, 0);
        
        glFlush();
        Refresh();
    }

   */ 
}
