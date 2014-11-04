#ifndef _XML_CONFIG_H_
#define _XML_CONFIG_H_

namespace ygopro
{

    struct ValueStruct {
        
        uint32_t val_type = 0;
        int64_t ivalue = 0;
        double fvalue = 0.0;
        std::wstring svalue;
        
        void operator = (int64_t val) { ivalue = val; val_type = 0; }
        void operator = (double val) { fvalue = val; val_type = 1; }
        void operator = (const wchar_t* val) { svalue = val; val_type = 2; }
        void operator = (const std::wstring& val) { svalue = val; val_type = 2; }
        
        operator uint32_t() { return static_cast<uint32_t>(ivalue); }
        operator int32_t() { return static_cast<int32_t>(ivalue); }
        operator uint64_t() { return static_cast<uint64_t>(ivalue); }
        operator int64_t() { return ivalue; }
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
