#pragma once

#include "MainPage.g.h"
#include "Native API/Miniport/Miniport API.h"
#include "Native API/Zune Media/Zune API.h"

namespace winrt::FMRadio::implementation
{
	struct MainPage : MainPageT<MainPage>
	{
		MainPage();

		void Initialise(RadioAPI *);

		void Activate();

		hstring PlayPauseButtonText();
		void PlayPauseButtonText(hstring);

		Windows::UI::Xaml::Controls::SymbolIcon PlayPauseButtonSymbol();
		void PlayPauseButtonSymbol(Windows::UI::Xaml::Controls::SymbolIcon);

		void ToggledOutput(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		void SeekedBackward(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		void ToggledPlayState(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		void SeekedForward(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		void RegionChangedToWorldwide_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		void RegionChangedToNorthAmerica(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		void RegionChangedToJapan(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value);
		void PropertyChanged(winrt::event_token const& token);
	private:
		//ZuneAPI Radio;
		RadioAPI * Radio;
		bool RadioPlaying;
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> PropertyChanged_;
	};
}

namespace winrt::FMRadio::factory_implementation
{
	struct MainPage : MainPageT<MainPage, implementation::MainPage>
	{
	};
}