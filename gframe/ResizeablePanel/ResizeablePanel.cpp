#include "ResizeablePanel.h"
#ifdef _IRR_COMPILE_WITH_GUI_
#include <algorithm>

namespace irr {
namespace gui {
	Panel::Panel(IGUIEnvironment* env, const core::rect<s32>& rectangle, s32 id, IGUIElement* parent, bool vertical, bool horizontal) : IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle),
		hasHorizontalScrolling(horizontal), hasVerticalScrolling(vertical)  {
		verticalScroll = Environment->addScrollBar(false, { 0, 0, 1, 1 }, this, -1);
		verticalScroll->setVisible(false);
		horizontalScroll = Environment->addScrollBar(true, { 0, 0, 1, 1 }, this, -1);
		horizontalScroll->setVisible(false);
		subpanel = new IGUIElement(EGUIET_ELEMENT, env, parent, id, rectangle);
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
					case irr::gui::EGET_SCROLL_BAR_CHANGED: {
						if(event.GUIEvent.Caller == verticalScroll || event.GUIEvent.Caller == horizontalScroll) {
							auto rect = getSubPaneRect();
							auto horizontal_size = horizontalScroll->isVisible() ? horizontalScroll->getPos() : 0;
							auto vertical_size = verticalScroll->isVisible() ? verticalScroll->getPos() : 0;
							subpanel->setRelativePosition({ -horizontal_size, -vertical_size, rect.getWidth() + horizontal_size, rect.getHeight() + vertical_size });
						}
					}
					default:
						break;
				}
			}
			default:
				break;
		}
		return false;
	}
	void Panel::updateAbsolutePosition() {
		IGUIElement::updateAbsolutePosition();
		doResizingStuff();
	}
	irr::core::recti Panel::getSubPaneRect() {
		auto width = AbsoluteClippingRect.getWidth();
		auto height = AbsoluteClippingRect.getHeight();
		if(horizontalScroll->isVisible()) {
			height -= 20;
			verticalScroll->setMax(verticalScroll->getMax() + 20);
		}
		if(verticalScroll->isVisible()) {
			width -= 30;
			horizontalScroll->setMax(horizontalScroll->getMax() + 30);
		}
		return { 0, 0, width, height };
	}
	void Panel::doResizingStuff() {
		auto scrollsize = Environment->getSkin()->getSize(EGDS_SCROLLBAR_SIZE);
		subpanel->setRelativePosition({ 0, 0, AbsoluteClippingRect.getWidth(), AbsoluteClippingRect.getHeight() });
		if(hasVerticalScrolling || hasHorizontalScrolling) {
			horizontalScroll->setVisible(false);
			verticalScroll->setVisible(false);
			auto& rect = AbsoluteClippingRect;
			s32 maxX = rect.LowerRightCorner.X;
			s32 maxY = rect.LowerRightCorner.Y;
			for(auto& child : subpanel->getChildren()) {
				maxX = std::max(maxX, child->getAbsolutePosition().LowerRightCorner.X);
				maxY = std::max(maxY, child->getAbsolutePosition().LowerRightCorner.Y);
			}
			if(hasHorizontalScrolling && maxX > rect.LowerRightCorner.X) {
				horizontalScroll->setRelativePosition(irr::core::rect<s32>(5, rect.getHeight() - (5 + scrollsize), rect.getWidth() - 5, rect.getHeight() - 5));
				horizontalScroll->setVisible(true);
				horizontalScroll->setMax(maxX - rect.LowerRightCorner.X);
				horizontalScroll->setPos(0);
			}
			if(hasVerticalScrolling && maxY > rect.LowerRightCorner.Y) {
				verticalScroll->setRelativePosition(irr::core::rect<s32>(rect.getWidth() - (5 + scrollsize), 10, rect.getWidth() - 5, rect.getHeight() - 25));
				verticalScroll->setVisible(true);
				verticalScroll->setMax(maxY - rect.LowerRightCorner.Y);
				verticalScroll->setPos(0);
			}
			subpanel->setRelativePosition(getSubPaneRect());
		}
	}
} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

