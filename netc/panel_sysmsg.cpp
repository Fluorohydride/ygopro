#include "panels.h"

namespace ygopro
{

	PanelSysMsg::PanelSysMsg(int type): PanelBase("PanelSysMsg.layout") {
		btnOK = _mainWidget->findWidget("btnMsgOK")->castType<MyGUI::Button>();
		tbMsg = _mainWidget->findWidget("tbSysMsg")->castType<MyGUI::TextBox>();
		switch(type) {
		case PANEL_SYSMSG_DEFAULT:
			tbMsg->setCaption(_mainWidget->getUserString("network_err"));
			btnOK->eventMouseButtonClick += MyGUI::newDelegate(this, &PanelSysMsg::btnOKExit);
			break;
		}
		MyGUI::InputManager::getInstance().setKeyFocusWidget(_mainWidget);
	}

	PanelSysMsg::~PanelSysMsg() {

	}

	void PanelSysMsg::btnOKExit(MyGUI::Widget* _sender) {
		
	}

}
