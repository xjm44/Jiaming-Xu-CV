#include "../include/picture.h"

picture::picture(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(QString("Pictures"));
	this->setWindowIcon(QIcon("../resources/images/new_logo.png"));


	connect(ui.btnplus1,SIGNAL(clicked()),this,SLOT(on_btnplus1_clicked()));
	connect(ui.btnminus1, SIGNAL(clicked()), this, SLOT(on_btnminus1_clicked()));

	connect(ui.btnplus2, SIGNAL(clicked()), this, SLOT(on_btnplus2_clicked()));
	connect(ui.btnminus2, SIGNAL(clicked()), this, SLOT(on_btnminus2_clicked()));

	connect(ui.btnplus3, SIGNAL(clicked()), this, SLOT(on_btnplus3_clicked()));
	connect(ui.btnminus3, SIGNAL(clicked()), this, SLOT(on_btnminus3_clicked()));
	
}

picture::~picture()
{
	pixH1 = NULL;
	scrollAreaH1 = NULL;
	pixRatio1 = NULL;

	delete &ui;
}

void picture::setFilepath(QString a, QString b, QString c)
{
	file1 = a;
	file2 = b;
	file3 = c;


	photo1.load(a);
	
	pixH1 = photo1.height();
	scrollAreaH1 = (this->height()/2)*0.6;
	pixRatio1 = float(scrollAreaH1) / pixH1;
	int w = pixRatio1 * photo1.width();
	int h = pixRatio1 * photo1.height();
	QPixmap temp = photo1.scaled(w, h);
	ui.label1->setPixmap(temp);


	photo2.load(b);

	pixH2 = photo2.height();
	scrollAreaH2 = (this->height() / 2) * 0.6;
	pixRatio2 = float(scrollAreaH2) / pixH2;
	w = pixRatio2 * photo2.width();
	h = pixRatio2 * photo2.height();
	QPixmap temp1 = photo2.scaled(w, h);
	ui.label2->setPixmap(temp1);



	photo3.load(c);

	pixH3 = photo3.height();
	scrollAreaH3 = (this->height() / 2) * 0.6;
	pixRatio3 = float(scrollAreaH3) / pixH3;
	w = pixRatio3 * photo3.width();
	h = pixRatio3 * photo3.height();
	QPixmap temp2 = photo3.scaled(w, h);
	ui.label3->setPixmap(temp2);
}

void picture::on_btnminus1_clicked()
{
	pixRatio1 = pixRatio1 * 0.8;
	int w = pixRatio1 * photo1.width();
	int h = pixRatio1 * photo1.height();
	QPixmap temp = photo1.scaled(w, h);
	ui.label1->setPixmap(temp);
}
void picture::on_btnplus1_clicked()
{

	pixRatio1 = pixRatio1 * 1.2;
	int w = pixRatio1 * photo1.width();
	int h = pixRatio1 * photo1.height();
	QPixmap temp = photo1.scaled(w, h);
	ui.label1->setPixmap(temp);

}
void picture::on_btnminus2_clicked()
{
	pixRatio2 = pixRatio2 * 0.8;
	int w = pixRatio2 * photo2.width();
	int h = pixRatio2 * photo2.height();
	QPixmap temp = photo2.scaled(w, h);
	ui.label2->setPixmap(temp);

}
void picture::on_btnplus2_clicked()
{
	pixRatio2 = pixRatio2 * 1.2;
	int w = pixRatio2 * photo2.width();
	int h = pixRatio2 * photo2.height();
	QPixmap temp = photo2.scaled(w, h);
	ui.label2->setPixmap(temp);
}
void picture::on_btnminus3_clicked()
{
	pixRatio3 = pixRatio3 * 0.8;
	int w = pixRatio3 * photo3.width();
	int h = pixRatio3 * photo3.height();
	QPixmap temp = photo3.scaled(w, h);
	ui.label3->setPixmap(temp);
}
void picture::on_btnplus3_clicked()
{
	pixRatio3 = pixRatio3 * 1.2;
	int w = pixRatio3 * photo3.width();
	int h = pixRatio3 * photo3.height();
	QPixmap temp = photo3.scaled(w, h);
	ui.label3->setPixmap(temp);
}

