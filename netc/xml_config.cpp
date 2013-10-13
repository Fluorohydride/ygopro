#include "../common/common.h"
#include "xml_config.h"

namespace ygopro
{

	void CommonConfig::LoadConfig(const wxString& name) {
		wxXmlDocument doc;
		if(!doc.Load(name, wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES))
			return;
		wxXmlNode* root = doc.GetRoot();
		wxXmlNode* child = root->GetChildren();
		while (child) {
			if (child->GetName() == wxT("setting")) {
				wxString name = child->GetAttribute("name");
				wxString value = child->GetAttribute("value");
				if(child->GetAttribute("type") == "string") {
					config_map[name.ToStdString()] = value;
				} else {
					long val = 0;
					value.ToLong(&val);
					config_map[name.ToStdString()] = val;
				}
			}
			child = child->GetNext();
		}
	}

	void CommonConfig::SaveConfig(const wxString& name) {
		if(config_map.size() == 0)
			return;
		wxXmlDocument doc;
		doc.SetVersion(wxT("1.0"));
		doc.SetFileEncoding(wxT("UTF-8"));
		wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Common");
		for(auto iter : config_map) {
			wxXmlNode* element = new wxXmlNode(wxXML_ELEMENT_NODE, "setting");
			if(iter.second.is_string) {
				element->AddAttribute("name", iter.first);
				element->AddAttribute("value", iter.second.svalue);
				element->AddAttribute("type", "string");
			} else {
				char buf[32];
				sprintf(buf, "%ld", iter.second.ivalue);
				element->AddAttribute("name", iter.first);
				element->AddAttribute("value", buf);
				element->AddAttribute("type", "integer");
			}
			root->AddChild(element);
		}
		doc.SetRoot(root);
		doc.Save(name);
	}

}
