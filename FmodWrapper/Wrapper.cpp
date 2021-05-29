#include "pch.h"
#include <iostream>
#include "Wrapper.h"
using std::cout;
using std::endl;

namespace NCWrapper {
	// =================================================
	// UTILITIES
	// =================================================
	static bool CheckOperationResult(FMOD_RESULT result) {
		if (result != FMOD_OK)
		{
			printf(FMOD_ErrorString(result));
			printf("\n");
			return false;
		}
		return true;
	}

	Wrapper* Wrapper::Init(int channels)
	{
		Wrapper* SystemInitialized = new Wrapper();

		FMOD_RESULT res = SystemInitialized->InitFMODSystem(channels);

		return CheckOperationResult(res) == false ? nullptr : SystemInitialized;
	}

	Wrapper::Wrapper() : m_FMOD_Instance(nullptr), m_Channels(nullptr), m_FreeChannels(0)
	{
		//make constructor private in order to force call to factory method
	}

	FMOD_RESULT Wrapper::InitFMODSystem(int channels)
	{
		FMOD_RESULT result;
		result = FMOD::System_Create(&m_FMOD_Instance);

		if (!CheckOperationResult(result))
		{
			cout << "FMOD Instance initialization failed with error " << result << endl;
			return result;
		}

		m_FMOD_Instance->init(channels, FMOD_INIT_NORMAL, 0);

		m_Channels = new FMOD::Channel * [channels];
		m_FreeChannels = channels;

		return result;
	}

	bool Wrapper::IsSystemInitialized() const
	{
		return m_FMOD_Instance != NULL;
	}

	int Wrapper::Load(const std::string& media_absolute_path)
	{
		
	}

	int Wrapper::LoadStreaming(const std::string& media_absolute_path)
	{

	}

	int Wrapper::Play(int resource_ID, int channel)
	{

	}

	int Wrapper::PlayLoop(int resource_ID, int channel)
	{

	}

	int Wrapper::Pause(int resource_ID)
	{

	}

	int Wrapper::Stop(int resource_ID)
	{

	}

	int Wrapper::SetPan(int resource_ID, float pan)
	{

	}

	int Wrapper::SetVolume(int resource_ID, float volume)
	{

	}

	void Wrapper::Close()
	{
		FMOD_RESULT result;
		result = m_FMOD_Instance->release();

		if (!CheckOperationResult(result))
		{
			cout << "An error has been detected releasing FMOD instance E: " << result << endl;
		}
	}

}