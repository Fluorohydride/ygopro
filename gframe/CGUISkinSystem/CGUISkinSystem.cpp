#include "CGUISkinSystem.h"

CGUISkinSystem::CGUISkinSystem(core::string<char*> path,IrrlichtDevice *dev) {
	device = dev;
	skinsPath = path;
	fs = dev->getFileSystem();	
	this->loadSkinList();
	
	
}
core::array<core::stringw> CGUISkinSystem::listSkins() {
	return skinsList;
}
// This is our version of the skinloader

// Generate a list of all directory names in skinsPath that have a skin.xml in them
// Don't rely on working directory going in.
// Careful with changeworkingdirectoryto and relative paths,
// remember that they are relative to the CURRENT working directory
bool CGUISkinSystem::loadSkinList() {
	io::IFileList *fileList;
	int i;
	io::path oldpath = fs->getWorkingDirectory();
	fs->changeWorkingDirectoryTo(skinsPath);
	fileList = fs->createFileList();
	i = fileList->getFileCount();
	core::stringw tmp;
	while(i--) {
		// Check only directories, skip . and ..
		// Side effect, on linux this ignores hidden directories
		if(fileList->isDirectory(i) && !fileList->getFileName(i).equalsn(".",1)) {
			if(fs->existFile(fileList->getFileName(i) + SKINSYSTEM_SKINFILE)) {
				tmp = fileList->getFileName(i);
				skinsList.push_back(tmp);						
			}
			
		}
	}	
	fileList->drop();
	fs->changeWorkingDirectoryTo(oldpath);
	if(skinsList.size() > 0) 
		return true;
	else 
		return false;
}
gui::CGUIProgressBar *CGUISkinSystem::addProgressBar(gui::IGUIElement *parent,core::rect<s32> rect, bool bindColorsToSkin) {
	gui::CGUIProgressBar* bar = new gui::CGUIProgressBar(parent,device->getGUIEnvironment(), rect,bindColorsToSkin);
	//gui::CImageGUISkin* skin = (gui::CImageGUISkin*)device->getGUIEnvironment()->getSkin();
	parent->addChild(bar);    
	bar->drop();
	return bar;
}


bool CGUISkinSystem::populateTreeView(gui::IGUITreeView *control,const core::stringc& skinname) {
	bool ret = false;
	io::path oldpath = fs->getWorkingDirectory();
	fs->changeWorkingDirectoryTo(skinsPath);
	registry = new CXMLRegistry(fs);	
	if(!registry->loadFile(SKINSYSTEM_SKINFILE,skinname.c_str())) {
		return ret;
	}
	ret = registry->populateTreeView(control);
	delete registry;
	registry = NULL;
	fs->changeWorkingDirectoryTo(oldpath);	
	return ret;
}

void CGUISkinSystem::ParseGUIElementStyle(gui::SImageGUIElementStyle& elem, const core::stringc& name, bool nullcolors) {
	core::stringw context;
	core::stringc ctmp,err = "";
	context = L"Skin/"; 
	context += name;
	core::rect<u32> box;
	video::SColor col;
	ctmp = registry->getValueAsCStr(L"texture",context.c_str());
	if(!ctmp.size()) 
		err += "Could not load texture property from skin file";	
	
	elem.Texture = device->getVideoDriver()->getTexture(ctmp);	
	
	box = registry->getValueAsRect((context + "/SrcBorder").c_str());
	elem.SrcBorder.Top = box.UpperLeftCorner.X;
	elem.SrcBorder.Left = box.UpperLeftCorner.Y;
	elem.SrcBorder.Bottom = box.LowerRightCorner.X;
	elem.SrcBorder.Right = box.LowerRightCorner.Y;
	
	box = registry->getValueAsRect((context + "/DstBorder").c_str());
	elem.DstBorder.Top = box.UpperLeftCorner.X;
	elem.DstBorder.Left = box.UpperLeftCorner.Y;
	elem.DstBorder.Bottom = box.LowerRightCorner.X;
	elem.DstBorder.Right = box.LowerRightCorner.Y;	
	if(nullcolors) elem.Color = NULL;
	col = registry->getValueAsColor((context +"/Color").c_str());
	if(col != NULL) 
		elem.Color = col;
	else {
		col = registry->getValueAsColor((context +"/Colour").c_str());
		if(col != NULL) 
			elem.Color = col;
	}
}

// Because my 'xmlloader/registry', already has the data access components
// We skip the 'configreader and skinloader' steps and just load right from the registry into the skin config
// Then create the skin from that, and set it up
// TO maintain compatability in case of upgrades
// We dont touch the iterface to the skin itself.

gui::CImageGUISkin* CGUISkinSystem::loadSkinFromFile(const c8 *skinname) {	
	gui::CImageGUISkin* skin ;
	gui::SImageGUISkinConfig skinConfig;
	gui::EGUI_SKIN_TYPE fallbackType;
	gui::IGUISkin *fallbackSkin;
	
	s32 i,x;
	core::stringc tmp;
	core::stringw wtmp;
	core::stringc path = "./";
	path += skinname;
	if(!registry->loadFile(SKINSYSTEM_SKINFILE,path.c_str())) {
		return NULL;
	}	
	// Easiest way to see if an xml is loading correctly
	// is to make the registry write out the root node and see what comes out.
	//registry->writeFile("Skin",".");
	
	// To switch on the fly, we have to set the skin to the fallback skin first	
	tmp = registry->getValueAsCStr(L"skin",L"Skin/Properties/Fallback");
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
	fs->changeWorkingDirectoryTo(path.c_str());
	ParseGUIElementStyle(skinConfig.Button,"Button");
	ParseGUIElementStyle(skinConfig.ButtonPressed, "Button/Pressed");
	ParseGUIElementStyle(skinConfig.ButtonDisabled, "Button/ButtonDisabled");
    ParseGUIElementStyle(skinConfig.SunkenPane, "SunkenPane");
    ParseGUIElementStyle(skinConfig.Window, "Window");
    ParseGUIElementStyle(skinConfig.ProgressBar, "ProgressBar",true);
    ParseGUIElementStyle(skinConfig.ProgressBarFilled, "ProgressBar/Filled",true);
	ParseGUIElementStyle(skinConfig.TabBody,"TabControl");
	ParseGUIElementStyle(skinConfig.TabButton,"TabControl/TabButton");
	ParseGUIElementStyle(skinConfig.TabButtonPressed,"TabControl/TabButtonPressed");
	ParseGUIElementStyle(skinConfig.MenuBar,"MenuBar");
	ParseGUIElementStyle(skinConfig.MenuPane,"MenuBar/MenuPane");
	ParseGUIElementStyle(skinConfig.MenuPressed,"MenuBar/MenuPressed");
	ParseGUIElementStyle(skinConfig.CheckBox,"CheckBox");
	ParseGUIElementStyle(skinConfig.CheckBoxDisabled,"CheckBox/CheckBoxDisabled");
	ParseGUIElementStyle(skinConfig.ComboBox,"ComboBox");
	ParseGUIElementStyle(skinConfig.ComboBoxDisabled,"ComboBox/ComboBoxDisabled");

	skinConfig.CheckBoxColor = registry->getValueAsColor(L"Skin/Global/CheckBoxColor");
	
	// If there was no progress bar colors set, set them in the config to the defaults
	// otherwise their 0,0,0,0. This is neccicary for the old klagui.	

	if(skinConfig.ProgressBar.Color == NULL) 
		skinConfig.ProgressBar.Color = video::SColor();	
	if(skinConfig.ProgressBarFilled.Color == NULL) 
		skinConfig.ProgressBarFilled.Color = video::SColor(255,255,0,0);

	// Load in the Info
	loadProperty((core::stringw)L"Name",skin);
	loadProperty((core::stringw)L"Author",skin);
	loadProperty((core::stringw)L"Version",skin);
	loadProperty((core::stringw)L"Date",skin);
	loadProperty((core::stringw)L"Desc",skin);
	loadProperty((core::stringw)L"SpecialColor",skin);
	skin->loadConfig(skinConfig);
	
	tmp = registry->getValueAsCStr(L"texture",L"Skin/Properties/Font");
	gui::IGUIFont *font = device->getGUIEnvironment()->getFont(tmp.c_str());
	if(font !=0) {
		device->getGUIEnvironment()->getSkin()->setFont(font, gui::EGDF_DEFAULT);
		device->getGUIEnvironment()->getSkin()->setFont(font, gui::EGDF_WINDOW);
	}
	// Get and set global alpha, problem with this, you can't set it to 0
	// He does this to make ALL the default stuff completly transparent
	// This has the downside that it whipes out things like checkbox and window button colors
	video::SColor newCol = video::SColor();
	video::SColor oldCol = newCol;
	x = registry->getValueAsInt(L"guialpha",L"Skin/Global/");
	if(x && x != NULL) {		
		i = gui::EGDC_COUNT;
		while(i--) {
			oldCol = skin->getColor((gui::EGUI_DEFAULT_COLOR)i);		
			
			newCol = oldCol;
			newCol.setAlpha(x);

			skin->setColor((gui::EGUI_DEFAULT_COLOR)i, newCol);	
		}
	}
	checkSkinColor(gui::EGDC_3D_DARK_SHADOW,L"Skin/Global/EGDC_32_DARK_SHADOW",skin);
	checkSkinColor(gui::EGDC_3D_SHADOW,L"Skin/Global/EGDC_3D_SHADOW",skin);
	checkSkinColor(gui::EGDC_3D_FACE,L"Skin/Global/EGDC_3D_FACE",skin);	
	checkSkinColor(gui::EGDC_3D_HIGH_LIGHT,L"Skin/Global/EGDC_3D_HIGH_LIGHT",skin);
	checkSkinColor(gui::EGDC_3D_LIGHT,L"Skin/Global/EGDC_3D_LIGHT",skin);
	checkSkinColor(gui::EGDC_ACTIVE_BORDER,L"Skin/Global/EGDC_ACTIVE_BORDER",skin);
	checkSkinColor(gui::EGDC_ACTIVE_CAPTION,L"Skin/Global/EGDC_ACTIVE_CAPTION",skin);
	checkSkinColor(gui::EGDC_APP_WORKSPACE,L"Skin/Global/EGDC_APP_WORKSPACE",skin);
	checkSkinColor(gui::EGDC_BUTTON_TEXT,L"Skin/Global/EGDC_BUTTON_TEXT",skin);
	checkSkinColor(gui::EGDC_GRAY_TEXT,L"Skin/Global/EGDC_GRAY_TEXT",skin);
	checkSkinColor(gui::EGDC_HIGH_LIGHT_TEXT,L"Skin/Global/EGDC_HIGH_LIGHT_TEXT",skin);
	checkSkinColor(gui::EGDC_INACTIVE_BORDER,L"Skin/Global/EGDC_INACTIVE_BORDER",skin);
	checkSkinColor(gui::EGDC_INACTIVE_CAPTION,L"Skin/Global/EGDC_INACTIVE_CAPTION",skin);
	checkSkinColor(gui::EGDC_TOOLTIP,L"Skin/Global/EGDC_TOOLTIP",skin);
	checkSkinColor(gui::EGDC_TOOLTIP_BACKGROUND,L"Skin/Global/EGDC_TOOLTIP_BACKGROUND",skin);
	checkSkinColor(gui::EGDC_SCROLLBAR,L"Skin/Global/EGDC_SCROLLBAR",skin);
	checkSkinColor(gui::EGDC_WINDOW,L"Skin/Global/EGDC_WINDOW",skin);
	checkSkinColor(gui::EGDC_WINDOW_SYMBOL,L"Skin/Global/EGDC_WINDOW_SYMBOL",skin);
	checkSkinColor(gui::EGDC_ICON,L"Skin/Global/EGDC_ICON",skin);	
	checkSkinColor(gui::EGDC_ICON_HIGH_LIGHT,L"Skin/Global/EGDC_ICON_HIGH_LIGHT",skin);
	
	
	
	checkSkinSize(gui::EGDS_WINDOW_BUTTON_WIDTH, L"Skin/Global/WindowButton",L"width",skin);	
	checkSkinSize(gui::EGDS_TITLEBARTEXT_DISTANCE_X, L"Skin/Global/Caption",L"tbardistancex",skin);
	checkSkinSize(gui::EGDS_TITLEBARTEXT_DISTANCE_Y, L"Skin/Global/Caption",L"tbardistancey",skin);
	

	return skin;
}
core:: stringw CGUISkinSystem::getProperty(core::stringw key) {
	gui::CImageGUISkin* skin = (gui::CImageGUISkin*)device->getGUIEnvironment()->getSkin();
	return skin->getProperty(key);
}

bool CGUISkinSystem::checkSkinColor(gui::EGUI_DEFAULT_COLOR colToSet,const wchar_t *context,gui::CImageGUISkin *skin) {
	video::SColor col = registry->getValueAsColor(context);
	if(col != NULL) {
		skin->setColor(colToSet,col);
		return true;
	}
	return false;
}

bool CGUISkinSystem::checkSkinSize(gui::EGUI_DEFAULT_SIZE sizeToSet,const wchar_t *context,const wchar_t *key,gui::CImageGUISkin *skin) {
	u16 i = registry->getValueAsInt(key,context);
	if(i != NULL) {
		skin->setSize(sizeToSet,i);
		return true;
	}
	return false;
}

bool CGUISkinSystem::loadProperty(core::stringw key,gui::CImageGUISkin *skin) {
	core::stringw wtmp = "Skin/Properties/";
	wtmp += key;
	wtmp = registry->getValueAsCStr(L"data",wtmp.c_str());
	if(wtmp.size()) {
		skin->setProperty(key,wtmp);
		return true;
	}
	return false;
}
bool CGUISkinSystem::applySkin(const wchar_t *skinname) {
	io::path oldpath = fs->getWorkingDirectory();
	core::stringc tmp = skinname;
	fs->changeWorkingDirectoryTo(skinsPath);
	registry = new CXMLRegistry(fs);
	gui::CImageGUISkin* skin = loadSkinFromFile(tmp.c_str());
    if(skin == NULL) return false;
	
    device->getGUIEnvironment()->setSkin(skin);
	// If we're going to switch skin we need to find all the progress bars and overwrite their colors	
    skin->drop();	
	delete registry;
	registry = NULL;
	fs->changeWorkingDirectoryTo(oldpath);
	
	return true;
}
CGUISkinSystem::~CGUISkinSystem() {
	skinsList.clear();
	

	
}