#ifndef _XML_CONFIG_H_
#define _XML_CONFIG_H_

#include "wx/xml/xml.h"
#include <unordered_map>

namespace ygopro
{
	class XMLConfig {
	public:
		virtual void LoadConfig(const wxString& name) = 0;
		virtual void SaveConfig(const wxString& name) = 0;
	};

	class CommonConfig : public XMLConfig {

		struct _ValueStruct {
			bool is_string;
			int ivalue;
			std::string svalue;

			_ValueStruct(): ivalue(0), is_string(false) {}
			
			void operator = (int val) { ivalue = val; is_string = false; }
			void operator = (const char* val) { svalue = val; is_string = true; }
			void operator = (const std::string& val) { svalue = val; is_string = true; }
			void operator = (const wxString& val) { svalue = val.ToStdString(); is_string = true; }
			operator const int&() { return ivalue; }
			operator std::string&() { return svalue; }
		};

	public:

		inline _ValueStruct& operator[] (const std::string& name) {
			return config_map[name];
		}

		virtual void LoadConfig(const wxString& name);
		virtual void SaveConfig(const wxString& name);

	private:
		std::unordered_map<std::string, _ValueStruct> config_map;

	};

}

#endif
