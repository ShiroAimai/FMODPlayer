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
		float mediaCurrentTime;
		float mediaTotalTime;
		float pan;
		bool isMuted;
		float volume;

		ChannelPlayingState playingState;

		static std::string to_string(ChannelPlayingState state);		
	};
}