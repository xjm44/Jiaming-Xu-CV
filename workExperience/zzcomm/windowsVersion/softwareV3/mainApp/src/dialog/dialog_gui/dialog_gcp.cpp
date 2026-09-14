#include "../../include/include_gui/dialog_gcp.h"
#include "../../include/include_gui/widget_newProcess.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <QStandardItemModel>
#include <QMessageBox>

dialog_gcp::dialog_gcp(widget_newProcess* parent)
	: QDialog(parent)
{


	//this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	setupUi(this);

	this->parent = parent;
	
}

dialog_gcp::~dialog_gcp()
{}
void dialog_gcp::closeEvent(QCloseEvent* event) {

	if (parent->labelRunning->text() == "waiting for selection..") {
		QMessageBox::StandardButtons buts = (QMessageBox::Yes | QMessageBox::No);
		QString title = QMessageBox::tr("Confirmation");
		QString text = QMessageBox::tr("Confirm to cancel the process?\n");
		QMessageBox msgB(nullptr);
		msgB.setIcon(QMessageBox::Icon::Question);
		msgB.setWindowTitle(title);
		msgB.setText(text);
		msgB.setStyleSheet("QMessageBox{font-size:14px;}\
							QPushButton {font-size:14px;}\
							");
		msgB.setStandardButtons(buts);
		int res = msgB.exec();



		switch (res) {
		case QMessageBox::Yes:
			storage.clear();
			gcpTxt = QString("");

			emit(_closed());
			break;
		case QMessageBox::No:
			event->ignore();
			break;
		default:
			event->ignore();
			break;
		}
	}
	else {
		emit(_closed());
	}





}

void dialog_gcp::init() {


	connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(_cancel_triggered()), Qt::UniqueConnection);
	connect(pushButtonContinue, SIGNAL(clicked()), this, SLOT(_continue_triggered()), Qt::UniqueConnection);
	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	tableWidget->horizontalHeader()->setMinimumSectionSize(120);


	std::cout << "gps txt: " << gcpTxt.toStdString() << std::endl;
	std::cout << "saving path: " << savePath.toStdString() << std::endl;
	std::cout << "file name: " << fileName.toStdString() << std::endl;

	// add real x y z : gcpTxt
	storage = readTxt(gcpTxt);


	int total_row = 0;
	for (double indi : storage.at(0)){			
		total_row++;
	}
	std::cout << "total row no. " << total_row << std::endl;


	if (storage.at(0).size() == 0) {
		std::cout << "invalid input gps txt file " << total_row << std::endl;
		tableWidget->insertRow(0);
		tableWidget->insertRow(1);
		tableWidget->setItem(0, 0, new QTableWidgetItem("invalid gps file input."));
		tableWidget->setItem(1, 0, new QTableWidgetItem("please follow the requirement."));
		tableWidget->update();

	}
	else {
		addRow(total_row);
	}


}

void dialog_gcp::addItem() {

	QString gcpEEtxt = savePath + "\\" + fileName + "_GCPEE.txt";
	gcpEEtxt = gcpEEtxt.replace('/', '\\');

	std::ofstream dataFile;
	dataFile.open(gcpEEtxt.toStdString(), std::ofstream::app);
	std::fstream file(gcpEEtxt.toStdString(), std::ios::out);


	// save into txt
	std::cout << " row "<< tableWidget->rowCount() << std::endl;
	std::cout << " col "<< tableWidget->columnCount() << std::endl;
	// to check if input xyz has empty
	bool check = false;


	for (int i = 0; i < tableWidget->rowCount(); i++) {
		for (int j = 0; j < tableWidget->columnCount(); j++) {
			auto it = tableWidget->item(i, j);			

			if (it == NULL && i != 0 ) {
				tableWidget->setItem(i, j, new QTableWidgetItem("0.000000"));
				it = tableWidget->item(i, j);
				check = true;
			}

			// check for input == non-num && more than 1 floating point
			if (it != NULL && i != 0) {
				try {
					double a = std::stod(it->text().toStdString());
					

					QStringList b = it->text().split(".");
					if (b.size() != 2 ){
						std::cout <<"invalid: " << it->text().toStdString() << std::endl;
						check = true;
					}


					//if (b.size() == 0 ) {
					//	std::cout << "*********" << 000 << std::endl;
					//	std::cout << it->text().toStdString() << std::endl;
					//}
					//if (b.size() == 1) {
					//	std::cout << "*********" << 111 << std::endl;
					//	std::cout << it->text().toStdString() << std::endl;
					//}
					//if (b.size() == 2) {
					//	//std::cout << "*********" << 222 << std::endl;
					//}
					//if (b.size() > 2) {
					//	std::cout << "*********" << 333 << std::endl;
					//	std::cout << it->text().toStdString() << std::endl;
					//}
				}
				catch (...) {
					std::cout << "invalid: " << it->text().toStdString() << std::endl;
					check = true;
				}
			}
			//if (it->text().toDouble() == 0 && i != 0) {
			//	check = true;
			//	//std::cout << "non-number input(s) detected " << std::endl;
			//}

			if (i == 0) {
				dataFile << std::fixed << std::setprecision(6) << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << tableWidget->item(i, 3)->text().toStdString() << "," << tableWidget->item(i, 4)->text().toStdString() << "," << tableWidget->item(i, 5)->text().toStdString(); // first line
				break;
			}
			else {
				
				if (j == 5) {// last item in the row
					//QString::number(x, 'd', 6)
					//it->text().toStdString()
					dataFile << std::fixed << std::setprecision(6) << it->text().toDouble();
				}
				else {
					dataFile << std::fixed << std::setprecision(6) << it->text().toDouble() << ",";
				}
			}			

			
		}
		if(i!= tableWidget->rowCount()-1) // prevent last empty line
			dataFile << std::endl;
		//std::cout << " ^^^ " << std::endl;	
	}


	if (check) {
		
		//std::ofstream ofs;
		//ofs.open(gcpEEtxt.toStdString(), std::ofstream::out | std::ofstream::trunc);
		//ofs.close();
		QMessageBox::warning(nullptr, QString(tr("Warning")), QString(tr("Empty/Non floating number input(s) detected.\nEnsure everything is in order and press Continue again.")));
	}
	else {
		// proceed to gcpEE
		emit(_continue());
	}


}


void dialog_gcp::addRow(int row) {

	int index = 0;

	for (int i = 0; i < row ; i++)
	{
		
		// real x y z
		tableWidget->insertRow(tableWidget->rowCount());
		double x = storage.at(0).at(i);
		double y = storage.at(1).at(i);
		double z = storage.at(2).at(i);
		tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(x,'d',6)));
		tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(y, 'd', 6)));
		tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(z, 'd', 6)));
		if (index == 0) {
			QTableWidgetItem* item = new QTableWidgetItem("0");
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem* item1 = new QTableWidgetItem("0");
			item1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem* item2 = new QTableWidgetItem("0");
			item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			/*tableWidget->setItem(i, 0, new QTableWidgetItem("0"));
			tableWidget->setItem(i, 1, new QTableWidgetItem("0"));
			tableWidget->setItem(i, 2, new QTableWidgetItem("0"));*/
			tableWidget->setItem(i, 0, item);
			tableWidget->setItem(i, 1, item1);
			tableWidget->setItem(i, 2, item2);
		}
		index++;
	}
	
	//tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	//tableWidget->horizontalHeader()->setMinimumSectionSize(120);
	tableWidget->update();

}

void dialog_gcp::_cancel_triggered() {
	QMessageBox::StandardButton but = QMessageBox::Ok;
	QString title = QMessageBox::tr("Information");
	QString text = QMessageBox::tr("Please go to Process page and cancel the \ncorresponding project.\n");
	QMessageBox msgB(nullptr);
	msgB.setIcon(QMessageBox::Icon::Information);
	msgB.setWindowTitle(title);
	msgB.setText(text);
	//msgB.setStandardButtons(but);
	QAbstractButton* pButton_cancel = msgB.addButton(but);
	pButton_cancel->setStyleSheet("");
	msgB.setStyleSheet("QMessageBox{font-size:14px;}\
						QPushButton {font-size:14px;}");
	//msgB.setFixedSize(50, 50);
	msgB.exec();

}
void dialog_gcp::_continue_triggered() {

	//QMessageBox::StandardButton check;
	//check = QMessageBox::question(nullptr, QString(tr("Confirmation")), QString(tr("Ready to run the bag?")), QMessageBox::Yes | QMessageBox::No);
	//if (check == QMessageBox::Yes) {
	//	std::cout << "saving data into txt file.. " << std::endl;
	//	addItem();

	//}
	QMessageBox::StandardButtons buts = (QMessageBox::Yes | QMessageBox::No);
	QString title = QMessageBox::tr("Confirmation");
	QString text = QMessageBox::tr("Ready to continue the process?\n");
	QMessageBox msgB(nullptr);
	msgB.setIcon(QMessageBox::Icon::Question);
	msgB.setWindowTitle(title);
	msgB.setText(text);	
	msgB.setStyleSheet("QMessageBox{font-size:14px;}\
							QPushButton {font-size:14px;}\
							");
	msgB.setStandardButtons(buts);
	int res = msgB.exec();



	switch (res) {
	case QMessageBox::Yes:
		std::cout << "saving data into txt file.. " << std::endl;
		addItem();
		break;

	default:
		break;
	}
}

std::vector<std::vector<double>> dialog_gcp::readTxt(QString txtPath) {

	std::vector<double> rx;
	std::vector<double> ry;
	std::vector<double> rz;

	std::vector<std::vector<double>> fin;

	// read GPS data form txt file    
	std::ifstream fi(txtPath.toStdString());
	if (fi.is_open()) {
		std::cout << "Open GPS(GCP) file " << txtPath.toStdString() << " succeed" << std::endl;
	}
	else {
		std::cout << "Open GPS(GCP) file " << txtPath.toStdString() << " failed" << std::endl;

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

		//lineCounter++;
		//if (lineCounter == 1) {
		//	continue;
		//}
		
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

void dialog_gcp::addxyz(double x, double y, double z) {

	// ==============================================================
	QItemSelectionModel* select = tableWidget->selectionModel();

	//if (select->hasSelection())std::cout << "selection" << std::endl;
	//; //check if has selection
	////elect->selectedRows(); // return selected row(s)

	//		//QModelIndexList selection = yourTableView->selectionModel()->selectedRows();

	//	// Multiple rows can be selected
	//for (int i = 0; i < select->selectedRows().count(); i++)
	//{
	//	QModelIndex index = select->selectedRows().at(i);
	//	//qDebug() << index.row();
	//	std::cout << "r " << index.row() << std::endl;
	//}
	if (select->hasSelection()) {
		if (select->selectedRows().count() == 1) {
			QModelIndex index = select->selectedRows().at(0);
			std::cout << "r " << index.row() << std::endl;
			// starting row cannot be edited
			if (index.row() != 0) {

				QMessageBox::StandardButtons buts = (QMessageBox::Yes | QMessageBox::No);
				QString title = QMessageBox::tr("Confirmation");
				QString text = QMessageBox::tr("Add %1, %2, %3 to row %4?\n").arg(x).arg(y).arg(z).arg(index.row()+1);
				QMessageBox msgB(nullptr);
				msgB.setIcon(QMessageBox::Icon::Question);
				msgB.setWindowTitle(title);
				msgB.setText(text);
				msgB.setStyleSheet("QMessageBox{font-size:14px;}\
							QPushButton {font-size:14px;}\
							"); // QPushButton[text='&Yes']{background-color:red}
				msgB.setStandardButtons(buts);
				int res = msgB.exec();
				int i;
				switch (res) {
				case QMessageBox::Yes:
					// point pick x y z
					i = index.row();
					tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(x, 'd', 6)));
					tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(y, 'd', 6)));
					tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(z, 'd', 6)));
					tableWidget->update();
					break;

				default:
					break;
				}			
			}
		}
	}





}