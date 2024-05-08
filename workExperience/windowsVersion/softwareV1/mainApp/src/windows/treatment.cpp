#include <iostream>
#include "../include/treatment.h"

treatment::treatment(QWidget* parent)
	: QWidget(parent)
{

	ui.setupUi(this);
	this->setWindowTitle(QString("Cloud's treatment"));
	this->setWindowIcon(QIcon("../resources/images/new_logo.png"));

	this->setFixedWidth(360);
	this->setFixedHeight(290);

	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(on_set_clicked()));
	ui.tabWidget->setCurrentIndex(0);

	// set limit to the input=============================================
	ui.lineedit_DS->setValidator(new QDoubleValidator(0,100,10,this));
	ui.lineedit_VDS->setValidator(new QDoubleValidator(0, 100, 10, this));
	ui.lineedit_SORK->setValidator(new QIntValidator(0, 10000,this));
	ui.lineedit_SORSTD->setValidator(new QDoubleValidator(0, 100, 10, this));
	ui.lineedit_RN->setValidator(new QIntValidator(0, 10000, this));
	ui.lineedit_RS->setValidator(new QDoubleValidator(0, 100, 10, this));
	ui.lineedit_MLSR->setValidator(new QDoubleValidator(0, 100, 10, this));
	ui.lineedit_MLSP->setValidator(new QIntValidator(0, 1000, this));
	ui.lineedit_MLSG->setValidator(new QDoubleValidator(0, 100, 10, this));
	ui.lineedit_MLSUR->setValidator(new QDoubleValidator(0, 100, 10, this));
	ui.lineedit_MLSUUR->setValidator(new QDoubleValidator(0, 100, 10, this));
	ui.lineedit_MLSUS->setValidator(new QDoubleValidator(0, 100, 10, this));
	//========================================================================

	ui.tabWidget->setTabToolTip(0,"Downsampling");
	ui.tabWidget->setTabToolTip(1,"Voxel Downsampling");
	ui.tabWidget->setTabToolTip(2,"Statistical Outlier Removal");
	ui.tabWidget->setTabToolTip(3,"Radius Outlier Removal");
	ui.tabWidget->setTabToolTip(4,"Moving Least Squares");
	ui.tabWidget->setTabToolTip(5, "Moving Least Squares Upsampling");
	
}

void treatment::paintEvent(QPaintEvent*) {

	ui.tabWidget->setGeometry(0, 0, this->width(), this->height() * 0.8);
	ui.pushButton->setGeometry(this->width() * 0.5 - 60, this->height() * 0.85, 120, 25);
	ui.groupBox1->setGeometry(this->width() * 0.5 - 150, this->height() * 0.4 - 60, 300, 125);
	ui.groupBox2->setGeometry(this->width() * 0.5 - 150, this->height() * 0.4 - 60, 300, 125);
	ui.groupBox3->setGeometry(this->width() * 0.5 - 150, this->height() * 0.4 - 60, 300, 125);
	ui.groupBox4->setGeometry(this->width() * 0.5 - 150, this->height() * 0.4 - 60, 300, 125);
	ui.groupBox5->setGeometry(this->width() * 0.5 - 150, this->height() * 0.4 - 60, 300, 125);
	ui.groupBox6->setGeometry(this->width() * 0.5 - 150, this->height() * 0.4 - 60, 300, 125);

}

treatment::~treatment()
{	
	std::cout << "quit tt \n";
	delete &ui;
}

void treatment::on_set_clicked() {

	int current_page = ui.tabWidget->currentIndex();
	QRegExp checkdouble("\\d*\\.?\\d*");  // a digit (\d), zero or more times (*)
	QRegExp checkint("\\d*");  // a digit (\d), zero or more times (*) + . + a digit (\d), zero or more times (*)
	bool can_set = false;

	switch (current_page) {

	case 0:

		
		if (checkdouble.exactMatch(ui.lineedit_DS->text())){ 

			emit(start_treat_the_cloud(QString("Start")));
			cloud = osgAlgo->uniform_downsampling(cloud, ui.lineedit_DS->text().toFloat());

			can_set = true;
		}
		else {
			QMessageBox::warning(NULL,"Wrong Input","Please input decimal number only.");
		}

		
		std::cout << "downsampling \n";
		break;

	case 1:


		if (checkdouble.exactMatch(ui.lineedit_VDS->text())) {

			emit(start_treat_the_cloud(QString("Start")));
			cloud = osgAlgo->voxel_downsampling(cloud, ui.lineedit_VDS->text().toFloat());
			can_set = true;
		}
		else {
			QMessageBox::warning(NULL, "Wrong Input", "Please input decimal number only.");
		}

		std::cout << "voxel downsampling \n";
		break;

	case 2:


		if (checkint.exactMatch(ui.lineedit_SORK->text()) && checkdouble.exactMatch(ui.lineedit_SORSTD->text())) {

			emit(start_treat_the_cloud(QString("Start")));
			cloud = osgAlgo->statistical_outlierremoval(cloud, ui.lineedit_SORK->text().toInt(), ui.lineedit_SORSTD->text().toFloat());
			can_set = true;
		}
		else {
			QMessageBox::warning(NULL, "Wrong Input", "Please input integer (for the first input) and decimal number (for the second input) only.");
		}
		std::cout << "sor \n";
		break;

	case 3:

		if (checkint.exactMatch(ui.lineedit_RN->text()) && checkdouble.exactMatch(ui.lineedit_RS->text())) {

			emit(start_treat_the_cloud(QString("Start")));
			cloud = osgAlgo->r_outlierremoval(cloud, ui.lineedit_RN->text().toInt(), ui.lineedit_RS->text().toFloat());
			can_set = true;
		}
		else {
			QMessageBox::warning(NULL, "Wrong Input", "Please input integer (for the first input) and decimal number (for the second input) only.");
		}



		std::cout << "r outlier \n";
		break;

	case 4:

		if (checkdouble.exactMatch(ui.lineedit_MLSR->text()) && checkint.exactMatch(ui.lineedit_MLSP->text()) && checkdouble.exactMatch(ui.lineedit_MLSG->text())) {

			emit(start_treat_the_cloud(QString("Start")));
			cloud = osgAlgo->mls(cloud, ui.lineedit_MLSR->text().toFloat(), ui.lineedit_MLSP->text().toInt(), ui.lineedit_MLSG->text().toFloat());
			can_set = true;
		}
		else {
			QMessageBox::warning(NULL, "Wrong Input", "Please input decimal number (for the first input), integer (for the second input) and decimal number (for the third input) only.");
		}

		std::cout << "mls \n";
		break;

	case 5:

		if (checkdouble.exactMatch(ui.lineedit_MLSUS->text()) && checkdouble.exactMatch(ui.lineedit_MLSUR->text()) && checkdouble.exactMatch(ui.lineedit_MLSUUR->text())) {

			emit(start_treat_the_cloud(QString("Start")));
			cloud = osgAlgo->mls_upsampling(cloud, ui.lineedit_MLSUR->text().toFloat(), ui.lineedit_MLSUUR->text().toFloat(), ui.lineedit_MLSUS->text().toFloat());
			can_set = true;
		}
		else {
			QMessageBox::warning(NULL, "Wrong Input", "Please input decimal number only.");
		}

		std::cout << "mls upsampling \n";
		break;

	default:

		break;

	}


	// start to treat the point cloud
	if (can_set) {

		std::cout << "set! \n";
		can_set = false;
		emit treat_the_cloud(QString("Done"));
	}
	else {
		std::cout << "not set! \n";
	}

}

void treatment::setCloud(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud) {
	this->cloud = cloud;
}
pcl::PointCloud<pcl::PointXYZRGB>::Ptr treatment::getCloud() {
	return cloud;
}
