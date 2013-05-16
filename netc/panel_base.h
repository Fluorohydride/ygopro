#ifndef _PANEL_BASE_H_
#define _PANEL_BASE_H_

#include "../common/common.h"
#include "MyGUI.h"
#include "MyGUI_OpenGLPlatform.h"

namespace ygopro
{

	class PanelBase {
	public:
		PanelBase(const std::string& layoutname);
		virtual ~PanelBase();

		void fadeShow(float speed = 5.0);
		void fadeDestroy(float speed = 5.0);

	protected:

		void _eventDestroy(MyGUI::Widget* _sender) {
			delete this;
		}

		MyGUI::VectorWidgetPtr _widgetsLoaded;
		MyGUI::Widget* _mainWidget;
	};

}
#endif //_PANEL_BASE_H_
