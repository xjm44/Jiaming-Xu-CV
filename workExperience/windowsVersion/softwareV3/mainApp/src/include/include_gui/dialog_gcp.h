#pragma once

#include <QDialog>
#include "ui_dialog_gcp.h"
#include <QCloseEvent>

class widget_newProcess;

class dialog_gcp : public QDialog, Ui::dialog_gcpClass
{
	Q_OBJECT

public:
	dialog_gcp(widget_newProcess* parent);
	~dialog_gcp();

	void init();

	void addItem();
	void addRow(int);

	//  path of gcp txt
	QString gcpTxt;
	std::vector<std::vector<double>> readTxt(QString txtPath);
	QString savePath;
	QString fileName;

	void addxyz(double,double,double);

protected:
	void closeEvent(QCloseEvent* event);

private Q_SLOTS:

	// login
	void _cancel_triggered();            
	void _continue_triggered();    

Q_SIGNALS:
	void _continue();                 //to process
	void _closed();


private:
	widget_newProcess* parent;

	// store real xyz
	std::vector<std::vector<double>> storage;

};
