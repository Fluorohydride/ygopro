#ifndef _PANEL_SYSMSG_H_
#define _PANEL_SYSMSG_H_

#include "panel_base.h"

#define PANEL_SYSMSG_DEFAULT	1

namespace ygopro
{

	class PanelSysMsg : public PanelBase {
	public:
		explicit PanelSysMsg(int type);
		~PanelSysMsg();

		void btnOKExit(MyGUI::Widget* _sender);

	private:
		MyGUI::Button* btnOK;
		MyGUI::TextBox* tbMsg;
	};

}

#endif// _PANEL_SYSMSG_H_
