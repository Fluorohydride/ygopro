#include "xml_config.h"
#include "wx/xml/xml.h"

namespace ygopro
{

	bool CommonConfig::LoadConfig(const std::wstring& name) {
		wxXmlDocument doc;
		if(!doc.Load(name, "UTF-8", wxXMLDOC_KEEP_WHITESPACE_NODES))
			return false;
		wxXmlNode* root = doc.GetRoot();
		wxXmlNode* child = root->GetChildren();
		while (child) {
            if(child->GetType() == wxXmlNodeType::wxXML_ELEMENT_NODE) {
                if (child->GetName() == "integer") {
                    wxString name = child->GetAttribute("name");
                    wxString value = child->GetAttribute("value");
                    long val = 0;
                    value.ToLong(&val, 0);
                    config_map[name.ToStdWstring()] = val;
                } if (child->GetName() == "float") {
                    wxString name = child->GetAttribute("name");
                    wxString value = child->GetAttribute("value");
                    double val = 0.0;
                    value.ToDouble(&val);
                    config_map[name.ToStdWstring()] = val;
                } else {
                    wxString name = child->GetAttribute("name");
                    wxString value = child->GetAttribute("value");
                    config_map[name.ToStdWstring()] = value;
                }
            }
			child = child->GetNext();
		}
        return true;
	}

	void CommonConfig::SaveConfig(const std::wstring& name) {
		if(config_map.size() == 0)
			return;
		wxXmlDocument doc;
		doc.SetVersion(wxT("1.0"));
		doc.SetFileEncoding(wxT("UTF-8"));
		wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Common");
		for(auto iter : config_map) {
            if(iter.second.val_type == 0) {
                wxXmlNode* element = new wxXmlNode(wxXML_ELEMENT_NODE, "integer");
				char buf[32];
				sprintf(buf, "%ld", iter.second.ivalue);
				element->AddAttribute("name", iter.first);
				element->AddAttribute("value", buf);
                root->AddChild(element);
			} else if(iter.second.val_type == 1) {
                wxXmlNode* element = new wxXmlNode(wxXML_ELEMENT_NODE, "float");
				char buf[32];
				sprintf(buf, "%lf", iter.second.fvalue);
				element->AddAttribute("name", iter.first);
				element->AddAttribute("value", buf);
                root->AddChild(element);
            } else {
                wxXmlNode* element = new wxXmlNode(wxXML_ELEMENT_NODE, "string");
				element->AddAttribute("name", iter.first);
				element->AddAttribute("value", iter.second.svalue);
                root->AddChild(element);
            }
		}
		doc.SetRoot(root);
		doc.Save(name);
	}

}
