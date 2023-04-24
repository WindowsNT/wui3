// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace wui3;
using namespace wui3::implementation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

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
/// Invoked when the application is launched.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
/// 

void tx(App* app)
{
    if (!app)
        return;
    auto w = app->window.as<winrt::wui3::implementation::MainWindow>();
    w->Run();
}

void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    auto ew = make<MainWindow>();
    window = ew;
    window.Activate();
    if (window)
    {
        auto n = window.as<IWindowNative>();
        if (n)
        {
            n->get_WindowHandle(&mw);
            if (mw)
            {
                ShowWindow(mw, SW_HIDE);
                auto ew2 = window.as<MainWindow>();
                ew2->Subclass();
                std::thread t(tx, this);
                t.detach();
            }
        }
    }
}
