#ifndef __C_GUISKIN_SYSTEM_H_INCLUDED__
#define __C_GUISKIN_SYSTEM_H_INCLUDED__

#include "CImageGUISkin.h"
#include <path.h>
#include <irrArray.h>
#include "../text_types.h"
#include <vector>

#define SKINSYSTEM_SKINFILE _IRR_TEXT("/skin.xml")

class CXMLRegistry;
namespace irr {
class IrrlichtDevice;
namespace gui {
class CGUIProgressBar;
class IGUISkin;
class CImageGUISkin;
class IGUITreeView;
}
namespace io {
class IFileSystem;
}
}

class CGUISkinSystem {
private:
	irr::IrrlichtDevice* device;
	irr::io::IFileSystem* fs;
	irr::io::path skinsPath;
	irr::io::path workingDir;
	std::vector<epro::path_string> skinsList;
	irr::gui::IGUISkin* loaded_skin;

	CXMLRegistry* registry;
	irr::gui::CImageGUISkin* loadSkinFromFile(const irr::fschar_t* skinfile);
	void ParseGUIElementStyle(irr::gui::SImageGUIElementStyle& elem, const irr::core::stringc& name, bool nullcolors = false);
	bool checkSkinColor(irr::gui::EGUI_DEFAULT_COLOR colToSet, const wchar_t* context, irr::gui::CImageGUISkin* skin);
	bool checkSkinSize(irr::gui::EGUI_DEFAULT_SIZE sizeToSet, const wchar_t* context, const wchar_t* key, irr::gui::CImageGUISkin* skin);
	bool checkSkinIcon(irr::gui::EGUI_DEFAULT_ICON iconToSet, const wchar_t* context, irr::gui::CImageGUISkin* skin);
	bool checkSkinText(irr::gui::EGUI_DEFAULT_TEXT textToSet, const wchar_t* context, irr::gui::CImageGUISkin* skin);
	bool loadProperty(irr::core::stringw key, irr::gui::CImageGUISkin* skin);
	void loadCustomColors(irr::gui::CImageGUISkin* skin);
public:
	// Constructor
	// path = Path to skins
	// dev = Irrlicht device
	CGUISkinSystem(irr::io::path path, irr::IrrlichtDevice* dev);
	~CGUISkinSystem() = default;
	const auto& listSkins() {
		return skinsList;
	};
	bool loadSkinList();
	bool applySkin(const irr::fschar_t* skinname);
	irr::gui::CGUIProgressBar* addProgressBar(irr::gui::IGUIElement* parent, irr::core::recti rect, bool bindColorsToSkin = true);
	// Gets property from current skin
	irr::core::stringw getProperty(irr::core::stringw key);
	irr::video::SColor getCustomColor(ygo::skin::CustomSkinElements key, irr::video::SColor fallback);
	bool populateTreeView(irr::gui::IGUITreeView* control, const irr::core::stringc& skinname);

};

#endif
