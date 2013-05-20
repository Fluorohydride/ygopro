#include "res_mgr.h"

namespace ygopro
{

	ResourceMgr resourceMgr;

	ResourceMgr::ResourceMgr() {

	}

	ResourceMgr::~ResourceMgr() {
		for(auto& iter : res_buff) {
			if(iter.second.data)
				delete iter.second.data;
		}
	}

	int ResourceMgr::GenerateUID() {
		int id = globalRandom.GetUInt32();
		while(res_buff.find(id) != res_buff.end())
			id = globalRandom.GetUInt32();
		return id;
	}

	ResourceInfo& ResourceMgr::LoadResource(int res_id) {
		char res_name[64];
		sprintf(res_name, "./_res/%02x/%08x", (res_id >> 24) & 0xff, res_id);
		FILE* fp = fopen(res_name, "rb+");
		ResourceInfo& ri = res_buff[res_id];
		if(fp == NULL) {
			ri.data = nullptr;
			ri.size = 0;
			return ri;
		}
		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);
		ri.data = new unsigned char[size];
		ri.size = (unsigned int)size;
		fseek(fp, 0, SEEK_SET);
		fread(ri.data, size, 1, fp);
		fclose(fp);
		return ri;
	}

	void ResourceMgr::SaveResource(int res_id, const unsigned char* data, unsigned int size) {
		char res_name[64];
		sprintf(res_name, "./_res/%02x/%08x", (res_id >> 24) & 0xff, res_id);
		FILE* fp = fopen(res_name, "wb+");
		if(fp == NULL)
			return;
		fwrite(data, size, 1, fp);
		fclose(fp);
	}

	ResourceInfo& ResourceMgr::GetResource(int res_id) {
		auto iter = res_buff.find(res_id);
		if(iter == res_buff.end()) {
			return LoadResource(res_id);
		} else {
			return iter->second;
		}
	}

	void ResourceMgr::UnloadAll() {
		for(auto& iter : res_buff) {
			if(iter.second.data)
				delete iter.second.data;
		}
		res_buff.clear();
	}

}
