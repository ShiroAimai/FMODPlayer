#pragma once

#include "pch.h"
#include <iostream>
#include <string>
#include "Wrapper.h"

#define PATH_MAX_LENGTH 255

using std::cout;
using std::endl;
using std::cin;
using std::string;

using namespace NCWrapper;

bool g_system_initialized = false;
bool g_execution_finished = false;
string error;

Wrapper* wrapper = nullptr;

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
	if (!error.empty()) 
	{
		cout << "ERROR: " << error << endl;
		cout << endl;
		error.clear();
	}
	cout << "Setup:" << endl;
	cout << "1) Load a new media" << endl;
	cout << "2) Load a new media as Stream" << endl;
	cout << "=============" << endl;
	cout << "Media:" << endl;
	cout << "3) Play" << endl;
	cout << "4) Play loop" << endl;
	cout << "5) Pause" << endl;
	cout << "6) Stop" << endl;
	cout << "7) Pan" << endl;
	cout << "8) Volume" << endl;
	cout << "=============" << endl;
	cout << "E) Exit" << endl;
	cout << endl;
	cout << "What do you want to do? ";
}

void Load()
{
	cout << "Insert media file name: ";
	cin.ignore();

	string new_media_file_name;
	getline(cin, new_media_file_name);
	
	wrapper->Load(new_media_file_name);
}

void LoadStreaming()
{
	cout << "Insert media file name: ";
	cin.ignore();

	string new_media_file_name;
	getline(cin, new_media_file_name);

	wrapper->LoadStreaming(new_media_file_name);
}

void Play(bool ShouldLoop)
{
	if (wrapper->GetNumberOfAvailableResourcesToPlay() > 0) {
		cout << "Resource ID: ";
		cin.ignore();

		string new_resource_id;
		getline(cin, new_resource_id);
		int resource_id = NCWrapper::Wrapper::WRAPPER_INVALID_RESOURCE_ID;

		if (!new_resource_id.empty()) {
			resource_id = atoi(new_resource_id.c_str());
		}

		cout << "Channel [1-" << wrapper->GetTotalNumberOfChannels() << "]: ";


		string new_channels;
		getline(cin, new_channels);
		int channel = -1;

		if (!new_channels.empty()) {
			channel = atoi(new_channels.c_str());
		}

		if (ShouldLoop)
		{
			wrapper->PlayLoop(resource_id, channel);
		}
		else
		{
			wrapper->Play(resource_id, channel);
		}
	}
	else 
	{
		error = "There's no media loaded. Load a media before using this option";
	}
}



void Pause()
{
	if (wrapper->GetNumberOfAvailableResourcesToPlay() > 0)
	{
		cout << "Resource ID: ";
		cin.ignore();

		string new_resource_id;
		getline(cin, new_resource_id);
		int resource_id = NCWrapper::Wrapper::WRAPPER_INVALID_RESOURCE_ID;

		if (!new_resource_id.empty()) {
			resource_id = atoi(new_resource_id.c_str());
		}
		
		if(resource_id >= 0)
		{
			wrapper->Pause(resource_id);
		}
		else {
			error = "Invalid resource id. Please provide a valid resource id";
		}
	}
	else 
	{
		error = "There's no media loaded. Load a media before using this option";
	}
}

void Stop()
{
	if (wrapper->GetNumberOfAvailableResourcesToPlay() > 0)
	{
		cout << "Resource ID: ";
		cin.ignore();

		string new_resource_id;
		getline(cin, new_resource_id);
		int resource_id = NCWrapper::Wrapper::WRAPPER_INVALID_RESOURCE_ID;

		if (!new_resource_id.empty()) {
			resource_id = atoi(new_resource_id.c_str());
		}
		
		if (resource_id >= 0)
		{
			wrapper->Stop(resource_id);
		}
		else
		{
			error = "Invalid resource id. Please provide a valid resource id";
		}
	}
	else 
	{
		error = "There's no media loaded. Load a media before using this option";
	}
}

void SetPan()
{
	if (wrapper->GetNumberOfAvailableResourcesToPlay() > 0)
	{
		cout << "Resource ID: ";
		cin.ignore();

		string inserted_resource_id;
		getline(cin, inserted_resource_id);
		int resource_id = NCWrapper::Wrapper::WRAPPER_INVALID_RESOURCE_ID;

		if (!inserted_resource_id.empty()) {
			resource_id = atoi(inserted_resource_id.c_str());
		}

		cout << "Pan [";
		cout << NCWrapper::Wrapper::MIN_2D_PAN << ",";
		cout << NCWrapper::Wrapper::MAX_2D_PAN << "]: ";

		string new_pan;
		getline(cin, new_pan);
		float pan = 0.f;

		if (!new_pan.empty()) {
			pan = atof(new_pan.c_str());
		}

		wrapper->SetPan(resource_id, pan);
	}
	else 
	{
		error = "There's no media loaded. Load a media before using this option";
	}
}


void SetVolume()
{
	if(wrapper->GetNumberOfAvailableResourcesToPlay() > 0)
	{
		cout << "Resource ID: ";
		cin.ignore();
		string inserted_resource_id;
		getline(cin, inserted_resource_id);
		int resource_id = NCWrapper::Wrapper::WRAPPER_INVALID_RESOURCE_ID;

		if (!inserted_resource_id.empty()) {
			resource_id = atoi(inserted_resource_id.c_str());
		}

		cout << "Volume [";
		cout << NCWrapper::Wrapper::MIN_VOLUME << ",";
		cout << NCWrapper::Wrapper::MAX_VOLUME << "]: ";

		string new_volume;
		getline(cin, new_volume);
		float volume = 0;
		if (!new_volume.empty()) {
			volume = atof(new_volume.c_str());
		}

		wrapper->SetVolume(resource_id, volume);
	}
	else
	{
		error = "There's no media loaded. Load a media before using this option";
	}
}

bool InitWrapper() 
{
	char c_absolute_path[PATH_MAX_LENGTH];

	GetCurrentDir(c_absolute_path, PATH_MAX_LENGTH);

	string absoulute_resources_path;
	if (absoulute_resources_path.empty())
	{
		absoulute_resources_path = std::string(c_absolute_path);
		absoulute_resources_path += "\\media\\";
	}

	cout << "Press Enter to confirm media absolute path [" << absoulute_resources_path.c_str() << "]" << endl;
	cout << "Or else provide a new path: "; 

	string new_absolute_path;
	getline(cin, new_absolute_path);

	if (!new_absolute_path.empty())
	{
		absoulute_resources_path = new_absolute_path;
	}
	
	int total_channels = 0;

	if (total_channels == 0)
	{
		total_channels = 8;
	}

	cout << "How many channels ? [" << total_channels << "]: ";
	string new_max_channels;
	getline(cin, new_max_channels);

	if (!new_max_channels.empty()) {
		total_channels = atoi(new_max_channels.c_str());
	}

	wrapper = Wrapper::Init(absoulute_resources_path, total_channels);

	if(wrapper)
	{
		g_system_initialized = true;
		cout << "NC FMOD wrapper initialized successfully!" << endl;
	}

	return wrapper != nullptr;
}

void EvaluateCommand(const char& Command) {
	switch (Command) {
	case 49: {		
		cout << "Load a new media" << endl;
		Load();
		break;
	}
	case 50: {
		cout << "Load a new media as Stream" << endl;
		LoadStreaming();
		break;
	}
	case 51: {		
		cout << "Play media" << endl;
		Play(false);
		break;
	}
	case 52: {
		cout << "Play media in loop" << endl;
		Play(true);
		break;
	}
	case 53: {		
		cout << "Pausing media" << endl;
		Pause();
		break;
	}
	case 54: {		
		cout << "Stopping media" << endl;
		Stop();
		break;
	}
	case 55: {	
		cout << "Set Pan" << endl;
		SetPan();
		break;
	}
	case 56: {		
		cout << "Set Volume" << endl;;
		SetVolume();
		break;
	}
	case 69:
	case 101: {
		cout << "EXITING...\n";
		wrapper->Close();
		g_execution_finished = true;
		break;
	}
	default: {
		cout << "Invalid command" << "Code: " << (int)Command << endl;
		break;
	}
	}
}

int main() {
	cout << "Initializing NC FMOD wrapper..." << endl;
	cout << endl;

	InitWrapper();

	while (g_system_initialized && !g_execution_finished)
	{
		ShowOptions();
		char command;
		cin >> command;
		
		cout << endl;
		EvaluateCommand(command);
	}

	return 0;
}