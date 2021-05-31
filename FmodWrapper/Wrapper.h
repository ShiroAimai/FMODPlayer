#pragma once

#include <string>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <map>

namespace NCWrapper {
	class Wrapper
	{
	public:
		static float MAX_VOLUME;
		static float MIN_VOLUME;
		static float MAX_2D_PAN;
		static float MIN_2D_PAN;
		static int WRAPPER_INVALID_RESOURCE_ID;

		static Wrapper* Init(const std::string& absolute_resources_path, int channels);

		int GetTotalNumberOfChannels() const;
		int GetNumberOfAvailableResourcesToPlay() const;
#pragma region FMOD Wrap
		void Load(const std::string& media_name);
		void LoadStreaming(const std::string& media_name);

		void Play(int resource_ID, int channel);
		void PlayLoop(int resource_ID, int channel);
		void Pause(int resource_ID);
		void Stop(int resource_ID);

		void SetPan(int resource_ID, float pan); //from -1 left, 0 center, 1 right
		void SetVolume(int resource_ID, float volume); //from 0 to 1

		void Close();
		bool IsSystemInitialized() const;

#pragma endregion
	private:
		Wrapper();
		FMOD_RESULT InitFMODSystem(const std::string& absolute_resources_path, int channels);
		int LoadAudio(const std::string& media_name, bool stream);
		void PlayAudio(int resource_ID, int channel, bool loop);
		int AddResource(FMOD::Sound* sound);
		FMOD::Channel* GetPlayingChannelFromResourceID(const int resource_id);
		bool ValidateResourceId(const int resource_id) const;

		FMOD::System* m_FMOD_Instance = nullptr;
		
		int m_ResourcesSize = 0;
		//resource_id, media name
		std::map<int, std::string> m_ResourcesNames;
		//resource_id, fmod sound ptr
		std::map<int, FMOD::Sound*> m_Resources;
		//resource_id key, channel value
		std::map<int, int> m_ResourcesInPlay;

		////// CHANNELS ///////
		int m_channel_total = 0;
		FMOD::Channel** m_Channels;

		//// MEDIA ////
		std::string m_absolute_resources_path;
	};
}