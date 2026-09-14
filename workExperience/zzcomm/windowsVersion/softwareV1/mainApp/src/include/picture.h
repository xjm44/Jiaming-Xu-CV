
#ifndef PICTURE_H
#define PICTURE_H

#include <QWidget>
#include "ui_picture.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>

class picture : public QWidget
{
	Q_OBJECT

public:
	picture(QWidget* parent = nullptr);
	~picture();

	void setFilepath(QString a, QString b, QString c);

private:
	Ui::pictureClass ui;

	QPixmap photo1;
	QPixmap photo2;
	QPixmap photo3;

	QString file1, file2, file3;
	float pixRatio1;
	float pixRatio2;
	float pixRatio3;

	int scrollAreaH1;
	int scrollAreaH2;
	int scrollAreaH3;


	int pixH1;
	int pixH2;
	int pixH3;


private Q_SLOTS:
	void on_btnplus1_clicked();
	void on_btnminus1_clicked();
	void on_btnplus2_clicked();
	void on_btnminus2_clicked();
	void on_btnplus3_clicked();
	void on_btnminus3_clicked();

};


#endif // PICTURE_H