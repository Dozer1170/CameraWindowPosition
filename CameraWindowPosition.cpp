// CameraWindowPosition.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <vector>
#include <iostream>
#include <string>
#include <windows.h>
#include <WinUser.h>

HWND cameraWinOne;
HWND cameraWinTwo;

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
        std::cout << "Moving window one to left top";
        SetWindowLong(cameraWinOne, GWL_STYLE, WS_BORDER);
        SetWindowPos(cameraWinOne, NULL, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
            halfDesktopWidth, halfDesktopHeight, SWP_SHOWWINDOW);
    }

    if (cameraWinTwo) {
		std::cout << "Moving window two to left bottom";
        SetWindowLong(cameraWinTwo, GWL_STYLE, WS_BORDER);
	    SetWindowPos(cameraWinTwo, NULL,  monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.top + halfDesktopHeight, halfDesktopWidth,
            halfDesktopHeight, SWP_SHOWWINDOW);
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
