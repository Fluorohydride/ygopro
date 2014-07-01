#ifndef _XML_CONFIG_H_
#define _XML_CONFIG_H_

#include "wx/string.h"
#include <unordered_map>

namespace ygopro
{

    struct ValueStruct {
        bool is_string;
        long ivalue;
        std::string svalue;
        
        ValueStruct(): is_string(false), ivalue(0) {}
        
        void operator = (long val) { ivalue = val; is_string = false; }
        void operator = (const char* val) { svalue = val; is_string = true; }
        void operator = (const std::string& val) { svalue = val; is_string = true; }
        void operator = (const wxString& val) { svalue = val.ToStdString(); is_string = true; }
        operator unsigned int() { return (unsigned int)ivalue; }
        operator int() { return (int)ivalue; }
        operator unsigned long() { return (unsigned long)ivalue; }
        operator long() { return ivalue; }
        operator const char*() { return svalue.c_str(); }
        operator const std::string&() { return svalue; }
        operator wxString() { return svalue; }
    };
    
	class CommonConfig {

	public:

		inline ValueStruct& operator[] (const std::string& name) {
			return config_map[name];
		}
        inline bool Exists(const std::string& name) {
            return config_map.find(name) != config_map.end();
        }
        
        template<typename FT>
        void ForEach(FT fun) {
            if(fun == nullptr)
                return;
            for(auto& iter : config_map)
                fun(iter.first, iter.second);
        }
        
		bool LoadConfig(const wxString& name);
		void SaveConfig(const wxString& name);

	private:
		std::unordered_map<std::string, ValueStruct> config_map;

	};

}

#endif
