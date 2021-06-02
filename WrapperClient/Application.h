#pragma once

#include <string>
#include "Wrapper.h"

class Application
{
	enum class State {
		INSERT_COMMAND,
		SHOW
	};
private:
	NCWrapper::Wrapper* m_wrapper = nullptr;
	bool system_initialized = false;
	bool execution_finished = false;
	std::string error;
	State m_state = State::SHOW;

public:
	static int InsertCommandKey;// = 32 spacebar
	static int QuitKey; // = Esc

	bool Init();
	void Update();
	bool IsTerminated() const;

	void Release();

private:
	void LoadMedia();
	void LoadMediaStreaming();
	void Play(bool ShouldLoop);
	void UpdatePause();
	void Stop();
	void SetPan();
	void SetVolume();
	
	//Evaluate Input
	void EvaluateInput(const std::string& Command);
	void ResetAppState();

	//CONSOLE DISPLAY
	void Render();
	void ShowUpdates();
	void ShowInsertCommandOptions();
	void ShowHeader() const;
	void ShowErrors();
	void ShowMediaOptions() const;
	void ShowMedia() const;
	void ShowChannelOptions() const;
	void ShowChannelsState() const;
	void ShowChannelsId() const;
	void ShowCommons() const;
};

