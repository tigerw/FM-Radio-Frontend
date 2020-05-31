#pragma once

#include "pch.h"
#include "Native API/Radio API.h"

class MiniportAPI final : public RadioAPI
{
	std::thread ListenerThread;

	Client ClientId;

	void NotificationProcessor();

	/// <summary>Ensures that the RPC server is present, excepting otherwise.</summary>
	/// <remarks>This function SHOULD be called in <see cref="MiniportAPI::Initialise" /> to display the "not supported" UI if the RPC server isn't running.</remarks>
	/// <exception cref="std::system_error" />Thrown when the remote procedure call excepted.</exception>
	void PingRPCServer();

public:
	/// <summary>Hello</summary>
	void Initialise() override;
	void AcquireInitialState() override;

	void EnableRadio() override;
	void DisableRadio() override;
	void SeekForwards() override;
	void SeekBackwards() override;
	void SetAudioEndpoint(AudioEndpoint) override;
	void SetFrequency(FrequencyType) override;
	unsigned GetSignalQuality() override;
};