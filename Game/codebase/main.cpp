#include "stdafx.h"
#include "Core.h"
#ifdef _DEBUG
//#include "vld.h"	// Visual Leak Detector, for debugging memory leaks. If you don't have vld installed simply comment this include out to compile.
#endif



int main(int argc, char** argv){
	Core core;
	if (!core.Init())
		return -1;
	core.Run();
	core.Shut();
	return 0;
}