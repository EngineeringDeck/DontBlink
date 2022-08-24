#include <QGridLayout>
#include "widgets.h"

CrossReference::CrossReference(QString windowTitle,QStringList sources,QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
	QHBoxLayout *layout=new QHBoxLayout(this);
	setLayout(layout);
	name=new QLabel(windowTitle,this);
	layout->addWidget(name);
	list=new QComboBox(this);
	list->setPlaceholderText("-= [NONE] =-");
	for (const QString source : sources) list->addItem(source);
	list->setCurrentIndex(-1);
	layout->addWidget(list);
}

QString CrossReference::Source()
{
	if (list->currentIndex() < 0) return {};
	return list->currentText();
}

ScrollingList::ScrollingList(QWidget *parent) : QScrollArea(parent)
{
	setWidgetResizable(true);
	content=new QWidget(this);
	layout=new QVBoxLayout(content);
	content->setLayout(layout);
	content->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
	setWidget(content);
}

void ScrollingList::AddEntries(const std::unordered_set<QString> &windowTitles,const QStringList &sources)
{
	std::map<QString,CrossReference*> mergedCrossReferences;
	for (const QString &windowTitle : windowTitles)
	{
		if (crossReferences.contains(windowTitle))
		{
			mergedCrossReferences.insert(std::move(crossReferences.extract(windowTitle)));
		}
		else
		{
			CrossReference *crossReference=new CrossReference(windowTitle,sources,this);
			mergedCrossReferences[windowTitle]=crossReference;
			layout->addWidget(crossReference);
		}
	}

	for (std::pair<QString,CrossReference*> pair : crossReferences) pair.second->deleteLater();
	crossReferences.clear();

	crossReferences.swap(mergedCrossReferences);
}

QString ScrollingList::Source(const QString &windowTitle)
{
	if (!crossReferences.contains(windowTitle)) return {};
	return crossReferences.at(windowTitle)->Source();
}
