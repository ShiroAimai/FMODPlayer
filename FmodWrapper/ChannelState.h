#pragma once
#include "pch.h"

namespace NCWrapper {
	struct ChannelState
	{
		enum class ChannelPlayingState {
			PLAYING,
			PAUSED,
			STOPPED,
		};

		int channelId;
		int mediaId;
		unsigned int mediaCurrentTimeMs;
		unsigned int mediaTotalTimeMs;
		float pan;
		bool isMuted;
		float volume;

		ChannelPlayingState playingState;

		static std::string to_string(ChannelPlayingState state);		
	};
}