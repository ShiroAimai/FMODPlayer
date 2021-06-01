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
		float mediaTime;
		float volume;
		float pan;
		ChannelPlayingState playingState;

		static std::string to_string(ChannelPlayingState state);		
	};
}