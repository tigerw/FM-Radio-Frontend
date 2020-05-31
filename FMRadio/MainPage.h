#pragma once

#include "MainPage.g.h"
#include "Native API/Zune Media/Zune API.h"
#include "Native API/Miniport/Miniport API.h"
#include "Converters/ActualWidth Halving Converter.h"

namespace winrt::FMRadio::implementation
{
	struct MainPage : MainPageT<MainPage>
	{
		MainPage();

		void PageLoaded(const Windows::Foundation::IInspectable &, const Windows::UI::Xaml::RoutedEventArgs &);

		hstring FrequencyText();
		void FrequencyText(hstring);

		hstring PlayPauseButtonText();
		void PlayPauseButtonText(hstring);

		Windows::UI::Xaml::Controls::SymbolIcon PlayPauseButtonSymbol();
		void PlayPauseButtonSymbol(Windows::UI::Xaml::Controls::SymbolIcon);

		void ToggledOutput(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &);
		void SeekedBackward(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &);
		void ToggledPlayState(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &);
		void SeekedForward(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &);
		void RegionChangedToWorldwide_Click(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &);
		void RegionChangedToNorthAmerica(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &);
		void RegionChangedToJapan(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &);

		void TunerWindowScrolled(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs const &);
		void TunerDialLoaded(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &);
		void TunerWindowScrolled(Windows::Foundation::IInspectable const &, Windows::Foundation::IInspectable const &);

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const &);
		void PropertyChanged(winrt::event_token const &);

		void SignalQualityUpdateTicked(Windows::Foundation::IInspectable const &, Windows::Foundation::IInspectable const &);

	private:
		bool RadioPlaying;
		hstring DisplayedFrequency;
		bool FrequencyUpdateUserOverride;
		FrequencyType DialFrequencyToDisplay;
		Windows::UI::Xaml::DispatcherTimer SignalQualityUpdateTimer;
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> PropertyChanged_;
	};
}

namespace winrt::FMRadio::factory_implementation
{
	struct MainPage : MainPageT<MainPage, implementation::MainPage>
	{
	};
}