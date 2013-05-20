#ifndef RES_MGR_H
#define RES_MGR_H

#include "../common/common.h"
#include <unordered_map>

namespace ygopro
{

	struct ResourceInfo {
		unsigned char* data;
		unsigned int size;
	};

	class ResourceMgr {
	public:
		ResourceMgr();
		~ResourceMgr();

		int GenerateUID();
		ResourceInfo& LoadResource(int res_id);
		void SaveResource(int res_id, const unsigned char* data, unsigned int size);
		ResourceInfo& GetResource(int res_id);
		void UnloadAll();

	private:
		std::unordered_map<int, ResourceInfo> res_buff;
	};

	extern ResourceMgr resourceMgr;

}

#endif
