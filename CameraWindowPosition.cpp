// CameraWindowPosition.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <vector>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#include <WinUser.h>

HWND cameraWinOne;
HWND cameraWinTwo;

void killProcessByName(const WCHAR* filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (wcscmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}

BOOL CALLBACK handleWindow(HWND hwnd, LPARAM lParam) {
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];

    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    int length = ::GetWindowTextLength(hwnd);
    std::wstring title(&windowTitle[0]);
    if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
        return TRUE;
    }

    if (title.find(L"streamed by RTSP") != std::string::npos) {
        if (!cameraWinOne) {
            cameraWinOne = hwnd;
        } else {
            cameraWinTwo = hwnd;
        }

    }

    return TRUE;
}

HMONITOR monitorOne;
HMONITOR monitorTwo;

BOOL CALLBACK getMonitor(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    if (!monitorOne) {
        monitorOne = hMonitor;
    }
    else if(!monitorTwo) {
        monitorTwo = hMonitor;
    }
    return TRUE;
}

int main()
{
    EnumWindows(handleWindow, NULL);
    EnumDisplayMonitors(NULL, NULL, getMonitor, NULL);
      
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(monitorTwo, &monitorInfo);
    RECT mr = monitorInfo.rcMonitor;
    long halfDesktopWidth = (mr.right - mr.left) / 2;
    long halfDesktopHeight = (mr.bottom - mr.top) / 2;

    std::cout << "Window size width(" << halfDesktopWidth << ") height(" << halfDesktopHeight << ")\n";

    if (cameraWinOne) {
        std::cout << "Moving window one to left top\n";
        SetWindowLong(cameraWinOne, GWL_STYLE, WS_BORDER);
        SetWindowPos(cameraWinOne, NULL, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
            halfDesktopWidth, halfDesktopHeight, SWP_SHOWWINDOW);
    }

    if (cameraWinTwo) {
		std::cout << "Moving window two to left bottom\n";
        SetWindowLong(cameraWinTwo, GWL_STYLE, WS_BORDER);
	    SetWindowPos(cameraWinTwo, NULL,  monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.top + halfDesktopHeight, halfDesktopWidth,
            halfDesktopHeight, SWP_SHOWWINDOW);
    }

    std::cout << "Press enter key to close windows";
	std::cin.get();

    if (cameraWinOne) {
        CloseWindow(cameraWinOne);
    }
    
    if (cameraWinTwo) {
        CloseWindow(cameraWinTwo);
    }

    killProcessByName(L"ffplay.exe");
}

