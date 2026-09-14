#pragma once
#include "widget_newProcess.h"

#include <QDialog>
#include "ui_widget_startProcess.h"
#include "dialog_rtk_coor.h"
class mainApp;


// input bag + param
class widget_startProcess : public QDialog, public Ui::widget_startProcessClass
{
	Q_OBJECT

public:
	widget_startProcess(mainApp*parent);
	~widget_startProcess();
	void init();

	QString slamBag;
	QString rtkFile;
	QString gcpFile;
	QStringList colorFileMp4;
	QString colorFilePc;
	QString colorFileMp4Path;

	void setParam(int i, widget_newProcess* np); // i = type // set param + type

	QString productParam;


protected:
	void closeEvent(QCloseEvent* event) override;

private Q_SLOTS: 
	void _setting(); // param
	void _start(); // start btn
	void _slam(); 
	void _colorMp4();
	void _colorPc();
	void _rtk(); 
	void _gcp(); 
	void _savingPath();

	void comboboxType(int);
	void comboboxEnvType(int);

	// for slam
	void _doubleSpinBox_lidarMaxXToFilter(); // set lidarMinXToFilter max range
	void _checkBox_loopClosure(int);		 // set loopClosureIntervalFrames
	void _slamproductParams(); // product.yaml
	void _checkBox_ba(int); // hba

	// for color
	void _colorProjPath();
	void _colorImu();
	void _colorTraj();
	void _colorCali();
	void _colorConfig();
	void _colorMask();

	// for gcp
	void _checkBox_switch(int);
	void _checkBox_zcoor(int);

	//for cuda
	void cudaCompute();
	void memCompute();
	
	// for rtk
	void _rtk_coor();
	void _rtk_coor_output(QString);
	void _rtk_coor_close();
	QString _rtk_coor_sys(QString);

	// for auto fill-up colorisation params
	void autoFillColor();

Q_SIGNALS:
	void _started();  // start the bag's process
	void _close();
	void _closeNonEmpty(); // close when lineedits non-empty

private:
	mainApp* window_;

	bool canRun;

	QProcess* cudaQ;
	void getCudaCompute();
	QProcess* cudaQM;
	void getMemory();

	void resetAllParams(); // for environments..

	QStringList compareMP4Names(QStringList files);

	void setSlamParam(widget_newProcess*);
	

	dialog_rtk_coor* r;
};
