#include "ResizeablePanel.h"
#ifdef _IRR_COMPILE_WITH_GUI_
#include <algorithm>
#include <IGUIEnvironment.h>
#include <IGUIScrollBar.h>

namespace irr {
namespace gui {

class SubPanel : public IGUIElement {
public:
	SubPanel(IGUIEnvironment* env, IGUIElement* parent, s32 id, const core::rect<s32>& rectangle) : IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle) {};
	virtual bool isPointInside(const core::position2d<s32>& point) const { return false; };
};

Panel::Panel(IGUIEnvironment* env, const core::rect<s32>& rectangle, s32 id, IGUIElement* parent, bool vertical, bool horizontal) : IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
hasHorizontalScrolling(horizontal), hasVerticalScrolling(vertical) {
	verticalScroll = Environment->addScrollBar(false, { 0, 0, 1, 1 }, this, -1);
	verticalScroll->setVisible(false);
	horizontalScroll = Environment->addScrollBar(true, { 0, 0, 1, 1 }, this, -1);
	horizontalScroll->setVisible(false);
	subpanel = new SubPanel(env, this, id, { 0,0,AbsoluteRect.getWidth(),AbsoluteRect.getHeight() });
	subpanel->drop();
}
Panel* Panel::addPanel(IGUIEnvironment* environment, IGUIElement* parent, s32 id, const core::rect<s32>& rectangle, bool vertical, bool horizontal) {
	if(!parent)
		parent = environment->getRootGUIElement();
	Panel* c = new Panel(environment, rectangle, id, parent, vertical, horizontal);
	c->drop();
	return c;
}
IGUIElement * Panel::getSubpanel() {
	return subpanel;
}
/*void Panel::draw() {
	if(!IsVisible)
		return;
	//setRelativePosition(irr::core::rect<s32>(0, 0, Parent->getRelativePosition().getWidth(), Parent->getRelativePosition().getHeight()));
	IGUIElement::draw();
}*/
bool Panel::OnEvent(const SEvent& event) {
	switch(event.EventType) {
		case irr::EET_GUI_EVENT: {
			switch(event.GUIEvent.EventType) {
				case irr::gui::EGET_SCROLL_BAR_CHANGED:	{
					if(event.GUIEvent.Caller == verticalScroll || event.GUIEvent.Caller == horizontalScroll) {
						auto rect = getSubPaneRect();
						auto horizontal_size = horizontalScroll->isVisible() ? horizontalScroll->getPos() : 0;
						auto vertical_size = verticalScroll->isVisible() ? verticalScroll->getPos() : 0;
						rect.UpperLeftCorner -= irr::core::vector2di(horizontal_size, vertical_size);
						subpanel->setRelativePosition(rect);
					}
				}
				default:
					break;
			}
		}
		default:
			break;
	}
	return IGUIElement::OnEvent(event);
}
void Panel::updateAbsolutePosition() {
	IGUIElement::updateAbsolutePosition();
	doResizingStuff();
}
irr::core::recti Panel::getSubPaneRect() {
	core::rect<s32> rect = { 0,0,AbsoluteRect.getWidth(),AbsoluteRect.getHeight() };
	if(horizontalScroll->isVisible()) {
		rect.LowerRightCorner.Y -= Environment->getSkin()->getSize(EGDS_SCROLLBAR_SIZE);
	}
	if(verticalScroll->isVisible()) {
		rect.LowerRightCorner.X -= (Environment->getSkin()->getSize(EGDS_SCROLLBAR_SIZE) + 10);
	}
	return rect;
}
void Panel::doResizingStuff() {
	auto scrollsize = Environment->getSkin()->getSize(EGDS_SCROLLBAR_SIZE);
	if(hasVerticalScrolling || hasHorizontalScrolling) {
		horizontalScroll->setVisible(false);
		verticalScroll->setVisible(false);
		const auto& rect = AbsoluteClippingRect;
		const auto& absRect = AbsoluteRect;
		s32 maxX = rect.LowerRightCorner.X;
		s32 maxY = rect.LowerRightCorner.Y;
		for(auto& child : subpanel->getChildren()) {
			maxX = std::max(maxX, child->getAbsolutePosition().LowerRightCorner.X);
			maxY = std::max(maxY, child->getAbsolutePosition().LowerRightCorner.Y);
		}
		if(hasHorizontalScrolling && maxX > rect.LowerRightCorner.X) {
			horizontalScroll->setRelativePosition(irr::core::rect<s32>(5, absRect.getHeight() - (5 + scrollsize), rect.getWidth() - 5, absRect.getHeight() - 5));
			horizontalScroll->setVisible(true);
			horizontalScroll->setMax(maxX - rect.LowerRightCorner.X);
			horizontalScroll->setPos(0);
		}
		if(hasVerticalScrolling && maxY > rect.LowerRightCorner.Y) {
			verticalScroll->setRelativePosition(irr::core::rect<s32>(absRect.getWidth() - (5 + scrollsize), 10, absRect.getWidth() - 5, rect.getHeight() - 25));
			verticalScroll->setVisible(true);
			verticalScroll->setMax(maxY - rect.LowerRightCorner.Y);
			verticalScroll->setPos(0);
		}
		subpanel->setRelativePosition(getSubPaneRect());
	} else
		subpanel->setRelativePosition({ 0,0,AbsoluteRect.getWidth(),AbsoluteRect.getHeight() });
}
} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

