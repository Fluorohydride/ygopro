#ifndef _CARD_DATA_S_H_
#define _CARD_DATA_S_H_

#include <string>
#include <unordered_map>
#include <mutex>
#include "../ocgcore/common.h"

namespace ygopro
{
    
	class DataMgrS {
	public:
        void LockData();
        void UnlockData();
		int LoadDatas(const std::string& file);
        void ReloadDatas();
		card_data* operator [] (uint32 code);
        
	private:
        std::string _dbfile;
        std::mutex _mlock;
		std::unordered_map<unsigned int, card_data> _datas;
	};
    
	extern DataMgrS dataMgrs;
    
}

#endif
