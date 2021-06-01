#include "pch.h"
#include <iostream>
#include "Wrapper.h"
#include "FMODWrapperResult.h"
using std::cout;
using std::endl;

namespace NCWrapper {

#pragma region NCMedia
	int Wrapper::NCMedia::INVALID_MEDIA_ID = -1;

	Wrapper::NCMedia::NCMedia() : m_mediaName(""), m_sound(nullptr)
	{

	}

	Wrapper::NCMedia::NCMedia(const std::string& name, FMOD::Sound* sound) :
		m_mediaName(name),
		m_sound(sound)
	{

	}

#pragma endregion
#pragma region NCChannel
	Wrapper::NCChannel::NCChannel() : m_bound_media_id(NCMedia::INVALID_MEDIA_ID),m_channel(nullptr)
	{

	}

	void Wrapper::NCChannel::ResetMedia()
	{
		m_bound_media_id = NCMedia::INVALID_MEDIA_ID;
	}
#pragma endregion

	FMODWrapperResult Wrapper::Init(Wrapper** OutWrapperInstance, std::string& absolute_resources_path, int channels)
	{
		FMODWrapperResult result;
		if (channels > MAX_CHANNELS)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Invalid desired Channel. Insert a number between 1 and " + std::to_string(MAX_CHANNELS));
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
	int Wrapper::MAX_CHANNELS = 12;

	int Wrapper::GetTotalNumberOfChannels() const
	{
		return m_Channels.size();
	}

	int Wrapper::GetNumberOfAvailableResourcesToPlay() const
	{
		return m_Resources.size();
	}

	void Wrapper::GetLoadedMediaNames(std::vector <std::string>& loaded_media_names)
	{
		for (auto media : m_Resources)
			loaded_media_names.push_back(media.m_mediaName);
	}

	Wrapper::Wrapper() : m_FMOD_Instance(nullptr)
	{
		//make constructor private in order to force call to factory method
	}

	FMODWrapperResult Wrapper::InitFMODSystem(const std::string& absolute_resources_path, int channels)
	{
		FMODWrapperResult result;	

		result.Update(FMOD::System_Create(&m_FMOD_Instance));

		if(!result.IsValid())
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

	FMODWrapperResult Wrapper::Pause(int channel)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;
		result = ValidateChannel(channel);
		if (!result.IsValid()) return result;

		NCChannel ChannelToPause = m_Channels[channel];

		bool IsPlaying = false;
		result.Update(ChannelToPause.m_channel->isPlaying(&IsPlaying));
		if (!IsPlaying)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Cannot pause a channel that is not playing");
			return result;
		}

		result.Update(ChannelToPause.m_channel->setPaused(true));

		if (!result.IsValid()) {
			result.msg.append("A problem has been detected while pausing channel " + std::to_string(channel));			
		}

		return result;
	}

	FMODWrapperResult Wrapper::Stop(int channel)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;
		result = ValidateChannel(channel);
		if (!result.IsValid()) return result;

		NCChannel ChannelToStop = m_Channels[channel];

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

		if (pan < MIN_2D_PAN && pan > MAX_2D_PAN)
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
		
		NCChannel ChannelToModify = m_Channels[channel];
		if (!ChannelToModify.m_channel || !result.IsValid()) return result;

		result.Update(ChannelToModify.m_channel->setPan(pan));

		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected trying to modify PAN value for channel " + std::to_string(channel));			
		}

		return result;
	}

	FMODWrapperResult Wrapper::SetVolume(int channel, float volume)
	{		
		FMODWrapperResult result = ValidateChannel(channel);
		if (!result.IsValid()) return result;

		if (volume < MIN_VOLUME && volume > MAX_VOLUME)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append(
				"Invalid value for Volume. Please provide a value in the interval ["
				+ std::to_string(MIN_VOLUME)
				+ ","
				+ std::to_string(MAX_VOLUME)
				+ "]"
			);
			return result;			
		}

		NCChannel ChannelToModify = m_Channels[channel];
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

	FMODWrapperResult Wrapper::Close()
	{
		FMODWrapperResult result = FMODWrapperResult::From(m_FMOD_Instance->release());

		if (!result.IsValid())
		{
			result.msg.append("An error has been detected releasing FMOD instance");		
		}
		return result;
	}

	FMODWrapperResult Wrapper::LoadAudio(const std::string& media_name, bool stream)
	{
		FMOD::Sound* sound;
		std::string absolute_file_path = m_absolute_resources_path + media_name;
		FMODWrapperResult result = FMODWrapperResult::From(m_FMOD_Instance->createSound(absolute_file_path.c_str(), stream == true ? FMOD_CREATESTREAM : FMOD_2D, 0, &sound));

		if (!result.IsValid()) return result;

		m_Resources.push_back(NCMedia(media_name, sound));

		return result;
	}

	FMODWrapperResult Wrapper::PlayAudio(int resource_ID, int channel, bool loop)
	{
		FMODWrapperResult result = ValidateResourceId(resource_ID);
		if (!result.IsValid()) return result;

		result = ValidateChannel(channel);
		if (!result.IsValid()) return result;		

		NCMedia& media = m_Resources[resource_ID];
		result.Update(media.m_sound->setMode(loop == true ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));
		if (!result.IsValid()) return result;
		
		NCChannel ch = m_Channels[channel];
		bool isPlaying;
		ch.m_channel->isPlaying(&isPlaying);
		bool IsMediaAssignedToThisChannel = ch.m_bound_media_id == resource_ID;

		if (isPlaying && IsMediaAssignedToThisChannel)
		{
			//remove pause if paused
			result.Update(ch.m_channel->setPaused(false));
		}
		else
		{
			if (isPlaying && !IsMediaAssignedToThisChannel)
			{
				result.Update(ch.m_channel->stop());
			}

			if (!result.IsValid()) return result;

			result.Update(m_FMOD_Instance->playSound(media.m_sound, 0, false, &ch.m_channel));

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
		}

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
}