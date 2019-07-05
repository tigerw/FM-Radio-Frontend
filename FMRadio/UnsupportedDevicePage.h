#pragma once

#include "UnsupportedDevicePage.g.h"

namespace winrt::FMRadio::implementation
{
    struct UnsupportedDevicePage : UnsupportedDevicePageT<UnsupportedDevicePage>
    {
        UnsupportedDevicePage();

		void Initialise(std::wstring);
    };
}

namespace winrt::FMRadio::factory_implementation
{
    struct UnsupportedDevicePage : UnsupportedDevicePageT<UnsupportedDevicePage, implementation::UnsupportedDevicePage>
    {
    };
}
