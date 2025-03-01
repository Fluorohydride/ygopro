#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include "config.h"

namespace ygo {

class MenuHandler: public irr::IEventReceiver {
public:
	bool OnEvent(const irr::SEvent& event) override;
	s32 prev_operation{ 0 };
	int prev_sel{ -1 };

};

}

#endif //MENU_HANDLER_H
