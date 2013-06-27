#include "../common/common.h"
#include "../common/hash.h"
#include "game_window.h"

int main(int argc, char* argv[]) {

	ygopro::mainGame.Initialise(1280, 720);
	ygopro::mainGame.MainLoop(1280, 720);

	return 0;
};
