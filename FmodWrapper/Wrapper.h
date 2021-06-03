#pragma once

#include <fmod.hpp>
#include <string>
#include <vector>
#include <memory>

namespace NCWrapper {
	struct FMODWrapperResult;
	struct MediaState;
	struct ChannelState;
	struct MasterGroupState;

	class Wrapper
	{
	private:
		struct NCMedia {
			static int INVALID_MEDIA_ID;
			const std::string m_mediaName;
			std::shared_ptr<FMOD::Sound> m_sound;
			bool m_isStream;
			bool m_isLooping;

			NCMedia();
			NCMedia(const std::string& name, bool isStream, FMOD::Sound* sound);
		};
		struct NCChannel {
			int m_bound_media_id;
			float m_channel_pan; //clamped by the container class in the interval [-1, 1]
			FMOD::Channel* m_channel;

			NCChannel();
			~NCChannel();
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
		void GetLoadedMedia(std::vector <MediaState>& OutLoadedMedia) const;;
		void GetAllChannelsState(std::vector<ChannelState>& channelsState) const;
		void GetMasterGroupState(MasterGroupState& state) const;
		~Wrapper();
#pragma region FMOD Wrap
		//MEDIA OPERATIONS
		FMODWrapperResult Load(const std::string& media_name);
		FMODWrapperResult LoadStreaming(const std::string& media_name);
		FMODWrapperResult Play(int resource_ID, int channel_id);
		FMODWrapperResult PlayLoop(int resource_ID, int channel_id);
		//SINGLE CHANNEL OPERATIONS
		FMODWrapperResult UpdateMute(int channel_id);
		FMODWrapperResult UpdatePause(int channel_id);
		FMODWrapperResult Stop(int channel_id);
		FMODWrapperResult SetPan(int channel_id, float pan); //from -1 left, 0 center, 1 right
		FMODWrapperResult SetVolume(int channel_id, float volume); //from 0 to 1
		//MULTI CHANNEL OPERATIONS
		FMODWrapperResult UpdateMuteForAllChannels();
		FMODWrapperResult UpdatePauseForAllChannels();
		FMODWrapperResult StopAllChannels();
		FMODWrapperResult SetVolumeForAllChannels(float volume);

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
		FMODWrapperResult ValidateVolumeValue(const float value) const;

		FMOD::System* m_FMOD_Instance = nullptr;
		FMOD::ChannelGroup* m_master = nullptr;

		//// LOADED MEDIA ////
		std::vector<NCMedia> m_Resources;

		//// CHANNEL////
		std::vector<NCChannel> m_Channels;

		//// MEDIA PATH////
		std::string m_absolute_resources_path;
	};
}