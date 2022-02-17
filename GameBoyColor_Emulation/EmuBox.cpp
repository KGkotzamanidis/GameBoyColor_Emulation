#include "EmuBox.h"

int EMUBOX::Start() {
	Init();
	mainLoop();
	return 0;
}