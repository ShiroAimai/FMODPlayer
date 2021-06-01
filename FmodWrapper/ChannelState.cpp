#include "pch.h"
#include "ChannelState.h"

std::string NCWrapper::ChannelState::to_string(ChannelPlayingState state)
{
	switch (state)
	{
		case ChannelPlayingState::PLAYING: return "playing";
		case ChannelPlayingState::PAUSED: return "paused";
		case ChannelPlayingState::STOPPED: return "stopped";
		default: return "";
	}
}
