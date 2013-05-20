#include "file_receiver.h"
#include "res_mgr.h"

namespace ygopro
{

	FileReceiver::FileReceiver(): dataptr(nullptr) {
	}

	FileReceiver::~FileReceiver() {
		if(!dataptr)
			delete dataptr;
	}

	int FileReceiver::FileReceiveBegin(unsigned int type, unsigned int size) {
		if(dataptr)
			FileReceiveEnd(false);
		resource_id = resourceMgr.GenerateUID();
		dataptr = new unsigned char[size];
		file_receive_size = size;
		file_receive_type = type;
		file_receive_current = 0;
		return resource_id;
	}

	void FileReceiver::FileReceiveAppend(unsigned char* data, unsigned int size) {
		if(!dataptr)
			return;
		if(file_receive_current + size < file_receive_size) {
			memcpy(&dataptr[file_receive_current], data, size);
			file_receive_current += size;
		} else {
			memcpy(&dataptr[file_receive_current], data, file_receive_size - file_receive_current);
			FileReceiveEnd(true);
		}
	}

	bool FileReceiver::FileReceiveEnd(bool save) {
		if(!dataptr)
			return false;
		if(!save) {
			delete dataptr;
			dataptr = nullptr;
			return false;
		}
		resourceMgr.SaveResource(resource_id, dataptr, file_receive_size);
		delete dataptr;
		dataptr = nullptr;
		return true;
	}

}
