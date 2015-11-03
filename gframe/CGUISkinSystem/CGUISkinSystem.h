#ifndef __C_GUISKIN_SYSTEM_H_INCLUDED__
#define __C_GUISKIN_SYSTEM_H_INCLUDED__

#ifdef _WIN32
#include <irrlicht.h>
#endif
#include "CImageGUISkin.h"
#include "CGUIProgressBar.h"
//#include "SkinLoader.h"
#include "../CXMLRegistry/CXMLRegistry.h"

#ifndef _WIN32
	// gcc dosnt do 'debug'
	#ifndef _DEBUG
		#define _DEBUG
	#endif
#endif


#define SKINSYSTEM_SKINFILE "/skin.xml"


using namespace irr;
class CGUISkinSystem {
private :
	IrrlichtDevice *device;
	io::IFileSystem *fs;
	io::path skinsPath;	
	core::array<core::stringw> skinsList;
	
	CXMLRegistry *registry;
	gui::CImageGUISkin* loadSkinFromFile(const c8 *skinfile); 
	void ParseGUIElementStyle(gui::SImageGUIElementStyle& elem, const core::stringc& name,bool nullcolors=false);
	bool checkSkinColor(gui::EGUI_DEFAULT_COLOR colToSet,const wchar_t *context,gui::CImageGUISkin *skin);
	bool checkSkinSize(gui::EGUI_DEFAULT_SIZE sizeToSet,const wchar_t *context,const wchar_t *key,gui::CImageGUISkin *skin);
	bool loadProperty(core::stringw key,gui::CImageGUISkin *skin);
public:
	// Constructor
	// path = Path to skins
	// dev = Irrlicht device
	CGUISkinSystem(core::string<char*> path,IrrlichtDevice *dev);
	~CGUISkinSystem();
	core::array<core::stringw> listSkins();
	bool loadSkinList();
	bool applySkin(const wchar_t *skinname);
	gui::CGUIProgressBar *addProgressBar(gui::IGUIElement *parent,core::rect<s32> rect,bool bindColorsToSkin=true);
	// Gets property from current skin
	core::stringw getProperty(core::stringw key);	
	bool populateTreeView(gui::IGUITreeView *control,const core::stringc& skinname);
	
};

#endif
