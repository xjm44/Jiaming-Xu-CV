#pragma once

#include <QWidget>
#include <QProcess>

class widget_newProcess;

// input bag + param
class colourise : public QWidget
{
	Q_OBJECT

public:
	colourise(widget_newProcess* parent);
	~colourise();
	void init();
	// to colourise the point cloud
	void run();

	void closeBtnPressed();

protected:
	void closeEvent(QCloseEvent* event) override;

	void colorFinished(); // done colourisation

private Q_SLOTS:
	void _starts(); // colour the cloud
	void _ColorExitCode(int, QProcess::ExitStatus);

Q_SIGNALS:




private:
	widget_newProcess* parent_;

	QProcess* colourisePc;
	bool running;
};
