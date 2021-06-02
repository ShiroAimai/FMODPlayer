#include "pch.h"
#include "FMODWrapperResult.h"
#include "ChannelState.h"
#include <algorithm>
#include "Application.h"

#define PATH_MAX_LENGTH 255

using namespace NCWrapper;
using std::cout;
using std::endl;
using std::cin;
using std::string;

namespace {
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

	bool is_number(const std::string& s) {
		return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
	}

}

int Application::InsertCommandKey = 32;
int Application::QuitKey = 69;

bool Application::Init()
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

	cout << "Press [Enter] to confirm media path [" << absoulute_resources_path.c_str() << "]" << endl;
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

	FMODWrapperResult InitResult = Wrapper::Init(&m_wrapper, absoulute_resources_path, total_channels);

	if (!InitResult.IsValid())
	{
		error = InitResult.msg;
	}

	if (m_wrapper)
	{
		system_initialized = true;
		cout << "NC FMOD wrapper initialized successfully!" << endl;
	}

	return m_wrapper != nullptr;
}

void Application::Update()
{
	if(_kbhit()) 
	{
		int key = _getch();
		if (key == QuitKey)
		{
			execution_finished = true;
			return;
		}
		m_state = key == InsertCommandKey ? State::INSERT_COMMAND : State::SHOW;
	}
	
	Render();
}

bool Application::IsTerminated() const
{
	return execution_finished;
}

void Application::Release()
{
	m_wrapper->Close();	
}

void Application::LoadMedia()
{
	cout << "Insert media file name: ";

	string new_media_file_name;
	getline(cin, new_media_file_name);

	FMODWrapperResult result = m_wrapper->Load(new_media_file_name);
	if (!result.IsValid())
	{
		error = result.msg;
	}
}

void Application::LoadMediaStreaming()
{
	cout << "Insert media file name: ";

	string new_media_file_name;
	getline(cin, new_media_file_name);

	FMODWrapperResult result = m_wrapper->LoadStreaming(new_media_file_name);
	if (!result.IsValid())
	{
		error = result.msg;
	}
}

void Application::Play(bool ShouldLoop)
{
	if (m_wrapper->GetNumberOfAvailableResourcesToPlay() > 0) {
		cout << "Media ID: ";

		string new_resource_id;
		getline(cin, new_resource_id);
		int resource_id = 0;

		if (!new_resource_id.empty()) {
			resource_id = atoi(new_resource_id.c_str());
		}

		cout << "Channel ID: ";

		string new_channels;
		getline(cin, new_channels);
		int channel = -1;

		if (!new_channels.empty()) {
			channel = atoi(new_channels.c_str());
		}

		FMODWrapperResult result;
		if (ShouldLoop)
		{
			result = m_wrapper->PlayLoop(resource_id, channel);
		}
		else
		{
			result = m_wrapper->Play(resource_id, channel);
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

void Application::Pause()
{
	if (m_wrapper->GetNumberOfAvailableResourcesToPlay() > 0)
	{
		cout << "Channel ID: ";

		string inserted_channe_id;
		getline(cin, inserted_channe_id);
		int channel_id = -1;

		if (!inserted_channe_id.empty()) {
			channel_id = atoi(inserted_channe_id.c_str());
		}

		FMODWrapperResult result = m_wrapper->Pause(channel_id);
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

void Application::Stop()
{
	if (m_wrapper->GetNumberOfAvailableResourcesToPlay() > 0)
	{
		cout << "Channel ID: ";

		string inserted_channel_Id;
		getline(cin, inserted_channel_Id);
		int channel_id = -1;

		if (!inserted_channel_Id.empty()) {
			channel_id = atoi(inserted_channel_Id.c_str());
		}

		FMODWrapperResult result = m_wrapper->Stop(channel_id);		if (!result.IsValid())
		{
			error = result.msg;
		}
	}
	else
	{
		error = "There's no media loaded. Load a media before using this option";
	}
}

void Application::SetPan()
{
	if (m_wrapper->GetNumberOfAvailableResourcesToPlay() > 0)
	{
		cout << "Channel ID: ";

		string inserted_channel_id;
		getline(cin, inserted_channel_id);
		int channel_id = -1;

		if (!inserted_channel_id.empty()) {
			channel_id = atoi(inserted_channel_id.c_str());
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

		FMODWrapperResult result = m_wrapper->SetPan(channel_id, pan);
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

void Application::SetVolume()
{
	if (m_wrapper->GetNumberOfAvailableResourcesToPlay() > 0)
	{
		cout << "Channel ID: ";
		string inserted_channel_id;
		getline(cin, inserted_channel_id);
		int channel_id = -1;

		if (!inserted_channel_id.empty()) {
			channel_id = atoi(inserted_channel_id.c_str());
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

		FMODWrapperResult result = m_wrapper->SetVolume(channel_id, volume);
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

void Application::EvaluateInput(const string& Command)
{	
	int CommandKey = is_number(Command) ? stoi(Command) : -1;
	switch (CommandKey) {
	case 1: {
		LoadMedia();
		break;
	}
	case 2: {
		LoadMediaStreaming();
		break;
	}
	case 3: {
		Play(false);
		break;
	}
	case 4: {
		Play(true);
		break;
	}
	case 5: {
		Pause();
		break;
	}
	case 6: {
		Stop();
		break;
	}
	case 7: {
		SetPan();
		break;
	}
	case 8: {
		SetVolume();
		break;
	}
	default: {
		error = "Invalid command. Please select a valid command";
		break;
	}
	}

	ResetAppState();
}

void Application::ResetAppState()
{
	m_state = State::SHOW;
}

void Application::Render()
{
	if (m_state == State::SHOW)
	{
		ShowUpdates();
	}
	else
	{
		error.clear();
		ShowInsertCommandOptions();
		cout << endl;
		cout << "Insert a command: ";

		string command;
		getline(cin, command);
		EvaluateInput(command);
	}
}

void Application::ShowUpdates()
{
	ClearConsole();
	ShowHeader();
	cout << endl;
	ShowErrors();
	cout << "=============" << endl;
	ShowMedia();
	cout << "=============" << endl;
	ShowChannelsState();
	cout << "=============" << endl;
	ShowCommons();
}

void Application::ShowInsertCommandOptions()
{
	ClearConsole();
	ShowHeader();
	cout << "=============" << endl;
	ShowMedia();
	cout << endl;
	ShowMediaOptions();
	cout << "=============" << endl;
	ShowChannelsId();
	cout << endl;
	ShowChannelOptions();
	cout << "=============" << endl;
}

void Application::ShowHeader() const
{
	cout << "Master Computer Game Development 2020/2021 - Sound Programming Assignment by Nicola Cisternino - v.0.0.1" << endl;
	cout << endl;
}

void Application::ShowErrors()
{
	if (!error.empty())
	{
		cout << "ERROR: " << error << endl;
		cout << endl;
	}
}

void Application::ShowMediaOptions() const
{
	cout << "Media Options:" << endl;
	cout << "[1] Load" << endl;
	cout << "[2] Load Stream" << endl;
	cout << "[3] Play" << endl;
	cout << "[5] Play loop" << endl;
}

void Application::ShowMedia() const
{
	cout << "Available Media:" << endl;
	cout << "ID" << " | " << "Name" << endl;
	std::vector<string> loadedMediaNames;
	m_wrapper->GetLoadedMediaNames(loadedMediaNames);
	if (loadedMediaNames.size() > 0)
	{
		for (int i = 0; i < loadedMediaNames.size(); ++i)
			cout << i << " | " << loadedMediaNames[i] << endl;
	}
}

void Application::ShowChannelOptions() const
{
	cout << "Channel Options:" << endl;
	cout << "[5] Pause" << endl;
	cout << "[6] Stop" << endl;
	cout << "[7] Pan" << endl;
	cout << "[8] Volume" << endl;
}

void Application::ShowChannelsState() const
{
	std::vector<ChannelState> availableChannelsState;
	m_wrapper->GetAllChannelsState(availableChannelsState);
	if (availableChannelsState.size() > 0)
	{
		cout << "Channels State:" << endl;
		cout << "ID" << " | " << "Media ID" << " | " << " Time" << " | ";
		cout << "Pan" << " | " << "Volume" << " | ";
		cout << "State" << endl;

		for (const ChannelState& channelState : availableChannelsState)
		{
			cout << channelState.channelId << " | " << channelState.mediaId << " | ";
			cout << channelState.mediaCurrentTime << "/" << channelState.mediaTotalTime << " | ";
			cout << channelState.pan << " | " << channelState.volume << " | ";
			cout << ChannelState::to_string(channelState.playingState) << endl;
		}
	}
}

void Application::ShowChannelsId() const
{
	std::vector<ChannelState> availableChannelsState;
	m_wrapper->GetAllChannelsState(availableChannelsState);
	if (availableChannelsState.size() > 0)
	{
		cout << "Available Channels ID: [";
		for (int i = 0; i < availableChannelsState.size(); ++i)
		{
			cout << availableChannelsState[i].channelId;
			if (i == (availableChannelsState.size() - 1))
				cout << "]" << endl;
			else 
				cout << ",";
		}
	}
}

void Application::ShowCommons() const
{
	cout << "Press [SPACE BAR] to insert a command!" << endl;
	cout << "Press [E] to quit!" << endl;
}