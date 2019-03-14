#include "CProgressBar.h"

IProgressBar::IProgressBar(IGUIEnvironment * guienv, const core::rect<s32>& rectangle, s32 id, IGUIElement * parent) : IGUIElement(EGUIET_ELEMENT, guienv, parent, id, rectangle) {
	//total = rectangle.LowerRightCorner.X - rectangle.UpperLeftCorner.X;
	bar = rectangle;
	fillcolor.set(255, 255, 255, 255);
	emptycolor.set(255, 0, 0, 0);
	border = bar;
	progress = 0;
	resize_ratio = vector2d<float>(0, 0);
}
void IProgressBar::setColors(irr::video::SColor progress, irr::video::SColor filling) {
	fillcolor = progress;
	emptycolor = filling;
}
void IProgressBar::addBorder(irr::s32 size, irr::video::SColor color) {
	bordercolor = color;
	border = bar;
	border.UpperLeftCorner.X -= size;
	border.UpperLeftCorner.Y -= size;
	border.LowerRightCorner.X += size;
	border.LowerRightCorner.Y += size;
}
void IProgressBar::setProgress(irr::s32 _progress) {
	if(_progress > 100)
		progress = 100;
	else if(_progress < 0)
		progress = 0;
	else
		progress = _progress;
}
void IProgressBar::draw() {
	if(!IsVisible)
		return;
	IGUISkin* skin = Environment->getSkin();
	updateResizeRatio();
	updateProgress();
	skin->draw2DRectangle(this, bordercolor, getAbsoluteRect(border, AbsoluteRect));
	skin->draw2DRectangle(this, fillcolor, getAbsoluteRect(tofill, AbsoluteRect));
	skin->draw2DRectangle(this, emptycolor, getAbsoluteRect(empty, AbsoluteRect));
}
void IProgressBar::updateProgress() {
	u32 xpercentage;
	xpercentage = (progress * bar.getWidth()) / (100); //Reducing to the bar size
	tofill.UpperLeftCorner.set(bar.UpperLeftCorner.X, bar.UpperLeftCorner.Y);
	tofill.LowerRightCorner.set(bar.UpperLeftCorner.X + xpercentage, bar.LowerRightCorner.Y);
	empty.UpperLeftCorner.set(tofill.LowerRightCorner.X, tofill.UpperLeftCorner.Y);
	empty.LowerRightCorner.set(bar.LowerRightCorner.X, bar.LowerRightCorner.Y);
}
core::rect<s32> IProgressBar::getAbsoluteRect(const core::rect<s32>& relative, const core::rect<s32>& absolute) {
	core::rect<s32> r = absolute;
	//r.UpperLeftCorner += relative.UpperLeftCorner;
	r.UpperLeftCorner.X += relative.UpperLeftCorner.X / resize_ratio.X;
	r.LowerRightCorner.Y = (int)((r.UpperLeftCorner.Y + relative.getHeight()) / resize_ratio.Y);
	r.LowerRightCorner.X = (int)((relative.UpperLeftCorner.X + relative.getWidth()) / resize_ratio.X);
	return r;
}
void IProgressBar::updateResizeRatio() {
	resize_ratio.X = (float)bar.getWidth() / (float)AbsoluteRect.getWidth();
	resize_ratio.Y = (float)bar.getHeight() / (float)AbsoluteRect.getHeight();
}
