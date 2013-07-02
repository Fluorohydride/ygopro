#include "panel_base.h"

namespace ygopro
{

	PanelBase::PanelBase(const std::string& layoutname): _mainWidget(0) {
		_widgetsLoaded = MyGUI::LayoutManager::getInstance().loadLayout(layoutname);
		if(_widgetsLoaded.size())
			_mainWidget = _widgetsLoaded[0];
		for(auto wit = _widgetsLoaded.begin(); wit != _widgetsLoaded.end(); ++wit)
			(*wit)->setVisible(false);
	}

	PanelBase::~PanelBase() {
		if(_widgetsLoaded.size())
			MyGUI::LayoutManager::getInstance().unloadLayout(_widgetsLoaded);
	}

	void PanelBase::setSize(int w, int h) {
		if(_mainWidget)
			_mainWidget->setSize(w, h);
	}

	void PanelBase::setPosition(int x, int y) {
		if(_mainWidget)
			_mainWidget->setRealPosition(x, y);
	}

	void PanelBase::fadeShow(float speed) {
		for(auto wit = _widgetsLoaded.begin(); wit != _widgetsLoaded.end(); ++wit) {
			MyGUI::ControllerItem* item = MyGUI::ControllerManager::getInstance().createItem(MyGUI::ControllerFadeAlpha::getClassTypeName());
			MyGUI::ControllerFadeAlpha* controller = item->castType<MyGUI::ControllerFadeAlpha>();
			controller->setAlpha(1.0);
			controller->setCoef(speed);
			controller->setEnabled(true);
			MyGUI::ControllerManager::getInstance().addItem((*wit), controller);
		}
	}

	void PanelBase::fadeDestroy(float speed) {
		bool desreg = true;
		for(auto wit = _widgetsLoaded.begin(); wit != _widgetsLoaded.end(); ++wit) {
			MyGUI::ControllerItem* item = MyGUI::ControllerManager::getInstance().createItem(MyGUI::ControllerFadeAlpha::getClassTypeName());
			MyGUI::ControllerFadeAlpha* controller = item->castType<MyGUI::ControllerFadeAlpha>();
			controller->setAlpha(0.0);
			controller->setCoef(speed);
			controller->setEnabled(true);
			if(desreg) {
				controller->eventPostAction += MyGUI::newDelegate(this, &PanelBase::_eventDestroy);
				desreg = false;
			}
			MyGUI::ControllerManager::getInstance().addItem((*wit), controller);
		}
	}

}
