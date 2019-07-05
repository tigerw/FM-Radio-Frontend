#pragma once

#include "pch.h"
#include "Native API/Radio API.h"

class RPCClient
{
public:
	static void Initialise(handle_t & ClientHandle, wchar_t Endpoint[])
	{
		// Relevant RPC parameters, sync with the server
		// Comments about allocation also at the server
		wchar_t ProtocolSequence[] = L"ncalrpc";

		// Magical RPC binding string
		RPC_WSTR StringBinding;
		RadioAPI::CheckedRPCCall(
			RpcStringBindingCompose,
			nullptr,
			reinterpret_cast<RPC_WSTR>(ProtocolSequence),
			nullptr,
			reinterpret_cast<RPC_WSTR>(Endpoint),
			nullptr,
			&StringBinding
		);

		// Magical RPC binding
		RadioAPI::CheckedRPCCall(
			RpcBindingFromStringBinding,
			StringBinding,
			&ClientHandle
		);

		// Above APIs allocate their own memory, wouldn't want a leak now, would we?
		RadioAPI::CheckedRPCCall(
			RpcStringFree,
			&StringBinding
		);
	}
};