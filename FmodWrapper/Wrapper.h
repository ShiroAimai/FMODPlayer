#pragma once

#include <string>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <map>
#include <string>

namespace NCWrapper {
	
	struct FMODWrapperResult
	{
		enum class FMODWrapperResultCode {
			OK,
			ERROR
		};
		static FMODWrapperResult From(FMOD_RESULT result);

		FMODWrapperResultCode code;
		std::string msg;

		void Update(FMOD_RESULT result);
		bool IsValid() const;
	};

	class Wrapper
	{
	private:
		struct Media {
			const std::string m_mediaName;
			FMOD::Sound* m_sound;
			int m_channel_id;

			Media();
			Media(const std::string& name, FMOD::Sound* sound);
		};
	public:
		static float MAX_VOLUME;
		static float MIN_VOLUME;
		static float MAX_2D_PAN;
		static float MIN_2D_PAN;
		static int WRAPPER_INVALID_RESOURCE_ID;
		static int INVALID_CHANNEL_ID;

		static 	Wrapper* Init(FMODWrapperResult& result, std::string& absolute_resources_path, int channels);

		int GetTotalNumberOfChannels() const;
		int GetNumberOfAvailableResourcesToPlay() const;
#pragma region FMOD Wrap
		FMODWrapperResult Load(const std::string& media_name);
		FMODWrapperResult LoadStreaming(const std::string& media_name);

		FMODWrapperResult Play(int resource_ID, int channel);
		FMODWrapperResult PlayLoop(int resource_ID, int channel);
		FMODWrapperResult Pause(int resource_ID);
		FMODWrapperResult Stop(int resource_ID);

		FMODWrapperResult SetPan(int resource_ID, float pan); //from -1 left, 0 center, 1 right
		FMODWrapperResult SetVolume(int resource_ID, float volume); //from 0 to 1

		FMODWrapperResult Close();
#pragma endregion
	private:
		Wrapper();
		FMODWrapperResult InitFMODSystem(const std::string& absolute_resources_path, int channels);
		FMODWrapperResult LoadAudio(const std::string& media_name, bool stream);
		FMODWrapperResult PlayAudio(int resource_ID, int channel, bool loop);
		void AddResource(const std::string& media_name, FMOD::Sound* sound);
		FMOD::Channel* GetPlayingChannelFromResourceID(FMODWrapperResult& result, int resource_id);
		FMODWrapperResult ValidateResourceId(const int resource_id) const;
		FMODWrapperResult ValidateResourceOperation() const;

		FMOD::System* m_FMOD_Instance = nullptr;
		
		int m_ResourcesSize = 0;
		//resource_id, media
		std::map<int, Media> m_Resources; //loaded media

		////// CHANNELS ///////
		int m_channel_total = 0;
		FMOD::Channel** m_Channels;

		//// MEDIA ////
		std::string m_absolute_resources_path;
	};
}