#include "wx/image.h"
#include "wx/clipbrd.h"
#include "image_mgr.h"
#include "game_frame.h"
#include "editor_frame.h"
#include "editor_canvas.h"
#include "card_data.h"
#include "deck_data.h"

namespace ygopro
{

    wxEditorCanvas::wxEditorCanvas(wxFrame* parent, int id, int* args) : wxGLCanvas(parent, id, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
        glcontext = new wxGLContext(this);
        Bind(wxEVT_SIZE, &wxEditorCanvas::EventResized, this);
        Bind(wxEVT_PAINT, &wxEditorCanvas::EventRender, this);
        Bind(wxEVT_MOTION, &wxEditorCanvas::EventMouseMoved, this);
        Bind(wxEVT_MOUSEWHEEL, &wxEditorCanvas::EventMouseWheelMoved, this);
        Bind(wxEVT_LEFT_DOWN, &wxEditorCanvas::EventMouseDown, this);
        Bind(wxEVT_LEFT_UP, &wxEditorCanvas::EventMouseReleased, this);
        Bind(wxEVT_LEAVE_WINDOW, &wxEditorCanvas::EventMouseLeftWindow, this);
        hover_timer.SetOwner(this);
        Bind(wxEVT_TIMER, &wxEditorCanvas::OnHoverTimer, this);
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        t_buildbg = &imageMgr.textures["buildbg"];
        t_deckbg = &imageMgr.textures["deckbg"];
        t_font = &imageMgr.textures["number"];
        t_hmask = &imageMgr.textures["hmask"];
        t_limits[0] = &imageMgr.textures["limit0"];
        t_limits[1] = &imageMgr.textures["limit1"];
        t_limits[2] = &imageMgr.textures["limit2"];
        hover_field = 0;
        hover_index = 0;
        hover_code = 0;
    }

    wxEditorCanvas::~wxEditorCanvas() {
        delete glcontext;
    }

    void wxEditorCanvas::ClearDeck() {
        wxGLCanvas::SetCurrent(*glcontext);
        current_deck.Clear();
        imageMgr.UnloadAllCardTexture();
    }

    void wxEditorCanvas::SaveScreenshot(const wxString& name, bool clipboard) {
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

    void wxEditorCanvas::EventResized(wxSizeEvent& evt) {
        glwidth = evt.GetSize().GetWidth();
        glheight = evt.GetSize().GetHeight();
        glViewport(0, 0, glwidth, glheight);
        Refresh();
    }

    void wxEditorCanvas::EventRender(wxPaintEvent& evt) {
        if(!IsShown())
            return;
        wxGLCanvas::SetCurrent(*glcontext);
        wxPaintDC(this);
        imageMgr.InitTextures();
        DrawScene();
        SwapBuffers();
    }

    void wxEditorCanvas::EventMouseMoved(wxMouseEvent& evt) {
        wxPoint pt = evt.GetPosition();
        float fx = pt.x / (float)glwidth * 2.0f - 1.0f;
        float fy = -pt.y / (float)glheight * 2.0f + 1.0f;
        short pre_field = hover_field;
        short pre_index = hover_index;
        unsigned int pre_code = hover_code;
        hover_field = 0;
        hover_index = 0;
        hover_code = 0;
        float wd = 0.2f * glheight / glwidth;
        float ht = 0.29f;
        if(fx >= -0.85f && fx <= 0.95f) {
            if(fy <= 0.92f && fy >= -0.28) { //main
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
                    if(fx - sx <= wd && sy - fy <= ht) {
                        hover_field = 1;
                        hover_index = (short)(yindex * line_size + xindex);
                        if(hover_index >= main_size) {
                            hover_field = 0;
                            hover_index = 0;
                        } else
                            hover_code = std::get<0>(current_deck.main_deck[hover_index])->code;
                    }
                }
            } else if(fy <= -0.31f && fy >= -0.60f) { //extra
                int extra_size = (int)current_deck.extra_deck.size();
                if(extra_size) {
                    float dx = (1.8f - wd) / (extra_size - 1);
                    if(dx > wd * 11.0f / 10.0f)
                        dx = wd * 11.0f / 10.0f;
                    int index = (fx + 0.85f) / dx;
                    if(index >= extra_size)
                        index = extra_size - 1;
                    float sx = -0.85 + dx * index;
                    if(fx - sx <= wd) {
                        hover_field = 2;
                        hover_index = (short)index;
                        hover_code = std::get<0>(current_deck.extra_deck[hover_index])->code;
                    }
                }
            } else if(fy <= -0.64f && fy >= -0.93f) { //side
                int side_size = (int)current_deck.side_deck.size();
                if(side_size) {
                    float dx = (1.8f - wd) / (side_size - 1);
                    if(dx > wd * 11.0f / 10.0f)
                        dx = wd * 11.0f / 10.0f;
                    int index = (fx + 0.85f) / dx;
                    if(index >= side_size)
                        index = side_size - 1;
                    float sx = -0.85 + dx * index;
                    if(fx - sx <= wd) {
                        hover_field = 3;
                        hover_index = (short)index;
                        hover_code = std::get<0>(current_deck.side_deck[hover_index])->code;
                    }
                }
            }
        }
        if(pre_field != hover_field || pre_index != hover_index)
            Refresh();
        int delay = (int)commonCfg["hover_info_delay"];
        if(pre_code != hover_code && hover_code != 0) {
            if(delay == 0)
                editorFrame->SetCardInfo(hover_code);
            else {
                hover_timer.Stop();
                hover_timer.SetClientData((void*)(long)hover_code);
                hover_timer.StartOnce(delay);
            }
        }
    }

    void wxEditorCanvas::EventMouseWheelMoved(wxMouseEvent& evt) {

    }

    void wxEditorCanvas::EventMouseDown(wxMouseEvent& evt) {
        if(hover_field == 0)
            return;
        click_field = hover_field;
        click_index = hover_index;
        wxMenu popmenu;
        popmenu.Append(ID_POP_ADDCARD, "Same card +1");
        popmenu.Append(ID_POP_DELCARD, "Remove card");
        if(hover_field == 1 || hover_field == 2)
            popmenu.Append(ID_POP_TOSIDE, "Move to side deck");
        else
            popmenu.Append(ID_POP_TOMAIN, "Move to main deck");
        popmenu.Bind(wxEVT_COMMAND_MENU_SELECTED, &wxEditorCanvas::OnPopupMenu, this);
        PopupMenu(&popmenu);
    }

    void wxEditorCanvas::EventMouseReleased(wxMouseEvent& evt) {

    }

    void wxEditorCanvas::EventMouseLeftWindow(wxMouseEvent& evt) {
        if(hover_field != 0) {
            hover_field = 0;
            hover_index = 0;
            hover_code = 0;
            Refresh();
        }
    }

    void wxEditorCanvas::OnHoverTimer(wxTimerEvent& evt) {
        unsigned int code = (unsigned int)(long)evt.GetTimer().GetClientData();
        if(code == hover_code)
            editorFrame->SetCardInfo(code);
    }

    void wxEditorCanvas::OnPopupMenu(wxCommandEvent& evt) {
        unsigned int id = evt.GetId();
        switch(id) {
        case ID_POP_ADDCARD:
            break;
        case ID_POP_DELCARD:
            break;
        case ID_POP_TOMAIN:
            break;
        case ID_POP_TOSIDE:
            break;
        }
    }

    void wxEditorCanvas::DrawString(const char* str, int size, unsigned int color, float lx, float ly, float rx, float ry, bool limit) {
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

    void wxEditorCanvas::DrawNumber(int number, unsigned int color, float lx, float ly, float rx, float ry) {
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

    void wxEditorCanvas::DrawScene() {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
        float iconw = 0.08f * glheight / glwidth;
        float iconh = 0.08f;
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
            CardTextureInfo* cti = std::get<1>(current_deck.main_deck[i]);
            int limit = std::get<2>(current_deck.main_deck[i]);
            if(cti == nullptr) {
                cti = &imageMgr.GetCardTexture(std::get<0>(current_deck.main_deck[i])->code);
                std::get<1>(current_deck.main_deck[i]) = cti;
            }
            TextureInfo* ti = &cti->ti;
            size_t lx = i % line_size;
            size_t ly = i / line_size;
            glBindTexture(GL_TEXTURE_2D, ti->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(ti->lx, ti->ly); glVertex2f(sx + lx * dx, sy - ly * dy);
                glTexCoord2f(ti->lx, ti->ry); glVertex2f(sx + lx * dx, sy - ly * dy - ht);
                glTexCoord2f(ti->rx, ti->ry); glVertex2f(sx + lx * dx + wd, sy - ly * dy - ht);
                glTexCoord2f(ti->rx, ti->ly); glVertex2f(sx + lx * dx + wd, sy - ly * dy);
            }
            glEnd();
            if(hover_field == 1 && hover_index == i) {
                glBindTexture(GL_TEXTURE_2D, t_hmask->tex());
                glBegin(GL_QUADS);
                glTexCoord2f(t_hmask->lx, t_hmask->ly); glVertex2f(sx + lx * dx, sy - ly * dy);
                glTexCoord2f(t_hmask->lx, t_hmask->ry); glVertex2f(sx + lx * dx, sy - ly * dy - ht);
                glTexCoord2f(t_hmask->rx, t_hmask->ry); glVertex2f(sx + lx * dx + wd, sy - ly * dy - ht);
                glTexCoord2f(t_hmask->rx, t_hmask->ly); glVertex2f(sx + lx * dx + wd, sy - ly * dy);
                glEnd();
            }
            if(limit >= 3)
                continue;
            glBindTexture(GL_TEXTURE_2D, t_limits[limit]->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ly); glVertex2f(sx - 0.01f + lx * dx, sy + 0.01f - ly * dy);
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ry); glVertex2f(sx - 0.01f + lx * dx, sy + 0.01f - ly * dy - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ry); glVertex2f(sx - 0.01f + lx * dx + iconw, sy + 0.01f - ly * dy - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ly); glVertex2f(sx - 0.01f + lx * dx + iconw, sy + 0.01f - ly * dy);
            }
            glEnd();
        }
        size_t extra_size = current_deck.extra_deck.size();
        sx = -0.85f;
        sy = -0.31f;
        dx = (1.8f - wd) / (extra_size - 1);
        if(dx > wd * 11.0f / 10.0f)
            dx = wd * 11.0f / 10.0f;
        for(size_t i = 0; i < extra_size; ++i) {
            CardTextureInfo* cti = std::get<1>(current_deck.extra_deck[i]);
            int limit = std::get<2>(current_deck.extra_deck[i]);
            if(cti == nullptr) {
                cti = &imageMgr.GetCardTexture(std::get<0>(current_deck.extra_deck[i])->code);
                std::get<1>(current_deck.extra_deck[i]) = cti;
            }
            TextureInfo* ti = &cti->ti;
            glBindTexture(GL_TEXTURE_2D, ti->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(ti->lx, ti->ly); glVertex2f(sx + i * dx, sy);
                glTexCoord2f(ti->lx, ti->ry); glVertex2f(sx + i * dx, sy - ht);
                glTexCoord2f(ti->rx, ti->ry); glVertex2f(sx + i * dx + wd, sy - ht);
                glTexCoord2f(ti->rx, ti->ly); glVertex2f(sx + i * dx + wd, sy);
            }
            glEnd();
            if(hover_field == 2 && hover_index == i) {
                glBindTexture(GL_TEXTURE_2D, t_hmask->tex());
                glBegin(GL_QUADS);
                glTexCoord2f(t_hmask->lx, t_hmask->ly); glVertex2f(sx + i * dx, sy);
                glTexCoord2f(t_hmask->lx, t_hmask->ry); glVertex2f(sx + i * dx, sy - ht);
                glTexCoord2f(t_hmask->rx, t_hmask->ry); glVertex2f(sx + i * dx + wd, sy - ht);
                glTexCoord2f(t_hmask->rx, t_hmask->ly); glVertex2f(sx + i * dx + wd, sy);
                glEnd();
            }
            if(limit >= 3)
                continue;
            glBindTexture(GL_TEXTURE_2D, t_limits[limit]->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ly); glVertex2f(sx - 0.01f + i * dx, sy + 0.01f);
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ry); glVertex2f(sx - 0.01f + i * dx, sy + 0.01f - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ry); glVertex2f(sx - 0.01f + i * dx + iconw, sy + 0.01f - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ly); glVertex2f(sx - 0.01f + i * dx + iconw, sy + 0.01f);
            }
            glEnd();
        }
        size_t side_size = current_deck.side_deck.size();
        sx = -0.85f;
        sy = -0.64f;
        dx = (1.8f - wd) / (side_size - 1);
        if(dx > wd * 11.0f / 10.0f)
            dx = wd * 11.0f / 10.0f;
        for(size_t i = 0; i < side_size; ++i) {
            CardTextureInfo* cti = std::get<1>(current_deck.side_deck[i]);
            int limit = std::get<2>(current_deck.side_deck[i]);
            if(cti == nullptr) {
                cti = &imageMgr.GetCardTexture(std::get<0>(current_deck.side_deck[i])->code);
                std::get<1>(current_deck.side_deck[i]) = cti;
            }
            TextureInfo* ti = &cti->ti;
            glBindTexture(GL_TEXTURE_2D, ti->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(ti->lx, ti->ly); glVertex2f(sx + i * dx, sy);
                glTexCoord2f(ti->lx, ti->ry); glVertex2f(sx + i * dx, sy - ht);
                glTexCoord2f(ti->rx, ti->ry); glVertex2f(sx + i * dx + wd, sy - ht);
                glTexCoord2f(ti->rx, ti->ly); glVertex2f(sx + i * dx + wd, sy);
            }
            glEnd();
            if(hover_field == 3 && hover_index == i) {
                glBindTexture(GL_TEXTURE_2D, t_hmask->tex());
                glBegin(GL_QUADS);
                glTexCoord2f(t_hmask->lx, t_hmask->ly); glVertex2f(sx + i * dx, sy);
                glTexCoord2f(t_hmask->lx, t_hmask->ry); glVertex2f(sx + i * dx, sy - ht);
                glTexCoord2f(t_hmask->rx, t_hmask->ry); glVertex2f(sx + i * dx + wd, sy - ht);
                glTexCoord2f(t_hmask->rx, t_hmask->ly); glVertex2f(sx + i * dx + wd, sy);
                glEnd();
            }
            if(limit >= 3)
                continue;
            glBindTexture(GL_TEXTURE_2D, t_limits[limit]->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ly); glVertex2f(sx - 0.01f + i * dx, sy + 0.01f);
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ry); glVertex2f(sx - 0.01f + i * dx, sy + 0.01f - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ry); glVertex2f(sx - 0.01f + i * dx + iconw, sy + 0.01f - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ly); glVertex2f(sx - 0.01f + i * dx + iconw, sy + 0.01f);
            }
            glEnd();
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
        glFlush();
    }

}
