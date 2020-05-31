#pragma once

#include "pch.h"
#include "ZMediaQueueTypes.h"
#include "Native API/Radio API.h"

class ZuneAPI final : public RadioAPI
{
	using ZMediaQueue_ConnectToService = WINUSERAPI int WINAPI();

	using ZMediaQueue_PlayItem = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAITEM ItemType
	);

	using ZMediaQueue_SetRadioRegion = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_RADIOREGION Region
	);

	using ZMediaQueue_SetFrequency = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_TUNEPARAMS * TuneParameters
	);

	using ZMediaQueue_GetFrequency = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_TUNEPARAMS * TuneParameters
	);

	using ZMediaQueue_StartSeekToNextStation = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_SEEKDIR Direction
	);

	using ZMediaQueue_RegisterForNotifications = WINUSERAPI HRESULT WINAPI(
		void(** Handler)(const ZMEDIAQUEUE_NOTIFICATIONDATA *),
		void * Unknown
	);

	using ZMediaQueue_SetPlayState = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_PLAYSTATE PlayState
	);

	using ZMediaQueue_GetPlayState = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_PLAYSTATE * PlayState
	);

	using ZMediaQueue_GetStationText = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_STATIONTEXT TextType,
		wchar_t * TextBuffer,
		size_t BufferLength
	);

	using ZMediaQueue_EnableStationData = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		bool Enable
	);

	using ZMediaQueue_SetProperty = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_PROPERTY Property,
		int PropertyValue,
		size_t PropertySize
	);

	using ZMediaQueue_GetProperty = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_PROPERTY Property,
		int * PropertyValue,
		size_t * PropertySize
	);

	using ZMediaQueue_GetSignalQuality = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		int * RSSI
	);

	using LoadLibraryExW = HMODULE WINAPI(
		LPCWSTR lpLibFileName,
		HANDLE hFile,
		DWORD dwFlags
	);

	ZMediaQueue_ConnectToService * ConnectToService_;
	ZMediaQueue_PlayItem * PlayItem_;
	ZMediaQueue_SetRadioRegion * SetRadioRegion_;
	ZMediaQueue_SetFrequency * SetFrequency_;
	ZMediaQueue_GetFrequency * GetFrequency_;
	ZMediaQueue_StartSeekToNextStation * StartSeekToNextStation_;
	ZMediaQueue_RegisterForNotifications * RegisterForNotifications_;
	ZMediaQueue_SetPlayState * SetPlayState_;
	ZMediaQueue_GetPlayState * GetPlayState_;
	ZMediaQueue_GetStationText * GetStationText_;
	ZMediaQueue_EnableStationData * EnableStationData_;
	ZMediaQueue_SetProperty * SetProperty_;
	ZMediaQueue_GetProperty* GetProperty_;
	ZMediaQueue_GetSignalQuality * GetSignalQuality_;

	static void NotificationHandler(const ZMEDIAQUEUE_NOTIFICATIONDATA * NotificationData);

	static FrequencyType AdjustFrequency(FrequencyType);

public:
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