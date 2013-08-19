#include "wx/app.h"
#include "wx/image.h"
#include "deframe.h"

namespace ygopro {

	class deApp : public wxApp {
	public:
		bool OnInit() {
			if(!wxApp::OnInit())
				return false;

			wxInitAllImageHandlers();

			deFrame = new DEFrame(1280, 800);
			deFrame->Center();
			deFrame->Show();
			return true;
		}
	};

	DECLARE_APP(deApp)
	IMPLEMENT_APP(deApp)

}
