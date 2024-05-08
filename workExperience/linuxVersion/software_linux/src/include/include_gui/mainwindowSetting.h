#pragma once

#include <QWidget>
#include "ui_mainwindowSetting.h"
#include <QProcess>
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
	void slotRescaleIntensity_(int, QProcess::ExitStatus);

	void btnBagRe_triggered();
	void slotBagReindex_();
	void slotBagReindex2_();
	void slotReindexExitCode(int, QProcess::ExitStatus);

Q_SIGNALS:
	void _closed();                 
};
