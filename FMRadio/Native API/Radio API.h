#pragma once

#include "pch.h"
#include "Miniport Service Interface.h"

class RadioAPI
{
public:
	template<typename FunctionType>
	static FunctionType* GetFunctionAddress(HMODULE StartAddress, LPCSTR ExportedName)
	{
		const auto FunctionAddress = reinterpret_cast<FunctionType*>(::GetProcAddress(StartAddress, ExportedName));
		if (FunctionAddress == nullptr)
		{
			throw std::system_error(::GetLastError(), std::system_category());
		}

		return FunctionAddress;
	}

	template<typename FunctionType, typename ...ArgumentTypes>
	static void CheckedAPICall(FunctionType Function, ArgumentTypes ...Arguments)
	{
		const auto Result = Function(Arguments...);
		if (SUCCEEDED(Result))
		{
			return;
		}

		throw std::system_error(Result, std::system_category());
	}

	template<typename FunctionType, typename ...ArgumentTypes>
	static void CheckedRPCCall(FunctionType Function, ArgumentTypes ...Arguments)
	{
		const auto Result = Function(Arguments...);
		if (Result == S_OK)
		{
			return;
		}

		throw std::system_error(MAKE_HRESULT(1, FACILITY_WIN32, Result), std::system_category());
	}

	template<typename FunctionType, typename ...ArgumentTypes>
	static void CheckedBooleanAPICall(FunctionType Function, ArgumentTypes ...Arguments)
	{
		if (Function(Arguments...))
		{
			return;
		}

		throw std::system_error(::GetLastError(), std::system_category());
	}

	virtual void Initialise() = 0;
	virtual void AcquireInitialState() = 0;

	virtual void EnableRadio() = 0;
	virtual void DisableRadio() = 0;
	virtual void SeekForwards() = 0;
	virtual void SeekBackwards() = 0;
	virtual void SetAudioEndpoint(AudioEndpoint) = 0;
	virtual void SetFrequency(FrequencyType) = 0;

	std::function<void(int)> OnFrequencyChanged;
	std::function<void()> OnAntennaRemoved;
	std::function<void()> OnAntennaInserted;
	std::function<void()> OnPlayed;
	std::function<void()> OnPaused;
	std::function<void(std::wstring)> OnProgrammeServiceNameReady;
	std::function<void(std::wstring)> OnProgrammeIdentificationReady;
	std::function<void(std::wstring)> OnRadioTextReady;

	static RadioAPI * Radio;
};