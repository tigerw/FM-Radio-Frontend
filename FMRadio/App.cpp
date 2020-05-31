#include "pch.h"

#include "App.h"
#include "MainPage.h"
#include "AntennaNotPresentPage.h"
#include "UnsupportedDevicePage.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace FMRadio;
using namespace FMRadio::implementation;

RadioAPI * RadioAPI::Radio;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();
    Suspending({ this, &App::OnSuspending });

	// Set the locale to the system locale
	std::locale::global(std::locale(""));

	RadioAPI::Radio = &Radio;

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const & Event)
{
	assert(!Event.PrelaunchActivated());

    Frame RootFrame{ nullptr };
    auto Content = Window::Current().Content();

    if (Content)
    {
        RootFrame = Content.as<Frame>();
    }

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (RootFrame == nullptr)
    {
        // Create a Frame to act as the navigation context and associate it with
        // a SuspensionManager key
        RootFrame = Frame();

        RootFrame.NavigationFailed({ this, &App::OnNavigationFailed });

        if (Event.PreviousExecutionState() == ApplicationExecutionState::Terminated)
        {
            // Restore the saved session state only when appropriate, scheduling the
            // final launch steps after the restore is complete
        }

        if (RootFrame.Content() == nullptr)
        {
            // When the navigation stack isn't restored navigate to the first page,
            // configuring the new page by passing required information as a navigation
            // parameter
			NavigateDependingOnInitialisation(RootFrame);
        }

        // Place the frame in the current Window
        Window::Current().Content(RootFrame);
    }
    else
    {
        if (RootFrame.Content() == nullptr)
        {
            // When the navigation stack isn't restored navigate to the first page,
            // configuring the new page by passing required information as a navigation
            // parameter
			NavigateDependingOnInitialisation(RootFrame);
        }
    }

	// Ensure the current window is active
	Window::Current().Activate();
}

void App::NavigateDependingOnInitialisation(Frame & RootFrame)
{
	try
	{
		Radio.OnAntennaInserted = [this] {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this] {
					Window::Current().Content().as<Frame>().Navigate(xaml_typename<FMRadio::MainPage>());
				}
			);
		};

		Radio.OnAntennaRemoved = [this] {
			Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher().RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				[this] {
					Window::Current().Content().as<Frame>().Navigate(xaml_typename<FMRadio::AntennaNotPresentPage>());
				}
			);
		};

		Radio.Initialise();
	}
	catch (std::system_error & Error)
	{
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter("Conversion to UTF-8 failed when displaying an error", L"Conversion to UTF-16 failed when displaying an error");
			std::wstring wide = converter.from_bytes(Error.what());
		}

		{
			std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter("Conversion to UTF-8 failed when displaying an error", L"Conversion to UTF-16 failed when displaying an error");
			std::wstring wide = converter.from_bytes(Error.what());
		}

		{
			auto w = Error.what();
			std::size_t len = 1 + std::mbsrtowcs(nullptr, &w, 0, nullptr);
			std::wstring wstr(len, wchar_t());
			std::mbsrtowcs(wstr.data(), &w, wstr.size(), nullptr);
		}

		{
			size_t len = MultiByteToWideChar(CP_ACP, 0, Error.what(), -1, nullptr, 0);
			std::wstring wstr(len, wchar_t());
			MultiByteToWideChar(CP_ACP, 0, Error.what(), -1, wstr.data(), wstr.size());
		}

		//RootFrame.Navigate(xaml_typename<FMRadio::UnsupportedDevicePage>(), box_value(std::move(wide)));
	}
	catch (...)
	{
		RootFrame.Navigate(xaml_typename<FMRadio::UnsupportedDevicePage>(), box_value(L"Unknown error"));
	}
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending([[maybe_unused]] IInspectable const& sender, [[maybe_unused]] SuspendingEventArgs const& e)
{
    // Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(IInspectable const&, NavigationFailedEventArgs const& e)
{
    throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}