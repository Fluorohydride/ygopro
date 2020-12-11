#include "logging.h"
#include <ctime>
#include <fstream>
#include <fmt/chrono.h>

namespace ygo {

void ErrorLog(epro::stringview msg) {
	std::ofstream log("error.log", std::ofstream::app);
	if (!log.is_open())
		return;
	auto now = std::time(nullptr);
	log << fmt::format("[{:%Y-%m-%d %H:%M:%S}] {}", *std::localtime(&now), msg) << std::endl;
}

}
