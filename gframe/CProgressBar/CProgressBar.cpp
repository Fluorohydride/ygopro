#include "CProgressBar.h"
#include <IGUIEnvironment.h>
#include <IGUIScrollBar.h>
#include <IGUIStaticText.h>
#include <string>

IProgressBar::IProgressBar(irr::gui::IGUIEnvironment* guienv, const irr::core::recti& rectangle, irr::s32 id, irr::gui::IGUIElement* parent) :
	irr::gui::IGUIElement(irr::gui::EGUIET_ELEMENT, guienv, parent, id, rectangle) {
	//total = rectangle.LowerRightCorner.X - rectangle.UpperLeftCorner.X;
	bar = rectangle;
	progressText = guienv->addStaticText(L"0%", { 0,0,rectangle.getWidth(),rectangle.getHeight() }, false, false, this);
	progressText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	progressText->setOverrideColor({ 255,0,0,0 });
	fillcolor.set(255, 255, 255, 255);
	emptycolor.set(255, 0, 0, 0);
	border = bar;
	progress = 0;
	resize_ratio = irr::core::vector2df(0, 0);
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
	progressText->setText((std::to_wstring(progress) + L"%").c_str());
	if(progress<50)
		progressText->setOverrideColor({ 255,255,255,255 });
	else
		progressText->setOverrideColor({ 255,0,0,0 });
}
void IProgressBar::draw() {
	if(!IsVisible)
		return;
	auto skin = Environment->getSkin();
	updateResizeRatio();
	updateProgress();
	skin->draw2DRectangle(this, bordercolor, getAbsoluteRect(border, AbsoluteRect));
	skin->draw2DRectangle(this, fillcolor, getAbsoluteRect(tofill, AbsoluteRect));
	skin->draw2DRectangle(this, emptycolor, getAbsoluteRect(empty, AbsoluteRect));
	progressText->setRelativePosition({ 0,0,AbsoluteRect.getWidth(),AbsoluteRect.getHeight() });
	IGUIElement::draw();
}
void IProgressBar::updateProgress() {
	irr::u32 xpercentage = (progress * bar.getWidth()) / (100); //Reducing to the bar size
	tofill.UpperLeftCorner.set(bar.UpperLeftCorner.X, bar.UpperLeftCorner.Y);
	tofill.LowerRightCorner.set(bar.UpperLeftCorner.X + xpercentage, bar.LowerRightCorner.Y);
	empty.UpperLeftCorner.set(tofill.LowerRightCorner.X, tofill.UpperLeftCorner.Y);
	empty.LowerRightCorner.set(bar.LowerRightCorner.X, bar.LowerRightCorner.Y);
}
irr::core::recti IProgressBar::getAbsoluteRect(const irr::core::recti& relative, const irr::core::recti& absolute) {
	irr::core::recti r = absolute;
	//r.UpperLeftCorner += relative.UpperLeftCorner;
	//r.UpperLeftCorner.X += relative.UpperLeftCorner.X / resize_ratio.X;
	r.LowerRightCorner.Y = (int)((r.UpperLeftCorner.Y + relative.getHeight()) / resize_ratio.Y);
	r.LowerRightCorner.X = (int)((r.UpperLeftCorner.X + relative.getWidth()) / resize_ratio.X);
	r.UpperLeftCorner.X += relative.UpperLeftCorner.X / resize_ratio.X;
	return r;
}
void IProgressBar::updateResizeRatio() {
	resize_ratio.X = (float)bar.getWidth() / (float)AbsoluteRect.getWidth();
	resize_ratio.Y = (float)bar.getHeight() / (float)AbsoluteRect.getHeight();
}
