#include "../common/common.h"
#include "../common/hash.h"
#include "game_frame.h"
#include "editor_frame.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
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
			if(!commonCfg.LoadConfig("common.xml"))
                return true;
			if(dataMgr.LoadDatas(wxString(static_cast<const std::string&>(commonCfg["database"]))))
                return true;
            if(!imageMgr.LoadImageConfig(wxString(static_cast<const std::string&>(commonCfg["textures"]))))
                return true;
            limitRegulationMgr.LoadLimitRegulation(wxString(static_cast<const std::string&>(commonCfg["limit_regulation"])));

			editorFrame = new EditorFrame(1080, 720);
			editorFrame->Center();
			editorFrame->Show();
			return true;
		}
	};

}

DECLARE_APP(ygopro::ygoApp)
IMPLEMENT_APP(ygopro::ygoApp)
