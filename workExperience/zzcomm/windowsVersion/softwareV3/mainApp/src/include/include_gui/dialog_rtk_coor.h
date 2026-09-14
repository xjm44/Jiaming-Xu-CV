#pragma once

#include <QDialog>
#include "ui_dialog_rtk_coor.h"

class dialog_rtk_coor : public QDialog, Ui::dialog_rtk_coorClass
{
	Q_OBJECT

public:
	dialog_rtk_coor(QWidget *parent = nullptr);
	~dialog_rtk_coor();


	void init();
	void firstPageInfo();
	void secondPageInfo();

protected:
	void closeEvent(QCloseEvent* event) override;

private Q_SLOTS:
	void next_triggered();
	void back_triggered();
	void continue_triggered();

Q_SIGNALS:
	void coor_(QString);
	void closed_();
};
