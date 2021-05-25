#include "pch.h"
#include <iostream>
#include "Wrapper.h"
using std::cout;
using std::endl;

Wrapper::Wrapper()
{

	myResult = System_Create(&mySystem);
	if (myResult != FMOD_OK)
	{
		cout << "FMOD error:  " << myResult << "  " << FMOD_ErrorString(myResult) << endl;
		exit(-1);
	}
	myResult = mySystem->init(8, FMOD_INIT_NORMAL, NULL);
	if (myResult != FMOD_OK)
	{
		cout << "FMOD error:  " << myResult << "  " << FMOD_ErrorString(myResult) << endl;
		exit(-1);
	}
	
	cout << FMOD_ErrorString(myResult) << endl;
}
