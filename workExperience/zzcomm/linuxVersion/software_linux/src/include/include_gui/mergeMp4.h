#pragma once

#include <QWidget>
#include <QProcess>

class widget_newProcess;

// input bag + param
class mergeMp4 : public QWidget
{
	Q_OBJECT

public:
	mergeMp4(widget_newProcess* parent);
	~mergeMp4();
	void init();
	// merge all mp4
	void startMerging(QStringList);
	void closeBtnPressed();

protected:
	void closeEvent(QCloseEvent* event) override;

private Q_SLOTS:
	void _mp4result();

Q_SIGNALS:
	void _startColourise();
	void _startColouriseMultiMp4();



private:
	widget_newProcess* parent_;

	QProcess* processMp4;
	bool running;
};
