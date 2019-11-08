#ifndef IPROGRESSBAR_H_
#define IPROGRESSBAR_H_
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
class IProgressBar : public IGUIElement {
public:

	IProgressBar(IGUIEnvironment * guienv, const core::rect<s32>& rectangle, s32 id = -1, IGUIElement * parent = 0);


	/*Set percentage in positive percentual (0~100). Please note that a call to this function with others values, will set the progress bar to 0.*/
	void setProgress(irr::s32 progress);

	/*Set bar Colors*/
	void setColors(irr::video::SColor progress = irr::video::SColor(255, 255, 255, 255), irr::video::SColor filling = irr::video::SColor(255, 0, 0, 0));

	/*Allow you to add a "border" into your bar. You MUST specify the size (of course in pixel) of the border. You can also pass a color parameter (Black by default)*/
	void addBorder(irr::s32 size, irr::video::SColor color = irr::video::SColor(255, 0, 0, 0));


	virtual void draw();
private:
	//irr::s32 total; //Dimension (X) of the bar, to calculate relative percentage.
	rect<s32> bar; //Dimension of the bar
	rect<s32> position; //Bar
	rect<s32> border; //Border 
	rect<s32> tofill; //Percentage indicator
	rect<s32> empty; //"Empty" indicator

	int progress;
	vector2d<float> resize_ratio;

	irr::video::SColor fillcolor;
	irr::video::SColor emptycolor;
	irr::video::SColor bordercolor;

	void updateProgress();

	core::rect<s32> getAbsoluteRect(const core::rect<s32>& relative, const core::rect<s32>& absolute);

	void updateResizeRatio();

};
#endif

