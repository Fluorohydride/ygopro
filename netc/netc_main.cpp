#include "../common/common.h"
#include "../common/hash.h"
#include "game_frame.h"
#include "editor_frame.h"
#include "image_mgr.h"
#include "card_data.h"
#include "wx/wx.h"
#include "wx/app.h"
#include "wx/image.h"

#ifdef __WXMAC__
#include <ApplicationServices/ApplicationServices.h>
#endif

namespace ygopro {

	class ygoApp : public wxApp {
	public:
		bool OnInit() {
#ifdef __WXMAC__
            ProcessSerialNumber PSN;
            GetCurrentProcess(&PSN);
            TransformProcessType(&PSN,kProcessTransformToForegroundApplication);
#endif
			if(!wxApp::OnInit())
				return false;

			wxInitAllImageHandlers();
			commonCfg.LoadConfig("common.xml");

//			if(dataMgr.LoadDatas("cards.cdb"))
//				return true;

			editorFrame = new EditorFrame(1000, 750);
			editorFrame->Center();
            
            imageMgr.LoadImageConfig("textures.xml");
            
			editorFrame->Show();
			return true;
		}
	};

}

DECLARE_APP(ygopro::ygoApp)
IMPLEMENT_APP(ygopro::ygoApp)
