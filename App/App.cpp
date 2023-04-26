// App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <MddBootstrap.h>
#include <uxtheme.h>
#undef GetCurrentTime
#include <winrt/Microsoft.UI.Xaml.h>
#include <MddBootstrap.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Xaml.XamlTypeInfo.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <thread>
#include <microsoft.ui.xaml.window.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>


using namespace winrt;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::XamlTypeInfo;
using namespace Microsoft::UI::Xaml::Markup;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::Foundation;

using namespace winrt;
using namespace Microsoft::UI::Xaml;

using namespace winrt;
using namespace Microsoft::UI::Xaml;

#pragma comment(lib,"uxtheme.lib")

LRESULT CALLBACK NEWP(HWND hh, UINT mm, WPARAM ww, LPARAM ll);
WNDPROC old = 0;
struct MyW : public Window
{
	bool Activated = 0;
	HWND hwnd = 0;


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

	void OnChangeSize(winrt::Windows::Foundation::IInspectable const& sender, bool f)
	{
		auto dlg = sender.as<winrt::Microsoft::UI::Xaml::Controls::StackPanel>();
		auto strn = dlg.Name();
		float xy = GetDpiForWindow(hwnd) / 96.0f;
		auto wi5 = dlg.ActualWidth() * xy;
		auto he5 = dlg.ActualHeight() * xy;
		wi5 += GetThemeSysSize(0, SM_CXBORDER);
		he5 += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYEDGE) * 2;
		SetWindowPos(hwnd, 0, 0, 0, (int)wi5, (int)he5, SWP_SHOWWINDOW | SWP_NOMOVE);
		CenterWindow(hwnd);
	}

	void Sub()
	{
		// Subclass
		auto n = as <IWindowNative>();
		if (n)
		{
			n->get_WindowHandle(&hwnd);
			old = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
			SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)NEWP);
		}

		// Other Stuff
		Panel p = Content().as<Panel>();
		p.FindName(L"MyButton").as<Button>().Click([&](IInspectable const&, RoutedEventArgs const&)
			{
				MessageBox(0, L"Clicked", 0, 0);
			});

		Panel sp = p.FindName(L"rsp2").as<Panel>();
			sp.SizeChanged([&](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable)
				{
					OnChangeSize(sender, false);
				});
	}
};

LRESULT CALLBACK NEWP(HWND hh, UINT mm, WPARAM ww, LPARAM ll)
{
	return CallWindowProc(old,hh,mm,ww,ll);
}

HRESULT ExtractResourceToFile(HINSTANCE hXX, const TCHAR* Name, const TCHAR* ty, const wchar_t* putfile, bool skipifexists = false)
{
	HRSRC R = FindResource(hXX, Name, ty);
	if (!R)
		return E_NOINTERFACE;
	HGLOBAL hG = LoadResource(hXX, R);
	if (!hG)
		return E_FAIL;
	DWORD S = SizeofResource(hXX, R);
	char* p = (char*)LockResource(hG);
	if (!p)
	{
		FreeResource(R);
		return E_FAIL;
	}
	HANDLE w = CreateFile(putfile, GENERIC_WRITE, 0, 0, skipifexists ? CREATE_NEW : CREATE_ALWAYS, 0, 0);

	if (w == INVALID_HANDLE_VALUE)
	{
		FreeResource(R);
		return E_FAIL;
	}
	DWORD A;
	WriteFile(w, p, S, &A, 0);
	CloseHandle(w);
	FreeResource(R);
	return S_OK;
}

std::wstring TempFile4(const wchar_t* prf, const wchar_t* tempp, const wchar_t* forcename)
{
	std::vector<wchar_t> td(1000);
	GetTempPathW(1000, td.data());

	if (wcslen(td.data()))
	{
		if (td[wcslen(td.data()) - 1] != '\\')
			wcscat_s(td.data(), 1000, L"\\");
	}
	if (tempp)
		wcscpy_s(td.data(), 1000, tempp);
	std::vector<wchar_t> x(1000);
	if (forcename)
	{
		swprintf_s(x.data(), 1000, L"%s\\%s", td.data(), forcename);
		return x.data();
	}
	int iCR = GetTempFileNameW(td.data(), prf, 0, x.data());
	DeleteFile(x.data());

	return x.data();
}



bool FirstRun = 1;
class AppL : public ApplicationT<AppL, IXamlMetadataProvider>
{
	XamlControlsXamlMetaDataProvider provider;
public:
	std::vector<std::wstring> urls;
	std::vector<MyW> windows;
	void BuildURLS()
	{
		urls.clear();
		wchar_t x[200] = {};
		auto tf = TempFile4(0,0,0);
		tf += L".xbf";
		ExtractResourceToFile(GetModuleHandle(0), L"L1", L"DATA", tf.c_str());
		swprintf_s(x, 200, L"ms-appx://local/%s", tf.c_str());
		urls.push_back(x);
	}
	void L2()
	{
		BuildURLS();
		if (FirstRun)
		{
			Resources().MergedDictionaries().Append(XamlControlsResources());
			for (size_t i = 0; i < urls.size(); i++)
			{
				windows.emplace_back(MyW());
			}
			for (size_t i = 0; i < urls.size(); i++)
			{
				Application::LoadComponent(windows[i], Uri(urls[i].c_str()));
			}
			FirstRun = 0;
			windows[0].Activate();
			windows[0].Activated = 1;
			windows[0].Sub();
		}
		else
		{
			for (size_t i = 0; i < urls.size(); i++)
			{
				windows[i].Activate();
				windows[i].Activated = 1;
			}
		}
	}
	void OnLaunched(LaunchActivatedEventArgs const&)
	{
		L2();
	}
	IXamlType GetXamlType(TypeName const& type)
	{
		return provider.GetXamlType(type);
	}
	IXamlType GetXamlType(hstring const& fullname)
	{
		return provider.GetXamlType(fullname);
	}
	com_array<XmlnsDefinition> GetXmlnsDefinitions()
	{
		return provider.GetXmlnsDefinitions();
	}
};



int __stdcall WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
{
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	PACKAGE_VERSION pg = {};
	typedef HRESULT(__stdcall* mi)(
		UINT32 majorMinorVersion,
		PCWSTR versionTag,
		PACKAGE_VERSION minVersion);
	const wchar_t* dll = L"Microsoft.WindowsAppRuntime.Bootstrap.dll";
	auto hL = LoadLibrary(dll);
	mi M = (mi)GetProcAddress(hL, "MddBootstrapInitialize");
	if (!M)
		return E_FAIL;
	auto hr = M(0x00010003, L"", pg);
	auto app3 = make<AppL>();
	Application::Start([&](auto&&) {
		app3;
		});
	return 0;
}
