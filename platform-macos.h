#pragma once

#include "platform.h"

class PlatformMacOS : public Platform
{
	Q_OBJECT
public:
	PlatformMacOS();
	~PlatformMacOS() override;
public slots:
	void UpdateAvailableWindows() override;
};
