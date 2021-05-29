#pragma once

#include <string>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <map>

namespace NCWrapper {
	class Wrapper
	{
	public:
		static Wrapper* Init(int channels);

#pragma region FMOD Wrap
		int Load(const std::string& media_absolute_path);
		int LoadStreaming(const std::string& media_absolute_path);

		int Play(int resource_ID, int channel);
		int PlayLoop(int resource_ID, int channel);
		int Pause(int resource_ID);
		int Stop(int resource_ID);

		int SetPan(int resource_ID, float pan);
		int SetVolume(int resource_ID, float volume);

		void Close();
		bool IsSystemInitialized() const;

#pragma endregion
	private:
		Wrapper();
		FMOD_RESULT InitFMODSystem(int channels);


		FMOD::System* m_FMOD_Instance = nullptr;
		std::map<int, FMOD::Sound*> m_Resources;
		std::map<int, int> m_ChannelsMap;

		////// CHANNELS ///////
		int m_FreeChannels = 0;
		FMOD::Channel** m_Channels;
	};
}