#pragma once

#include <QWidget>
#include "ui_mainwindowSetting.h"
#include <Qprocess>

class mainApp;
class mainwindowSettingDialog : public QDialog, public Ui::mainwindowSettingDialog
{
	Q_OBJECT

public:
	mainwindowSettingDialog(mainApp* parent);
	~mainwindowSettingDialog();

private:
	void init();
private:
	mainApp* parent_;
	int languageIndex;

	void rescaleIntensity(QString);
	QProcess* rescaleIn;
	int rescaleIndex;
	bool rescaleBool;

	void bagReindex(QString);
	QProcess* bagReindex_;
	int bagReIndex;
	void bagReindex2();
	QProcess* bagReindex2_;
	QString bagReindexFile;

protected:
	void closeEvent(QCloseEvent* event);

private Q_SLOTS:
	void btnBg_triggered();


	void comboboxLang(int);
	void btnLangOk_triggered();

	void btnIntensity_triggered();
	//void rescaleIntensity_();
	void slotRescaleIntensity_(int, QProcess::ExitStatus);

	void btnBagRe_triggered();
	void slotBagReindex_();
	void slotBagReindex2_();
	void slotReindexExitCode(int, QProcess::ExitStatus);


	void btnCmd_triggered();

	void btnUpdate_triggered();

Q_SIGNALS:
	void _closed();             
	void _cmd();
};