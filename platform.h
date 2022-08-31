#pragma once

#include <QObject>
#include <unordered_set>

class Platform : public QObject
{
	Q_OBJECT
public:
	static Platform* Create();
	virtual ~Platform();
protected:
	static Platform *self;
signals:
	void ForegroundWindowChanged(const QString &title);
	void AvailableWindowsUpdated(const std::unordered_set<QString> &titles);
	void Log(const QString &message);
public slots:
	virtual void UpdateAvailableWindows()=0;
};
