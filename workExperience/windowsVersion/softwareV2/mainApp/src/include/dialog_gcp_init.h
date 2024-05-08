#pragma once

#include <QWidget>
#include "ui_dialog_gcp_init.h"

class mainwindow;

class dialog_gcp_init : public QWidget , public Ui::dialog_gcp_initClass
{
	Q_OBJECT

public:
	dialog_gcp_init(mainwindow *parent);
	~dialog_gcp_init();

	QString savingPath;
	QString pointCloud;
	QString routeCloud;

private Q_SLOTS:
	void _pointCloud();
	void _savingPath();
	void _NoofGcp();
	void _routeCloud();
	void _done();

Q_SIGNALS:
	void _proceed(); // after input all parameters


private:
	bool checkPc;
	bool checkSp;
	bool checkNoG;
	mainwindow* window_;
};
