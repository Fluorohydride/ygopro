#include "../common/common.h"
#include "../common/hash.h"
#include "game_window.h"
#include "image_mgr.h"
#include "wx/app.h"
#include "wx/image.h"

namespace ygopro {

	class ygoApp : public wxApp {
	public:
		bool OnInit() {
			if(!wxApp::OnInit())
				return false;

			wxInitAllImageHandlers();
			commonCfg.LoadConfig("common.xml");
			imageMgr.LoadConfig("textures.xml");

			mainFrame = new GameFrame(1140, 800);
			mainFrame->Center();
			mainFrame->Show();
			return true;
		}
	};

	DECLARE_APP(ygoApp)
	IMPLEMENT_APP(ygoApp)

}
