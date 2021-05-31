#include "pch.h"
#include <iostream>
#include "Wrapper.h"
using std::cout;
using std::endl;

namespace NCWrapper {
	namespace {
		// =================================================
		// UTILITIES
		// =================================================
		bool CheckOperationResult(FMOD_RESULT result) {
			if (result != FMOD_OK)
			{
				printf(FMOD_ErrorString(result));
				printf("\n");
				return false;
			}
			return true;
		}
	}

	Wrapper* Wrapper::Init(const std::string& absolute_resources_path, int channels)
	{
		Wrapper* SystemInitialized = new Wrapper();

		FMOD_RESULT res = SystemInitialized->InitFMODSystem(absolute_resources_path, channels);

		return CheckOperationResult(res) == false ? nullptr : SystemInitialized;
	}

	int Wrapper::WRAPPER_INVALID_RESOURCE_ID = -1;
	float Wrapper::MAX_VOLUME = 100.f;
	float Wrapper::MIN_VOLUME = 0.f;
	float Wrapper::MAX_2D_PAN = 1.f; //rightmost
	float Wrapper::MIN_2D_PAN = -1.f; //leftmost

	int Wrapper::GetTotalNumberOfChannels() const
	{
		return m_channel_total;
	}

	int Wrapper::GetNumberOfAvailableResourcesToPlay() const
	{
		return m_ResourcesSize;
	}

	Wrapper::Wrapper() : m_FMOD_Instance(nullptr), m_Channels(nullptr)
	{
		//make constructor private in order to force call to factory method
	}

	FMOD_RESULT Wrapper::InitFMODSystem(const std::string& absolute_resources_path, int channels)
	{
		FMOD_RESULT result;
		result = FMOD::System_Create(&m_FMOD_Instance);

		if (!CheckOperationResult(result))
		{
			cout << "FMOD Instance initialization failed with error " << endl;
			return result;
		}

		result = m_FMOD_Instance->init(channels, FMOD_INIT_NORMAL, 0);

		if (!CheckOperationResult(result))
		{
			cout << "FMOD Instance initialization failed with error " << endl;
			return result;
		}

		m_Channels = new FMOD::Channel * [channels];
		m_channel_total = channels;

		m_absolute_resources_path = absolute_resources_path;

		return result;
	}

	bool Wrapper::IsSystemInitialized() const
	{
		return m_FMOD_Instance != NULL;
	}

	void Wrapper::Load(const std::string& media_name)
	{
		if (!IsSystemInitialized()) return;

		LoadAudio(media_name, false);
	}

	void Wrapper::LoadStreaming(const std::string& media_name)
	{
		if (!IsSystemInitialized()) return;

		LoadAudio(media_name, true);
	}

	void Wrapper::Play(int resource_ID, int channel)
	{
		if (!IsSystemInitialized()) return;
		if (m_ResourcesSize == 0) return;
		PlayAudio(resource_ID, channel, false);
	}

	void Wrapper::PlayLoop(int resource_ID, int channel)
	{
		if (!IsSystemInitialized()) return;
		if (m_ResourcesSize == 0) return;

		PlayAudio(resource_ID, channel, true);
	}

	void Wrapper::Pause(int resource_ID)
	{
		if (m_ResourcesSize == 0) return;

		FMOD::Channel* ChannelToPause = GetPlayingChannelFromResourceID(resource_ID);
		FMOD_RESULT result = ChannelToPause->setPaused(true);

		if (!CheckOperationResult(result)) {
			cout << "A problem has been detected while pausing resource  " << resource_ID;
			cout << " in channel " << ChannelToPause << endl;
		}
	}

	void Wrapper::Stop(int resource_ID)
	{
		if (m_ResourcesSize == 0) return;

		FMOD::Channel* ChannelToStop = GetPlayingChannelFromResourceID(resource_ID);
		FMOD_RESULT result = ChannelToStop->stop();

		if (!CheckOperationResult(result))
		{
			cout << "A problem has been detected while stopping resource " << resource_ID;
			cout << " in channel " << ChannelToStop << endl;
		}

		m_ResourcesInPlay[resource_ID] = 0;
	}

	void Wrapper::SetPan(int resource_ID, float pan)
	{
		if (m_ResourcesSize == 0) return;
		if (!ValidateResourceId(resource_ID)) return;
		if (pan < MIN_2D_PAN && pan > MAX_2D_PAN)
		{
			cout << "Invalid value for PAN. Please provide a value in the interval [";
			cout << MIN_2D_PAN;
			cout << ",";
			cout << MAX_2D_PAN;
			cout << "]" << endl;
			return;
		}
		FMOD::Channel* channel = GetPlayingChannelFromResourceID(resource_ID);
		FMOD_RESULT result = channel->setPan(pan);

		if (!CheckOperationResult(result))
		{
			cout << "A problem has been detected trying to modify PAN value for resource " << resource_ID;
			cout << " in channel " << channel << endl;
		}
	}

	void Wrapper::SetVolume(int resource_ID, float volume)
	{
		if (m_ResourcesSize == 0) return;
		if (!ValidateResourceId(resource_ID)) return;
		if (volume < MIN_VOLUME && volume > MAX_VOLUME)
		{
			cout << "Invalid value for Volume. Please provide a value in the interval [";
			cout << MIN_VOLUME;
			cout << ",";
			cout << MAX_VOLUME;
			cout << "]" << endl;
			return;
		}
		FMOD::Channel* channel = GetPlayingChannelFromResourceID(resource_ID);
		float normalizedVolume = volume / MAX_VOLUME;
		FMOD_RESULT result = channel->setVolume(normalizedVolume);

		if (!CheckOperationResult(result))
		{
			cout << "A problem has been detected trying to modify volume to " << volume << " for resource  " << resource_ID;
			cout << " in channel " << channel << endl;
		}
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

	int Wrapper::LoadAudio(const std::string& media_name, bool stream)
	{
		if (!IsSystemInitialized()) return WRAPPER_INVALID_RESOURCE_ID;

		FMOD::Sound* sound;
		std::string absolute_file_path = m_absolute_resources_path + media_name;
		FMOD_RESULT result = m_FMOD_Instance->createSound(absolute_file_path.c_str(), stream == true ? FMOD_CREATESTREAM : FMOD_2D, 0, &sound);
		
		if (!CheckOperationResult(result)) return WRAPPER_INVALID_RESOURCE_ID;
		
		m_ResourcesNames[m_ResourcesSize] = media_name;

		return AddResource(sound);
	}

	void Wrapper::PlayAudio(int resource_ID, int channel, bool loop)
	{
		if (!IsSystemInitialized()) return;
		if (!ValidateResourceId(resource_ID)) return;
		if (channel <= 0 || channel > m_channel_total)
		{
			cout << "Invalid channel received in input. Please provide a valid channel" << endl;
			return;
		}

		FMOD::Sound* sound;
		FMOD::Channel* ch;

		FMOD_RESULT result;

		sound = m_Resources[resource_ID];
		sound->setMode(loop == true ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

		ch = m_Channels[channel - 1];
		int play_channel = m_ResourcesInPlay[resource_ID];

		if (play_channel > 0)
		{
			bool playing;
			result = ch->isPlaying(&playing);
			ch->setPaused(false);

			CheckOperationResult(result);
		}
		else
		{
			result = m_FMOD_Instance->playSound(sound, 0, false, &ch);

			if (!CheckOperationResult(result)) return;

			m_Channels[channel - 1] = ch;
			m_ResourcesInPlay[resource_ID] = channel - 1;
		}
	}

	int Wrapper::AddResource(FMOD::Sound* sound)
	{
		if (!IsSystemInitialized()) return WRAPPER_INVALID_RESOURCE_ID;
		
		m_Resources[m_ResourcesSize] = sound;
		m_ResourcesSize++;
		
		return m_ResourcesSize;
	}

	FMOD::Channel* Wrapper::GetPlayingChannelFromResourceID(const int resource_id)
	{
		FMOD::Sound* sound;
		int channel_Id;

		sound = m_Resources[resource_id];
		channel_Id = m_ResourcesInPlay[resource_id];
		return m_Channels[channel_Id];
	}

	bool Wrapper::ValidateResourceId(const int resource_id) const
	{
		bool IsValid = resource_id != WRAPPER_INVALID_RESOURCE_ID && resource_id >= 0 && resource_id < m_ResourcesSize;
		if(!IsValid) 
			cout << "Invalid Resource ID received in input. Please provide a valid id" << endl;
		return IsValid;
	}

}