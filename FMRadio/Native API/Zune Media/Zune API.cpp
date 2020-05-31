#include "pch.h"
#include "Zune API.h"

void ZuneAPI::NotificationHandler(const ZMEDIAQUEUE_NOTIFICATIONDATA * NotificationData)
{
	if (NotificationData->queue != ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO)
	{
		return;
	}

	const auto ZuneRadio = static_cast<ZuneAPI *>(RadioAPI::Radio);

	switch (NotificationData->notifyCode)
	{
		case ZMEDIAQUEUE_NOTIFICATION::ZMEDIAQUEUE_NOTIFICATION_ANTENNASTATUSCHANGE:
		{
			switch (NotificationData->param1)
			{
				case 0: if (ZuneRadio->OnAntennaRemoved) ZuneRadio->OnAntennaRemoved(); return;
				case 1: if (ZuneRadio->OnAntennaInserted) ZuneRadio->OnAntennaInserted(); return;
			}

			assert(!"Unexpected value for antenna status");
			return;
		}
		case ZMEDIAQUEUE_NOTIFICATION::ZMEDIAQUEUE_NOTIFICATION_PLAYSTATECHANGED:
		{
			switch (static_cast<ZMEDIAQUEUE_PLAYSTATE>(NotificationData->param1))
			{
				case ZMEDIAQUEUE_PLAYSTATE::ZMEDIAQUEUE_PLAYSTATE_PLAYING: if (ZuneRadio->OnPlayed) ZuneRadio->OnPlayed(); return;
				case ZMEDIAQUEUE_PLAYSTATE::ZMEDIAQUEUE_PLAYSTATE_UNKNOWN:
				case ZMEDIAQUEUE_PLAYSTATE::ZMEDIAQUEUE_PLAYSTATE_STOPPED: if (ZuneRadio->OnPaused) ZuneRadio->OnPaused(); return;
			}

			assert(!"Unexpected value for radio play state");
			return;
		}
		case ZMEDIAQUEUE_NOTIFICATION::ZMEDIAQUEUE_NOTIFICATION_FREQUENCYCHANGED:
		{
			if (ZuneRadio->OnFrequencyChanged)
			{
				ZMEDIAQUEUE_TUNEPARAMS TuneParameters;
				RadioAPI::CheckedAPICall(ZuneRadio->GetFrequency_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, &TuneParameters);
				ZuneRadio->OnFrequencyChanged(TuneParameters.kHz);
			}
			return;
		}
		case ZMEDIAQUEUE_NOTIFICATION::ZMEDIAQUEUE_NOTIFICATION_RDSREADY:
		{
			if (ZuneRadio->OnProgrammeServiceNameReady)
			{
				std::wstring Name;
				Name.resize(9);
				RadioAPI::CheckedAPICall(ZuneRadio->GetStationText_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_STATIONTEXT::ZMEDIAQUEUE_STATIONTEXT_NAME, Name.data(), Name.length());
				ZuneRadio->OnProgrammeServiceNameReady(std::move(Name));
			}

			if (ZuneRadio->OnProgrammeIdentificationReady)
			{
				std::wstring Callsign;
				Callsign.resize(5);
				RadioAPI::CheckedAPICall(ZuneRadio->GetStationText_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_STATIONTEXT::ZMEDIAQUEUE_STATIONTEXT_CALLSIGN, Callsign.data(), Callsign.length());
				ZuneRadio->OnProgrammeIdentificationReady(std::move(Callsign));
			}

			if (ZuneRadio->OnRadioTextReady)
			{
				std::wstring Description;
				Description.resize(65);
				CheckedAPICall(ZuneRadio->GetStationText_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_STATIONTEXT::ZMEDIAQUEUE_STATIONTEXT_DESCRIPTION, Description.data(), Description.length());
				ZuneRadio->OnRadioTextReady(std::move(Description));
			}
		}
	}
}

FrequencyType ZuneAPI::AdjustFrequency(FrequencyType Frequency)
{
	const FrequencyType MaximumPrecision = 100;
	const auto ExcessPrecision = Frequency % MaximumPrecision;
	const auto BaseFrequency = Frequency - ExcessPrecision;

	if (ExcessPrecision > (MaximumPrecision / 2))
	{
		return BaseFrequency + MaximumPrecision;
	}

	return BaseFrequency;
}

void ZuneAPI::Initialise()
{
	MEMORY_BASIC_INFORMATION Information;

	if (::VirtualQuery(&VirtualQuery, &Information, sizeof(Information)) == 0)
	{
		throw std::system_error(::GetLastError(), std::system_category());
	}

	const auto SyscallBegin = static_cast<HMODULE>(Information.AllocationBase);
	const auto LoadLibrary = GetFunctionAddress<LoadLibraryExW>(SyscallBegin, "LoadLibraryExW");

	const auto MediaQueueAddress = static_cast<HMODULE>(LoadLibrary(L"ZMediaQueueClient.dll", nullptr, 0));
	if (MediaQueueAddress == nullptr)
	{
		throw std::system_error(::GetLastError(), std::system_category());
	}

	ConnectToService_ = GetFunctionAddress<ZMediaQueue_ConnectToService>(MediaQueueAddress, "ZMediaQueue_ConnectToService");
	PlayItem_ = GetFunctionAddress<ZMediaQueue_PlayItem>(MediaQueueAddress, "ZMediaQueue_PlayItem");
	SetRadioRegion_ = GetFunctionAddress<ZMediaQueue_SetRadioRegion>(MediaQueueAddress, "ZMediaQueue_SetRadioRegion");
	SetFrequency_ = GetFunctionAddress<ZMediaQueue_SetFrequency>(MediaQueueAddress, "ZMediaQueue_SetFrequency");
	GetFrequency_ = GetFunctionAddress<ZMediaQueue_GetFrequency>(MediaQueueAddress, "ZMediaQueue_GetFrequency");
	StartSeekToNextStation_ = GetFunctionAddress<ZMediaQueue_StartSeekToNextStation>(MediaQueueAddress, "ZMediaQueue_StartSeekToNextStation");
	RegisterForNotifications_ = GetFunctionAddress<ZMediaQueue_RegisterForNotifications>(MediaQueueAddress, "ZMediaQueue_RegisterForNotifications");
	SetPlayState_ = GetFunctionAddress<ZMediaQueue_SetPlayState>(MediaQueueAddress, "ZMediaQueue_SetPlayState");
	GetPlayState_ = GetFunctionAddress<ZMediaQueue_GetPlayState>(MediaQueueAddress, "ZMediaQueue_GetPlayState");
	GetStationText_ = GetFunctionAddress<ZMediaQueue_GetStationText>(MediaQueueAddress, "ZMediaQueue_GetStationText");
	EnableStationData_ = GetFunctionAddress<ZMediaQueue_EnableStationData>(MediaQueueAddress, "ZMediaQueue_EnableStationData");
	SetProperty_ = GetFunctionAddress<ZMediaQueue_SetProperty>(MediaQueueAddress, "ZMediaQueue_SetProperty");
	GetProperty_ = GetFunctionAddress<ZMediaQueue_GetProperty>(MediaQueueAddress, "ZMediaQueue_GetProperty");
	GetSignalQuality_ = GetFunctionAddress<ZMediaQueue_GetSignalQuality>(MediaQueueAddress, "ZMediaQueue_GetSignalQuality");

	CheckedAPICall(ConnectToService_);
	CheckedAPICall(EnableStationData_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, true);
	CheckedAPICall(SetRadioRegion_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_RADIOREGION::ZMEDIAQUEUE_RADIOREGION_EUROPE);

	{
		int PropertyValue;
		size_t PropertySize;
		CheckedAPICall(GetProperty_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_PROPERTY::ZMEDIAQUEUE_PROPERTY_RADIO_ANTENNASTATUS, &PropertyValue, &PropertySize);

		ZMEDIAQUEUE_NOTIFICATIONDATA NotificationData{
				ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO,
				ZMEDIAQUEUE_NOTIFICATION::ZMEDIAQUEUE_NOTIFICATION_ANTENNASTATUSCHANGE,
				0, 0,
				static_cast<size_t>(PropertyValue)
		};
		NotificationHandler(&NotificationData);
	}

	auto WrapperAddress = ZuneAPI::NotificationHandler;
	void* UnknownParameter = nullptr;

	CheckedAPICall(RegisterForNotifications_, &WrapperAddress, &UnknownParameter);
}

void ZuneAPI::AcquireInitialState()
{
	{
		ZMEDIAQUEUE_PLAYSTATE PlayState;
		CheckedAPICall(GetPlayState_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, &PlayState);

		ZMEDIAQUEUE_NOTIFICATIONDATA NotificationData{
				ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO,
				ZMEDIAQUEUE_NOTIFICATION::ZMEDIAQUEUE_NOTIFICATION_PLAYSTATECHANGED,
				0, 0,
				static_cast<size_t>(PlayState)
		};
		NotificationHandler(&NotificationData);
	}
	{
		ZMEDIAQUEUE_TUNEPARAMS Tune;
		CheckedAPICall(GetFrequency_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, &Tune);

		ZMEDIAQUEUE_NOTIFICATIONDATA NotificationData{
				ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO,
				ZMEDIAQUEUE_NOTIFICATION::ZMEDIAQUEUE_NOTIFICATION_FREQUENCYCHANGED,
				0, 0,
				static_cast<size_t>(Tune.kHz)
		};
		NotificationHandler(&NotificationData);
	}
}

void ZuneAPI::EnableRadio()
{
	CheckedAPICall(PlayItem_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAITEM_FMRADIO);
}

void ZuneAPI::DisableRadio()
{
	CheckedAPICall(SetPlayState_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_PLAYSTATE::ZMEDIAQUEUE_PLAYSTATE_STOPPED);
}

void ZuneAPI::SeekForwards()
{
	CheckedAPICall(StartSeekToNextStation_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_SEEKDIR::ZMEDIAQUEUE_SEEKDIR_UP);
}

void ZuneAPI::SeekBackwards()
{
	CheckedAPICall(StartSeekToNextStation_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_SEEKDIR::ZMEDIAQUEUE_SEEKDIR_DOWN);
}

void ZuneAPI::SetAudioEndpoint(AudioEndpoint Endpoint)
{
	ZMEDIAQUEUE_AUDIOENDPOINT MediaEndpoint;

	switch (Endpoint)
	{
		case AudioEndpoint::Headset:
		{
			MediaEndpoint = ZMEDIAQUEUE_AUDIOENDPOINT::ZMEDIAQUEUE_AUDIOENDPOINT_HEADSET;
			break;
		}
		case AudioEndpoint::Speakers:
		{
			MediaEndpoint = ZMEDIAQUEUE_AUDIOENDPOINT::ZMEDIAQUEUE_AUDIOENDPOINT_SPEAKER;
			break;
		}
		default: throw std::invalid_argument("Request to switch output endpoint to invalid value. Available are: headset, speakers.");
	}

	CheckedAPICall(SetProperty_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, ZMEDIAQUEUE_PROPERTY::ZMEDIAQUEUE_PROPERTY_AUDIOROUTINGPREFERENCE, static_cast<int>(MediaEndpoint), sizeof(ZMEDIAQUEUE_AUDIOENDPOINT));
}

void ZuneAPI::SetFrequency(FrequencyType Frequency)
{
	ZMEDIAQUEUE_HDTUNEPARAMS HD;
	HD.hdStatus = ZMEDIAQUEUE_HDRADIO_STATUS::ZMEDIAQUEUE_SIGNAL_ANALOG;

	ZMEDIAQUEUE_TUNEPARAMS Tune;
	Tune.HDParams = HD;
	Tune.kHz = ZuneAPI::AdjustFrequency(Frequency);

	CheckedAPICall(SetFrequency_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, &Tune);
}

unsigned ZuneAPI::GetSignalQuality()
{
	int RSSI;
	CheckedAPICall(GetSignalQuality_, ZMEDIAQUEUE::ZMEDIAQUEUE_RADIO, &RSSI);

	static const auto Minimum = 80L;
	static const auto Maximum = 135L;

	// Map a 80-135 scale (by experiment) to 0-100
	return static_cast<unsigned>(std::max(0L, (100 * (static_cast<signed long>(RSSI) - Minimum)) / (Maximum - Minimum)));
}