#include "CGUICustomText.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include <IGUISkin.h>
#include <IGUIEnvironment.h>
#include <IGUIScrollBar.h>
#include <IGUIFont.h>
#include <IVideoDriver.h>
#include <rect.h>
#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
#include "../IrrlichtCommonIncludes1.9/os.h"
#else
#include "../IrrlichtCommonIncludes/os.h"
#endif
#include <algorithm>
#include <cmath>

namespace irr {
namespace gui {

//! constructor
CGUICustomText::CGUICustomText(const wchar_t* text, bool border, IGUIEnvironment* environment, IGUIElement* parent,
							   s32 id, const core::rect<s32>& rectangle, bool background)
	: IGUIStaticText(environment, parent, id, rectangle),
	HAlign(EGUIA_UPPERLEFT), VAlign(EGUIA_UPPERLEFT),
	Border(border), OverrideColorEnabled(false), OverrideBGColorEnabled(false), WordWrap(false), Background(background),
	RestrainTextInside(true), RightToLeft(false),
	OverrideColor(video::SColor(101, 255, 255, 255)), BGColor(video::SColor(101, 210, 210, 210)), animationWaitStart(0),
	OverrideFont(nullptr), LastBreakFont(nullptr), scrText(nullptr), prev_time(0), scrolling(NO_SCROLLING), maxFrame(0), curFrame(0.0f),
	frameTimer(0.0f), forcedSteps(0), forcedStepsRatio(0.0f), animationStep(0), animationWaitEnd(0), increasingFrame(false),
	waitingEndFrame(false), ScrollWidth(0), ScrollRatio(0.0f), TouchControl(false), was_pressed(false), prev_position(core::position2di(0, 0)) {
#ifdef _DEBUG
	setDebugName("CGUICustomText");
#endif

	Text = text;
	if(environment && environment->getSkin()) {
		BGColor = environment->getSkin()->getColor(gui::EGDC_3D_FACE);
	}
}

CGUICustomText* CGUICustomText::addCustomText(const wchar_t* text, bool border, IGUIEnvironment* environment,
											  IGUIElement* parent, s32 id, const core::rect<s32>& rectangle, bool background) {
	CGUICustomText* obj = new CGUICustomText(text, border, environment, parent, id, rectangle, background);
	obj->drop();
	return obj;
}


//! destructor
CGUICustomText::~CGUICustomText() {
	if(OverrideFont)
		OverrideFont->drop();
}

bool CGUICustomText::OnEvent(const SEvent & event) {
	if(isEnabled()) {
		switch(event.EventType) {
			case EET_MOUSE_INPUT_EVENT:	{
				switch(event.MouseInput.Event) {
					case EMIE_LMOUSE_PRESSED_DOWN: {
						if(!was_pressed) {
							was_pressed = true;
							prev_position = core::position2di(event.MouseInput.X, event.MouseInput.Y);
						}
						break;
					}
					case EMIE_LMOUSE_LEFT_UP: {
						was_pressed = false;
						prev_position = core::position2di(0, 0);
						break;
					}
					case EMIE_MOUSE_MOVED: {
						if(was_pressed) {
							if(scrText && scrText->isEnabled()) {
								auto diff = prev_position.Y - event.MouseInput.Y;
								prev_position = core::position2di(event.MouseInput.X, event.MouseInput.Y);
								scrText->setPos(scrText->getPos() + diff);
								return true;
							}
						}
						break;
					}
					case EMIE_MOUSE_WHEEL: {
						if(scrText && scrText->isEnabled()) {
							if(event.MouseInput.Wheel < 0) {
								scrText->setPos(scrText->getPos() + scrText->getSmallStep());
							} else {
								scrText->setPos(scrText->getPos() - scrText->getSmallStep());
							}
							return true;
						}
					}
				}
			}
		}
	}
	return IGUIElement::OnEvent(event);
}


//! draws the element and its children
void CGUICustomText::draw() {
	if(!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	if(!skin)
		return;
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> frameRect(AbsoluteRect);

	// draw background

	if(Background) {
		if(!OverrideBGColorEnabled)	// skin-colors can change
			BGColor = skin->getColor(gui::EGDC_3D_FACE);

		driver->draw2DRectangle(BGColor, frameRect, &AbsoluteClippingRect);
	}

	// draw the border

	if(Border) {
		skin->draw3DSunkenPane(this, 0, true, false, frameRect, &AbsoluteClippingRect);
		frameRect.UpperLeftCorner.X += skin->getSize(EGDS_TEXT_DISTANCE_X);
	}
	auto now = os::Timer::getTime();
	auto delta_time = now - prev_time;
	prev_time = now;
	// draw the text
	if(Text.size()) {
		IGUIFont* font = getActiveFont();

		if(font) {
			bool autoscrolling = scrolling != NO_SCROLLING && !!animationStep;
			if(frameTimer)
				frameTimer -= std::min((float)delta_time * 60.0f / 1000.0f, frameTimer);
			else {
				curFrame += (increasingFrame) ? (float)delta_time * 60.0f / 1000.0f : -(float)delta_time * 60.0f / 1000.0f;
				if(curFrame > maxFrame || curFrame < 0) {
					if(scrolling == LEFT_TO_RIGHT || scrolling == TOP_TO_BOTTOM) {
						waitingEndFrame = !waitingEndFrame;
						if(!waitingEndFrame)
							curFrame = 0;
						frameTimer = animationWaitEnd;
					} else if(scrolling == RIGHT_TO_LEFT || scrolling == BOTTOM_TO_TOP) {
						waitingEndFrame = !waitingEndFrame;
						if(!waitingEndFrame)
							curFrame = maxFrame;
						frameTimer = animationWaitEnd;
					} else if(scrolling == LEFT_TO_RIGHT_BOUNCING || scrolling == RIGHT_TO_LEFT_BOUNCING || scrolling == TOP_TO_BOTTOM_BOUNCING || scrolling == BOTTOM_TO_TOP_BOUNCING) {
						increasingFrame = !increasingFrame;
						if(increasingFrame) {
							curFrame += (float)delta_time * 60.0f / 1000.0f;
							frameTimer = animationWaitEnd;
						} else {
							curFrame -= (float)delta_time * 60.0f / 1000.0f;
							frameTimer = animationWaitStart;
						}
					}
				}
			}
			if(!WordWrap) {
				if(VAlign == EGUIA_LOWERRIGHT) {
					frameRect.UpperLeftCorner.Y = frameRect.LowerRightCorner.Y -
						font->getDimension(L"A").Height - font->getKerningHeight();
				}
				if(HAlign == EGUIA_LOWERRIGHT) {
					frameRect.UpperLeftCorner.X = frameRect.LowerRightCorner.X -
						font->getDimension(Text.c_str()).Width;
				}

				if(hasHorizontalAutoscrolling())
					frameRect.UpperLeftCorner.X -= round((float)curFrame * animationStep);
				if(hasVerticalAutoscrolling())
					frameRect.UpperLeftCorner.Y -= round((float)curFrame * animationStep);

				font->draw(Text.c_str(), frameRect,
						   OverrideColorEnabled ? OverrideColor : skin->getColor(isEnabled() ? EGDC_BUTTON_TEXT : EGDC_GRAY_TEXT),
						   HAlign == EGUIA_CENTER && !autoscrolling && hasHorizontalAutoscrolling(), VAlign == EGUIA_CENTER && !autoscrolling && hasVerticalAutoscrolling(), (RestrainTextInside ? &AbsoluteClippingRect : NULL));
			} else {
				if(font != LastBreakFont)
					breakText();

				int offset = (scrText && scrText->isEnabled()) ? scrText->getPos() : 0;

				core::rect<s32> r = frameRect;
				s32 height = font->getDimension(L"A").Height + font->getKerningHeight();
				s32 totalHeight = height * BrokenText.size();
				if(VAlign == EGUIA_CENTER) {
					r.UpperLeftCorner.Y = r.getCenter().Y - (totalHeight / 2);
				} else if(VAlign == EGUIA_LOWERRIGHT) {
					r.UpperLeftCorner.Y = r.LowerRightCorner.Y - totalHeight;
				}

				r.UpperLeftCorner.Y -= offset;

				if(hasHorizontalAutoscrolling())
					r.UpperLeftCorner.X -= round((float)curFrame * animationStep);
				if(hasVerticalAutoscrolling())
					r.UpperLeftCorner.Y -= round((float)curFrame * animationStep);

				u32 end = std::min(BrokenText.size(), (u32)(std::floor((AbsoluteClippingRect.getHeight() + offset) / height) + 1));
				u32 start = std::max(0, (s32)(end - std::floor(AbsoluteClippingRect.getHeight() / height) - 2));
				r.UpperLeftCorner.Y += height * start;
				for(u32 i = start; i < end; ++i, r.UpperLeftCorner.Y += height) {
					if(HAlign == EGUIA_LOWERRIGHT) {
						r.UpperLeftCorner.X = frameRect.LowerRightCorner.X -
							font->getDimension(BrokenText[i].c_str()).Width;
					}
					font->draw(BrokenText[i].c_str(), r,
							   OverrideColorEnabled ? OverrideColor : skin->getColor(isEnabled() ? EGDC_BUTTON_TEXT : EGDC_GRAY_TEXT),
							   HAlign == EGUIA_CENTER, false, (RestrainTextInside ? &AbsoluteClippingRect : NULL));
				}
			}
		}
	}

	IGUIElement::draw();
}


//! Sets another skin independent font.
void CGUICustomText::setOverrideFont(IGUIFont* font) {
	if(OverrideFont == font)
		return;

	if(OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if(OverrideFont)
		OverrideFont->grab();

	breakText();
}

//! Gets the override font (if any)
IGUIFont * CGUICustomText::getOverrideFont() const {
	return OverrideFont;
}

//! Get the font which is used right now for drawing
IGUIFont* CGUICustomText::getActiveFont() const {
	if(OverrideFont)
		return OverrideFont;
	IGUISkin* skin = Environment->getSkin();
	if(skin)
		return skin->getFont();
	return 0;
}

//! Sets another color for the text.
void CGUICustomText::setOverrideColor(video::SColor color) {
	OverrideColor = color;
	OverrideColorEnabled = true;
}


//! Sets another color for the text.
void CGUICustomText::setBackgroundColor(video::SColor color) {
	BGColor = color;
	OverrideBGColorEnabled = true;
	Background = true;
}


//! Sets whether to draw the background
void CGUICustomText::setDrawBackground(bool draw) {
	Background = draw;
}


//! Gets the background color
video::SColor CGUICustomText::getBackgroundColor() const {
	return BGColor;
}


//! Checks if background drawing is enabled
bool CGUICustomText::isDrawBackgroundEnabled() const {
	return Background;
}


//! Sets whether to draw the border
void CGUICustomText::setDrawBorder(bool draw) {
	Border = draw;
}


//! Checks if border drawing is enabled
bool CGUICustomText::isDrawBorderEnabled() const {
	return Border;
}


void CGUICustomText::setTextRestrainedInside(bool restrainTextInside) {
	RestrainTextInside = restrainTextInside;
}


bool CGUICustomText::isTextRestrainedInside() const {
	return RestrainTextInside;
}


void CGUICustomText::setTextAlignment(EGUI_ALIGNMENT horizontal, EGUI_ALIGNMENT vertical) {
	HAlign = horizontal;
	VAlign = vertical;
}


video::SColor CGUICustomText::getOverrideColor() const {
	return OverrideColor;
}


//! Sets if the static text should use the overide color or the
//! color in the gui skin.
void CGUICustomText::enableOverrideColor(bool enable) {
	OverrideColorEnabled = enable;
}


bool CGUICustomText::isOverrideColorEnabled() const {
	return OverrideColorEnabled;
}


//! Enables or disables word wrap for using the static text as
//! multiline text control.
void CGUICustomText::setWordWrap(bool enable) {
	WordWrap = enable;
	breakText();
	updateAutoScrollingStuff();
}


bool CGUICustomText::isWordWrapEnabled() const {
	return WordWrap;
}


void CGUICustomText::setRightToLeft(bool rtl) {
	if(RightToLeft != rtl) {
		RightToLeft = rtl;
		breakText();
	}
}


bool CGUICustomText::isRightToLeft() const {
	return RightToLeft;
}

void CGUICustomText::breakText() {
	breakText(false);
	if(scrText) {
		scrText->setVisible(false);
		scrText->setEnabled(false);
		if(getTextHeight() > RelativeRect.getHeight()) {
			scrText->setEnabled(true);
			if(!TouchControl) {
				scrText->setVisible(true);
				breakText(true);
			}
			scrText->setMin(0);
			scrText->setMax((getTextHeight() - RelativeRect.getHeight()));
			scrText->setPos(0);
			int step = getActiveFont()->getDimension(L"A").Height + getActiveFont()->getKerningHeight();
			scrText->setSmallStep(step);
			scrText->setLargeStep(step);
		}
	}
}

//! Breaks the single text line.
void CGUICustomText::breakText(bool scrollbar_spacing) {
	if(!WordWrap)
		return;

	if(scrollbar_spacing && !scrText)
		return;

	BrokenText.clear();

	IGUISkin* skin = Environment->getSkin();
	IGUIFont* font = getActiveFont();
	if(!font)
		return;

	LastBreakFont = font;

	core::stringw line;
	core::stringw word;
	core::stringw whitespace;
	s32 size = Text.size();
	s32 length = 0;
	s32 elWidth = RelativeRect.getWidth();
	if(Border)
		elWidth -= 2 * skin->getSize(EGDS_TEXT_DISTANCE_X);
	if(scrollbar_spacing)
		elWidth -= scrText->getRelativePosition().getWidth();
	elWidth = std::max(elWidth, 0);
	if(elWidth < font->getDimension(L"A").Width)
		return;
	wchar_t c;

	// We have to deal with right-to-left and left-to-right differently
	// However, most parts of the following code is the same, it's just
	// some order and boundaries which change.
	if(!RightToLeft) {
		// regular (left-to-right)
		for(s32 i = 0; i < size; ++i) {
			c = Text[i];
			bool lineBreak = false;

			if(c == L'\r') // Mac or Windows breaks
			{
				lineBreak = true;
				if(Text[i + 1] == L'\n') // Windows breaks
				{
					Text.erase(i + 1);
					--size;
				}
				c = '\0';
			} else if(c == L'\n') // Unix breaks
			{
				lineBreak = true;
				c = '\0';
			}

			bool isWhitespace = (c == L' ' || c == 0);
			if(!isWhitespace) {
				// part of a word
				word += c;
			}

			if(isWhitespace || i == (size - 1)) {
				if(word.size()) {
					// here comes the next whitespace, look if
					// we must break the last word to the next line.
					const s32 whitelgth = font->getDimension(whitespace.c_str()).Width;
					s32 wordlgth = font->getDimension(word.c_str()).Width;

					if(wordlgth > elWidth) {
						// This word is too long to fit in the available space, look for
						// the Unicode Soft HYphen (SHY / 00AD) character for a place to
						// break the word at
						int where = word.findFirst(wchar_t(0x00AD));
						if(where != -1) {
							core::stringw first = word.subString(0, where);
							core::stringw second = word.subString(where, word.size() - where);
							BrokenText.push_back(line + first + L"-");
							const s32 secondLength = font->getDimension(second.c_str()).Width;

							length = secondLength;
							line = second;
						} else {
							// No soft hyphen found, so there's nothing more we can do
							// break line in pieces
							core::stringw second;
							s32 secondLength;
							while(wordlgth > elWidth) {
								int j = (word.size() > 1) ? 1 : 0;
								for(; j < word.size() - 1; j++) {
									if(font->getDimension((line + whitespace + word.subString(0, j + 1)).c_str()).Width > elWidth)
										break;
								}
								//if not enough space for the word just give up and dont' try to calculate the broken lines
								if(j == 0) {
									BrokenText.push_back(line);
									length = wordlgth;
									line = word;
									break;
								}
								core::stringw first = word.subString(0, j);
								second = word.subString(j, word.size() - j);
								BrokenText.push_back(line + whitespace + first);
								secondLength = font->getDimension(second.c_str()).Width;
								word = second;
								wordlgth = secondLength;
								whitespace = L"";
								line = L"";
							}
							line = second;
							length = secondLength;
						}
					} else if(length && (length + wordlgth + whitelgth > elWidth)) {
						// break to next line
						BrokenText.push_back(line);
						length = wordlgth;
						line = word;
					} else {
						// add word to line
						line += whitespace;
						line += word;
						length += whitelgth + wordlgth;
					}

					word = L"";
					whitespace = L"";
				}

				if(isWhitespace) {
					whitespace += c;
				}

				// compute line break
				if(lineBreak) {
					line += whitespace;
					line += word;
					BrokenText.push_back(line);
					line = L"";
					word = L"";
					whitespace = L"";
					length = 0;
				}
			}
		}

		line += whitespace;
		line += word;
		BrokenText.push_back(line);
	} else {
		// right-to-left
		for(s32 i = size; i >= 0; --i) {
			c = Text[i];
			bool lineBreak = false;

			if(c == L'\r') // Mac or Windows breaks
			{
				lineBreak = true;
				if((i > 0) && Text[i - 1] == L'\n') // Windows breaks
				{
					Text.erase(i - 1);
					--size;
				}
				c = '\0';
			} else if(c == L'\n') // Unix breaks
			{
				lineBreak = true;
				c = '\0';
			}

			if(c == L' ' || c == 0 || i == 0) {
				if(word.size()) {
					// here comes the next whitespace, look if
					// we must break the last word to the next line.
					const s32 whitelgth = font->getDimension(whitespace.c_str()).Width;
					const s32 wordlgth = font->getDimension(word.c_str()).Width;

					if(length && (length + wordlgth + whitelgth > elWidth)) {
						// break to next line
						BrokenText.push_back(line);
						length = wordlgth;
						line = word;
					} else {
						// add word to line
						line = whitespace + line;
						line = word + line;
						length += whitelgth + wordlgth;
					}

					word = L"";
					whitespace = L"";
				}

				if(c != 0)
					whitespace = core::stringw(&c, 1) + whitespace;

				// compute line break
				if(lineBreak) {
					line = whitespace + line;
					line = word + line;
					BrokenText.push_back(line);
					line = L"";
					word = L"";
					whitespace = L"";
					length = 0;
				}
			} else {
				// yippee this is a word..
				word = core::stringw(&c, 1) + word;
			}
		}

		line = whitespace + line;
		line = word + line;
		BrokenText.push_back(line);
	}
}

void CGUICustomText::updateAutoScrollingStuff() {
	if(Text.empty())
		return;
	waitingEndFrame = false;
	frameTimer = animationWaitStart;
	increasingFrame = (scrolling == LEFT_TO_RIGHT || scrolling == TOP_TO_BOTTOM || scrolling == LEFT_TO_RIGHT_BOUNCING || scrolling == TOP_TO_BOTTOM_BOUNCING);
	core::rect<s32> frameRect(AbsoluteRect);
	if(Border) {
		int size = Environment->getSkin()->getSize(EGDS_TEXT_DISTANCE_X);
		frameRect.UpperLeftCorner.X += size;
		frameRect.LowerRightCorner.X -= size;
	}
	animationStep = 0;
	int offset = 0;
	if(hasHorizontalAutoscrolling())
		offset = getActiveFont()->getDimension(Text.c_str()).Width - frameRect.getWidth();
	if(hasVerticalAutoscrolling())
		offset = getTextHeight() - frameRect.getHeight();
	if(forcedSteps) {
		if(offset > 0) {
			animationStep = forcedSteps;
			maxFrame = ceil((float)offset / (float)animationStep);
		}
	} else if(forcedStepsRatio) {
		float steps = (float)offset / forcedStepsRatio;
		maxFrame = ceil(steps);
		animationStep = (offset <= 0) ? 0 : (float)offset / (float)maxFrame;
	} else {
		animationStep = (offset <= 0) ? 0 : (float)offset / (float)maxFrame;
	}
	curFrame = increasingFrame ? 0 : maxFrame;
}

//! Sets the new caption of this element.
void CGUICustomText::setText(const wchar_t* text) {
	IGUIElement::setText(text);
	breakText();
	updateAutoScrollingStuff();
}


void CGUICustomText::updateAbsolutePosition() {
	auto prev_rect = RelativeRect;
	IGUIElement::updateAbsolutePosition();
	if(scrText) {
		int width = RelativeRect.getWidth();
		int width2 = 0;
		if(ScrollWidth)
			width2 = ScrollWidth;
		else if(ScrollRatio)
			width2 = width - round(width*ScrollRatio);
		else
			width2 = width - Environment->getSkin()->getSize(EGDS_SCROLLBAR_SIZE);
		scrText->setRelativePosition(irr::core::rect<s32>(width2, 0, width, RelativeRect.getHeight()));
	}
	if(prev_rect.getHeight() != RelativeRect.getHeight() || prev_rect.getWidth() != RelativeRect.getWidth()) {
		breakText();
		updateAutoScrollingStuff();
	}
}


//! Returns the height of the text in pixels when it is drawn.
s32 CGUICustomText::getTextHeight() const {
	IGUIFont* font = getActiveFont();
	if(!font)
		return 0;

	s32 height = font->getDimension(L"A").Height + font->getKerningHeight();

	if(WordWrap)
		height *= BrokenText.size();

	return height;
}


s32 CGUICustomText::getTextWidth() const {
	IGUIFont * font = getActiveFont();
	if(!font)
		return 0;

	if(WordWrap) {
		s32 widest = 0;

		for(u32 line = 0; line < BrokenText.size(); ++line) {
			s32 width = font->getDimension(BrokenText[line].c_str()).Width;

			if(width > widest)
				widest = width;
		}

		return widest;
	} else {
		return font->getDimension(Text.c_str()).Width;
	}
}


//! Writes attributes of the element.
//! Implement this to expose the attributes of your element for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CGUICustomText::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options = 0) const {
	IGUIStaticText::serializeAttributes(out, options);

	out->addBool("Border", Border);
	out->addBool("OverrideColorEnabled", OverrideColorEnabled);
	out->addBool("OverrideBGColorEnabled", OverrideBGColorEnabled);
	out->addBool("WordWrap", WordWrap);
	out->addBool("Background", Background);
	out->addBool("RightToLeft", RightToLeft);
	out->addBool("RestrainTextInside", RestrainTextInside);
	out->addColor("OverrideColor", OverrideColor);
	out->addColor("BGColor", BGColor);
	out->addEnum("HTextAlign", HAlign, GUIAlignmentNames);
	out->addEnum("VTextAlign", VAlign, GUIAlignmentNames);

	// out->addFont ("OverrideFont",	OverrideFont);
}


//! Reads attributes of the element
void CGUICustomText::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options = 0) {
	IGUIStaticText::deserializeAttributes(in, options);

	Border = in->getAttributeAsBool("Border");
	enableOverrideColor(in->getAttributeAsBool("OverrideColorEnabled"));
	OverrideBGColorEnabled = in->getAttributeAsBool("OverrideBGColorEnabled");
	setWordWrap(in->getAttributeAsBool("WordWrap"));
	Background = in->getAttributeAsBool("Background");
	RightToLeft = in->getAttributeAsBool("RightToLeft");
	RestrainTextInside = in->getAttributeAsBool("RestrainTextInside");
	OverrideColor = in->getAttributeAsColor("OverrideColor");
	BGColor = in->getAttributeAsColor("BGColor");

	setTextAlignment((EGUI_ALIGNMENT)in->getAttributeAsEnumeration("HTextAlign", GUIAlignmentNames),
		(EGUI_ALIGNMENT)in->getAttributeAsEnumeration("VTextAlign", GUIAlignmentNames));

	// OverrideFont = in->getAttributeAsFont("OverrideFont");
}

void CGUICustomText::enableScrollBar(int scroll_width, float scroll_ratio) {
	if(scrText)
		return;
	core::rect<s32> frameRect(AbsoluteRect);
	core::rect<s32> r = frameRect;
	ScrollWidth = scroll_width;
	ScrollRatio = scroll_ratio;
	int width = RelativeRect.getWidth();
	int width2 = 0;
	if(ScrollWidth)
		width2 = ScrollWidth;
	else if(ScrollRatio)
		width2 = width - round(width*ScrollRatio);
	else
		width2 = width - round(width*0.1);
	scrText = Environment->addScrollBar(false, irr::core::rect<s32>(width2, 0, width, RelativeRect.getHeight()), this, -1);
	scrText->setSmallStep(1);
	scrText->setLargeStep(1);
}

irr::gui::IGUIScrollBar * CGUICustomText::getScrollBar() {
	return scrText;
}

bool CGUICustomText::hasScrollBar() {
	return !!scrText;
}

void CGUICustomText::setTextAutoScrolling(CTEXT_SCROLLING_TYPE type, int frames, float steps_ratio, int steps, int waitstart, int waitend) {
	scrolling = type;
	maxFrame = frames;
	animationWaitStart = waitstart;
	animationWaitEnd = waitend;
	forcedStepsRatio = steps_ratio;
	forcedSteps = steps;
	updateAutoScrollingStuff();
}

bool CGUICustomText::hasVerticalAutoscrolling() const {
	return scrolling == TOP_TO_BOTTOM || scrolling == BOTTOM_TO_TOP || scrolling == TOP_TO_BOTTOM_BOUNCING || scrolling == BOTTOM_TO_TOP_BOUNCING;
}

bool CGUICustomText::hasHorizontalAutoscrolling() const {
	return scrolling == LEFT_TO_RIGHT || scrolling == RIGHT_TO_LEFT || scrolling == LEFT_TO_RIGHT_BOUNCING || scrolling == RIGHT_TO_LEFT_BOUNCING;
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

