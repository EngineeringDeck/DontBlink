#include <obs-module.h>
#include <obs-source.h>
#include <obs-frontend-api.h>
#include <Windows.h>
#include <QString>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include "widgets.h"

OBS_DECLARE_MODULE()

std::unordered_map<QString,obs_source_t*> sources;
std::unordered_map<QString,QString> triggers;

ScrollingList *sourcesWidget;

HWINEVENTHOOK windowEvents;
HHOOK shellEvents;

const char *SUBSYSTEM_NAME="[Don't Blink]";

using SourcePtr=std::unique_ptr<obs_source_t,decltype(&obs_source_release)>;
using SourceListPtr=std::unique_ptr<obs_frontend_source_list,decltype(&obs_frontend_source_list_free)>;
using SceneItemPtr=std::unique_ptr<obs_sceneitem_t,decltype(&obs_sceneitem_release)>;

void Log(const QString &message)
{
	if (!message.isNull()) blog(LOG_INFO,"%s%s%s",SUBSYSTEM_NAME," ",message.toLocal8Bit().constData());
}

void Warn(const QString &message)
{
	blog(LOG_WARNING,"%s%s%s",SUBSYSTEM_NAME," ",message.toLocal8Bit().data());
}

QStringList SourceNames()
{
	QStringList list;
	for (const std::pair<QString,obs_source_t*> &pair : sources) list.append(pair.first);
	return list;
}

QString GetWindowTitle(HWND window)
{
	int titleLength=GetWindowTextLength(window); // TODO: does this have to be +1 now?
	if (titleLength < 1) return {};
	QByteArray title(++titleLength,'\0');
	if (!GetWindowText(window,title.data(),titleLength)) // FIXME: don't assume LPSTR here
	{
		DWORD errorCode=GetLastError();
		Log("Failed to obtain window title text (" + QString::number(GetLastError()) + ")");
		// for some reason, EnumWindows results in a lot of windows with 1 character long titles that
		// I can't call GetWindowText on, so just filter them out
		return {};
	}

	return {title};
}

VOID CALLBACK ForegroundWindowChanged(HWINEVENTHOOK windowEvents,DWORD event,HWND window,LONG objectID,LONG childID,DWORD eventThread,DWORD timestamp)
{
	QString title=GetWindowTitle(window);
	if (title.isNull()) return;

	obs_scene_t *scene=obs_scene_from_source(SourcePtr(obs_frontend_get_current_scene(),&obs_source_release).get()); // passing NULL into obs_scene_from_source() does not crash
	if (!scene) throw std::runtime_error("Could not determine current scene");
	if (QString sourceName=sourcesWidget->Source(title); !sourceName.isNull())
	{
		Log("Change Window: " + sourceName);
		obs_sceneitem_set_visible(obs_scene_find_source(scene,sourceName.toLocal8Bit().data()),true);
	}
	Log(GetWindowTitle(window));
}

LRESULT CALLBACK ShellEvent(int code,WPARAM wParam,LPARAM lParam)
{
	if (code < 0) return CallNextHookEx(0,code,wParam,lParam);

	switch (code)
	{
	case HSHELL_WINDOWCREATED:
		Log("Window Created");
		break;
	case HSHELL_WINDOWDESTROYED:
		Log("Window Destroyed");
		break;
	default:
		break;
	}

	return 0;
}

BOOL CALLBACK WindowAvailable(HWND window,LPARAM lParam)
{
	if (GetWindowLong(window,GWL_STYLE) & WS_CHILD) return TRUE;
	if (QString title=GetWindowTitle(window); !title.isNull()) if (!triggers.contains(title)) triggers[title]=QString();
	return TRUE;
}

void UpdateAvailbleWindows()
{
	EnumWindows(WindowAvailable,NULL);
	QStringList sourceNames=SourceNames();
	for (const std::pair<QString,QString> &pair : triggers) sourcesWidget->AddEntry(new CrossReference(pair.first,sourceNames,sourcesWidget));
}

bool AvailableSource(obs_scene_t *scene,obs_sceneitem_t *item,void *data)
{
	obs_source_t *source=obs_sceneitem_get_source(item);
	sources[obs_source_get_name(source)]=source;
	return true;
}

void UpdateAvailableSources()
{
	obs_scene_t *scene=obs_scene_from_source(SourcePtr(obs_frontend_get_current_scene(),&obs_source_release).get()); // get current scene
	obs_scene_enum_items(scene,&AvailableSource,nullptr);
}

void HandleEvent(obs_frontend_event event,void *data)
{
	switch (event)
	{
	case OBS_FRONTEND_EVENT_SCENE_CHANGED:
		UpdateAvailableSources();
		UpdateAvailbleWindows();
		for (const std::pair<QString,obs_source_t*> &pair : sources) Log("Source: " + pair.first);
		break;
	}
}

void BuildUI()
{
	QMainWindow *window=static_cast<QMainWindow*>(obs_frontend_get_main_window());
	QDockWidget *dock=new QDockWidget("Don't Blink",window);
	sourcesWidget=new ScrollingList(dock);
	dock->setWidget(sourcesWidget);
	dock->setObjectName("dont_blink");
	window->addDockWidget(Qt::BottomDockWidgetArea,dock);
	obs_frontend_add_dock(dock);
}

bool obs_module_load()
{
	obs_frontend_add_event_callback(HandleEvent,nullptr);

	BuildUI();

	windowEvents=SetWinEventHook(EVENT_SYSTEM_FOREGROUND,EVENT_SYSTEM_FOREGROUND,nullptr,ForegroundWindowChanged,0,0,WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS);
	if (!windowEvents)
		Log("Failed to subscribe to system window events");
	else
		Log("Subscribed to system window events");

	shellEvents=SetWindowsHookEx(WH_SHELL,ShellEvent,0,0);
	if (!shellEvents)
		Log("Failed to subscribe to system shell events (" + std::to_string(GetLastError()) + ")");
	else
		Log("Subscribed to system shell events");

	return true;
}

void obs_module_unload()
{
	obs_frontend_remove_event_callback(HandleEvent,nullptr);

	if (UnhookWinEvent(windowEvents))
		Log("Unsubscribed from system window events");
	else
		Log("Failed to unsubscribe from system window events");

	if (UnhookWindowsHookEx(shellEvents))
		Log("Unsubscribed from system shell events");
	else
		Log("Failed to unsubscribe from system window events");
}