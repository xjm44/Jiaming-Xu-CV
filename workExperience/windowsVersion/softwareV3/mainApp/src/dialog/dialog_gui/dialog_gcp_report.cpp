#include "../../include/include_gui/dialog_gcp_report.h"
#include "../../include/include_gui/widget_newProcess.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <QMessageBox>

gcp_report::gcp_report(widget_newProcess* parent)
	: QDialog(parent)
{

	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	setupUi(this);

	this->parent = parent;
	init();
}
gcp_report::~gcp_report() {

}

void gcp_report::init() {

	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//tableWidget->horizontalHeader()->setMinimumSectionSize(120);
	
	connect(pushButtonSave, SIGNAL(clicked()), this, SLOT(_save_triggered()), Qt::UniqueConnection);

}

void gcp_report::_save_triggered() {

	std::cout << "save pressed" << std::endl;
	QMessageBox::StandardButtons buts = (QMessageBox::Yes | QMessageBox::No);
	QString title = QMessageBox::tr("Confirmation");
	QString text = QMessageBox::tr("Save output as a csv file?\nIt will be saved in the same folder as the input gps txt file.");
	QMessageBox msgB(nullptr);
	msgB.setIcon(QMessageBox::Icon::Question);
	msgB.setWindowTitle(title);
	msgB.setText(text);
	msgB.setStyleSheet("QMessageBox{font-size:14px;}\
							QPushButton {font-size:14px;}\
							");
	msgB.setStandardButtons(buts);
	int res = msgB.exec();


	QString csvFile;
	csvFile = gpsFile + "_report.csv";
	QFile fi(csvFile);
	std::fstream fout;
	switch (res) {
	case QMessageBox::Yes:
		// save csv 
		// check/clear csv file first		
		if (fi.exists()) {
			fi.remove();
			std::cout << "deleted" << std::endl;
		}
		std::cout << "csv file: " << csvFile.toStdString() << " opened." << std::endl;
		// create a new csv file
		
		fout.open(csvFile.toStdString(), std::ios::out | std::ios::trunc);
		// insert first row (xyztrealxyz)
		fout << "x" << ","
			<< "y" << ","
			<< "z" << ","
			<< "real_x" << ","
			<< "real_y" << ","
			<< "real_z" << ","
			<< "error" << ","
			<< "\n";
		// add data from tablewidget
		for (int i = 0; i < tableWidget->rowCount(); i++)
		{
			for (int j = 0; j < tableWidget->columnCount(); j++) {
				if (tableWidget->item(i, j) == NULL) {
					fout << " " << ",";
				}
				else {
					fout << tableWidget->item(i, j)->text().toStdString() << ",";
				}
			}
			fout << "\n";
		}

		std::cout << "save csv done" << std::endl;
		fout.close();	
		break;

	default:
		break;
	}

}

void gcp_report::addItem() {


	// for original input gps txt (ignore starting line(point))

	// - gpsFile
	storageGps = readGpsFile();
	int total_row = 0;
	for (double indi : storageGps.at(0)) {
		total_row++;
	}
	std::cout << "total row no. " << total_row << std::endl;
	if (storageGps.at(0).size() == 0) {
		std::cout << "invalid input gps txt file " << total_row << std::endl;
		tableWidget->insertRow(0);
		tableWidget->insertRow(1);
		tableWidget->setItem(0, 0, new QTableWidgetItem("invalid gps file input."));
		tableWidget->setItem(1, 0, new QTableWidgetItem("please follow the requirement."));
		tableWidget->update();
	}
	else if(total_row > finalXyz.size()) { // != this is because to prevent the output csv has more lines of outputs such as 0,0,0,1,1,1

	
		std::cout << "gps no. != control points after error elimination " << total_row << std::endl;
		tableWidget->insertRow(0);
		tableWidget->insertRow(1);
		tableWidget->setItem(0, 0, new QTableWidgetItem("The number of control points and number of gps inputs are not equal."));
		tableWidget->setItem(1, 0, new QTableWidgetItem("Hence, accuracy report is unable to output."));
		tableWidget->update();
	

	}
	else {
		addRow(total_row);
	}



	// for the output from gcpEE
	// TO BE DONE ==============
	// rmb add auto show after process done. - not only button
}

void gcp_report::addRow(int row) {

	std::vector<double> tempList;

	for (int i = 0; i < row; i++)
	{
		// real x y z
		tableWidget->insertRow(tableWidget->rowCount());
		double x = storageGps.at(0).at(i);
		double y = storageGps.at(1).at(i);
		double z = storageGps.at(2).at(i);
		tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(x, 'd', 6)));
		tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(y, 'd', 6)));
		tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(z, 'd', 6)));



		// for the output from gcpEE && xyz of cps have already transformed
		// =========================================================================
		std::vector<double> tempXyz = finalXyz.at(i);
		tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(tempXyz.at(0), 'd', 6)));
		tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(tempXyz.at(1), 'd', 6)));
		tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(tempXyz.at(2), 'd', 6)));
		// ====================================================================================


		// for last column (accuracy result)
		double acc = calculateAccuracy(
			tableWidget->item(i, 0)->text().toDouble(),tableWidget->item(i, 1)->text().toDouble(), tableWidget->item(i, 2)->text().toDouble(),
			tableWidget->item(i, 3)->text().toDouble(), tableWidget->item(i, 4)->text().toDouble(), tableWidget->item(i, 5)->text().toDouble());
		tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(acc, 'd', 6))); 
		tempList.emplace_back(acc);

	}



	// last row
	tableWidget->insertRow(tableWidget->rowCount());
	//tableWidget->setItem(row, 5, new QTableWidgetItem("Average Accuracy:")); // TO BE DELETED
	tableWidget->setItem(row, 5, new QTableWidgetItem("RMS:")); // TO BE DELETED
	double final = 0.0;
	for (double a : tempList) {
		final += a;
	}



	final = final / (tempList.size());
	tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(final, 'd', 6))); // TO BE DELETED

	tableWidget->update();
	this->setMinimumWidth(tableWidget->sizeHint().width()*1.15);
	this->update();
}

double gcp_report::calculateAccuracy(double x1, double y1, double z1, double x2, double y2, double z2) {

	long double dist = sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0) + pow(z1 - z2, 2.0));
	return dist;
}


std::vector<std::vector<double>> gcp_report::readGpsFile() {
	QString gcpEEtxt = gpsFile;
	gcpEEtxt = gcpEEtxt.replace('/', '\\');

	std::vector<double> rx;
	std::vector<double> ry;
	std::vector<double> rz;
	std::vector<std::vector<double>> fin;

	// read GPS data form txt file    
	std::ifstream fi(gpsFile.toStdString());
	if (fi.is_open()) {
		std::cout << "Open GPS(GCP) file " << gpsFile.toStdString() << " succeed" << std::endl;
	}
	else {
		std::cout << "Open GPS(GCP) file " << gpsFile.toStdString() << " failed" << std::endl;
		return fin;
	}



	int64_t length = 0;
	std::string strGPS;
	fi.seekg(0, std::ios::end);
	length = fi.tellg();
	fi.seekg(0, std::ios::beg);
	char* buffer = new char[length];
	fi.read(buffer, length);
	fi.close();
	strGPS = buffer;

	// loadGPSFile(strGPS);
	std::istringstream ifs(strGPS);

	std::string line;

	int lineCounter = 0;

	while (std::getline(ifs, line))
	{

		lineCounter++;
		if (lineCounter == 1) {
			continue;
		}

		double x, y, z;

		std::stringstream ss(line);
		std::string subline;
		std::getline(ss, subline, ',');


		// for checking file invalidity==
		try
		{
			std::stod(subline);
		}
		catch (...)
		{
			break;
		}
		// ===============================

		y = std::stod(subline);
		//std::stringstream(subline) >>  y;

		if (isnan(y))
		{
			std::cout << "GPS file over" << std::endl;
			break;
		}
		std::getline(ss, subline, ',');
		x = std::stod(subline);
		//std::stringstream(subline) >>  x;

		std::getline(ss, subline, ',');
		z = std::stod(subline);
		//std::stringstream(subline) >>  z;

		rx.push_back(x);
		ry.push_back(y);
		rz.push_back(z);

	}

	fin.emplace_back(rx);
	fin.emplace_back(ry);
	fin.emplace_back(rz);
	return fin;
}