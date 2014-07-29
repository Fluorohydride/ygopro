#include <iostream>

#include "scene_mgr.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "sungui.h"

using namespace ygopro;

int main(int argc, char* argv[]) {
    if(!commonCfg.LoadConfig(L"common.xml"))
        return 0;

    int width = commonCfg[L"window_width"];
    int height = commonCfg[L"window_height"];
    sf::RenderWindow window(sf::VideoMode(width, height), "Ygopro", sf::Style::Default, sf::ContextSettings(32));
    if((int)commonCfg[L"vertical_sync"])
        window.setVerticalSyncEnabled(true);
    window.setActive();
    glewInit();
    
    imageMgr.InitTextures();
    if(!stringCfg.LoadConfig(commonCfg[L"string_path"]))
        return 0;
    if(dataMgr.LoadDatas(commonCfg[L"database"]))
        return 0;
    if(!imageMgr.LoadImageConfig(commonCfg[L"textures"]))
        return 0;
    limitRegulationMgr.LoadLimitRegulation(commonCfg[L"limit_regulation"], stringCfg[L"eui_list_default"]);
    stringCfg.ForEach([](const std::wstring& name, ValueStruct& value) {
        if(name.find(L"setname_") == 0 ) {
            dataMgr.RegisterSetCode(static_cast<unsigned int>(value), name.substr(8));
        }
    });
    
    sgui::SGGUIRoot::GetSingleton().LoadConfigs();
    sgui::SGGUIRoot::GetSingleton().SetSceneSize({width, height});
    
    sceneMgr.Init();
    sceneMgr.SetSceneSize({width, height});
    sceneMgr.InitDraw();
    sceneMgr.SwitchScene(SceneMgr::SceneType::Builder);
    sceneMgr.SetFrameRate((int)commonCfg[L"frame_rate"]);
    
    bool running = true;
    sf::Clock clock;
    float tm1 = clock.getElapsedTime().asSeconds() - 5.0f;
    int fps = 0;
    while (running) {
        fps++;
        sf::Event evt;
        float tm2 = clock.getElapsedTime().asSeconds();
        if(tm2 - tm1 >= 5.0f) {
            std::cout << "Average fps in 5s : " << fps / 5.0f << std::endl;
            tm1 += 5.0f;
            fps = 0;
        }
        sceneMgr.CheckFrameRate();
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
                    if(!sgui::SGGUIRoot::GetSingleton().InjectKeyDownEvent(evt.key))
                        sceneMgr.KeyDown(evt.key);
                    break;
                case sf::Event::KeyReleased:
                    if(!sgui::SGGUIRoot::GetSingleton().InjectKeyUpEvent(evt.key))
                        sceneMgr.KeyUp(evt.key);
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
    
    sceneMgr.Uninit();
    sgui::SGGUIRoot::GetSingleton().ClearChild();
    imageMgr.UninitTextures();
    
    return 0;
}
