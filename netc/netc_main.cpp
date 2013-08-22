#include "../common/common.h"
#include "../common/hash.h"
#include "game_frame.h"
#include "editor_frame.h"
#include "image_mgr.h"
#include "card_data.h"
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
			imageMgr.LoadImageConfig("textures.xml");

//			if(dataMgr.LoadDatas("cards.cdb"))
//				return true;

			editorFrame = new EditorFrame(1000, 750);
			editorFrame->Center();
			editorFrame->Show();
			return true;
		}
	};

	DECLARE_APP(ygoApp)
	IMPLEMENT_APP(ygoApp)

}
