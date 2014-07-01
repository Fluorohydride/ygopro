#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

#include "game_frame.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "sungui.h"

using namespace ygopro;

int main(int argc, char* argv[]) {
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
    
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);
    //window.setFramerateLimit(120);
    window.setActive();
    glewInit();
    
    sgui::SGGUIRoot::GetSingleton().LoadConfigs();
    sgui::SGGUIRoot::GetSingleton().SetSceneSize({800, 600});
    auto w1 = sgui::SGWindow::Create(nullptr, {100, 100}, {300, 400});
    w1->SetTitle(L"Test Caption");
    auto b1 = sgui::SGCheckbox::Create(w1, {10, 30}, L"Click me!");
    auto r1 = sgui::SGRadio::Create(w1, {10, 55}, L"A Radio");
    auto r2 = sgui::SGRadio::Create(w1, {10, 80}, L"Another Radio");
    r1->SetCustomValue(100);
    r2->SetCustomValue(200);
    r2->SetGroup(r1);
    auto r3 = sgui::SGRadio::Create(w1, {10, 105}, L"A Radio");
    auto r4 = sgui::SGRadio::Create(w1, {10, 130}, L"Another Radio");
    r4->SetGroup(r3);
    auto bt1 = sgui::SGButton::Create(w1, {10, 160}, {100, 50});
    bt1->SetText(L"A Button", 0xffff0000);
    auto lb = sgui::SGLabel::Create(w1, {10, 220}, L"", 0);
    lb->SetText(L"Line1\nLine2Line2", 0xff000000);
    auto t1 = sgui::SGTextEdit::Create(w1, {10, 300}, {150, 30});
    t1->SetText(L"InputText12345  67890", 0xff000000);
    bool running = true;
    sf::Clock clock;
    float tm1 = clock.getElapsedTime().asSeconds();
    while (running) {
        sf::Event evt;
        float tm2 = clock.getElapsedTime().asSeconds();
        float tm_delta = tm2 - tm1;
        tm1 = tm2;
        tm_delta = 0;
        while (window.pollEvent(evt)) {
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.2f, 1.2f, 1.2f, 1.2f);
        sgui::SGGUIRoot::GetSingleton().Draw();
        window.display();
    }
    
    return 0;
}
