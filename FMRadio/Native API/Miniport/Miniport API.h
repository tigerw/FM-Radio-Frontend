#pragma once

#include "pch.h"
#include "Native API/Radio API.h"
#include "Miniport Service Interface.h"

class MiniportAPI : public RadioAPI
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
	void Initialise() final override;
	void AcquireInitialState() final override;

	void EnableRadio() final override;
	void DisableRadio() final override;
	void SeekForwards() final override;
	void SeekBackwards() final override;
	void SetAudioEndpoint(AudioEndpoint) final override;
};