#pragma once

#include "App.xaml.g.h"

#include "MainPage.h"
#include "UnsupportedDevicePage.h"

#ifdef _M_ARM
#include "Native API/Zune Media/Zune API.h"
#else
#include "Native API/Miniport/Miniport API.h"
#endif

namespace winrt::FMRadio::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
        void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
        void OnNavigationFailed(IInspectable const&, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const&);

	private:
#ifdef _M_ARM
		using RadioAPI = ZuneAPI;
#else
		using RadioAPI = MiniportAPI;
#endif

		RadioAPI Radio;

		void NavigateDependingOnInitialisation(winrt::Windows::UI::Xaml::Controls::Frame &, const Windows::ApplicationModel::Activation::LaunchActivatedEventArgs &, bool);
		void NavigateToMainPage(winrt::Windows::UI::Xaml::Controls::Frame &, const Windows::ApplicationModel::Activation::LaunchActivatedEventArgs &, bool);
		void NavigateToUnsupportedDevicePage(winrt::Windows::UI::Xaml::Controls::Frame &, const Windows::ApplicationModel::Activation::LaunchActivatedEventArgs &, bool, std::wstring Message);
    };
}