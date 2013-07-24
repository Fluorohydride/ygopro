#ifndef _CLICKABLE_H_
#define _CLICKABLE_H_

namespace ygopro
{

	class ClickableObject {
	public:
		ClickableObject* parent;
		unsigned int lx;
		unsigned int ly;
		unsigned int rx;
		unsigned int ry;

	};

}

#endif