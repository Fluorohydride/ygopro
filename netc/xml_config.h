#ifndef _XML_CONFIG_H_
#define _XML_CONFIG_H_

namespace ygopro
{

    struct ValueStruct {
        
        unsigned int val_type = 0;
        long ivalue = 0;
        double fvalue = 0.0;
        std::wstring svalue;
        
        void operator = (long val) { ivalue = val; val_type = 0; }
        void operator = (double val) { fvalue = val; val_type = 1; }
        void operator = (const wchar_t* val) { svalue = val; val_type = 2; }
        void operator = (const std::wstring& val) { svalue = val; val_type = 2; }
        
        operator unsigned int() { return static_cast<unsigned int>(ivalue); }
        operator int() { return static_cast<int>(ivalue); }
        operator unsigned long() { return static_cast<unsigned long>(ivalue); }
        operator long() { return ivalue; }
        operator float() { return static_cast<float>(fvalue); }
        operator double() { return fvalue; }
        operator const std::wstring&() { return svalue; }
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
        
		bool LoadConfig(const std::wstring& name);
		void SaveConfig(const std::wstring& name);

	private:
		std::unordered_map<std::string, ValueStruct> config_map;

	};

}

#endif
