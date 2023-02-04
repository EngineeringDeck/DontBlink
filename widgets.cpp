#include <QGridLayout>
#include "widgets.h"

void ComboBox::showPopup()
{
	emit Popup(true);
	QComboBox::showPopup();
}

void ComboBox::hidePopup()
{
	emit Popup(false);
	QComboBox::hidePopup();
}

CrossReference::CrossReference(QString windowTitle,QStringList sources,QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
	setStyleSheet("padding: 0px");
	QHBoxLayout *layout=new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);
	name=new QLabel(windowTitle,this);
	name->setAlignment(Qt::AlignRight);
	layout->addWidget(name);
	list=new ComboBox(this);
	list->setPlaceholderText("-= [NONE] =-");
	PopulateComboBox(sources);
	list->setCurrentIndex(-1);
	layout->addWidget(list);

	connect(list,&ComboBox::Popup,this,&CrossReference::ComboBoxPopup);
}

QString CrossReference::Source()
{
	if (list->currentIndex() < 0) return {};
	return list->currentText();
}

void CrossReference::PopulateComboBox(const QStringList &sources)
{
	// save the currently selected name and index
	QString name=list->currentText();

	// repopulate the new list of source names
	list->clear();
	for (const QString &source : sources) list->addItem(source);

	// if the old selection is in the new list of source names, restore the selected index, otherwise set it to "none"
	if (int index=list->findText(name); index > -1)
	{
		list->setCurrentIndex(index);
	}
	else
	{
		list->setCurrentIndex(-1);
		list->setCurrentText("");
	}
}

ScrollingList::ScrollingList(QWidget *parent) : QScrollArea(parent)
{
	setWidgetResizable(true);
	content=new QWidget(this);
	layout=new QVBoxLayout(content);
	layout->setContentsMargins(0,0,10,0);
	content->setLayout(layout);
	content->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
	setWidget(content);
}

void ScrollingList::AddEntries(const std::unordered_set<QString> &windowTitles,const QStringList &sources)
{
	std::unordered_map<QString,CrossReference*> mergedCrossReferences;
	for (const QString &windowTitle : windowTitles)
	{
		if (auto candidate=crossReferences.find(windowTitle); candidate != crossReferences.end())
		{
			std::unordered_map<QString,CrossReference*>::node_type retainedCrossReference=crossReferences.extract(candidate);
			retainedCrossReference.mapped()->PopulateComboBox(sources);
			mergedCrossReferences.insert(std::move(retainedCrossReference));
		}
		else
		{
			CrossReference *crossReference=new CrossReference(windowTitle,sources,this);
			connect(crossReference,&CrossReference::ComboBoxPopup,this,&ScrollingList::ComboBoxPopup);
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
