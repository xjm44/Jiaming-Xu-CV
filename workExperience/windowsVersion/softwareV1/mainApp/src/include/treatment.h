
#ifndef TREATMENT_H
#define TREATMENT_H

#include <QWidget>
#include "ui_treatment.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include "osgAlgorithm.h"

class treatment : public QWidget
{
	Q_OBJECT

public:
	treatment(QWidget* parent = nullptr);
	~treatment();
	void paintEvent(QPaintEvent*); // positioning

	void setCloud(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud);
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr getCloud();


private:
	Ui::treatmentForm ui;
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud;
	osgAlgorithm* osgAlgo;

Q_SIGNALS:
	void treat_the_cloud(QString); 
	void start_treat_the_cloud(QString); 


private Q_SLOTS:
	void on_set_clicked();

};


#endif // TREATMENT_H