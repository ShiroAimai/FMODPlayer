#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>

class Wrapper
{
public:
	Wrapper();
private:

	FMOD::System* mySystem;
	FMOD_RESULT myResult;

};

