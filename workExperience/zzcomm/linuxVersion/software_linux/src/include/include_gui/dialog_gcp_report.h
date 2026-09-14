#ifndef GCPREPORT_H
#define GCPREPORT_H

#include <QDialog>
#include <QWidget>
#include "ui_dialog_gcp_report.h"



class widget_newProcess;

class gcp_report : public QDialog, Ui::dialog_gcp_reportClass
{
	Q_OBJECT

public:
	gcp_report(widget_newProcess* parent);
	~gcp_report();

	void setOriGpsFile(QString a) { gpsFile = a; }

	// ori gps file + gcpEE output
	void addItem();

	void setFinalXyz(std::vector<std::vector<double>> a) { finalXyz = a; }
private:
	void init();
	

private Q_SLOTS:

	// login
	void _save_triggered();

private:
	widget_newProcess* parent;

	QString gpsFile;
	std::vector<std::vector<double>> storageGps;
	std::vector<std::vector<double>> readGpsFile();
	void addRow(int row);

	double calculateAccuracy(double, double, double, double, double, double);

	std::vector<std::vector<double>> finalXyz;
};

#endif // GCPREPORT_H