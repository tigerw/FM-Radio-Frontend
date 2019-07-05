#include "pch.h"
#include "UnsupportedDevicePage.h"
#if __has_include("UnsupportedDevicePage.g.cpp")
#include "UnsupportedDevicePage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::FMRadio::implementation
{
    UnsupportedDevicePage::UnsupportedDevicePage()
    {
        InitializeComponent();
    }

	void UnsupportedDevicePage::Initialise(std::wstring Text)
	{
		ErrorMessage().Text(std::move(Text));
	}
}
