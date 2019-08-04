#pragma once

#include "UnsupportedDevicePage.g.h"

namespace winrt::FMRadio::implementation
{
	struct UnsupportedDevicePage : UnsupportedDevicePageT<UnsupportedDevicePage>
	{
		UnsupportedDevicePage();

		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const &);
	};
}

namespace winrt::FMRadio::factory_implementation
{
	struct UnsupportedDevicePage : UnsupportedDevicePageT<UnsupportedDevicePage, implementation::UnsupportedDevicePage>
	{
	};
}