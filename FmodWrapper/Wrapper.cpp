#include "pch.h"
#include <iostream>
#include "Wrapper.h"
using std::cout;
using std::endl;

namespace NCWrapper {
	
	FMODWrapperResult FMODWrapperResult::From(FMOD_RESULT result)
	{
		FMODWrapperResult wrapperResult;
		wrapperResult.code = result == FMOD_OK
			? FMODWrapperResult::FMODWrapperResultCode::OK
			: FMODWrapperResult::FMODWrapperResultCode::ERROR;
		wrapperResult.msg = FMOD_ErrorString(result);

		return wrapperResult;
	}

	void FMODWrapperResult::Update(FMOD_RESULT result)
	{
		code = result == FMOD_OK
			? FMODWrapperResult::FMODWrapperResultCode::OK
			: FMODWrapperResult::FMODWrapperResultCode::ERROR;
		msg = FMOD_ErrorString(result);
	}

	bool FMODWrapperResult::IsValid() const
	{
		return code == FMODWrapperResultCode::OK;
	}

	Wrapper::Media::Media() : m_mediaName(""), m_sound(nullptr), m_channel_id(INVALID_CHANNEL_ID)
	{

	}

	Wrapper::Media::Media(const std::string& name, FMOD::Sound* sound) :
		m_mediaName(name),
		m_sound(sound),
		m_channel_id(Wrapper::INVALID_CHANNEL_ID)
	{

	}

	Wrapper* Wrapper::Init(FMODWrapperResult& result, std::string& absolute_resources_path, int channels)
	{
		Wrapper* SystemInitialized = new Wrapper();

		result = SystemInitialized->InitFMODSystem(absolute_resources_path, channels);
		return SystemInitialized;
	}

	int Wrapper::WRAPPER_INVALID_RESOURCE_ID = -1;
	int Wrapper::INVALID_CHANNEL_ID = -1;
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

		m_Channels = new FMOD::Channel * [channels];
		m_channel_total = channels;

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

	FMODWrapperResult Wrapper::Pause(int resource_ID)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;

		FMOD::Channel* ChannelToPause = GetPlayingChannelFromResourceID(result, resource_ID);
		if (!ChannelToPause || !result.IsValid()) return result;

		bool IsPlaying = false;
		result.Update(ChannelToPause->isPlaying(&IsPlaying));
		if (!IsPlaying)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Cannot pause a channel that is not playing");
			return result;
		}

		result.Update(ChannelToPause->setPaused(true));

		if (!result.IsValid()) {
			result.msg.append("A problem has been detected while pausing resource " + std::to_string(resource_ID));			
		}

		return result;
	}

	FMODWrapperResult Wrapper::Stop(int resource_ID)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;

		FMOD::Channel* ChannelToStop = GetPlayingChannelFromResourceID(result, resource_ID);
		if (!ChannelToStop || !result.IsValid()) return result;

		bool IsPlaying = false;
		result.Update(ChannelToStop->isPlaying(&IsPlaying));
		if (!IsPlaying)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Cannot stop a channel that is not playing");
			return result;
		}

		result.Update(ChannelToStop->stop());

		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected while stopping resource " + std::to_string(resource_ID));
		}

		Media& media = m_Resources[resource_ID];
		media.m_channel_id = INVALID_CHANNEL_ID;

		return result;
	}

	FMODWrapperResult Wrapper::SetPan(int resource_ID, float pan)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;
		
		result = ValidateResourceId(resource_ID);
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
		
		FMOD::Channel* channel = GetPlayingChannelFromResourceID(result, resource_ID);
		if (!channel || !result.IsValid()) return result;

		result.Update(channel->setPan(pan));

		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected trying to modify PAN value for resource " + std::to_string(resource_ID));			
		}

		return result;
	}

	FMODWrapperResult Wrapper::SetVolume(int resource_ID, float volume)
	{
		FMODWrapperResult result = ValidateResourceOperation();
		if (!result.IsValid()) return result;

		result = ValidateResourceId(resource_ID);
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

		FMOD::Channel* channel = GetPlayingChannelFromResourceID(result, resource_ID);
		if (!channel || !result.IsValid()) return result;

		float normalizedVolume = volume / MAX_VOLUME;
		result.Update(channel->setVolume(normalizedVolume));

		if (!result.IsValid())
		{
			result.msg.append("A problem has been detected trying to modify volume to " 
				+ std::to_string(volume) 
				+ " for resource " 
				+ std::to_string(resource_ID));
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

		AddResource(media_name, sound);

		return result;
	}

	FMODWrapperResult Wrapper::PlayAudio(int resource_ID, int channel, bool loop)
	{
		FMODWrapperResult result = ValidateResourceId(resource_ID);
		if (channel <= 0 || channel > m_channel_total)
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Invalid channel received in input. Please provide a valid channel");			
			return result;
		}

		Media& media = m_Resources[resource_ID];
		result.Update(media.m_sound->setMode(loop == true ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));
		if (!result.IsValid()) return result;
		
		FMOD::Channel* ch = m_Channels[channel - 1];
		bool isPlaying;
		ch->isPlaying(&isPlaying);

		bool IsMediaAssignedToAChannel = media.m_channel_id != INVALID_CHANNEL_ID;
		if (isPlaying && IsMediaAssignedToAChannel && media.m_channel_id != (channel - 1))
		{
			result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
			result.msg.append("Channel is already assigned to another media. Please select another channel");
			return result;
		}

		if (IsMediaAssignedToAChannel)
		{
			bool IsMediaAssignedToThisChannel = media.m_channel_id == (channel - 1);
			if (IsMediaAssignedToThisChannel)
			{
				//remove pause if paused
				result.Update(ch->setPaused(false));
			}
			else
			{
				result.code = FMODWrapperResult::FMODWrapperResultCode::ERROR;
				result.msg.append("This media is already playing in channel " + std::to_string(media.m_channel_id));
			}
		}
		else
		{
			result.Update(m_FMOD_Instance->playSound(media.m_sound, 0, false, &ch));

			if (!result.IsValid())
			{
				result.msg.append("An error occurred while trying to play media "
					+ media.m_mediaName
					+ " in channel "
					+ std::to_string(channel)
				);
			}

			m_Channels[channel - 1] = ch;
			media.m_channel_id = channel - 1;
		}

		return result;
	}

	void Wrapper::AddResource(const std::string& media_name, FMOD::Sound* sound)
	{
		Media NewMediaResource(media_name, sound);
		m_Resources.insert(std::pair<int, Media>(m_ResourcesSize, NewMediaResource));
		m_ResourcesSize++;
	}

	FMOD::Channel* Wrapper::GetPlayingChannelFromResourceID(FMODWrapperResult& result, const int resource_id)
	{
		Media& media = m_Resources[resource_id];
		result.code = media.m_channel_id == INVALID_CHANNEL_ID
			? FMODWrapperResult::FMODWrapperResultCode::ERROR
			: FMODWrapperResult::FMODWrapperResultCode::OK;

		if (!result.IsValid())
		{
			result.msg.append("There is not channel associated to this resource");
			return nullptr;
		}

		return m_Channels[media.m_channel_id];
	}

	FMODWrapperResult Wrapper::ValidateResourceId(const int resource_id) const
	{
		bool IsValid = resource_id != WRAPPER_INVALID_RESOURCE_ID && resource_id >= 0 && resource_id < m_ResourcesSize;
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

	FMODWrapperResult Wrapper::ValidateResourceOperation() const
	{
		bool IsValid = m_ResourcesSize > 0;
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