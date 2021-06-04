#include "pch.h"
#include <iostream>
#include "Wrapper.h"
#include "FMODWrapperResult.h"
#include "ChannelState.h"
#include "MediaState.h"
#include "MasterGroupState.h"

using std::cout;
using std::endl;

namespace NCWrapper {
#pragma region NCMedia
	int Wrapper::NCMedia::INVALID_MEDIA_ID = -1;

	Wrapper::NCMedia::NCMedia() : 
		m_mediaName(""), 
		m_sound(nullptr), 
		m_isStream(false),
		m_isLooping(false)
	{

	}

	Wrapper::NCMedia::NCMedia(const std::string& name,bool isStream, FMOD::Sound* sound) :
		m_mediaName(name),
		m_sound(sound),
		m_isStream(isStream),
		m_isLooping(false)
	{

	}

#pragma endregion
#pragma region NCChannel
	Wrapper::NCChannel::NCChannel() :
		m_bound_media_id(NCMedia::INVALID_MEDIA_ID),
		m_channel(nullptr),
		m_channel_pan(0.f)
	{

	}

	Wrapper::NCChannel::~NCChannel()
	{
		delete m_channel;
	}

	void Wrapper::NCChannel::ResetMedia()
	{
		m_bound_media_id = NCMedia::INVALID_MEDIA_ID;
	}
#pragma endregion

	FMODWrapperResult Wrapper::Init(Wrapper** OutWrapperInstance, std::string& absolute_resources_path, int channels)
	{
		FMODWrapperResult result;
		if (channels < MIN_CHANNELS || channels > MAX_CHANNELS)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Invalid desired Channel. Insert a number in the interval ["
				+ std::to_string(MIN_CHANNELS)
				+ ","
				+ std::to_string(MAX_CHANNELS)
				+ "]");
			return result;
		}

		Wrapper* SystemInitialized = new Wrapper();
		*OutWrapperInstance = SystemInitialized;

		result = SystemInitialized->InitFMODSystem(absolute_resources_path, channels);

		return result;
	}

	float Wrapper::MAX_VOLUME = 100.f;
	float Wrapper::MIN_VOLUME = 0.f;
	float Wrapper::MAX_2D_PAN = 1.f; //rightmost
	float Wrapper::MIN_2D_PAN = -1.f; //leftmost
	int Wrapper::MIN_CHANNELS = 1;
	int Wrapper::MAX_CHANNELS = 8;

	int Wrapper::GetTotalNumberOfChannels() const
	{
		return m_Channels.size();
	}

	int Wrapper::GetNumberOfAvailableResourcesToPlay() const
	{
		return m_Resources.size();
	}

	void Wrapper::GetLoadedMedia(std::vector <MediaState>& OutLoadedMedia) const
	{
		for (auto& media : m_Resources)
		{ 
			MediaState state;
			state.mediaName = media.m_mediaName;
			state.isStream = media.m_isStream;
			OutLoadedMedia.push_back(state);
		}
	}

	void Wrapper::GetAllChannelsState(std::vector<ChannelState>& channelsState) const
	{
		for (int i = 0; i < m_Channels.size(); ++i)
		{
			const NCChannel& channel = m_Channels[i];
			ChannelState _channelState;
			_channelState.channelId = i;

			bool isPLaying;
			channel.m_channel->isPlaying(&isPLaying);
			_channelState.mediaId = isPLaying ? channel.m_bound_media_id : NCMedia::INVALID_MEDIA_ID;

			unsigned int mediaPlayingTime;
			channel.m_channel->getPosition(&mediaPlayingTime, FMOD_TIMEUNIT_MS);
			_channelState.mediaCurrentTimeMs = isPLaying ? mediaPlayingTime : 0;

			_channelState.mediaTotalTimeMs = 0.f;
			_channelState.isMediaLooping = false;
			if (isPLaying)
			{
				const NCMedia& media = m_Resources[channel.m_bound_media_id];
				
				unsigned int mediaTotalTime = 0;
				media.m_sound->getLength(&mediaTotalTime, FMOD_TIMEUNIT_MS);
				_channelState.mediaTotalTimeMs = mediaTotalTime;
		
				_channelState.isMediaLooping = isPLaying ? media.m_isLooping : false;
			}


			_channelState.pan = isPLaying ? channel.m_channel_pan : 0.f;

			float volume = 0;
			channel.m_channel->getVolume(&volume);
			_channelState.volume = isPLaying ? volume * 100 : 0; //de-normalize volume			

			bool isMute;
			channel.m_channel->getMute(&isMute);
			_channelState.isMuted = isPLaying ? isMute : false;

			if (isPLaying)
			{
				bool paused;
				channel.m_channel->getPaused(&paused);
				_channelState.playingState = paused
					? ChannelState::ChannelPlayingState::PAUSED
					: ChannelState::ChannelPlayingState::PLAYING;
			}
			else
			{
				_channelState.playingState = ChannelState::ChannelPlayingState::STOPPED;
			}

			channelsState.push_back(_channelState);
		}
	}

	void Wrapper::GetMasterGroupState(MasterGroupState& state) const
	{
		bool isMasterPaused, isMasterMuted;
		m_master->getPaused(&isMasterPaused);
		m_master->getMute(&isMasterMuted);

		state.isMuted = isMasterMuted;
		state.isPaused = isMasterPaused;
		float volume = 0;
		m_master->getVolume(&volume);
		state.volume = volume * 100; //de-normalize volume
	}

	Wrapper::Wrapper() : m_FMOD_Instance(nullptr)
	{
		//make constructor private in order to force call to factory method
	}

	Wrapper::~Wrapper()
	{
		Close();
	}

	FMODWrapperResult Wrapper::InitFMODSystem(const std::string& absolute_resources_path, int channels)
	{
		FMODWrapperResult result;

		result.Update(FMOD::System_Create(&m_FMOD_Instance));

		if (!result.IsValid())
		{
			result.msg.append("FMOD System Creation failed with error");
			return result;
		}

		result.Update(m_FMOD_Instance->init(channels, FMOD_INIT_NORMAL, 0));

		if (!result.IsValid())
		{
			result.msg.append("FMOD Instance initialization failed with error");
			return result;
		}

		result.Update(m_FMOD_Instance->getMasterChannelGroup(&m_master));
		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected trying to get reference to Master Channel Group");
			return result;
		}

		m_Channels.resize(channels);

		m_absolute_resources_path = absolute_resources_path;

		return result;
	}

	FMODWrapperResult Wrapper::Load(const std::string& media_name)
	{
		return LoadAudio(media_name, false);
	}

	FMODWrapperResult Wrapper::LoadStreaming(const std::string& media_name)
	{
		return LoadAudio(media_name, true);
	}

	FMODWrapperResult Wrapper::Play(int resource_ID, int channel)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;

		result = PlayAudio(resource_ID, channel, false);
		return result;
	}

	FMODWrapperResult Wrapper::PlayLoop(int resource_ID, int channel)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;

		result = PlayAudio(resource_ID, channel, true);
		return result;
	}

	FMODWrapperResult Wrapper::UpdateMute(int channel_id)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;
		result = ValidateChannel(channel_id);
		if (!result.IsValid()) return result;

		NCChannel& _ch = m_Channels[channel_id];

		bool IsPlaying = false;
		result.Update(_ch.m_channel->isPlaying(&IsPlaying));
		if (!IsPlaying)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Cannot mute/unmute a channel that is not playing");
			return result;
		}

		bool isMuted;
		_ch.m_channel->getMute(&isMuted);
		result.Update(_ch.m_channel->setMute(!isMuted));

		if (!result.IsValid()) {
			result.msg.append("A problem has been detected while muting/unmuting channel " + std::to_string(channel_id));
		}

		return result;
	}

	FMODWrapperResult Wrapper::UpdatePause(int channel)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;
		result = ValidateChannel(channel);
		if (!result.IsValid()) return result;

		NCChannel& _ch = m_Channels[channel];

		bool IsPlaying = false;
		result.Update(_ch.m_channel->isPlaying(&IsPlaying));
		if (!IsPlaying)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Cannot pause/restart a channel that is not playing");
			return result;
		}
		
		bool isPaused;
		_ch.m_channel->getPaused(&isPaused);
		result.Update(_ch.m_channel->setPaused(!isPaused));

		if (!result.IsValid()) {
			result.msg.append("A problem has been detected while pausing/restarting channel " + std::to_string(channel));
		}

		return result;
	}

	FMODWrapperResult Wrapper::Stop(int channel)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;
		result = ValidateChannel(channel);
		if (!result.IsValid()) return result;

		NCChannel& ChannelToStop = m_Channels[channel];

		bool IsPlaying = false;
		result.Update(ChannelToStop.m_channel->isPlaying(&IsPlaying));
		if (!IsPlaying)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Cannot stop a channel that is not playing");
			return result;
		}

		result.Update(ChannelToStop.m_channel->stop());

		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected while stopping channel " + std::to_string(channel));
		}

		ChannelToStop.ResetMedia();

		return result;
	}

	FMODWrapperResult Wrapper::SetPan(int channel, float pan)
	{
		FMODWrapperResult result = ValidateChannel(channel);
		if (!result.IsValid()) return result;

		if (pan < MIN_2D_PAN || pan > MAX_2D_PAN)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append(
				"Invalid value for PAN. Please provide a value in the interval ["
				+ std::to_string(MIN_2D_PAN)
				+ ","
				+ std::to_string(MAX_2D_PAN)
				+ "]"
			);
			return result;
		}

		NCChannel& ChannelToModify = m_Channels[channel];
		if (!ChannelToModify.m_channel || !result.IsValid()) return result;

		result.Update(ChannelToModify.m_channel->setPan(pan));

		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected trying to modify PAN value for channel " + std::to_string(channel));
		}

		ChannelToModify.m_channel_pan = pan; //now we can safely set the pan

		return result;
	}

	FMODWrapperResult Wrapper::SetVolume(int channel, float volume)
	{
		FMODWrapperResult result = ValidateChannel(channel);
		if (!result.IsValid()) return result;
		result = ValidateVolumeValue(volume);
		if (!result.IsValid()) return result;

		NCChannel& ChannelToModify = m_Channels[channel];
		if (!ChannelToModify.m_channel || !result.IsValid()) return result;

		float normalizedVolume = volume / MAX_VOLUME;
		result.Update(ChannelToModify.m_channel->setVolume(normalizedVolume));

		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected trying to modify volume to "
				+ std::to_string(volume)
				+ " for channel "
				+ std::to_string(channel));
		}

		return result;
	}

	FMODWrapperResult Wrapper::UpdateMuteForAllChannels()
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;

		bool muted;
		result.Update(m_master->getMute(&muted));
		result.Update(m_master->setMute(!muted));

		return result;
	}

	FMODWrapperResult Wrapper::UpdatePauseForAllChannels()
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;

		bool paused;
		result.Update(m_master->getPaused(&paused));
		result.Update(m_master->setPaused(!paused));
		
		return result;
	}

	FMODWrapperResult Wrapper::StopAllChannels()
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;
		result.Update(m_master->stop());
		return result;
	}

	FMODWrapperResult Wrapper::SetVolumeForAllChannels(float volume)
	{
		FMODWrapperResult result = ValidateVolumeValue(volume);
		if (!result.IsValid()) return result;

		float normalizedVolume = volume / MAX_VOLUME;
		result.Update(m_master->setVolume(normalizedVolume));

		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected trying to modify volume to "
				+ std::to_string(volume)
				+ " for Master Group Channel");
		}

		return result;
	}

	FMODWrapperResult Wrapper::Close()
	{
		FMODWrapperResult result;

		if (m_master)
		{
			result.Update(m_master->release());
			if (!result.IsValid())
			{
				result.msg.append("An error has been detected releasing FMOD Master Channel Group");
			}
		}

		if (m_FMOD_Instance)
		{
			result = FMODWrapperResult::From(m_FMOD_Instance->release());
			m_FMOD_Instance = nullptr;
			if (!result.IsValid())
			{
				result.msg.append("An error has been detected releasing FMOD instance");
			}
		}
		return result;
	}

	FMODWrapperResult Wrapper::LoadAudio(const std::string& media_name, bool stream)
	{
		FMOD::Sound* sound;
		//add safety slash
		std::string absolute_file_path = m_absolute_resources_path + "\\" + media_name;
		FMODWrapperResult result = FMODWrapperResult::From(m_FMOD_Instance->createSound(absolute_file_path.c_str(), stream == true ? FMOD_CREATESTREAM : FMOD_2D, 0, &sound));

		if (!result.IsValid())
		{
			result.msg.append("Cannot find sound in path " + absolute_file_path);
			return result;
		}

		m_Resources.push_back(NCMedia(media_name, stream, sound));

		return result;
	}

	FMODWrapperResult Wrapper::PlayAudio(int resource_ID, int channel, bool loop)
	{
		FMODWrapperResult result = ValidateResourceId(resource_ID);
		if (!result.IsValid()) return result;

		result = ValidateChannel(channel);
		if (!result.IsValid()) return result;

		NCMedia& media = m_Resources[resource_ID];
		media.m_isLooping = loop;

		result.Update(media.m_sound->setMode(loop == true ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));
		if (!result.IsValid()) return result;

		NCChannel& ch = m_Channels[channel];
		bool isPlaying;
		ch.m_channel->isPlaying(&isPlaying);

		if (isPlaying)
		{
			result.Update(ch.m_channel->stop());
		}

		if (!result.IsValid()) return result;

		result.Update(m_FMOD_Instance->playSound(media.m_sound.get(), 0, false, &ch.m_channel));

		if (!result.IsValid())
		{
			result.msg.append("An error occurred while trying to play media "
				+ media.m_mediaName
				+ " in channel "
				+ std::to_string(channel)
			);
		}

		ch.m_bound_media_id = resource_ID;
		m_Channels[channel] = ch;


		return result;
	}

	FMODWrapperResult Wrapper::ValidateResourceId(const int resource_id) const
	{
		bool IsValid = resource_id != NCMedia::INVALID_MEDIA_ID && resource_id >= 0 && resource_id < m_Resources.size();
		FMODWrapperResult result;
		result.code = IsValid
			? FMODWrapperResult::FMODWrapperResultCode::OK
			: FMODWrapperResult::FMODWrapperResultCode::ERROR;

		if (!result.IsValid())
		{
			result.msg.append("Invalid Resource ID received in input. Please provide a valid id");
		}

		return result;
	}

	FMODWrapperResult Wrapper::ValidateChannel(const int channel) const
	{
		bool IsValid = channel >= 0 && channel < m_Channels.size();

		FMODWrapperResult result;
		result.code = IsValid
			? FMODWrapperResult::FMODWrapperResultCode::OK
			: FMODWrapperResult::FMODWrapperResultCode::ERROR;

		if (!result.IsValid())
		{
			result.msg.append("Invalid channel received in input. Please provide a valid channel");
		}

		return result;
	}

	FMODWrapperResult Wrapper::ValidateResourceOperation() const
	{
		bool IsValid = m_Resources.size() > 0;
		FMODWrapperResult result;
		result.code = IsValid
			? FMODWrapperResult::FMODWrapperResultCode::OK
			: FMODWrapperResult::FMODWrapperResultCode::ERROR;
		if (!result.IsValid())
		{
			result.msg.append("Invalid resource operation. To execute this operation you need to load at least a resource");
		}
		return result;
	}

	FMODWrapperResult Wrapper::ValidateVolumeValue(const float value) const
	{
		FMODWrapperResult result;
		result.code = value >= MIN_VOLUME && value <= MAX_VOLUME
			? FMODWrapperResult::FMODWrapperResultCode::OK
			: FMODWrapperResult::FMODWrapperResultCode::ERROR;
		
		if (!result.IsValid())
		{
			result.msg.append(
				"Invalid value for Volume. Please provide a value in the interval ["
				+ std::to_string(MIN_VOLUME)
				+ ","
				+ std::to_string(MAX_VOLUME)
				+ "]"
			);
		}

		return result;
	}

}