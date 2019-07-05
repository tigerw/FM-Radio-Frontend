#pragma once

#include "pch.h"
#include "ZMediaQueueTypes.h"
#include "Native API/Radio API.h"

class ZuneAPI : public RadioAPI
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
		UINT * KHz
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

	using LoadLibraryExW = HMODULE WINAPI(
		LPCWSTR lpLibFileName,
		HANDLE hFile,
		DWORD dwFlags
	);

	using ZMediaQueue_SetProperty = WINUSERAPI HRESULT WINAPI(
		ZMEDIAQUEUE QueueType,
		ZMEDIAQUEUE_PROPERTY Property,
		int PropertyValue,
		size_t PropertySize
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

	static void NotificationHandler(const ZMEDIAQUEUE_NOTIFICATIONDATA * NotificationData);

public:
	ZuneAPI();
	void Initialise() final override;
	void AcquireInitialState() final override;

	void EnableRadio() final override;
	void DisableRadio() final override;
	void SeekForwards() final override;
	void SeekBackwards() final override;
	void SetAudioEndpoint(AudioEndpoint) final override;
};