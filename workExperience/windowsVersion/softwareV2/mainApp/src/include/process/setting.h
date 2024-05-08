#pragma once

#include <QDialog>
#include "ui_setting.h"

class gui;

class settingParam : public QDialog , public Ui::settingClass
{
	Q_OBJECT

public:
	settingParam(gui* parent);
	~settingParam();
protected:
	void closeEvent(QCloseEvent* event) override;

private Q_SLOTS:
	void _doubleSpinBox_lidarMaxXToFilter(); // set lidarMinXToFilter max range
	void _checkBox_loopClosure(int);		 // set loopClosureIntervalFrames
	void _btnClicked();						 // btn set

private:
	gui* window_;
	void init(); // all connections, pos, etc
	void setParams(); // for gui's rosparam

Q_SIGNALS:
	void param_close(); // close btn -> setting button change 
};
