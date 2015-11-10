#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

//special cards
#define CARD_MARINE_DOLPHIN	78734254
#define CARD_TWINKLE_MOSS 	13857930

#include "config.h"
#include "game.h"
#include "client_card.h"

namespace ygo {

class EventHandler : public irr::IEventReceiver {
public:
	virtual bool OnEvent(const irr::SEvent& event);
	void GetHoverField(int x, int y);
	void ShowMenu(int flag, int x, int y);
	
	irr::gui::IGUIElement* panel;
	int hovered_controler;
	int hovered_location;
	int hovered_sequence;
	ClientCard* hovered_card;
	ClientCard* clicked_card;
	wchar_t formatBuffer[256];
};

}

#endif //EVENT_HANDLER_H
