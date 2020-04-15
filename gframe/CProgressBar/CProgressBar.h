#ifndef IPROGRESSBAR_H_
#define IPROGRESSBAR_H_
#include <irrlicht.h>

class IProgressBar : public irr::gui::IGUIElement {
public:

	IProgressBar(irr::gui::IGUIEnvironment* guienv, const irr::core::recti& rectangle, irr::s32 id = -1, irr::gui::IGUIElement* parent = 0);


	/*Set percentage in positive percentual (0~100). Please note that a call to this function with others values, will set the progress bar to 0.*/
	void setProgress(irr::s32 progress);

	/*Set bar Colors*/
	void setColors(irr::video::SColor progress = irr::video::SColor(255, 255, 255, 255), irr::video::SColor filling = irr::video::SColor(255, 0, 0, 0));

	/*Allow you to add a "border" into your bar. You MUST specify the size (of course in pixel) of the border. You can also pass a color parameter (Black by default)*/
	void addBorder(irr::s32 size, irr::video::SColor color = irr::video::SColor(255, 0, 0, 0));


	virtual void draw();
private:
	//irr::s32 total; //Dimension (X) of the bar, to calculate relative percentage.
	irr::core::recti bar; //Dimension of the bar
	irr::core::recti position; //Bar
	irr::core::recti border; //Border 
	irr::core::recti tofill; //Percentage indicator
	irr::core::recti empty; //"Empty" indicator

	int progress;
	irr::core::vector2df resize_ratio;

	irr::video::SColor fillcolor;
	irr::video::SColor emptycolor;
	irr::video::SColor bordercolor;

	irr::gui::IGUIStaticText* progressText;

	void updateProgress();

	irr::core::recti getAbsoluteRect(const irr::core::recti& relative, const irr::core::recti& absolute);

	void updateResizeRatio();

};
#endif

