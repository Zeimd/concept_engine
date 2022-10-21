
#include <iostream>

#include <ddsplugin.h>

#ifdef _MSC_VER

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#define _CRT_SECURE_NO_WARNINGS
#include <crtdbg.h>

#endif

#endif

int main(int argc,char **argv)
{
#ifdef _MSC_VER
#ifdef _DEBUG
	// Detect memory leaks after main-function ends
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif

	/*
	for (int k = 1; k < argc; k++)
	{
		std::cout << "arg[" << k << "] = " << argv[k] << std::endl;
	}
	*/

	int result = Compress_Texture2D_DXT(".\\door.png", "c:\\projektit\\ddsplugin\\x64\\debug\\.\\a b\\door.dds", DDS_COMPRESS_BC1, 0, DDS_MIPMAP_FILTER_DEFAULT, DDS_MIPMAP_WRAP_DEFAULT);

	std::cout << result << std::endl;

	return 0;
}