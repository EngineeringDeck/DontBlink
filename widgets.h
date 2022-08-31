#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <unordered_map>
#include <unordered_set>

class CrossReference : public QWidget
{
	Q_OBJECT
public:
	CrossReference(QString windowTitle,QStringList sources,QWidget *parent);
	QString Source();
protected:
	QLabel *name;
	QComboBox *list;
};

class ScrollingList : public QScrollArea
{
	Q_OBJECT
public:
	ScrollingList(QWidget *parent);
	void AddEntries(const std::unordered_set<QString> &windowTitles,const QStringList &sources);
	QString Source(const QString &windowTitle);
protected:
	QWidget *content;
	QVBoxLayout *layout;
	std::unordered_map<QString,CrossReference*> crossReferences;
};
