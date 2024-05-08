#pragma once

#include <QWidget>
#include <QMouseEvent>
#include "ui_terminal.h"

class terminal : public QWidget, public Ui::terminalClass
{
	Q_OBJECT

public:
	terminal(QWidget *parent = nullptr);
	~terminal();

	void add(QString);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void closeEvent(QCloseEvent* event);

private:
	bool move1;
	QPoint lastPos;

	int maxSize;

Q_SIGNALS:
	void _closed();

private Q_SLOTS:
	void btnClear_triggered();
	void btnClose_triggered();
};
