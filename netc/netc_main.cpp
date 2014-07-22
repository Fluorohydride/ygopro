#include <iostream>

#include "scene_mgr.h"
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
    
    sceneMgr.Init();
    sceneMgr.SetSceneSize({1024, 640});
    sceneMgr.InitDraw();
    sceneMgr.SwitchScene(SceneMgr::SceneType::Builder);
    
    bool running = true;
    sf::Clock clock;
    float tm1 = clock.getElapsedTime().asSeconds() - 10.0f;
    int fps = 0;
    while (running) {
        fps++;
        sf::Event evt;
        float tm2 = clock.getElapsedTime().asSeconds();
        if(tm2 - tm1 >= 10.0f) {
            std::cout << "Average fps in 10s : " << fps / 10.0f << std::endl;
            tm1 += 10.0f;
            fps = 0;
        }
        sceneMgr.Update();
        sceneMgr.InitDraw();
        while (window.pollEvent(evt)) {
            switch(evt.type) {
                case sf::Event::Closed:
                    running = false;
                    break;
                case sf::Event::Resized:
                    sceneMgr.SetSceneSize(sgui::v2i{(int)evt.size.width, (int)evt.size.height});
                    sgui::SGGUIRoot::GetSingleton().SetSceneSize(sgui::v2i{(int)evt.size.width, (int)evt.size.height});
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
                    if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonDownEvent(evt.mouseButton))
                        sceneMgr.MouseButtonDown(evt.mouseButton);
                    break;
                case sf::Event::MouseButtonReleased:
                    if(!sgui::SGGUIRoot::GetSingleton().InjectMouseButtonUpEvent(evt.mouseButton))
                        sceneMgr.MouseButtonUp(evt.mouseButton);
                    break;
                case sf::Event::MouseMoved:
                    if(!sgui::SGGUIRoot::GetSingleton().InjectMouseMoveEvent(evt.mouseMove))
                        sceneMgr.MouseMove(evt.mouseMove);
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
        sceneMgr.Draw();
        sgui::SGGUIRoot::GetSingleton().Draw();

        window.display();
    }
    
    return 0;
}
