#include "logging.h"
#include <ctime>
#include <fstream>

namespace ygo {

void ErrorLog(const std::string& msg) {
	std::ofstream log("error.log", std::ofstream::app);
	if (!log.is_open())
		return;
	time_t nowtime = time(NULL);
	tm* localedtime = localtime(&nowtime);
	char timebuf[40];
	strftime(timebuf, 40, "%Y-%m-%d %H:%M:%S", localedtime);
	log << "[" << timebuf << "]" << msg << std::endl;
	log.close();
}

}
