#ifndef _CONFIG_MGR_H
#define _CONFIG_MGR_H

#include <string>
#include <unordered_map>
#include <stdio.h>

namespace ygopro
{

	class ConfigMgr {

	private:
		std::string config_file;
		std::unordered_map<std::string, int> config_int;
		std::unordered_map<std::string, std::string> config_str;

	public:

		ConfigMgr() {
		}

		ConfigMgr(const std::string& name) {
			config_file = name;
			FILE* fp = fopen(config_file.c_str(), "r");
			char linebuf[256], *pkey, *pvalue;
			int state = 0;
			if(fp) {
				fseek(fp, 0, SEEK_END);
				long fsize = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				while(ftell(fp) < fsize) {
					fgets(linebuf, 256, fp);
					pkey = nullptr;
					pvalue = nullptr;
					state = 0;
					for(char* p = linebuf; *p != 0; ++p) {
						if(*p == '\"') {
							if(state == 0) {
								pkey = p + 1;
								state++;
							} else if(state == 1) {
								*p = 0;
								state++;
							} else if(state == 2) {
								pvalue = p + 1;
								state++;
							} else if(state == 3) {
								*p = 0;
								state++;
								break;
							}
						} else if(*p == '=') {
							if(state == 2)
								pvalue = p + 1;
						}
					}
					if(state == 1 || state == 3)
						continue;
					if(state == 4)
						config_str[pkey] = pvalue;
					if(state == 2 && pvalue)
						config_int[pkey] = atoi(pvalue);
				}
				fclose(fp);
			}
		}

		virtual ~ConfigMgr() {
			if(config_file.length()) {
				FILE* fp = fopen(config_file.c_str(), "w+");
				if(fp) {
					fprintf(fp, "#configures\n");
					for(auto iter = config_int.begin(); iter != config_int.end(); ++iter)
						fprintf(fp, "\"%s\" = %d\n", iter->first.c_str(), iter->second);
					for(auto iter = config_str.begin(); iter != config_str.end(); ++iter)
						fprintf(fp, "\"%s\" = \"%s\"\n", iter->first.c_str(), iter->second.c_str());
					fclose(fp);
				}
			}
		}

		int GetConfigInt(std::string& name) {
			auto iter = config_int.find(name);
			if(iter != config_int.end())
				return iter->second;
			return 0;
		}

		const char* GetConfigString(std::string& name) {
			auto iter = config_str.find(name);
			if(iter != config_str.end())
				return iter->second.c_str();
			return "";
		}

		const char* GetConfigString(const char* name) {
			auto iter = config_str.find(name);
			if(iter != config_str.end())
				return iter->second.c_str();
			return "";
		}

		void SaveConfig(const std::string& name, int val) {
			config_int[name] = val;
		}

		void SaveConfig(const std::string& name, std::string& val) {
			config_str[name] = val;
		}

	};

}

#endif
