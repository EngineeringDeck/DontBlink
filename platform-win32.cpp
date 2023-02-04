#include "platform-win32.h"

PlatformWin32::PlatformWin32()
{
	windowEvents=SetWinEventHook(EVENT_SYSTEM_FOREGROUND,EVENT_SYSTEM_FOREGROUND,nullptr,&PlatformWin32::ForwardForegroundWindowChanged,0,0,WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS);
	if (!windowEvents) throw std::runtime_error("Failed to subscribe to system window events");
}

PlatformWin32::~PlatformWin32()
{
	if (!UnhookWinEvent(windowEvents))
		emit Log("Failed to unsubscribe from system window events");
	else
		emit Log("Unsubscribed from system window events");
}

QString PlatformWin32::GetWindowTitle(HWND window)
{
	int titleLength=GetWindowTextLength(window); // TODO: does this have to be +1 now?
	if (titleLength < 1) return {};
	QByteArray title(++titleLength,'\0');
	if (!GetWindowTextA(window,title.data(),titleLength)) // FIXME: don't assume LPSTR here
	{
		DWORD errorCode=GetLastError();
		Log("Failed to obtain window title text (" + QString::number(GetLastError()) + ")");
		return {};
	}

	int delimiterIndex=title.lastIndexOf("- ");
	if (delimiterIndex < 0) return {title};
	delimiterIndex+=2;
	return QString(title).right(title.size()-delimiterIndex);
}

VOID CALLBACK PlatformWin32::ForegroundWindowChangedWin32(HWINEVENTHOOK windowEvents,DWORD event,HWND window,LONG objectID,LONG childID,DWORD eventThread,DWORD timestamp)
{
	if (QString title=GetWindowTitle(window); ValidTargetWindow(title)) emit ForegroundWindowChanged(title); // FIXME: why can't compiler choose the correct ForegroundWindowChanged by the number/type of parameters
}

VOID CALLBACK PlatformWin32::ForwardForegroundWindowChanged(HWINEVENTHOOK windowEvents,DWORD event,HWND window,LONG objectID,LONG childID,DWORD eventThread,DWORD timestamp)
{
	static_cast<PlatformWin32*>(self)->ForegroundWindowChangedWin32(windowEvents,event,window,objectID,childID,eventThread,timestamp);
}

void PlatformWin32::UpdateAvailableWindows()
{
	std::unordered_set<QString> titles;
	EnumWindows(&PlatformWin32::ForwardWindowAvailable,reinterpret_cast<LPARAM>(&titles));
	emit AvailableWindowsUpdated(titles);
}

BOOL CALLBACK PlatformWin32::WindowAvailable(HWND window,LPARAM data) // data is the set of triggers
{
	if (!IsWindowVisible(window) || (GetWindowLong(window,GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) return TRUE; // ignore tooltip windows (why ignore non-visible windows?)

	HWND tryHandle=nullptr;
	HWND walkHandle=nullptr;
	tryHandle=GetAncestor(window,GA_ROOTOWNER);
	while (tryHandle != walkHandle)
	{
		walkHandle=tryHandle;
		tryHandle=GetLastActivePopup(walkHandle);
		if (IsWindowVisible(tryHandle)) break;
	}
	if (walkHandle != window) return TRUE;

	TITLEBARINFO titlebarInfo={
		.cbSize=sizeof(titlebarInfo)
	};
	GetTitleBarInfo(window,&titlebarInfo);
	if (titlebarInfo.rgstate[0] & STATE_SYSTEM_INVISIBLE) return TRUE;

	std::unordered_set<QString> *triggers=reinterpret_cast<std::unordered_set<QString>*>(data); // cast the data to the set of triggers
	if (GetWindowLong(window,GWL_STYLE) & WS_CHILD) return TRUE;
	if (QString title=GetWindowTitle(window); ValidTargetWindow(title)) if (!triggers->contains(title)) triggers->insert(title);

	return TRUE;
}

BOOL CALLBACK PlatformWin32::ForwardWindowAvailable(HWND window,LPARAM data)
{
	return static_cast<PlatformWin32*>(self)->WindowAvailable(window,data);
}

bool PlatformWin32::ValidTargetWindow(const QString &title)
{
	return !title.isNull() && !(title.startsWith("OBS") && title.contains("Profile") && title.contains("Scenes"));
}