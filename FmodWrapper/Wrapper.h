#pragma once

#include <string>
#include <fmod.hpp>
#include <map>
#include <vector>

namespace NCWrapper {
	struct FMODWrapperResult;
	
	class Wrapper
	{
	private:
		struct NCMedia {
			static int INVALID_MEDIA_ID;
			const std::string m_mediaName;
			FMOD::Sound* m_sound;

			NCMedia();
			NCMedia(const std::string& name, FMOD::Sound* sound);
		};
		struct NCChannel {
			int m_bound_media_id;
			FMOD::Channel* m_channel;

			NCChannel();
			void ResetMedia();
		};
	public:
		static float MAX_VOLUME;
		static float MIN_VOLUME;
		static float MAX_2D_PAN;
		static float MIN_2D_PAN;
		static int MIN_CHANNELS;
		static int MAX_CHANNELS;

		static FMODWrapperResult Init(Wrapper** OutWrapperInstance, std::string& absolute_resources_path, int channels);

		int GetTotalNumberOfChannels() const;
		int GetNumberOfAvailableResourcesToPlay() const;
		void GetLoadedMediaNames(std::vector <std::string>& loaded_media_names);
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
		FMODWrapperResult ValidateResourceId(const int resource_id) const;
		FMODWrapperResult ValidateChannel(const int channel) const;
		FMODWrapperResult ValidateResourceOperation() const;

		FMOD::System* m_FMOD_Instance = nullptr;
		
		//// LOADED MEDIA ////
		std::vector<NCMedia> m_Resources;

		//// CHANNEL////
		std::vector<NCChannel> m_Channels;

		//// MEDIA PATH////
		std::string m_absolute_resources_path;
	};
}