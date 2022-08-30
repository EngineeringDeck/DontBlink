#include <QtGlobal>
#include "platform.h"

#ifdef Q_OS_WIN32
#include "platform-win32.h"
#endif

Platform* Platform::Create()
{
	if (self) return self;

#ifdef Q_OS_WIN32
	self=new PlatformWin32();
#endif

	return self;
}

Platform::~Platform()
{
	self=nullptr;
}

Platform *Platform::self=nullptr;
