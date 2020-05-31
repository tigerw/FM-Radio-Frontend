#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "Converters/Frequency Offset Converter.h"
#include "Converters/Frequency Formatting Converter.h"

#include <winrt/Windows.Media.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::FMRadio::implementation
{
	MainPage::MainPage() :
		RadioPlaying(false),
		FrequencyUpdateUserOverride(false)
	{
		InitializeComponent();

		NavigationCacheMode(Windows::UI::Xaml::Navigation::NavigationCacheMode::Required);

		{
			Windows::Media::SystemMediaTransportControls c = Windows::Media::SystemMediaTransportControls::GetForCurrentView();
			c.IsPlayEnabled(true);
			c.IsPauseEnabled(true);
			c.IsStopEnabled(true);
			c.IsEnabled(true);
			Windows::Media::SystemMediaTransportControlsDisplayUpdater updater = c.DisplayUpdater();
			updater.Type(Windows::Media::MediaPlaybackType::Music);
			updater.MusicProperties().Artist(L"artist");
			updater.MusicProperties().AlbumArtist(L"album artist");
			updater.MusicProperties().Title(L"song title");
			updater.Update();
			c.PlaybackStatus(Windows::Media::MediaPlaybackStatus::Playing);
		}

		RadioAPI::Radio->OnPlayed = [this] {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this] {
					RadioPlaying = true;
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayPauseButtonText"));
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayPauseButtonSymbol"));
				}
			);
		};

		RadioAPI::Radio->OnPaused = [this] {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this] {
					RadioPlaying = false;
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayPauseButtonText"));
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayPauseButtonSymbol"));
				}
			);
		};

		RadioAPI::Radio->OnFrequencyChanged = [this](int Frequency) {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this, Frequency] {
					DialFrequencyToDisplay = Frequency;
					if (!FrequencyUpdateUserOverride)
					{
						TunerWindow().ChangeView(FrequencyOffsetConverter::Convert(DialFrequencyToDisplay), nullptr, nullptr);
					}

					const auto TextualFrequency = to_hstring(Frequency);
					DisplayedFrequency = FrequencyFormattingConverter::Convert(TextualFrequency);
					PropertyChanged_(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"FrequencyText"));
				}
			);
		};

		RadioAPI::Radio->OnProgrammeIdentificationReady = [this](std::wstring Text) {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this, Text] {
					StationCallsignTextBlock().Text(Text);
				}
			);
		};

		RadioAPI::Radio->OnProgrammeServiceNameReady = [this](std::wstring Text) {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this, Text] {
					StationNameTextBlock().Text(Text);
				}
			);
		};

		RadioAPI::Radio->OnRadioTextReady = [this](std::wstring Text) {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this, Text] {
					ProgrammeDescriptionTextBlock().Text(Text);
				}
			);
		};

		SignalQualityUpdateTimer.Interval(std::chrono::seconds(5));
		SignalQualityUpdateTimer.Tick({ this, &MainPage::SignalQualityUpdateTicked });

		try
		{
			RadioAPI::Radio->AcquireInitialState();
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

	void MainPage::PageLoaded(const Windows::Foundation::IInspectable &, const Windows::UI::Xaml::RoutedEventArgs &)
	{
		Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Normal,
			[this] {
				TunerWindow().ScrollToHorizontalOffset(FrequencyOffsetConverter::Convert(DialFrequencyToDisplay));
			}
		);

		SignalQualityUpdateTimer.Start();
	}

	hstring MainPage::FrequencyText()
	{
		return DisplayedFrequency;
	}

	void MainPage::FrequencyText(hstring)
	{
		throw hresult_not_implemented();
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
				RadioAPI::Radio->SetAudioEndpoint(AudioEndpoint::Speakers);
			}
			else
			{
				RadioAPI::Radio->SetAudioEndpoint(AudioEndpoint::Headset);
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
			RadioAPI::Radio->SeekBackwards();
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
				RadioAPI::Radio->DisableRadio();
			}
			else
			{
				RadioAPI::Radio->EnableRadio();
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
			RadioAPI::Radio->SeekForwards();
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

	void MainPage::TunerWindowScrolled(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs const & Event)
	{
		// Ideally we only want to update when everything has settled, and the ScrollView is no longer moving
		// to prevent instability: RadioAPI -> FrequencyChanged -> ScrollView -> OnScroll -> RadioAPI -> etc (causing jittery movement)

		if (FrequencyUpdateUserOverride)
		{
			try
			{
				RadioAPI::Radio->SetFrequency(FrequencyOffsetConverter::ConvertBack(TunerWindow().HorizontalOffset()));
			}
			catch (std::system_error & e)
			{
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				std::wstring wide = converter.from_bytes(e.what());

				Windows::UI::Popups::MessageDialog msg(wide);
				msg.ShowAsync();
			}
		}

		if (!Event.IsIntermediate())
		{
			FrequencyUpdateUserOverride = false;
		}
	}

	void MainPage::TunerDialLoaded(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &)
	{
		using namespace Windows::UI::Xaml;
		using namespace FrequencyOffsetConverter;

		const auto TunerWidth = (MaximumFrequency - MinimumFrequency) * ScaleFactor;
		const auto MajorDivisionFrequencyOffset = 200 * ScaleFactor;
		const auto MinorDivisionFrequencyOffset = MajorDivisionFrequencyOffset / 2;
		const auto ThickMarkHeightScale = 0.5;
		const auto ThinMarkHeightScale = 0.25;
		const auto ThickMarkWidth = 4.0;
		const auto ThinMarkWidth = 2.0;

		TunerDial().Children().Clear();
		TunerDial().Width(TunerWidth);

		// The first half line
		{
			auto Line = Shapes::Line();
			Line.X1(-MinorDivisionFrequencyOffset);
			Line.X2(-MinorDivisionFrequencyOffset);
			Line.Y1(TunerDial().ActualHeight() * (1 - ThinMarkHeightScale));
			Line.Y2(TunerDial().ActualHeight());
			Line.Stroke(TunerWindow().Foreground());
			Line.StrokeThickness(ThinMarkWidth);
			TunerDial().Children().Append(Line);
		}

		for (double X = 0; X < static_cast<int>(TunerWidth);)
		{
			auto Frequency = Controls::Border();
			{
				auto Binding = Data::Binding();
				Binding.Source(box_value(to_hstring(FrequencyOffsetConverter::ConvertBack(X))));
				Binding.Converter(make<FMRadio::implementation::FrequencyFormattingConverter>());

				auto FrequencyText = Controls::TextBlock();
				FrequencyText.SetBinding(Controls::TextBlock::TextProperty(), Binding);
				FrequencyText.VerticalAlignment(VerticalAlignment::Center);

				Frequency.Child(FrequencyText);
			}

			Frequency.SetValue(Controls::Canvas::TopProperty(), box_value(0));
			Frequency.SetValue(Controls::Canvas::LeftProperty(), box_value(X - Frequency.ActualWidth() / 2));
			Frequency.SetValue(FrameworkElement::HeightProperty(), box_value(TunerDial().ActualHeight() * ThickMarkHeightScale));
			TunerDial().Children().Append(Frequency);

			// the main line
			{
				auto Line = Shapes::Line();
				Line.X1(X);
				Line.X2(X);
				Line.Y1(TunerDial().ActualHeight() * (1 - ThickMarkHeightScale));
				Line.Y2(TunerDial().ActualHeight());
				Line.Stroke(TunerWindow().Foreground());
				Line.StrokeThickness(ThickMarkWidth);
				TunerDial().Children().Append(Line);
			}

			// the half line
			{
				auto Line = Shapes::Line();
				Line.X1(X + MinorDivisionFrequencyOffset);
				Line.X2(X + MinorDivisionFrequencyOffset);
				Line.Y1(TunerDial().ActualHeight() * (1 - ThinMarkHeightScale));
				Line.Y2(TunerDial().ActualHeight());
				Line.Stroke(TunerWindow().Foreground());
				Line.StrokeThickness(ThinMarkWidth);
				TunerDial().Children().Append(Line);
			}

			X += MajorDivisionFrequencyOffset;
		}
	}

	void MainPage::TunerWindowScrolled(Windows::Foundation::IInspectable const &, Windows::Foundation::IInspectable const &)
	{
		FrequencyUpdateUserOverride = true;
	}

	winrt::event_token MainPage::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const & handler)
	{
		return PropertyChanged_.add(handler);
	}

	void MainPage::PropertyChanged(winrt::event_token const & token)
	{
		PropertyChanged_.remove(token);
	}

	void MainPage::SignalQualityUpdateTicked(Windows::Foundation::IInspectable const &, Windows::Foundation::IInspectable const &)
	{
		unsigned Quality;
		try
		{
			Quality = RadioAPI::Radio->GetSignalQuality();
		}
		catch (std::system_error &)
		{
			SignalQualitySymbolIcon().Symbol(Windows::UI::Xaml::Controls::Symbol::ZeroBars);
			return;
		}

		if (Quality <= 5)
		{
			SignalQualitySymbolIcon().Symbol(Windows::UI::Xaml::Controls::Symbol::ZeroBars);
		}
		else if (Quality <= 25)
		{
			SignalQualitySymbolIcon().Symbol(Windows::UI::Xaml::Controls::Symbol::OneBar);
		}
		else if (Quality <= 50)
		{
			SignalQualitySymbolIcon().Symbol(Windows::UI::Xaml::Controls::Symbol::TwoBars);
		}
		else if (Quality <= 75)
		{
			SignalQualitySymbolIcon().Symbol(Windows::UI::Xaml::Controls::Symbol::ThreeBars);
		}
		else
		{
			SignalQualitySymbolIcon().Symbol(Windows::UI::Xaml::Controls::Symbol::FourBars);
		}
	}
}