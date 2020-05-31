#include "pch.h"
#include "Miniport API.h"
#include "Native API/Miniport/RPC Client.h"
#include "Miniport Service Interface_c.c"

void __RPC_FAR* __RPC_USER midl_user_allocate(size_t len)
{
	return operator new(len);
}

void __RPC_USER midl_user_free(void __RPC_FAR* ptr)
{
	delete ptr;
}

void MiniportAPI::NotificationProcessor()
{
	while (true)
	{
		RpcTryExcept
		{
			Notification Event;
			::AcquireEvent(ClientId, &Event);
			switch (Event.Type)
			{
				case Event::FrequencyChanged:
				{
					// TODO: OnFrequencyChanged has wrong type, int??
					OnFrequencyChanged(Event.tagged_union.KHz);
					break;
				}
				case Event::PlayStateChanged:
				{
					Event.tagged_union.PlayState ? OnPlayed() : OnPaused();
					break;
				}
				case Event::AntennaStateChanged:
				{
					Event.tagged_union.Present ? OnAntennaInserted() : OnAntennaRemoved();
					break;
				}
			}
		}
		RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		RpcEndExcept
	}
}

void MiniportAPI::PingRPCServer()
{
	RpcTryExcept
	{
		ClientId = ::AcquireClientId();
	}
	RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
	{
		throw std::system_error(RpcExceptionCode(), std::system_category());
	}
	RpcEndExcept
}

void MiniportAPI::Initialise()
{
	wchar_t Endpoint[] = L"FM Radio Miniport Service Interface";
	RPCClient::Initialise(MiniportServiceInterfaceHandle, Endpoint);

	// Make sure the RPC server is running, throw an exception and show "not supported" on the UI otherwise
	PingRPCServer();

	ListenerThread = std::thread(&MiniportAPI::NotificationProcessor, this);
}

void MiniportAPI::AcquireInitialState()
{
	::AcquireInitialState();
}

void MiniportAPI::EnableRadio()
{
	RpcTryExcept
	{
		::EnableRadio();
	}
	RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
	{
		throw std::system_error(::RpcExceptionCode(), std::system_category());
	}
	RpcEndExcept
}

void MiniportAPI::DisableRadio()
{
	RpcTryExcept
	{
		::DisableRadio();
	}
	RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
	{
		throw std::system_error(::RpcExceptionCode(), std::system_category());
	}
	RpcEndExcept
}

void MiniportAPI::SeekForwards()
{
	RpcTryExcept
	{
		::SeekForwards();
	}
	RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
	{
		throw std::system_error(::RpcExceptionCode(), std::system_category());
	}
	RpcEndExcept
}

void MiniportAPI::SeekBackwards()
{
	RpcTryExcept
	{
		::SeekBackwards();
	}
	RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
	{
		throw std::system_error(::RpcExceptionCode(), std::system_category());
	}
	RpcEndExcept
}

void MiniportAPI::SetAudioEndpoint(AudioEndpoint Endpoint)
{
	RpcTryExcept
	{
		::SetAudioEndpoint(Endpoint);
	}
	RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
	{
		throw std::system_error(::RpcExceptionCode(), std::system_category());
	}
	RpcEndExcept
}

void MiniportAPI::SetFrequency(FrequencyType Frequency)
{
	RpcTryExcept
	{
		::SetFrequency(Frequency);
	}
	RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
	{
		throw std::system_error(::RpcExceptionCode(), std::system_category());
	}
	RpcEndExcept
}

unsigned MiniportAPI::GetSignalQuality()
{
	RpcTryExcept
	{
		return ::GetSignalQuality();
	}
	RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
	{
		throw std::system_error(::RpcExceptionCode(), std::system_category());
	}
	RpcEndExcept
}