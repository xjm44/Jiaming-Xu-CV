#pragma once

#include <QDialog>
#include "ui_startFolder.h"
#include <QDir>
class mainApp;

class startFolder : public QDialog, Ui::startFolderClass
{
	Q_OBJECT

public:
	startFolder(mainApp*parent);
	~startFolder();

	void setDir(QDir d) { dir = d; }
	bool checkDir(QString);

private:
	void init();
	QDir dir;

public:
	QString getProjPath() { return projPath; }
	QString getSlamBag() { return slamBag; }
	QString getSlamYaml() { return slamYaml; }
	QString getRtkTxt() { return rtkTxt; }
	QString getGcpTxt() { return gcpTxt; }
	QStringList getMp4File() { return mp4Files; }
	QStringList getPanoFile() { return panoFile; }
	QString getCaliGp() { return colorCaliGp; }
	QString getCaliPa() { return colorCaliPa; }
	QString getMask() { return colorMask; }
	QString getPc() { return pointCloud; }
	QString getCsv() { return colorCsv; }

private:
	QString projPath;
	// slam
	QString slamBag;
	QString slamYaml;
	// rtk
	QString rtkTxt;
	// gcp
	QString gcpTxt;
	// loop a path and find a file
	QString findFileInFolder(QString filter, QString path, QString reg);
	// loop a path and find n file(s)
	QStringList findFilesInFolder(QString filter, QString path, bool mp4);
	// find a find using regex
	QString findFile(QString reg, QFileInfo file);
	//color
	QString colorCsv;
	QString colorCaliGp;
	QString colorCaliPa;
	QStringList mp4Files;
	QStringList panoFile;
	QString pointCloud;
	QString colorMask;

protected:
	void closeEvent(QCloseEvent* event);

Q_SIGNALS:
	void _project(QString);        
	void _exit();


private Q_SLOTS:


	void _slam_triggered();
	void _color_triggered();
	void _rtk_triggered();
	void _gcp_triggered();



private:
	mainApp* parent;


};
