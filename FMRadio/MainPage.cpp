#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::FMRadio::implementation
{
	MainPage::MainPage() :
		RadioPlaying(false)
	{
		InitializeComponent();
	}

	void MainPage::Initialise(RadioAPI * RadioObject)
	{
		Radio = RadioObject;

		Radio->OnPlayed = [this] {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this] {
					RadioPlaying = true;
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayPauseButtonText"));
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayPauseButtonSymbol"));
				}
			);
		};

		Radio->OnPaused = [this] {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this] {
					RadioPlaying = false;
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayPauseButtonText"));
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayPauseButtonSymbol"));
				}
			);
		};

		Radio->OnFrequencyChanged = [this](int Frequency) {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this, Frequency] {
					const unsigned Kilo = 3;
					const auto TextualFrequency = std::to_wstring(Frequency);
					if (TextualFrequency.length() < Kilo)
					{
						FrequencyTextBlock().Text(TextualFrequency);
					}
					else
					{
						const auto IntegerPartLength = TextualFrequency.length() - Kilo;
						const auto IntegerPart = TextualFrequency.substr(0, IntegerPartLength);
						const auto FractionalPart = TextualFrequency.substr(IntegerPartLength, std::wstring::npos);
						FrequencyTextBlock().Text(IntegerPart + std::use_facet<std::numpunct<wchar_t>>(std::locale()).decimal_point() + FractionalPart);
					}
				}
			);
		};

		Radio->OnProgrammeIdentificationReady = [this](std::wstring Text) {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this, Text] {
					StationCallsignTextBlock().Text(Text);
				}
			);
		};

		Radio->OnProgrammeServiceNameReady = [this](std::wstring Text) {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this, Text] {
					StationNameTextBlock().Text(Text);
				}
			);
		};

		Radio->OnRadioTextReady = [this](std::wstring Text) {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this, Text] {
					ProgrammeDescriptionTextBlock().Text(Text);
				}
			);
		};
	}

	void MainPage::Activate()
	{
		try
		{
			Radio->AcquireInitialState();
		}
		catch (std::system_error& e)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter("Conversion to UTF-8 failed when displaying an error", L"Conversion to UTF-16 failed when displaying an error");
			std::wstring wide = converter.from_bytes(e.what());

			Windows::UI::Popups::MessageDialog msg(wide);
			msg.ShowAsync();
		}
		catch (...)
		{
			Windows::UI::Popups::MessageDialog msg(L"Unknown error");
			msg.ShowAsync();
		}
	}

	hstring MainPage::PlayPauseButtonText()
	{
		return RadioPlaying ? L"Pause" : L"Play";
	}

	void MainPage::PlayPauseButtonText(hstring)
	{
		throw hresult_not_implemented();
	}

	Windows::UI::Xaml::Controls::SymbolIcon MainPage::PlayPauseButtonSymbol()
	{
		using namespace Windows::UI::Xaml::Controls;
		return SymbolIcon(RadioPlaying ? Symbol::Pause : Symbol::Play);
	}

	void MainPage::PlayPauseButtonSymbol(Windows::UI::Xaml::Controls::SymbolIcon)
	{
		throw hresult_not_implemented();
	}

	void MainPage::ToggledOutput(Windows::Foundation::IInspectable const & ToggleButton, Windows::UI::Xaml::RoutedEventArgs const &)
	{
		try
		{
			if (ToggleButton.as<Windows::UI::Xaml::Controls::AppBarToggleButton>().IsChecked().Value())
			{
				Radio->SetAudioEndpoint(AudioEndpoint::Speakers);
			}
			else
			{
				Radio->SetAudioEndpoint(AudioEndpoint::Headset);
			}
		}
		catch (std::system_error& e)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter("Conversion to UTF-8 failed when displaying an error", L"Conversion to UTF-16 failed when displaying an error");
			std::wstring wide = converter.from_bytes(e.what());

			Windows::UI::Popups::MessageDialog msg(wide);
			msg.ShowAsync();
		}
		catch (...)
		{
			Windows::UI::Popups::MessageDialog msg(L"Unknown error");
			msg.ShowAsync();
		}
	}

	void MainPage::SeekedBackward(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &)
	{
		try
		{
			Radio->SeekBackwards();
		}
		catch (std::system_error& e)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter("Conversion to UTF-8 failed when displaying an error", L"Conversion to UTF-16 failed when displaying an error");
			std::wstring wide = converter.from_bytes(e.what());

			Windows::UI::Popups::MessageDialog msg(wide);
			msg.ShowAsync();
		}
		catch (...)
		{
			Windows::UI::Popups::MessageDialog msg(L"Unknown error");
			msg.ShowAsync();
		}
	}

	void MainPage::ToggledPlayState(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &)
	{
		try
		{
			if (RadioPlaying)
			{
				Radio->DisableRadio();
			}
			else
			{
				Radio->EnableRadio();
			}
		}
		catch (std::system_error& e)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter("Conversion to UTF-8 failed when displaying an error", L"Conversion to UTF-16 failed when displaying an error");
			std::wstring wide = converter.from_bytes(e.what());

			Windows::UI::Popups::MessageDialog msg(wide);
			msg.ShowAsync();
		}
		catch (...)
		{
			Windows::UI::Popups::MessageDialog msg(L"Unknown error");
			msg.ShowAsync();
		}
	}

	void MainPage::SeekedForward(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &)
	{
		try
		{
			Radio->SeekForwards();
		}
		catch (std::system_error & e)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wide = converter.from_bytes(e.what());

			Windows::UI::Popups::MessageDialog msg(wide);
			msg.ShowAsync();
		}
	}

	void MainPage::RegionChangedToWorldwide_Click(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &)
	{
	}

	void MainPage::RegionChangedToNorthAmerica(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &)
	{
	}

	void MainPage::RegionChangedToJapan(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &)
	{
	}

	winrt::event_token MainPage::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return PropertyChanged_.add(handler);
	}

	void MainPage::PropertyChanged(winrt::event_token const& token)
	{
		PropertyChanged_.remove(token);
	}
}
