#ifndef FILE_RECEIVER_H
#define FILE_RECEIVER_H

#include "../common/common.h"

namespace ygopro
{

	class FileReceiver {
	public:
		FileReceiver();
		~FileReceiver();

		int FileReceiveBegin(unsigned int type, unsigned int size);
		void FileReceiveAppend(unsigned char* data, unsigned int size);
		virtual bool FileReceiveEnd(bool save = true);

	protected:
		unsigned int file_receive_type;
		unsigned int file_receive_size;
		unsigned int file_receive_current;
		int resource_id;
		unsigned char* dataptr;
	};

}

#endif
