#include "xml_config.h"
#include "wx/xml/xml.h"

namespace ygopro
{

	bool CommonConfig::LoadConfig(const wxString& name) {
		wxXmlDocument doc;
		if(!doc.Load(name, wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES))
			return false;
		wxXmlNode* root = doc.GetRoot();
		wxXmlNode* child = root->GetChildren();
		while (child) {
            if(child->GetType() == wxXmlNodeType::wxXML_ELEMENT_NODE) {
                if (child->GetName() == wxT("integer")) {
                    wxString name = child->GetAttribute("name");
                    wxString value = child->GetAttribute("value");
                    long val = 0;
                    value.ToLong(&val, 0);
                    config_map[name.ToStdString()] = val;
                } else {
                    wxString name = child->GetAttribute("name");
                    wxString value = child->GetAttribute("value");
                    config_map[name.ToStdString()] = value;
                }
            }
			child = child->GetNext();
		}
        return true;
	}

	void CommonConfig::SaveConfig(const wxString& name) {
		if(config_map.size() == 0)
			return;
		wxXmlDocument doc;
		doc.SetVersion(wxT("1.0"));
		doc.SetFileEncoding(wxT("UTF-8"));
		wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Common");
		for(auto iter : config_map) {
            if(iter.second.is_string) {
                wxXmlNode* element = new wxXmlNode(wxXML_ELEMENT_NODE, "string");
				element->AddAttribute("name", iter.first);
				element->AddAttribute("value", iter.second.svalue);
                root->AddChild(element);
			} else {
                wxXmlNode* element = new wxXmlNode(wxXML_ELEMENT_NODE, "integer");
				char buf[32];
				sprintf(buf, "%ld", iter.second.ivalue);
				element->AddAttribute("name", iter.first);
				element->AddAttribute("value", buf);
                root->AddChild(element);
			}
		}
		doc.SetRoot(root);
		doc.Save(name);
	}

}
