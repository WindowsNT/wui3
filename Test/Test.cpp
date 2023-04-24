// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>


#include <iostream>
#include <windows.h>
#include <functional>
#include <thread>
#include <vector>
HANDLE hX = 0;
#include <CommCtrl.h>
#include "..\\xml3all.h"
#include "..\\usm.h"

HANDLE hProcessWUI3 = 0;
DWORD Run(const wchar_t* y, bool W, DWORD flg)
{
	PROCESS_INFORMATION pInfo = { 0 };
	STARTUPINFO sInfo = { 0 };

	sInfo.cb = sizeof(sInfo);
	wchar_t yy[1000];
	swprintf_s(yy, 1000, L"%s", y);
	CreateProcess(0, yy, 0, 0, 0, flg, 0, 0, &sInfo, &pInfo);
	SetPriorityClass(pInfo.hProcess, IDLE_PRIORITY_CLASS);
	SetThreadPriority(pInfo.hThread, THREAD_PRIORITY_IDLE);
	if (W)
		WaitForSingleObject(pInfo.hProcess, INFINITE);
	DWORD ec = 0;
	GetExitCodeProcess(pInfo.hProcess, &ec);
	hProcessWUI3 = pInfo.hProcess;
	CloseHandle(pInfo.hThread);
	return ec;
}

#pragma comment(lib,"comctl32.lib")


#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "..\\common.h"
USMLIBRARY::usm<> u(usm_cid, 0, 1024 * 1024, 10);

void wmsg(HWND hh)
{
	MSG msg;
	if (GetMessage(&msg, hh, 0, 0))
	{
		if (msg.message == WM_LBUTTONDOWN || msg.message == WM_LBUTTONUP || msg.message == WM_RBUTTONDOWN || msg.message == WM_RBUTTONUP || msg.message == WM_MOUSEMOVE || msg.message == WM_KEYDOWN || msg.message == WM_KEYUP || msg.message == WM_PAINT)
			return;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void SetOffWUI3()
{
	if (!hProcessWUI3)
		return;
	TerminateProcess(hProcessWUI3, 0);
	CloseHandle(hProcessWUI3);
	hProcessWUI3 = 0;
}


int __stdcall WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
{
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	SetProcessDPIAware();
	InitCommonControls();
	TASKDIALOGCONFIG tc = { 0 };
	tc.cbSize = sizeof(tc);
	tc.hwndParent = 0;
	tc.pszWindowTitle = L"Test";
	//	PCWSTR ic2 = TD_INFORMATION_ICON;
	tc.pszMainIcon = TD_SHIELD_ICON;
	tc.dwFlags = TDF_ENABLE_HYPERLINKS | TDF_CAN_BE_MINIMIZED;
	tc.pszMainInstruction = L"Run";
	tc.pszContent = L"Run";
	tc.cButtons = 2;
	TASKDIALOG_BUTTON btn[2] = {
		{101,L"AskText"},
		{102,L"Message"},
	};
	tc.pButtons = btn;
	tc.pfCallback = [](_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lpx, _In_ LONG_PTR lp) ->HRESULT
	{
		if (msg == TDN_CREATED)
		{
			unsigned long long first = (unsigned long long)hwnd;
			u.Initialize();
			u.WriteData((char*)&first, 8, 0);
			Run(L"..\\wui3\\x64\\debug\\wui3\\wui3.exe", false, 0);

			return S_FALSE;
		}
		if (msg == TDN_BUTTON_CLICKED)
		{
			DWORD dw = 0;
			GetExitCodeProcess(hProcessWUI3, &dw);
			if (dw != STILL_ACTIVE)
				return S_OK;

			auto Yup = [&](HWND hh,unsigned long long id, const char* x1)
			{
				unsigned long long wl = strlen(x1);
				std::vector<char> what(1000);
				memcpy(what.data(), &id, sizeof(id));
				memcpy(what.data() + sizeof(id), &wl, sizeof(wl));
				memcpy(what.data() + sizeof(id) + sizeof(wl), x1, wl);
				ResetEvent(u.hEventAux1);
				ResetEvent(u.hEventAux2);
				u.WriteData((char*)what.data(), sizeof(id) + sizeof(wl) + wl, 0);

				HANDLE h2[2] = { u.hEventAux1,u.hEventAux2 };
				for (;;)
				{
					auto wi = WaitForMultipleObjects(2, h2, false, 2000);
					if (wi == WAIT_OBJECT_0)
					{
						wmsg(hh);
						continue; // OK, dialog is showing
					}
					if (wi != (WAIT_OBJECT_0 + 1))
					{
						
						SetOffWUI3();
						// Fail here
					}
					break; // dialog ended
				}

				unsigned long long how = 0;
				what.clear();
				auto rd = u.BeginRead();
				memcpy(&how, rd, sizeof(how));
				if (how < 1024 * 1024)
				{
					what.resize(how);
					memcpy(what.data(), rd + sizeof(how), how);
				}
				u.EndRead();
				XML3::XML x;
				what.resize(what.size() + 1);
				x = what.data();
				
				// Parse x to check return values of the dialogs

			};

			if (wParam == 101)
			{
				auto id = DIALOGID_ASKTEXT;
				const char* x1 = R"(<?xml?><e title="Ask" t0="This is bold title" t1="Enter value:" t2="100" t3="Placeholder text" tOK="OK" tCancel="Cancel" />)";
				Yup(hwnd,id,x1);
				return S_FALSE;
			}

		
			if (wParam == 102)
			{
				auto id = DIALOGID_MESSAGE1;
				const char* x1 = R"(<?xml?><e title="Tralalalala" t1="Hello there" tOK="OK"  />)";
				Yup(hwnd,id, x1);
				return S_FALSE;
			}

		
		}
		return S_OK;
	};


	int rv = 0;
	BOOL ch = 0;
	TaskDialogIndirect(&tc, &rv, 0, &ch);



}
