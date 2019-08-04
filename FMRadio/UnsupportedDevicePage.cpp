#include "pch.h"
#include "UnsupportedDevicePage.h"
#include "UnsupportedDevicePage.g.cpp"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::FMRadio::implementation
{
    UnsupportedDevicePage::UnsupportedDevicePage()
    {
        InitializeComponent();
    }

	void UnsupportedDevicePage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const & Arguments)
	{
		ErrorMessage().Text(std::move(unbox_value<hstring>(Arguments.Parameter())));
	}
}