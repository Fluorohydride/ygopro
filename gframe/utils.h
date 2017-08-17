#pragma once
#ifndef UTILS_H
#define UTILS_H

#include "config.h"

namespace ygo {
	class Utils {
	public:
		void initUtils();
		void takeScreenshot(irr::IrrlichtDevice* device);
		void changeCursor(ECURSOR_ICON icon);
	};
	extern Utils utils;
}

#endif //UTILS_H
