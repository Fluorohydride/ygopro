#include "../common/common.h"
#include "../common/hash.h"
#include "game_window.h"

int main(int argc, char* argv[]) {

	ygopro::mainGame.Initialise();
	ygopro::mainGame.MainLoop();

	return 0;
};
