#ifndef _EMUBOX_H
#pragma once
class EMUBOX {
	public:
		int Start();
		void Shutdown();
	private:
		int Init();
		void mainLoop();
};
#endif
