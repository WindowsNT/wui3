 // Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

HWND mw = 0;

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

static WNDPROC Old = 0;


BOOL CenterWindow(HWND hwndWindow)
{
    RECT rectWindow = {}, rectParent = {};

    rectParent.right = GetSystemMetrics(SM_CXSCREEN);
    rectParent.bottom = GetSystemMetrics(SM_CYSCREEN);

    // make the window relative to its parent
    GetWindowRect(hwndWindow, &rectWindow);

    int nWidth = rectWindow.right - rectWindow.left;
    int nHeight = rectWindow.bottom - rectWindow.top;

    int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
    int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // make sure that the dialog box never moves outside of the screen
    if (nX < 0) nX = 0;
    if (nY < 0) nY = 0;
    if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
    if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

    MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

    return TRUE;
}


winrt::wui3::implementation::MainWindow* mwt = 0;
LRESULT CALLBACK NEWW_WP(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
    if (mm == WM_CLOSE && ww != 0xFEFEFEFE)
    {
        XML3::XML x;
        if (mwt->current_id == DIALOGID_ASKTEXT)
        {
            auto dlg = mwt->AskText();
            dlg.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        }
        if (mwt->current_id == DIALOGID_MESSAGE1)
        {
            auto dlg = mwt->Message1();
            dlg.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        }
        mwt->Cancel(x);
        return 0;
    }
    if (mm == WM_APP)
    {
        if (ww < DIALOGID_MAX)
            mwt->RunDialog(mm + ww, 0, ll);
        return 0;
    }
    if (mm == WM_TIMER)
    {
        if (mwt->current_id != DIALOGID_NONE)
        {
            SetEvent(mwt->u->hEventAux1);
        }
        return 0;
    }
    return CallWindowProc(Old, hh, mm, ww, ll);
}


unsigned long long HwndOfWin32App = 0;

namespace winrt::wui3::implementation
{

    void MainWindow::Cancel(XML3::XML& x)
    {
        AskText().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        Message1().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        Off(x);
    }


    void MainWindow::RunDialog(int64_t mm, WPARAM ww, LPARAM ll)
    {
        unsigned long long id = mm - WM_APP;
        XML3::XML xx;
        if (ll)
            xx = (char*)ll;
        current_id = id;
        AskText().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        Message1().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        if (id == DIALOGID_ASKTEXT)
        {
            XML3::XML* x = (XML3::XML*)&xx;
            Title(x->GetRootElement().vv("title").GetWideValue().c_str());
            AskText_Question().Title(x->GetRootElement().vv("t0").GetWideValue().c_str());
            AskText_Question().Message(x->GetRootElement().vv("t1").GetWideValue().c_str());
            AskText_Response().Text(x->GetRootElement().vv("t2").GetWideValue().c_str());
            AskText_Response().PlaceholderText(x->GetRootElement().vv("t3").GetWideValue().c_str());

            if (x->GetRootElement().vv("big").GetValueInt())
            {
                AskText_Response().TextWrapping(winrt::Microsoft::UI::Xaml::TextWrapping::Wrap);
                AskText_Response().AcceptsReturn(true);
                AskText_Response().MinHeight(100);
            }
            AskText_Response().SelectAll();
            AskText_OK().Content(box_value(x->GetRootElement().vv("tOK").GetWideValue().c_str()));
            AskText_Cancel().Content(box_value(x->GetRootElement().vv("tCancel").GetWideValue().c_str()));


            SetWindowPos(mw, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
            auto dlg = AskText();
            dlg.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
            if (ChangedSizeOnce[current_id])
            {
                OnChangeSize(dlg, 1);
            }
        }

        if (id == DIALOGID_MESSAGE1)
        {
            XML3::XML* x = (XML3::XML*)&xx;
            Title(x->GetRootElement().vv("title").GetWideValue().c_str());
            Message1_Question().Title(x->GetRootElement().vv("t0").GetWideValue().c_str());
            Message1_Question().Message(x->GetRootElement().vv("t1").GetWideValue().c_str());
            Message1_OK().Content(box_value(x->GetRootElement().vv("tOK").GetWideValue().c_str()));
            SetWindowPos(mw, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
            auto dlg = Message1();
            dlg.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
            if (ChangedSizeOnce[current_id])
            {
                OnChangeSize(dlg, 1);
            }
        }

    }

    void MainWindow::Run()
    {
        // Create the mutex 
        u = std::make_shared<USMLIBRARY::usm<>>(usm_cid, 0, 1024 * 1024, 10);
        u->Initialize();
        u->ReadData((char*)&HwndOfWin32App, 8, 0);
        SetTimer(mw, 1, 500, 0);
        u->WriteData((char*)&mw, sizeof(HWND), 0, 0);
        SetEvent(u->hEventAux1);
        for (;;)
        {
            auto j = u->NotifyWrite(true, 5000);
            if (j == WAIT_TIMEOUT)
            {
                if (!IsWindow((HWND)HwndOfWin32App))
                    break;
            }
            else
            {
                unsigned long long id = DIALOGID_NONE;
                auto rd = u->BeginRead();
                if (!rd)
                    continue;
                unsigned long long xlen = 0;
                memcpy(&id, rd + 0, sizeof(id));
                memcpy(&xlen, rd + sizeof(id), sizeof(xlen));
                std::vector<char> xmld;
                if (xlen < (1024 * 1024))
                {
                    xmld.resize(xlen + 1);
                    memcpy(xmld.data(), rd + sizeof(id) + sizeof(xlen), xlen);
                }
                u->EndRead();
                if (xlen < (1024 * 1024))
                    SendMessage(mw, WM_APP, id, (LPARAM)xmld.data());
            }
        }
        if (mw)
            PostMessage(mw, WM_CLOSE, 0xFEFEFEFE, 0);

    }

    MainWindow::MainWindow()
    {
        InitializeComponent();
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

  

    void MainWindow::Subclass()
    {
        Old = (WNDPROC)GetWindowLongPtr(mw, GWLP_WNDPROC);
        SetWindowLongPtr(mw, GWLP_WNDPROC, (LONG_PTR)NEWW_WP);
        mwt = this;

    }

    void MainWindow::KeyD(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs r)
    {
        auto k = r.Key();
        if (k == winrt::Windows::System::VirtualKey::Enter)
        {
            if (current_id == DIALOGID_MESSAGE1)
            {
                Microsoft::UI::Xaml::RoutedEventArgs a;
                AskText_ClickCancel(sender, a);
            }
        }
        if (k == winrt::Windows::System::VirtualKey::Escape)
        {
            if (current_id == DIALOGID_MESSAGE1)
            {
                Microsoft::UI::Xaml::RoutedEventArgs a;
                AskText_ClickCancel(sender, a);
            }
        }
    }
    void MainWindow::KeyD2(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs r)
    {
        auto k = r.Key();
        if (k == winrt::Windows::System::VirtualKey::Enter)
        {
            if (current_id == DIALOGID_ASKTEXT)
            {
                Microsoft::UI::Xaml::RoutedEventArgs a;
                AskText_ClickOK(sender, a);
            }
            if (current_id == DIALOGID_MESSAGE1)
            {
                Microsoft::UI::Xaml::RoutedEventArgs a;
                AskText_ClickCancel(sender, a);
            }
        }
        if (k == winrt::Windows::System::VirtualKey::Escape)
        {
            XML3::XML x;
            Cancel(x);
        }
    }
    void MainWindow::szch(winrt::Windows::Foundation::IInspectable const& sender, const winrt::Microsoft::UI::Xaml::SizeChangedEventArgs)
    {
        OnChangeSize(sender);

    }
    void MainWindow::szch2(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable)
    {
        OnChangeSize(sender);

    }
    void MainWindow::OnChangeSize(winrt::Windows::Foundation::IInspectable const& sender, bool f)
    {
        if (f)
            ChangedSizeOnce[current_id] = 0;

        if (ChangedSizeOnce[current_id])
            return;
        ChangedSizeOnce[current_id] = 1;

        auto dlg = sender.as<winrt::Microsoft::UI::Xaml::Controls::StackPanel>();
        float xy = GetDpiForWindow(mw) / 96.0f;
        auto wi5 = dlg.ActualWidth() * xy;
        auto he5 = dlg.ActualHeight() * xy;
        wi5 += GetThemeSysSize(0, SM_CXBORDER);
        he5 += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYEDGE) * 2;
        SetWindowPos(mw, HWND_TOPMOST, 0, 0, (int)wi5, (int)he5, SWP_SHOWWINDOW | SWP_NOMOVE);
        CenterWindow(mw);

        if (current_id == DIALOGID_ASKTEXT)
        {
            AskText_Response().SelectAll();
            AskText_Response().Focus(winrt::Microsoft::UI::Xaml::FocusState::Keyboard);
        }


    }

    void MainWindow::AskText_ClickOK(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        XML3::XML x;
        x.GetRootElement().vv("a").SetValueInt(1);
        x.GetRootElement().vv("t2").SetWideValue(AskText_Response().Text().c_str());
        Off(x);
    }
    void MainWindow::AskText_ClickCancel(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args)
    {
        XML3::XML x;
        x.GetRootElement().vv("a").SetValueInt(0);
        Cancel(x);
    }

    void MainWindow::Off(XML3::XML& x)
    {
        ShowWindow(mw, SW_HIDE);
        auto s = x.Serialize();
        auto rd = u->BeginWrite();
        if (!rd)
            return;
        unsigned long long xlen = s.length();
        memcpy(rd, &xlen, sizeof(xlen));
        memcpy(rd + sizeof(xlen), s.data(), s.length());
        u->EndWrite();
        current_id = DIALOGID_NONE;
        SetEvent(u->hEventAux2);

    }

}
