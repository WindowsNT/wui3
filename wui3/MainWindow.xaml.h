// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"
extern HWND mw;
#include "..\\common.h"
#include "..\\usm.h"
namespace winrt::wui3::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        int32_t MyProperty();
        void MyProperty(int32_t value);
        void Subclass();
        void Run();
        std::shared_ptr<USMLIBRARY::usm<>> u;
        void RunDialog(int64_t what, WPARAM ww, LPARAM ll);
        unsigned long long current_id = DIALOGID_NONE;
        std::map<unsigned long long, int> ChangedSizeOnce;
        void Cancel(XML3::XML& x);
        void KeyD(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs);
        void KeyD2(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs);
        void szch(winrt::Windows::Foundation::IInspectable const& sender, const winrt::Microsoft::UI::Xaml::SizeChangedEventArgs);
        void szch2(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable);
        void OnChangeSize(winrt::Windows::Foundation::IInspectable const& sender, bool f = 0);
        void Off(XML3::XML& x);
        void AskText_ClickOK(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void AskText_ClickCancel(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

    };
}

namespace winrt::wui3::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
