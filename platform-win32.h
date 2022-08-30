#pragma once

#include <Windows.h>
#include "platform.h"

class PlatformWin32 : public Platform
{
public:
	PlatformWin32();
	~PlatformWin32() override;
protected:
	HWINEVENTHOOK windowEvents;
	QString GetWindowTitle(HWND window);
	VOID CALLBACK ForegroundWindowChangedWin32(HWINEVENTHOOK windowEvents,DWORD event,HWND window,LONG objectID,LONG childID,DWORD eventThread,DWORD timestamp);
	BOOL CALLBACK WindowAvailable(HWND window,LPARAM data);
	static VOID CALLBACK ForwardForegroundWindowChanged(HWINEVENTHOOK windowEvents,DWORD event,HWND window,LONG objectID,LONG childID,DWORD eventThread,DWORD timestamp);
	static BOOL CALLBACK ForwardWindowAvailable(HWND window,LPARAM data);
public slots:
	void UpdateAvailableWindows() override;
};
