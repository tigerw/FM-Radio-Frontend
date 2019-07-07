#include "pch.h"

#include "App.h"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace FMRadio;
using namespace FMRadio::implementation;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale());
	std::wcout.imbue(std::locale());

	InitializeComponent();
	Suspending({ this, &App::OnSuspending });

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
void App::OnLaunched(LaunchActivatedEventArgs const& e)
{
	Frame rootFrame{ nullptr };
	auto content = Window::Current().Content();
	if (content)
	{
		rootFrame = content.try_as<Frame>();
	}

	// Do not repeat app initialization when the Window already has content,
	// just ensure that the window is active
	if (rootFrame == nullptr)
	{
		// Create a Frame to act as the navigation context and associate it with
		// a SuspensionManager key
		rootFrame = Frame();

		rootFrame.NavigationFailed({ this, &App::OnNavigationFailed });

		if (e.PreviousExecutionState() == ApplicationExecutionState::Terminated)
		{
			// Restore the saved session state only when appropriate, scheduling the
			// final launch steps after the restore is complete
		}

		if (e.PrelaunchActivated() == false)
		{
			NavigateDependingOnInitialisation(rootFrame, e, true);
		}
	}
	else
	{
		if (e.PrelaunchActivated() == false)
		{
			NavigateDependingOnInitialisation(rootFrame, e, false);
		}
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

void App::NavigateDependingOnInitialisation(Frame & RootFrame, const LaunchActivatedEventArgs & Event, bool FrameCreatedAnew)
{
	try
	{
		Radio.Initialise();
		NavigateToMainPage(RootFrame, Event, FrameCreatedAnew);
	}
	catch (std::system_error& Error)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter("Conversion to UTF-8 failed when displaying an error", L"Conversion to UTF-16 failed when displaying an error");
		std::wstring wide = converter.from_bytes(Error.what());

		NavigateToUnsupportedDevicePage(RootFrame, Event, FrameCreatedAnew, std::move(wide));
	}
	catch (...)
	{
		NavigateToUnsupportedDevicePage(RootFrame, Event, FrameCreatedAnew, L"Unknown error");
	}
}

void App::NavigateToMainPage(Frame & RootFrame, const LaunchActivatedEventArgs & Event, bool FrameCreatedAnew)
{
	if (RootFrame.Content() == nullptr)
	{
		// When the navigation stack isn't restored navigate to the first page,
		// configuring the new page by passing required information as a navigation
		// parameter
		RootFrame.Navigate(xaml_typename<FMRadio::MainPage>(), box_value(Event.Arguments()));
	}

	if (FrameCreatedAnew)
	{
		// Place the frame in the current Window
		Window::Current().Content(RootFrame);
	}

	// Ensure the current window is active
	Window::Current().Activate();

	// Do secondary initialisation on the MainPage (acquire initial state) only if the hardware is supported
	RootFrame.Content().as<struct MainPage>()->Initialise(&Radio);
}

void App::NavigateToUnsupportedDevicePage(Frame & RootFrame, const LaunchActivatedEventArgs & Event, bool FrameCreatedAnew, std::wstring Message)
{
	if (RootFrame.Content() == nullptr)
	{
		// When the navigation stack isn't restored navigate to the error page,
		// configuring the new page by passing required information as a navigation
		// parameter
		RootFrame.Navigate(xaml_typename<FMRadio::UnsupportedDevicePage>(), box_value(Event.Arguments()));
	}

	if (FrameCreatedAnew)
	{
		// Place the frame in the current Window
		Window::Current().Content(RootFrame);
	}

	// Ensure the current window is active
	Window::Current().Activate();

	// Do secondary initialisation on the UnsupportedDevicePage to give it the error message to display
	RootFrame.Content().as<struct UnsupportedDevicePage>()->Initialise(std::move(Message));
}