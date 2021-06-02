#pragma once

#include "pch.h"
#include "Application.h"

int main() {
	std::cout << "Initializing NC FMOD wrapper..." << std::endl << std::endl;
	Application app;

	bool IsInitialized = false;
	while (!IsInitialized) {
		IsInitialized = app.Init();
	}

	while (!app.IsTerminated())
	{
		app.Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	app.Release();
	return 0;
}