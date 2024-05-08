#pragma once

#include <QWidget>
#include "ui_rtk.h"


class gui;

class rtk : public QWidget
{
	Q_OBJECT

public:
	rtk(gui *parent);
	~rtk();/* override = default;*/

	QString bagPath;
	QString txtPath;

protected:

	void paintEvent(QPaintEvent*); // positioning

protected:
	void dragEnterEvent(QDragEnterEvent* e);
	void dropEvent(QDropEvent* e);
	void closeEvent(QCloseEvent* event) override;

private Q_SLOTS:
	void _done_clicked();

Q_SIGNALS:

	void rtk_ready(QString bag, QString txt); // press done
	void rtk_close(); // close btn -> RTK button change 

private:
	Ui::rtkClass ui;

	bool countBag;
	bool countTxt;
	gui* gui_;
};
