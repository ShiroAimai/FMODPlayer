#pragma once

#include "pch.h"
#include <iostream>
#include <string>
#include "Wrapper.h"
#include "FMODWrapperResult.h"

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
	cout << "Media:" << endl;
	cout << "1) Load" << endl;
	cout << "2) Load Stream" << endl;
	cout << "3) Play" << endl;
	cout << "4) Play loop" << endl;
	
	std::vector<string> loadedMediaNames;
	wrapper->GetLoadedMediaNames(loadedMediaNames);
	if(loadedMediaNames.size() > 0)
	{
		cout << "=============" << endl;
		cout << "Available Media:" << endl;
		cout << "ID" << " " << "Name" <<  endl;
		for(int i = 0; i < loadedMediaNames.size(); ++i)
			cout << i << " " << loadedMediaNames[i] << endl;
	}
	
	cout << "=============" << endl;
	cout << "Channel:" << endl;
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
	
	FMODWrapperResult result = wrapper->Load(new_media_file_name);
	if (!result.IsValid())
	{
		error = result.msg;
	}
}

void LoadStreaming()
{
	cout << "Insert media file name: ";
	cin.ignore();

	string new_media_file_name;
	getline(cin, new_media_file_name);

	FMODWrapperResult result = wrapper->LoadStreaming(new_media_file_name);
	if (!result.IsValid())
	{
		error = result.msg;
	}
}

void Play(bool ShouldLoop)
{
	if (wrapper->GetNumberOfAvailableResourcesToPlay() > 0) {
		cout << "Media ID: ";
		cin.ignore();

		string new_resource_id;
		getline(cin, new_resource_id);
		int resource_id = 0;

		if (!new_resource_id.empty()) {
			resource_id = atoi(new_resource_id.c_str());
		}

		cout << "Channel [0-" << wrapper->GetTotalNumberOfChannels() << "]: ";

		string new_channels;
		getline(cin, new_channels);
		int channel = -1;

		if (!new_channels.empty()) {
			channel = atoi(new_channels.c_str());
		}

		FMODWrapperResult result;
		if (ShouldLoop)
		{
			result = wrapper->PlayLoop(resource_id, channel);
		}
		else
		{
			result = wrapper->Play(resource_id, channel);
		}
		
		if (!result.IsValid())
		{
			error = result.msg;
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
		cout << "Channel: ";
		cin.ignore();

		string new_resource_id;
		getline(cin, new_resource_id);
		int resource_id = 0;

		if (!new_resource_id.empty()) {
			resource_id = atoi(new_resource_id.c_str());
		}
		
		if(resource_id >= 0)
		{			
			FMODWrapperResult result = wrapper->Pause(resource_id);
			if (!result.IsValid())
			{
				error = result.msg;
			}
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
		cout << "Channel: ";
		cin.ignore();

		string new_resource_id;
		getline(cin, new_resource_id);
		int resource_id = 0;

		if (!new_resource_id.empty()) {
			resource_id = atoi(new_resource_id.c_str());
		}
		
		if (resource_id >= 0)
		{
			FMODWrapperResult result = wrapper->Stop(resource_id);
			if (!result.IsValid())
			{
				error = result.msg;
			}
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
		cout << "Channel: ";
		cin.ignore();

		string inserted_resource_id;
		getline(cin, inserted_resource_id);
		int resource_id = 0;

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

		FMODWrapperResult result = wrapper->SetPan(resource_id, pan);
		if (!result.IsValid())
		{
			error = result.msg;
		}
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
		cout << "Channel: ";
		cin.ignore();
		string inserted_resource_id;
		getline(cin, inserted_resource_id);
		int resource_id = 0;

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

		FMODWrapperResult result = wrapper->SetVolume(resource_id, volume);
		if (!result.IsValid())
		{
			error = result.msg;
		}
	}
	else
	{
		error = "There's no media loaded. Load a media before using this option";
	}
}

bool InitWrapper() 
{
	ClearConsole();
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
	
	int total_channels = NCWrapper::Wrapper::MAX_CHANNELS;

	cout << "How many channels ? [" << NCWrapper::Wrapper::MIN_CHANNELS << "," << NCWrapper::Wrapper::MAX_CHANNELS << "]: ";
	string new_max_channels;
	getline(cin, new_max_channels);

	if (!new_max_channels.empty()) {
		total_channels = atoi(new_max_channels.c_str());
	}

	FMODWrapperResult InitResult = Wrapper::Init(&wrapper, absoulute_resources_path, total_channels);

	if (!InitResult.IsValid())
	{
		error = InitResult.msg;
	}

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
		error = "Invalid command. Please select a valid command";
		break;
	}
	}
}

int main() {
	cout << "Initializing NC FMOD wrapper..." << endl;
	cout << endl;

	while (!g_system_initialized) {
		InitWrapper();
	}

	while (!g_execution_finished)
	{
		ShowOptions();
		char command;
		cin >> command;
		
		cout << endl;
		EvaluateCommand(command);
	}

	return 0;
}