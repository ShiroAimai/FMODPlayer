#pragma once

#include "pch.h"
#include <iostream>
#include "Wrapper.h"

#define FILENAME_MAX 255

using namespace std;
using namespace NCWrapper;

void ClearConsole()
{
#if defined _WIN32
	system("cls");
	//clrscr(); // including header file : conio.h
#elif defined (LINUX) || defined(gnu_linux) || defined(linux)
	system("clear");
	//std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences 
#elif defined (APPLE)
	system("clear");
#endif
}

void ShowOptions() {
	ClearConsole();
	cout << "Master Computer Game Development 2020/2021 - Sound Programming Assignment by Nicola Cisternino - v.0.0.1" << endl;
	cout << endl;

	cout << "Setup:" << endl;
	cout << "1) Load a new resource" << endl;
	cout << "2) Load a new resource as Stream" << endl;
	cout << "=============" << endl;
	cout << "Media:" << endl;
	cout << "3) Play" << endl;
	cout << "4) Play and loop" << endl;
	cout << "5) Pause" << endl;
	cout << "6) Stop" << endl;
	cout << "7) Set Pan" << endl;
	cout << "8) Set Volume" << endl;
	cout << "=============" << endl;
	cout << "O) Options" << endl;
	cout << "E) Exit" << endl;
}

bool g_system_initialized = false;
bool g_execution_finished = false;

int g_channel_number = 0;
string absoulte_resource_path;
Wrapper* wrapper = nullptr;

bool InitWrapper() 
{
	char c_absolute_path[FILENAME_MAX];

	GetCurrentDir(c_absolute_path, FILENAME_MAX);

	if (absoulte_resource_path.empty())
	{
		absoulte_resource_path = std::string(c_absolute_path);
		absoulte_resource_path += "\\media\\";
	}

	cout << "Press Enter to confirm media absolute path [" << absoulte_resource_path.c_str() << "]" << endl;
	cout << "Or else provide a new path: "; 

	string new_absolute_path;
	getline(cin, new_absolute_path);

	if (!new_absolute_path.empty())
	{
		absoulte_resource_path = new_absolute_path;
	}

	if (g_channel_number == 0)
	{
		g_channel_number = 8;
	}

	cout << "How many channels ? [" << g_channel_number << "]: ";
	string new_max_channels;
	getline(cin, new_max_channels);

	if (!new_max_channels.empty()) {
		g_channel_number = atoi(new_max_channels.c_str());
	}

	wrapper = Wrapper::Init(g_channel_number);

	if(wrapper)
	{
		g_system_initialized = true;
		//SWS->Info(); todo maybe add
		cout << "NC FMOD wrapper initialized successfully!" << endl;
	}

	return wrapper != nullptr;
}
int main() {
	cout << "Initializing NC FMOD wrapper..." << endl;
	cout << endl;

	InitWrapper();
	

	ShowOptions();
	return 0;
}