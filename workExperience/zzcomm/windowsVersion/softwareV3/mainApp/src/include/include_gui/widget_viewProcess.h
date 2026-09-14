#pragma once

#include <QDialog>
#include "ui_widget_viewProcess.h"
#include "../include_process/processBag.h"
class mainApp;
class widget_newProcess;

// view bag name + param + log
class widget_viewProcess : public QDialog, public Ui::widget_viewProcessClass
{
	Q_OBJECT

public:
	widget_viewProcess(widget_newProcess* parent);
	~widget_viewProcess();

	void init();
	// display selected param
	void _showParam(process_type);

	void onAccuracyReport();
private:
	widget_newProcess* parent;
	bool paramClicked;

	void setSlamParam(QStringList);

private Q_SLOTS:

	void _viewParam();
	
	// gcp
	void _coor();
	void _pc();
	void _pcPath();
	void _continue();

	void _gcpReportBtn();
	

Q_SIGNALS:
	void _gcpEE();
	void _gcpReport();
};
