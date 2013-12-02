#ifndef _GAME_WINDOW_H_
#define _GAME_WINDOW_H_

#include "wx/frame.h"
#include "xml_config.h"
#include "duel_message.h"
#include "../common/random.h"
#include <vector>
#include <queue>

namespace ygopro
{

	class GameFrame : public wxFrame, public DuelMessage
	{

		enum
		{
			ID_RESERVE = wxID_HIGHEST + 1,
		};

	public:
		GameFrame(int sx, int sy);
		~GameFrame();

		virtual void HandleMessage(unsigned int msg, unsigned int size, void* data);

	protected:
		DECLARE_EVENT_TABLE()
	};

	extern GameFrame* mainFrame;
    extern Random globalRandom;
	extern CommonConfig commonCfg;
    extern CommonConfig stringCfg;
}

#endif //_GAME_H_
