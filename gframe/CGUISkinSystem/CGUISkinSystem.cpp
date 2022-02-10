#include "CGUISkinSystem.h"
#include <IrrlichtDevice.h>
#include <IFileSystem.h>
#include <IrrlichtDevice.h>
#include <IGUIEnvironment.h>
#include "../CXMLRegistry/CXMLRegistry.h"
#include <map>
#include <string>
#include <set>
#include "../utils.h"

using namespace irr;

CGUISkinSystem::CGUISkinSystem(io::path path, IrrlichtDevice *dev) {
	loaded_skin = nullptr;
	device = dev;
	skinsPath = path;
	fs = dev->getFileSystem();
	loadSkinList();
}

// This is our version of the skinloader

// Generate a list of all directory names in skinsPath that have a skin.xml in them

bool CGUISkinSystem::loadSkinList() {
	epro::path_stringview skinpath{ skinsPath.data(), skinsPath.size() };
	ygo::Utils::FindFiles(skinpath, [this, &skinpath](epro::path_stringview name, bool isdir) {
		if(!isdir || name == EPRO_TEXT(".") || (name == EPRO_TEXT("..")))
			return;
		if(ygo::Utils::FileExists(fmt::format("{}/{}" SKINSYSTEM_SKINFILE, skinpath, name)))
			skinsList.emplace_back(name.data(), name.size());
	});
	std::sort(skinsList.begin(), skinsList.end(), ygo::Utils::CompareIgnoreCase<epro::path_string>);
	return !skinsList.empty();
}
gui::CGUIProgressBar *CGUISkinSystem::addProgressBar(gui::IGUIElement *parent, core::rect<s32> rect, bool bindColorsToSkin) {
	/*
	gui::CGUIProgressBar* bar = new gui::CGUIProgressBar(parent,device->getGUIEnvironment(), rect,bindColorsToSkin);
	//gui::CImageGUISkin* skin = (gui::CImageGUISkin*)device->getGUIEnvironment()->getSkin();
	parent->addChild(bar);
	bar->drop();
	return bar;*/
	return nullptr;
}


bool CGUISkinSystem::populateTreeView(gui::IGUITreeView *control, const core::stringc& skinname) {
	/*
	bool ret = false;
	io::path oldpath = fs->getWorkingDirectory();
	fs->changeWorkingDirectoryTo(skinsPath);
	registry = new CXMLRegistry(fs);
	if(!registry->loadFile(SKINSYSTEM_SKINFILE,skinname.data())) {
		fs->changeWorkingDirectoryTo(oldpath);
		return ret;
	}
	ret = registry->populateTreeView(control);
	delete registry;
	registry = nullptr;
	fs->changeWorkingDirectoryTo(oldpath);
	return ret;*/
	return false;
}

void CGUISkinSystem::ParseGUIElementStyle(gui::SImageGUIElementStyle& elem, const core::stringc& name, bool nullcolors) {
	core::stringw context;
	core::stringc ctmp, err = "";
	context = L"Skin/";
	context += name;
	core::rect<u32> box;
	video::SColor col;
	ctmp = registry->getValueAsCStr(L"texture", context.data());
	if(!ctmp.size())
		err += "Could not load texture property from skin file";

	elem.Texture = device->getVideoDriver()->getTexture(workingDir + "/" + ctmp);

	box = registry->getValueAsRect((context + "/SrcBorder").data());
	elem.SrcBorder.Top = box.UpperLeftCorner.X;
	elem.SrcBorder.Left = box.UpperLeftCorner.Y;
	elem.SrcBorder.Bottom = box.LowerRightCorner.X;
	elem.SrcBorder.Right = box.LowerRightCorner.Y;

	box = registry->getValueAsRect((context + "/DstBorder").data());
	elem.DstBorder.Top = box.UpperLeftCorner.X;
	elem.DstBorder.Left = box.UpperLeftCorner.Y;
	elem.DstBorder.Bottom = box.LowerRightCorner.X;
	elem.DstBorder.Right = box.LowerRightCorner.Y;
	if(nullcolors) elem.Color = { 0 };
	col = registry->getValueAsColor((context + "/Color").data());
	if(col.color)
		elem.Color = col;
	else {
		col = registry->getValueAsColor((context + "/Colour").data());
		if(col.color)
			elem.Color = col;
	}
}

// Because my 'xmlloader/registry', already has the data access components
// We skip the 'configreader and skinloader' steps and just load right from the registry into the skin config
// Then create the skin from that, and set it up
// TO maintain compatability in case of upgrades
// We dont touch the iterface to the skin itself.

gui::CImageGUISkin* CGUISkinSystem::loadSkinFromFile(const fschar_t *skinname) {
	gui::CImageGUISkin* skin;
	gui::SImageGUISkinConfig skinConfig;
	gui::EGUI_SKIN_TYPE fallbackType;
	gui::IGUISkin *fallbackSkin;

	core::stringc tmp;
	io::path path = workingDir + "/./";
	path += skinname;
	if(!registry->loadFile(SKINSYSTEM_SKINFILE, path.data()))
		return nullptr;
	// Easiest way to see if an xml is loading correctly
	// is to make the registry write out the root node and see what comes out.
	//registry->writeFile("Skin",".");

	// To switch on the fly, we have to set the skin to the fallback skin first	
	tmp = registry->getValueAsCStr(L"skin", L"Skin/Properties/Fallback");
	// Always default to metalic
	fallbackType = gui::EGST_WINDOWS_METALLIC;
	if(tmp.equals_ignore_case("EGST_WINDOWS_CLASSIC"))
		fallbackType = gui::EGST_WINDOWS_CLASSIC;
	else if(tmp.equals_ignore_case("EGST_BURNING_SKIN"))
		fallbackType = gui::EGST_BURNING_SKIN;
	// I'm not dealing with the 'unknown skin'
	fallbackSkin = device->getGUIEnvironment()->createSkin(fallbackType);
	device->getGUIEnvironment()->setSkin(fallbackSkin);
	fallbackSkin->drop();

	skin = new gui::CImageGUISkin(device->getVideoDriver(), device->getGUIEnvironment()->getSkin());
	workingDir = path;
	//fs->changeWorkingDirectoryTo(path.data());
	ParseGUIElementStyle(skinConfig.Button, "Button");
	ParseGUIElementStyle(skinConfig.ButtonPressed, "Button/Pressed");
	ParseGUIElementStyle(skinConfig.ButtonDisabled, "Button/ButtonDisabled");
	ParseGUIElementStyle(skinConfig.SunkenPane, "SunkenPane");
	ParseGUIElementStyle(skinConfig.Window, "Window");
	ParseGUIElementStyle(skinConfig.ProgressBar, "ProgressBar", true);
	ParseGUIElementStyle(skinConfig.ProgressBarFilled, "ProgressBar/Filled", true);
	ParseGUIElementStyle(skinConfig.TabBody, "TabControl");
	ParseGUIElementStyle(skinConfig.TabButton, "TabControl/TabButton");
	ParseGUIElementStyle(skinConfig.TabButtonPressed, "TabControl/TabButtonPressed");
	ParseGUIElementStyle(skinConfig.MenuBar, "MenuBar");
	ParseGUIElementStyle(skinConfig.MenuPane, "MenuBar/MenuPane");
	ParseGUIElementStyle(skinConfig.MenuPressed, "MenuBar/MenuPressed");
	ParseGUIElementStyle(skinConfig.CheckBox, "CheckBox");
	ParseGUIElementStyle(skinConfig.CheckBoxDisabled, "CheckBox/CheckBoxDisabled");
	ParseGUIElementStyle(skinConfig.ComboBox, "ComboBox");
	ParseGUIElementStyle(skinConfig.ComboBoxDisabled, "ComboBox/ComboBoxDisabled");

	skinConfig.CheckBoxColor = registry->getValueAsColor(L"Skin/Global/CheckBoxColor");

	// If there was no progress bar colors set, set them in the config to the defaults
	// otherwise their 0,0,0,0. This is neccicary for the old klagui.	

	if(skinConfig.ProgressBarFilled.Color == 0)
		skinConfig.ProgressBarFilled.Color = video::SColor(255, 255, 0, 0);

	// Load in the Info
	loadProperty(L"Name", skin);
	loadProperty(L"Author", skin);
	loadProperty(L"Version", skin);
	loadProperty(L"Date", skin);
	loadProperty(L"Desc", skin);

	loadCustomColors(skin);

	skin->loadConfig(skinConfig);

	/*tmp = registry->getValueAsCStr(L"texture",L"Skin/Properties/Font");
	gui::IGUIFont *font = device->getGUIEnvironment()->getFont(tmp.data());
	if(font !=0) {
		device->getGUIEnvironment()->getSkin()->setFont(font, gui::EGDF_DEFAULT);
		device->getGUIEnvironment()->getSkin()->setFont(font, gui::EGDF_WINDOW);
	}*/
	// Get and set global alpha, problem with this, you can't set it to 0
	// He does this to make ALL the default stuff completly transparent
	// This has the downside that it whipes out things like checkbox and window button colors
	auto guialpha = registry->getValueAsInt(L"guialpha", L"Skin/Global/");
	if(guialpha) {
		s32 i = gui::EGDC_COUNT;
		while(i--) {
			auto col = skin->getColor((gui::EGUI_DEFAULT_COLOR)i);

			col.setAlpha(guialpha);

			skin->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
		}
	}
#define CHECKCOLOR(elem) do { checkSkinColor(gui::elem, L"Skin/Global/"#elem,skin); } while(0)
	CHECKCOLOR(EGDC_3D_DARK_SHADOW);
	CHECKCOLOR(EGDC_3D_SHADOW);
	CHECKCOLOR(EGDC_3D_FACE);
	CHECKCOLOR(EGDC_3D_HIGH_LIGHT);
	CHECKCOLOR(EGDC_3D_LIGHT);
	CHECKCOLOR(EGDC_ACTIVE_BORDER);
	CHECKCOLOR(EGDC_ACTIVE_CAPTION);
	CHECKCOLOR(EGDC_APP_WORKSPACE);
	CHECKCOLOR(EGDC_BUTTON_TEXT);
	CHECKCOLOR(EGDC_GRAY_TEXT);
	CHECKCOLOR(EGDC_HIGH_LIGHT);
	CHECKCOLOR(EGDC_HIGH_LIGHT_TEXT);
	CHECKCOLOR(EGDC_INACTIVE_BORDER);
	CHECKCOLOR(EGDC_INACTIVE_CAPTION);
	CHECKCOLOR(EGDC_TOOLTIP);
	CHECKCOLOR(EGDC_TOOLTIP_BACKGROUND);
	CHECKCOLOR(EGDC_SCROLLBAR);
	CHECKCOLOR(EGDC_WINDOW);
	CHECKCOLOR(EGDC_WINDOW_SYMBOL);
	CHECKCOLOR(EGDC_ICON);
	CHECKCOLOR(EGDC_ICON_HIGH_LIGHT);
	CHECKCOLOR(EGDC_GRAY_WINDOW_SYMBOL);
	CHECKCOLOR(EGDC_EDITABLE);
	CHECKCOLOR(EGDC_GRAY_EDITABLE);
	CHECKCOLOR(EGDC_FOCUSED_EDITABLE);
#undef CHECKCOLOR



	checkSkinSize(gui::EGDS_WINDOW_BUTTON_WIDTH, L"Skin/Global/WindowButton", L"width", skin);
	checkSkinSize(gui::EGDS_TITLEBARTEXT_DISTANCE_X, L"Skin/Global/Caption", L"tbardistancex", skin);
	checkSkinSize(gui::EGDS_TITLEBARTEXT_DISTANCE_Y, L"Skin/Global/Caption", L"tbardistancey", skin);

#define CHECKSIZE(elem) do { checkSkinSize(gui::elem, L"Skin/Global/"#elem,L"value",skin); } while(0)
	CHECKSIZE(EGDS_SCROLLBAR_SIZE);
	CHECKSIZE(EGDS_MENU_HEIGHT);
	CHECKSIZE(EGDS_WINDOW_BUTTON_WIDTH);
	CHECKSIZE(EGDS_CHECK_BOX_WIDTH);
	CHECKSIZE(EGDS_MESSAGE_BOX_WIDTH);
	CHECKSIZE(EGDS_MESSAGE_BOX_HEIGHT);
	CHECKSIZE(EGDS_BUTTON_WIDTH);
	CHECKSIZE(EGDS_BUTTON_HEIGHT);
	CHECKSIZE(EGDS_TEXT_DISTANCE_X);
	CHECKSIZE(EGDS_TEXT_DISTANCE_Y);
	CHECKSIZE(EGDS_TITLEBARTEXT_DISTANCE_X);
	CHECKSIZE(EGDS_TITLEBARTEXT_DISTANCE_Y);
	CHECKSIZE(EGDS_MESSAGE_BOX_GAP_SPACE);
	CHECKSIZE(EGDS_MESSAGE_BOX_MIN_TEXT_WIDTH);
	CHECKSIZE(EGDS_MESSAGE_BOX_MAX_TEXT_WIDTH);
	CHECKSIZE(EGDS_MESSAGE_BOX_MIN_TEXT_HEIGHT);
	CHECKSIZE(EGDS_MESSAGE_BOX_MAX_TEXT_HEIGHT);
	CHECKSIZE(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_X);
	CHECKSIZE(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_Y);
	CHECKSIZE(EGDS_BUTTON_PRESSED_TEXT_OFFSET_X);
	CHECKSIZE(EGDS_BUTTON_PRESSED_TEXT_OFFSET_Y);
#undef CHECKSIZE

#define CHECKICON(elem) do { checkSkinIcon(gui::elem, L"Skin/Global/"#elem,skin); } while(0)
	CHECKICON(EGDI_WINDOW_MAXIMIZE);
	CHECKICON(EGDI_WINDOW_RESTORE);
	CHECKICON(EGDI_WINDOW_CLOSE);
	CHECKICON(EGDI_WINDOW_MINIMIZE);
	CHECKICON(EGDI_WINDOW_RESIZE);
	CHECKICON(EGDI_CURSOR_UP);
	CHECKICON(EGDI_CURSOR_DOWN);
	CHECKICON(EGDI_CURSOR_LEFT);
	CHECKICON(EGDI_CURSOR_RIGHT);
	CHECKICON(EGDI_MENU_MORE);
	CHECKICON(EGDI_CHECK_BOX_CHECKED);
	CHECKICON(EGDI_DROP_DOWN);
	CHECKICON(EGDI_SMALL_CURSOR_UP);
	CHECKICON(EGDI_SMALL_CURSOR_DOWN);
	CHECKICON(EGDI_RADIO_BUTTON_CHECKED);
	CHECKICON(EGDI_MORE_LEFT);
	CHECKICON(EGDI_MORE_RIGHT);
	CHECKICON(EGDI_MORE_UP);
	CHECKICON(EGDI_MORE_DOWN);
	CHECKICON(EGDI_EXPAND);
	CHECKICON(EGDI_COLLAPSE);
	CHECKICON(EGDI_FILE);
	CHECKICON(EGDI_DIRECTORY);
#undef CHECKICON


#define CHECKTEXT(elem) do { checkSkinText(gui::elem, L"Skin/Global/"#elem,skin); } while(0)
	CHECKTEXT(EGDT_MSG_BOX_OK);
	CHECKTEXT(EGDT_MSG_BOX_CANCEL);
	CHECKTEXT(EGDT_MSG_BOX_YES);
	CHECKTEXT(EGDT_MSG_BOX_NO);
	CHECKTEXT(EGDT_WINDOW_CLOSE);
	CHECKTEXT(EGDT_WINDOW_MAXIMIZE);
	CHECKTEXT(EGDT_WINDOW_MINIMIZE);
	CHECKTEXT(EGDT_WINDOW_RESTORE);
#undef CHECKTEXT

	return skin;
}
core::stringw CGUISkinSystem::getProperty(core::stringw key) {
	gui::CImageGUISkin* skin = (gui::CImageGUISkin*)device->getGUIEnvironment()->getSkin();
	return loaded_skin == skin ? skin->getProperty(key) : core::stringw("");
}

video::SColor CGUISkinSystem::getCustomColor(ygo::skin::CustomSkinElements key, video::SColor fallback) {
	gui::CImageGUISkin* skin = (gui::CImageGUISkin*)device->getGUIEnvironment()->getSkin();
	return loaded_skin == skin ? skin->getCustomColor(key, fallback) : fallback;
}

bool CGUISkinSystem::checkSkinColor(gui::EGUI_DEFAULT_COLOR colToSet, const wchar_t *context, gui::CImageGUISkin *skin) {
	video::SColor col = registry->getValueAsColor(context);
	if(col.color) {
		skin->setColor(colToSet, col);
		return true;
	}
	return false;
}

bool CGUISkinSystem::checkSkinSize(gui::EGUI_DEFAULT_SIZE sizeToSet, const wchar_t *context, const wchar_t *key, gui::CImageGUISkin *skin) {
	u16 i = registry->getValueAsInt(key, context);
	if(i) {
		skin->setSize(sizeToSet, i);
		return true;
	}
	return false;
}

bool CGUISkinSystem::checkSkinIcon(gui::EGUI_DEFAULT_ICON iconToSet, const wchar_t * context, gui::CImageGUISkin * skin) {
	u32 i = registry->getValueAsInt(L"index", context);
	if(i) {
		skin->setIcon(iconToSet, i);
		return true;
	}
	return false;
}

bool CGUISkinSystem::checkSkinText(gui::EGUI_DEFAULT_TEXT textToSet, const wchar_t * context, gui::CImageGUISkin * skin) {
	auto txt = registry->getValueAsCStr(L"text", context);
	if(txt != nullptr) {
		skin->setDefaultText(textToSet, txt);
		return true;
	}
	return false;
}

bool CGUISkinSystem::loadProperty(core::stringw key, gui::CImageGUISkin *skin) {
	core::stringw wtmp = "Skin/Properties/";
	wtmp += key;
	wtmp = registry->getValueAsCStr(L"data", wtmp.data());
	if(wtmp.size()) {
		skin->setProperty(key, wtmp);
		return true;
	}
	return false;
}
void CGUISkinSystem::loadCustomColors(gui::CImageGUISkin * skin) {
	static const std::map<epro::wstringview, ygo::skin::CustomSkinElements> alias = {
#define DECLR(what,val) { L""#what, ygo::skin::CustomSkinElements::what },
#include "../custom_skin_enum.inl"
#undef DECLR
	};
	constexpr epro::wstringview wtmp = L"Skin/Custom/";
	auto* children = registry->listNodeChildren(L"", wtmp.data());
	if(!children)
		return;
	for(u32 i = 0; i < children->size(); i++) {
		epro::wstringview tmpchild = (*children)[i];
		video::SColor color = registry->getValueAsColor(fmt::format(L"{}{}", wtmp, tmpchild).data());
		if(color.color) {
			auto found = alias.find(tmpchild);
			if(found != alias.end())
				skin->setCustomColor(found->second, color);
		}
	}
	delete children;
}
bool CGUISkinSystem::applySkin(const fschar_t *skinname) {
	/*io::path oldpath = fs->getWorkingDirectory();
	fs->changeWorkingDirectoryTo(skinsPath);*/
	workingDir = skinsPath;
	CXMLRegistry reg{ fs };
	registry = &reg;
	loaded_skin = nullptr;
	gui::CImageGUISkin* skin = loadSkinFromFile(skinname);
	if(skin == nullptr) {
		return false;
	}
	device->getGUIEnvironment()->setSkin(skin);
	loaded_skin = skin;
	// If we're going to switch skin we need to find all the progress bars and overwrite their colors	
	skin->drop();
	registry = nullptr;
	//fs->changeWorkingDirectoryTo(oldpath);

	return true;
}