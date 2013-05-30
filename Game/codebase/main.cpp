#include "stdafx.h"
#include "Core.h"
#ifdef _DEBUG
//#include "vld.h"	// Visual Leak Detector, for debugging memory leaks. If you don't have vld installed simply comment this include out to compile.
#endif

// comment out this part to compile with console window and set project properites accordingly
//#define NO_CONSOLE

#ifdef NO_CONSOLE
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char** argv)
#endif
{
	Core core;
	if (!core.Init()){
		core.Shut();
		return -1;
	}
	core.Run();
	core.Shut();
	return 0;
}