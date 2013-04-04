#include "stdafx.h"
#include "Core.h"

int main(int argc, char** argv){
	Core core;
	if (!core.Init())
		return -1;
	core.Run();
	core.Shut();
	return 0;
}