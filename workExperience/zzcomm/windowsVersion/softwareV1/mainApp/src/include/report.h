#ifndef REPORT_H
#define REPORT_H

#include "ui_report.h"
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <PDFWriter.h>
#include <PDFPage.h>
#include <AbstractContentContext.h>
#include<PageContentContext.h>
#include "PDFFormXObject.h"
#include <iostream>

class report : public QWidget
{
	Q_OBJECT

public:

	report(QWidget* parent = nullptr);
	~report();

	bool generateReport();

	void setName(QString name);
	void setPoints(QString no);
	void setLocation(QString loc);
	void refresh(); // get contents

private:
	Ui::reportForm ui;
	void paintEvent(QPaintEvent*);
	QString name;
	QString points;
	QString location;
	QString picture;
	QStringList pictureList;

private Q_SLOTS:
	void on_btnGet_clicked();
	void on_btnLoc_clicked();
	void on_btnPic_clicked();

};



#endif // REPORT_H