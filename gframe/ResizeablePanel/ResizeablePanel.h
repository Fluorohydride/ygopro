#ifndef __RESIZEABLE_PANEL_H_INCLUDED__
#define __RESIZEABLE_PANEL_H_INCLUDED__

#include <IrrCompileConfig.h>
#ifdef _IRR_COMPILE_WITH_GUI_

#include <IGUIElement.h>

namespace irr {
namespace gui {
class IGUIScrollBar;
class Panel : public IGUIElement {
public:
	Panel(IGUIEnvironment* env, const core::rect<s32>& rectangle, s32 id = -1, IGUIElement * parent = nullptr, bool vertical = true, bool horizontal = true);
	static Panel* addPanel(IGUIEnvironment* environment, IGUIElement* parent, s32 id, const core::rect<s32>& rectangle, bool vertical = true, bool horizontal = true);
	IGUIElement* getSubpanel();
	//virtual void draw();
	virtual bool OnEvent(const SEvent& event);
	virtual void updateAbsolutePosition();
private:
	bool hasVerticalScrolling;
	bool hasHorizontalScrolling;
	irr::core::recti getSubPaneRect();
	void doResizingStuff();
	irr::gui::IGUIScrollBar* verticalScroll;
	irr::gui::IGUIScrollBar* horizontalScroll;
	irr::gui::IGUIElement* subpanel;
};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif

