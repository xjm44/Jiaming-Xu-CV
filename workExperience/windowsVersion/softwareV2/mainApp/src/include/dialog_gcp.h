#pragma once

#include <QWidget>
#include "ui_dialog_gcp.h"
#include "widget_gcp_list.h" 

class mainwindow;
class widget_gcp_list;

class dialog_gcp : public QWidget
{
	Q_OBJECT

public:
	dialog_gcp(mainwindow *parent);
	~dialog_gcp();

	void init(mainwindow* w, std::vector<float> xyzCoor, std::vector<int> nameIndex); // link with mainwindow + set property
	void addData(std::vector<float> xyzCoor, std::vector<int> nameIndex);		      // add data in the list, from vector
	QStandardItemModel* getModel() { return model; } // get all items
	gcpTableView* getTable() { return t; } // get table


private Q_SLOTS:

	void _save();
	void _add();
	void _remove();
	void _confirm();
	void _geographicCoordinateConversion();
	void _automaticMerging();
	void _eliminateOffsetErrors();

Q_SIGNALS:
	void _saved();				    // when loading pop-up is needed
	void _chooseGcp();				// add gcp manually
	void _delGcp(int);				// del gcp manually
	void _confirmGcp();				// confirm gcp
	void _gcc();					// Geographic Coordinate Conversion
	void _autoMerge();				// automatic merging
	void _eliError();				// eliminate error

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	mainwindow* window_;
	gcpTableView* t;
	//QString filePath;
	//std::vector<QString> readTxt();
	QStandardItemModel* model; // all items in the table


private:
	Ui::dialog_gcpClass ui;
};
