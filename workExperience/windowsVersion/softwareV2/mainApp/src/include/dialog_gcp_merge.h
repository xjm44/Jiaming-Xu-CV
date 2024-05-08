#pragma once

#include <QWidget>
#include "ui_dialog_gcp_merge.h"
#include <QStandardItemModel>
#include <QTableView>
#include "Yuan.h"

class mainwindow;

class dialog_gcp_merge : public QWidget , public Ui::dialog_gcp_mergeClass
{
	Q_OBJECT

public:
	dialog_gcp_merge(mainwindow *parent);
	~dialog_gcp_merge();
	void init(mainwindow* parent);

private Q_SLOTS:

	void _txt1();				   // first cloud: txt
	void _txt2();				   // 2nd cloud: txt
	void _autoMerge();             // merge: using 2 txt files

Q_SIGNALS:
	void _toMerge(std::vector<Yuan>);	 // merge btn pressed: final vectorYuan + cloudpath(first cloud)


private:
	mainwindow* window_;

	bool checkFirst;
	bool checkSecond;
	QStandardItemModel* firstTable;
	QStandardItemModel* secondTable;
	std::vector<Yuan> first_namexyz;
	std::vector<Yuan> second_namexyz;

	
	void readTxt_and_show(QString filename, int a); // filename: txt. a: table1/table2
};
