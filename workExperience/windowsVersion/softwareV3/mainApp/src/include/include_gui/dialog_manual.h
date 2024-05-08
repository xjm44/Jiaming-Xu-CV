#ifndef DIALOGMANUAL_H
#define DIALOGMANUAL_H

#include <QDialog>
#include <QWidget>
#include "ui_dialog_manual.h"



class mainApp;

class dialog_manual : public QDialog, Ui::dialog_manual
{
	Q_OBJECT

public:
	dialog_manual(mainApp* parent);
	~dialog_manual();

	void init();
private:
	mainApp* parent;
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	void setLabel(QLabel &l, QString link);

private slots:
	void _btnFeedback_clicked();
};

#endif // DIALOGMANUAL_H