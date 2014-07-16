#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

#include "game_scene.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "sungui.h"

using namespace ygopro;

int main(int argc, char* argv[]) {
    sf::RenderWindow window(sf::VideoMode(1024, 640), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);
    //window.setFramerateLimit(60);
    window.setActive();
    glewInit();
    
    imageMgr.InitTextures();
    if(!commonCfg.LoadConfig("common.xml"))
        return 0;
    if(!stringCfg.LoadConfig(commonCfg["string_path"]))
        return 0;
    if(dataMgr.LoadDatas(commonCfg["database"]))
        return 0;
    if(!imageMgr.LoadImageConfig(commonCfg["textures"]))
        return 0;
    limitRegulationMgr.LoadLimitRegulation(commonCfg["limit_regulation"], stringCfg["eui_list_default"]);
    stringCfg.ForEach([](const wxString& name, ValueStruct& value) {
        if(name.Left(8) == wxT("setname_")) {
            dataMgr.RegisterSetCode((unsigned int)value, name.Right(name.Length() - 8));
        }
    });
    
    sgui::SGGUIRoot::GetSingleton().LoadConfigs();
    sgui::SGGUIRoot::GetSingleton().SetSceneSize({1024, 640});
    auto w1 = sgui::SGWindow::Create(nullptr, {100, 100}, {500, 400});
    w1->SetTitle(L"Test Caption");
    
    auto tab = sgui::SGTabControl::Create(w1, {10, 30}, {480, 300});
    for(int i = 0; i < 5; ++i)
        tab->AddTab(L"Option", 0xffff0000);
    tab->SetSize({-20, 300}, {1.0f, 0.0f});
    auto cbx = sgui::SGComboBox::Create(tab->GetTab(0), {10, 10}, {150, 30});
    for(int i = 0; i < 10; ++i)
        cbx->AddItem(L"Item", 0xff000000 | globalRandom.GetRandomUInt32());
    cbx->SetSelection(3);
    auto sch = sgui::SGListBox::Create(tab->GetTab(1), {10, 10}, {150, 100});
    for(int i = 0; i < 10; ++i)
        sch->AddItem(i + 1, L"Item with icon", 0xff000000 | globalRandom.GetRandomUInt32());
    sch->SetSelection(2);
    auto b1 = sgui::SGCheckbox::Create(tab->GetTab(2), {10, 30}, L"Click me!");
    auto r1 = sgui::SGRadio::Create(tab->GetTab(2), {10, 55}, L"A Radio");
    auto r2 = sgui::SGRadio::Create(tab->GetTab(2), {10, 80}, L"Another Radio");
    r1->SetCustomValue(100);
    r2->SetCustomValue(200);
    r2->AttachGroup(r1);
    auto r3 = sgui::SGRadio::Create(tab->GetTab(2), {10, 115}, L"A Radio");
    auto r4 = sgui::SGRadio::Create(tab->GetTab(2), {10, 140}, L"Another Radio");
    r4->AttachGroup(r3);
    auto bt1 = sgui::SGButton::Create(tab->GetTab(3), {10, 10}, {100, 50});
    auto bt2 = sgui::SGButton::Create(tab->GetTab(3), {120, 10}, {100, 50}, true);
    bt1->SetText(L"A Button", 0xffff0000);
    bt2->SetText(L"Push Button", 0xffff0000);
    auto t1 = sgui::SGTextEdit::Create(tab->GetTab(3), {10, 150}, {150, 30});
    t1->SetText(L"InputText12345  67890", 0xff000000);
    auto lb = sgui::SGIconLabel::Create(tab->GetTab(3), {10, 70}, L"", 0);
    lb->SetText(L"Line1\ue004\ue005\ue086\nLine2\ue001Line2\nLine3Line3", 0xff000000);
    
    gameScene.Init();
    gameScene.UpdateScene();
    bool running = true;
    sf::Clock clock;
    float tm1 = clock.getElapsedTime().asSeconds();
    int fps = 0;
    sf::Texture t;
    t.loadFromFile("../ygo/pics/98558751.jpg");;
    sf::Sprite sp(t);
    sp.setPosition(100, 100);
    sf::Sprite sp2(t);
    sp.setPosition(400, 100);
    while (running) {
        fps++;
        sf::Event evt;
        float tm2 = clock.getElapsedTime().asSeconds();
        if(tm2 - tm1 >= 10.0f) {
            std::cout << "Average fps in 10s : " << fps / 10.0f << std::endl;
            tm1 += 10.0f;
            fps = 0;
        }
        while (window.pollEvent(evt)) {
            gameScene.InitDraw();
            switch(evt.type) {
                case sf::Event::Closed:
                    running = false;
                    break;
                case sf::Event::Resized:
                    glViewport(0, 0, evt.size.width, evt.size.height);
                    sgui::SGGUIRoot::GetSingleton().SetSceneSize(sgui::v2i(evt.size.width, evt.size.height));
                    break;
                case sf::Event::TextEntered:
                    sgui::SGGUIRoot::GetSingleton().InjectCharEvent(evt.text);
                    break;
                case sf::Event::KeyPressed:
                    sgui::SGGUIRoot::GetSingleton().InjectKeyDownEvent(evt.key);
                    break;
                case sf::Event::KeyReleased:
                    sgui::SGGUIRoot::GetSingleton().InjectKeyUpEvent(evt.key);
                    break;
                case sf::Event::MouseButtonPressed:
                    sgui::SGGUIRoot::GetSingleton().InjectMouseButtonDownEvent(evt.mouseButton);
                    break;
                case sf::Event::MouseButtonReleased:
                    sgui::SGGUIRoot::GetSingleton().InjectMouseButtonUpEvent(evt.mouseButton);
                    break;
                case sf::Event::MouseMoved:
                    sgui::SGGUIRoot::GetSingleton().InjectMouseMoveEvent(evt.mouseMove);
                    break;
                case sf::Event::MouseEntered:
                    sgui::SGGUIRoot::GetSingleton().InjectMouseEnterEvent();
                    break;
                case sf::Event::MouseLeft:
                    sgui::SGGUIRoot::GetSingleton().InjectMouseLeaveEvent();
                    break;
                case sf::Event::MouseWheelMoved:
                    sgui::SGGUIRoot::GetSingleton().InjectMouseWheelEvent(evt.mouseWheel);
                    break;
                default:
                    break;
            }
        }
        //window.capture().saveToFile("sc.png");
        glViewport(0, 0, window.getSize().x, window.getSize().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gameScene.Draw();
        sgui::SGGUIRoot::GetSingleton().Draw();

        window.display();
    }
    
    return 0;
}
