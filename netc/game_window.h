#ifndef _GAME_WINDOW_H_
#define _GAME_WINDOW_H_

#include "../common/common.h"
#include "config_mgr.h"
#include "wx/frame.h"
#include <vector>
#include <queue>

#include "duel_message.h"

namespace ygopro
{

	class GameFrame : public wxFrame, public DuelMessage
	{
	public:
		GameFrame(int sx, int sy);
		~GameFrame();

		virtual void HandleMessage(unsigned int msg, unsigned int size, void* data);

	protected:
		DECLARE_EVENT_TABLE()
	};

	extern GameFrame* mainFrame;
	extern Random globalRandom;
	extern ConfigMgr commonCfg;

}

#endif //_GAME_H_
