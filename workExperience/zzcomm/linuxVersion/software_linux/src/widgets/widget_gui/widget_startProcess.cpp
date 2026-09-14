#include <iostream>
#include "../../include/include_gui/widget_startProcess.h"
#include "../../include/include_gui/mainApp.h"
#include <QMessageBox>
#include <QFileDialog>
#include <stdlib.h>
#include <QGuiApplication>
#include <QScreen>

widget_startProcess::widget_startProcess(mainApp* parent)
	: QDialog(parent),
	slamBag(QString()),rtkFile(QString()),gcpFile(QString()),colorFileMp4(QString()), colorFilePc(QString()),
	colorFileMp4Path(QString())
	,ProductParam(QString())
	,canRun(false)
	,cudaQ(nullptr)
	, cudaQM(nullptr)
	, r(nullptr)
{
	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	/*this->setFixedHeight(250);*/
	this->setMinimumWidth(600);
	setupUi(this);
	init();
	window_ = parent;

	adjustSize(); // hide -> adjust ui size
}

widget_startProcess::~widget_startProcess()
{}

void widget_startProcess::closeEvent(QCloseEvent* event) {
	if (r) {
		_rtk_coor_close();
	}


	if(slamBag.isEmpty())
		emit(_close());
	else
		if(!canRun)
			emit(_closeNonEmpty());
}

void widget_startProcess::init() {
	widget_setting->hide(); // hide setting first
	btnSlam->setStyleSheet("font-size: 16px;");
	btnColorMp4->setStyleSheet("font-size: 16px;");
	btnColorPc->setStyleSheet("font-size: 16px;");
	btnRtk->setStyleSheet("font-size: 16px;");
	btnGcp->setStyleSheet("font-size: 16px;");
	
	// push button
	connect(btnParam, SIGNAL(clicked()), this, SLOT(_setting()), Qt::UniqueConnection);
	connect(btnStart, SIGNAL(clicked()), this, SLOT(_start()), Qt::UniqueConnection);
	connect(btnSlam, SIGNAL(clicked()), this, SLOT(_slam()), Qt::UniqueConnection);
	connect(btnProductParams, SIGNAL(clicked()), this, SLOT(_slamProductParams()), Qt::UniqueConnection);
	connect(btnColorMp4, SIGNAL(clicked()), this, SLOT(_colorMp4()), Qt::UniqueConnection);
	connect(btnColorPc, SIGNAL(clicked()), this, SLOT(_colorPc()), Qt::UniqueConnection);
	connect(btnRtk, SIGNAL(clicked()), this, SLOT(_rtk()), Qt::UniqueConnection);
	connect(btnGcp, SIGNAL(clicked()), this, SLOT(_gcp()), Qt::UniqueConnection);
	connect(btnSavingPath, SIGNAL(clicked()), this, SLOT(_savingPath()), Qt::UniqueConnection);
	connect(btnSavingPath_Color, SIGNAL(clicked()), this, SLOT(_colorProjPath()), Qt::UniqueConnection);
	connect(btnImu, SIGNAL(clicked()), this, SLOT(_colorImu()), Qt::UniqueConnection);
	connect(btnTraj, SIGNAL(clicked()), this, SLOT(_colorTraj()), Qt::UniqueConnection);
	connect(btnCali, SIGNAL(clicked()), this, SLOT(_colorCali()), Qt::UniqueConnection);
	connect(pushButtonCoor, SIGNAL(clicked()), this, SLOT(_rtk_coor()), Qt::UniqueConnection);
	
	
	// starting = slam only
	connect(comboBoxType, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxType(int)), Qt::UniqueConnection);
	frameColor->hide();
	frameRtk->hide();
	frameGcp->hide();
	tabSetting->removeTab(3);
	tabSetting->removeTab(2);
	tabSetting->removeTab(1);
	//checkBoxAlgorithm1->setDisabled(1); // TO BE DELETED ----- FOR MEL ONLY ************************	
	// ====================

	// pre set parameters
	// Slam ==================================]
	doubleSpinBox_lidarMinXToFilter->setMaximum(doubleSpinBox_lidarMaxXToFilter->value() - 0.01);
	spinBox_intervalFrames->setDisabled(true);
	spinBoxBaIter->setDisabled(true);
	doubleSpinBoxBaVoxel->setDisabled(true);
	comboBoxBa->setDisabled(true);
	connect(doubleSpinBox_lidarMaxXToFilter, SIGNAL(valueChanged(double)), this, SLOT(_doubleSpinBox_lidarMaxXToFilter()), Qt::UniqueConnection);
	connect(checkBox_loopClosure, SIGNAL(stateChanged(int)), this, SLOT(_checkBox_loopClosure(int)), Qt::UniqueConnection);
	connect(checkBoxBa, SIGNAL(stateChanged(int)), this, SLOT(_checkBox_ba(int)), Qt::UniqueConnection);
	lineEditSavingPath->setText(QDir::currentPath());
	checkBoxAlgorithm1->setChecked(1);
	checkBoxRotation->setChecked(1);
	checkBoxCsv->setChecked(1);

	// =======================================

	// RTK ==================================
	comboBoxOutputCoordinateSystem->setCurrentIndex(0); // TO BE DELETED ***** FOR MEL ONLY
	comboBoxHemi->setCurrentIndex(0); // TO BE DELETED ----- FOR MEL ONLY ************************
	checkBoxGeoid->setChecked(0);
	labelSystem->hide();
	comboBoxOutputCoordinateSystem->hide();
	// =======================================

	// GCP ==================================
	checkBoxManual_2->setEnabled(0);
	doubleSpinBoxZCoorMax->setDisabled(1);
	doubleSpinBoxZCoorMin->setDisabled(1);
	spinBoxFilterDist_2->setDisabled(1);
	connect(checkBoxSwitch_2, SIGNAL(stateChanged(int)), this, SLOT(_checkBox_switch(int)), Qt::UniqueConnection);
	connect(checkBoxZCoor_2, SIGNAL(stateChanged(int)), this, SLOT(_checkBox_zcoor(int)), Qt::UniqueConnection);
	checkBoxSwitch_2->setChecked(1);
	spinBoxGcps_2->hide();
	label_48->hide();
	// =======================================

	// colourisation =========================
	QString cali = "../Lib/company/camera-calib.yaml";   // each camera has its own cali-file *********
	QFileInfo fi(cali);
	lineEditCali->setText(fi.absoluteFilePath());
	connect(btnConfigFile, SIGNAL(clicked()), this, SLOT(_colorConfig()), Qt::UniqueConnection);
	// =======================================

	// cuda 
	getCudaCompute();
	getMemory();

	// combobox for environment
	connect(comboBoxEnv, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxEnvType(int)), Qt::UniqueConnection);

	// Product param ********* TO BE DELETED when this yaml file is outputed from device
	QString cP = "../Lib/company/Product.yaml";   // each camera has its own cali-file *********
	QFileInfo fi1(cP);
	QString cPPath = fi1.absoluteFilePath();
	lineEditProductParams->setText(cPPath);
	
}

void widget_startProcess::_setting() {
	if (widget_setting->isVisible()) {
		widget_setting->hide();
		this->setMinimumHeight(10);
		this->setMinimumWidth(450);
	}	
	else {
		widget_setting->update();
		tabSetting->setCurrentIndex(comboBoxType->currentIndex());
		widget_setting->show();

		QSize size = QGuiApplication::primaryScreen()->size();
		std::cout << size.height() << std::endl;
		std::cout << size.width() << std::endl;
		this->setMinimumHeight(int(size.height()*0.75));
		this->setMinimumWidth(500);
	}	
	adjustSize(); 	
	update();
}

void widget_startProcess::setParam(int i, widget_newProcess* np) {
	if (i == 0) { // slam
		// to be deleted =========================
		np->baIter = spinBoxBaIter->value();
		np->baVoxel = doubleSpinBoxBaVoxel->value();
		np->baSplit = comboBoxBa->currentText();
		// =======================================
		setSlamParam(np);
		np->setType(process_type::Slam);
	}
	else if (i == 1) { // color

		QString proj = lineEditSavingPath_Color->text();
		QString imu = lineEditImu->text();
		QString traj = lineEditTraj->text();
		QString cali = lineEditCali->text();
		//QString cali = "../Lib/company/camera-calib.yaml";   // each camera has its own cali-file *********
		int frameskip = spinBoxFrameSkip->value();
		int firstframe = spinBoxFrameStart->value();
		int lastframe = spinBoxFrameEnd->value();
		float visiblerange = doubleSpinBoxVisibleRange->value();
		float quality = doubleSpinBoxQuality->value();
		bool unclorPts = checkBoxUncolorPts->isChecked();
		QString penetration = comboBoxPenetrate->currentText(); 

		np->config = lineEditConfig->text(); // to be deleted. used for manual inputs now

		np->setColourParam(proj, imu, traj, cali, frameskip, firstframe, lastframe, visiblerange,quality,unclorPts,penetration);


		np->setType(process_type::Colourisation);
	}
	else if (i == 2) { // rtk

		bool geoid = checkBoxGeoid->isChecked();
		//QString outputCoordinateSystem = comboBoxOutputCoordinateSystem->currentText();
		QString outputCoordinateSystem = lineEditCoor->text();
		float doubleSpinBoxXRtk1 = doubleSpinBoxXRtk->value();
		float doubleSpinBoxYRtk1 = doubleSpinBoxYRtk->value();;
		float doubleSpinBoxZRtk1 = doubleSpinBoxZRtk->value();
		int spinBoxGpsFreq1 = spinBoxGpsFreq->value();
		QString hemisphere = comboBoxHemi->currentText(); // hemisphere: only northern and southern for now 
		bool hemi = false;

		outputCoordinateSystem = _rtk_coor_sys(lineEditCoor->text());

		if (hemisphere == "Northern") {
			hemi = false;
		}
		else if (hemisphere == "Southern") {
			hemi = true;
		}
		std::cout << outputCoordinateSystem.toStdString() << std::endl;

		np->setRtkParam(
			geoid,
			outputCoordinateSystem,
			doubleSpinBoxXRtk1,
			doubleSpinBoxYRtk1,
			doubleSpinBoxZRtk1,
			spinBoxGpsFreq1,
			hemisphere,
			hemi
		);

		np->setType(process_type::RTK);

		// set slam param =====================
		setSlamParam(np);
		// ====================================
	}
	else if (i == 3) { // gcp

		bool manualCheck = checkBoxManual_2->isChecked();
		float radMax = doubleSpinBoxRadmax_2->value();
		float radMin = doubleSpinBoxRadmin_2->value();
		int no_of_gcp = spinBoxGcps_2->value();
		int intensityMax = spinBoxIntensitymax->value();
		int intensityMin = spinBoxIntensitymin->value();
		int iteration = spinBoxIteration_2->value();
		int circlePts = spinBoxMinpoints_2->value();
		bool filterDist = checkBoxSwitch_2->isChecked();
		int filterDistance = spinBoxFilterDist_2->value();
		bool filterZCoor = checkBoxZCoor_2->isChecked();
		float filterZCoorMax = doubleSpinBoxZCoorMax->value();
		float filterZCoorMin = doubleSpinBoxZCoorMin->value();
		
		manualCheck = false;

		np->setGcpParam(
			manualCheck,
			radMax,
			radMin,
			no_of_gcp,
			intensityMax,
			intensityMin,
			iteration,
			circlePts,
			filterDist,
			filterDistance,
			filterZCoor,
			filterZCoorMax,
			filterZCoorMin
		);
		np->setType(process_type::GCP);

		// set slam param =====================
		setSlamParam(np);
		// ====================================
	}
}

void widget_startProcess::_start() {	
	QMessageBox::StandardButton check;
	check = QMessageBox::question(this, QString(tr("Confirmation")), QString(tr("Ready to run the bag?")), QMessageBox::Yes | QMessageBox::No);
	if (check == QMessageBox::Yes) {

		if (comboBoxType->currentIndex() == 0) {
			if (slamBag.isEmpty()) {
				QMessageBox::warning(this,QString(tr("Warning")), QString(tr("Invalid .bag file.")));
				return;
			}
		}
		else if (comboBoxType->currentIndex() == 1) {
			if (colorFileMp4.isEmpty() || colorFilePc.isEmpty()) {
				QMessageBox::warning(this, QString(tr("Warning")), QString(tr("Invalid file inputs.")));
				return;
			}else if (lineEditImu->text().isEmpty() || lineEditTraj->text().isEmpty()) {
				QMessageBox::warning(this, QString(tr("Warning")), QString(tr("Missing file inputs.")));
				return;
			}
		}
		else if (comboBoxType->currentIndex() == 2) {
			if (slamBag.isEmpty() ) {
				QMessageBox::warning(this, QString(tr("Warning")), QString(tr("Invalid file inputs.")));
				return;
			}
			if (lineEditCoor->text().isEmpty()) {
				QMessageBox::warning(nullptr, QString(tr("Warning")), QString(tr("Missing coordinate system input.")));
				return;
			}
		}
		else if (comboBoxType->currentIndex() == 3) {
			if (slamBag.isEmpty() || gcpFile.isEmpty()) {
				QMessageBox::warning(this, QString(tr("Warning")), QString(tr("Invalid file inputs.")));
				return;
			}
			
		}

		if (comboBoxType->currentIndex() == 2) {
			if (rtkFile.isEmpty())
				rtkFile = "";
		}

		// check for path's validity
		if (window_->checkFilePath(slamBag)) {
			canRun = true;
			emit(_started());
		}
	}
}

void widget_startProcess::comboboxType(int i) {
	
	switch (i)
	{
	case 0: //slam
		// first layout
		frameSlam->show();
		frameColor->hide();
		frameRtk->hide();
		frameGcp->hide();
		// 2nd layout
		tabSetting->removeTab(3);
		tabSetting->removeTab(2);
		tabSetting->removeTab(1);
		tabSetting->removeTab(0);
		tabSetting->insertTab(0, frameSlamSetting, "Slam"); // restore tab

		break;
	case 1: // color
		frameSlam->hide();
		frameColor->show();
		frameRtk->hide();
		frameGcp->hide();
		// 2nd layout
		tabSetting->removeTab(3);
		tabSetting->removeTab(2);
		tabSetting->removeTab(1);
		tabSetting->removeTab(0);
		tabSetting->insertTab(0, frameColorSetting, "Colourisation"); // restore tab

		break;
	case 2: // rtk
		frameSlam->show();
		frameColor->hide();
		frameRtk->show();
		frameGcp->hide();
		// 2nd layout
		tabSetting->removeTab(3);
		tabSetting->removeTab(2);
		tabSetting->removeTab(1);
		tabSetting->removeTab(0);
		tabSetting->insertTab(0, frameSlamSetting, "Slam"); // restore tab
		tabSetting->insertTab(1, frameRtkSetting, "RTK"); // restore tab

		break;
	case 3: //gcp
		frameSlam->show();
		frameColor->hide();
		frameRtk->hide();
		frameGcp->show();
		// 2nd layout
		tabSetting->removeTab(3);
		tabSetting->removeTab(2);
		tabSetting->removeTab(1);
		tabSetting->removeTab(0);
		tabSetting->insertTab(0, frameSlamSetting, "Slam"); // restore tab
		tabSetting->insertTab(1, frameGcpSetting, "GCP"); // restore tab

		break;
	default:
		break;
	}
	
	//tabSetting->setCurrentIndex(comboBoxType->currentIndex());	
	
	widget_setting->update();
	adjustSize();
	update();

}

void widget_startProcess::comboboxEnvType(int i) {

	switch (i)
	{
	case 0: //default


		resetAllParams();

		break;
	case 1: //indoor


		resetAllParams();


		doubleSpinBox_lidarMaxRange->setValue(25.0);
		comboBoxSigma->setCurrentIndex(1);
		doubleSpinBoxVoxel->setValue(0.3);

		break;
	case 2: // outdoor


		resetAllParams();

		spinBox_intervalFrames->setValue(3000);
		doubleSpinBox_intensityFilter->setValue(1.0);
		doubleSpinBox_lidarMaxRange->setValue(100.0);
		doubleSpinBoxAlgorithm2Uphold->setValue(0.5);

		break;
	case 3: // glasses
		resetAllParams();


		// slam
		doubleSpinBox_intensityFilter->setValue(5.0);
		comboBoxSigma->setCurrentIndex(1);


		break;
	case 4: // narrow spaces

		resetAllParams();


		// slam
		//checkBox_narrowSpace->setChecked(true);
		//doubleSpinBox_lidarMaxRange->setValue(50.0);
		//spinBox_icpMaximumIterations->setValue(8);
		comboBoxSigma->setCurrentIndex(1);
		//doubleSpinBox_lidarMaxRange->setValue(25.0);
		doubleSpinBoxVoxel->setValue(0.3);
		spinBox_icpMaximumIterations->setValue(8);

		

		break;
	case 5: //uav,drone


		resetAllParams();
		spinBox_intervalFrames->setValue(1500);
		doubleSpinBox_lidarMaxRange->setValue(100.0);
		doubleSpinBox_translationThreshold->setValue(1000.0);
		doubleSpinBox_intensityFilter->setValue(1.0);
		doubleSpinBoxAlgorithm2Uphold->setValue(0.5);


		break;
	default:
		break;
	}

	widget_setting->update();
	update();
	adjustSize();

}

// ================================================= SLAM ====================================================
void widget_startProcess::_slam() {
	QString file = QFileDialog::getOpenFileName(
		this,
		"Bag File",
		QDir::currentPath(),
		"Supported formats (*.bag)"
	);
	if (file.isEmpty()) {
		return;
	}
	if (!window_->checkFilePath(file)) {
		return;
	}

	lineEditSlam->setText(file);
	slamBag = file;
	lineEditSlamName->setText(window_->getFileName(file));
	lineEditSavingPath->setText(window_->getFilePath(file));
	// add a function to find the Product.yaml automatically 
	// ********************************************* TO BE DONE *************************************
}

void widget_startProcess::_doubleSpinBox_lidarMaxXToFilter() {
	doubleSpinBox_lidarMinXToFilter->setMaximum(doubleSpinBox_lidarMaxXToFilter->value() - 0.01);
}

void widget_startProcess::_checkBox_loopClosure(int b) {
	if (b) 
		spinBox_intervalFrames->setEnabled(true);
	else 
		spinBox_intervalFrames->setDisabled(true);
}
void widget_startProcess::_checkBox_ba(int b) {
	if (b) {
		spinBoxBaIter->setEnabled(true);
		doubleSpinBoxBaVoxel->setEnabled(true);
		comboBoxBa->setEnabled(true);
	}
	else {
		spinBoxBaIter->setDisabled(true);
		doubleSpinBoxBaVoxel->setDisabled(true);
		comboBoxBa->setDisabled(true);
	}

}
void widget_startProcess::_savingPath() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Saving Path"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!window_->checkFilePath(dir)) {
		return;
	}
	if (!dir.isEmpty()) {
		lineEditSavingPath->setText(dir);
	}		
}
void widget_startProcess::_slamProductParams() {
	QString file;
	if (slamBag != NULL) {
		file = QFileDialog::getOpenFileName(
			this,
			"Yaml File",
			slamBag,
			"Supported formats (*.yaml)"
		);
	}
	else {
		file = QFileDialog::getOpenFileName(
			this,
			"Yaml File",
			QDir::currentPath(),
			"Supported formats (*.yaml)"
		);
	}
	
	if (file.isEmpty()) {
		return;
	}
	if (!window_->checkFilePath(file)) {
		return;
	}

	lineEditProductParams->setText(file);
}
void widget_startProcess::setSlamParam(widget_newProcess* np) {

	int no_of_cores = spinBox_noOfCores->value();
	bool ba = checkBoxBa->isChecked();
	bool csv = checkBoxCsv->isChecked();
	bool rotatingFrames = checkBoxRotation->isChecked();
	bool loopClosure = checkBox_loopClosure->isChecked();
	int loopClosureIntervalFrames = spinBox_intervalFrames->value();
	bool narrowSpace = checkBox_narrowSpace->isChecked();
	float minIntensityFilter = doubleSpinBox_intensityFilter->value();
	float lidarMaxRange = doubleSpinBox_lidarMaxRange->value();
	float lidarMinRange = doubleSpinBox_lidarMinRange->value();
	float lidarMaxXToFilter = doubleSpinBox_lidarMaxXToFilter->value();
	float lidarMinXToFilter = doubleSpinBox_lidarMinXToFilter->value();
	float translationThreshold = doubleSpinBox_translationThreshold->value();
	int icpMaximumIterations = spinBox_icpMaximumIterations->value();
	QString bagName = lineEditSlamName->text();
	QString bagPath = lineEditSavingPath->text();
	bool Algorithm1 = checkBoxAlgorithm1->isChecked();
	QString lossSigma = comboBoxSigma->currentText();
	bool checkBoxtolaz1 = checkBoxtolaz->isChecked();
	float doubleSpinBoxResolution1 = doubleSpinBoxResolution->value();
	bool cuda = checkBoxCuda->isChecked();
	float voxel_size = doubleSpinBoxVoxel->value();
	QString ProductParams = lineEditProductParams->text();
	int Algorithm2Frame = spinBoxAlgorithm2Frame->value();
	float Algorithm2Uphold = doubleSpinBoxAlgorithm2Uphold->value();

	//Algorithm1 = true; // TO BE DELETED ----- FOR MEL ONLY ************************
	checkBoxtolaz1 = true; // for hp OR if normal Algorithm2 output needs to be outputed

	np->setSlamParam(
		no_of_cores,
		ba,
		csv,
		rotatingFrames,
		loopClosure,
		loopClosureIntervalFrames,
		narrowSpace,
		minIntensityFilter,
		lidarMaxRange,
		lidarMinRange,
		lidarMaxXToFilter,
		lidarMinXToFilter,
		translationThreshold,
		icpMaximumIterations,
		bagName,
		bagPath,
		Algorithm1,
		lossSigma,
		checkBoxtolaz1,
		doubleSpinBoxResolution1,
		cuda,
		voxel_size,
		ProductParams,
		Algorithm2Frame,
		Algorithm2Uphold);

	
	//std::cout << "cuda: " << cuda << std::endl;
}
// =========================================== COLOR ================================================================

void widget_startProcess::_colorMp4() {
	QStringList files = QFileDialog::getOpenFileNames(
		this,
		"Video File",
		QDir::currentPath(),
		"Supported formats (*.mp4 *.MP4)"
	);
	if (files.isEmpty()) {
		return;
	}
		if (!window_->checkFilePath(files.at(0))) {
		return;
	}

	// for (const auto& name : files) {
	// 	std::cout << name.toStdString()<<"\n"<< std::endl;
	// }
	// std::cout << "=================="<< std::endl;
	
	// sort the list
	files = compareMP4Names(files);


	QString temp;
	for (const auto& name : files) {
		QString name1 = name + QString(" ");
		temp += name1;

		std::cout << name.toStdString()<< std::endl;
	}
	
	lineEditColorMp4->setText(temp);
	colorFileMp4 = files;

	colorFileMp4Path = window_->getFilePath(files.at(0)) + "/";
}
QStringList widget_startProcess::compareMP4Names(QStringList files) //const QString& s1,const QString& s2
{
  	//apply section(),mid() or whatever to take out the integer part and compare
	for(int i=0; i<files.length(); i++){
		for(int j=0; j<files.length()-i-1; j++){


			QString cur = files.at(j);
			QString tbc = files.at(j+1); // to be compared
						
			//GX010013.MP4
			QString temp1=cur.section("GX",1);//temp1=="XXX.mp4";
			temp1=temp1.section('.',0,0);//temp1=="XXX"
			//std::cout << "temp1: " <<temp1.toStdString()<< std::endl;

			QString temp2=tbc.section("GX",1);
			temp2=temp2.section('.',0,0);
			//std::cout << "temp2: " <<temp2.toStdString()<< std::endl;

			//std::cout << "current j = "<< std::to_string(j)<< std::endl;
			// compare the two ints after GX
			if(temp1.toInt()>temp2.toInt()){
				QString to_be_del = tbc;
				files.removeAt(j+1);
				files.insert(j,to_be_del);
				//std::cout << "j: " <<files.at(j).toStdString()<< std::endl;
				//std::cout << "j+1: " <<files.at(j+1).toStdString()<< std::endl;
			}else{
				//std::cout << "pass"<< std::endl;
			}	
			//std::cout << "\n"<< std::endl;
			
		}
	}

	// for (const auto& name : files) {

	// 	std::cout << name.toStdString()<< std::endl;
	// }

	QStringList f = files;
	return f;
}


void widget_startProcess::_colorPc() {
	QString file;
	if (colorFileMp4Path != NULL) {
		file = QFileDialog::getOpenFileName(
			this,
			"Point Cloud File",
			colorFileMp4Path,
			"Supported formats (*.ply *.las *.laz)"
		);
	}
	else {
		file = QFileDialog::getOpenFileName(
			this,
			"Point Cloud File",
			QDir::currentPath(),
			"Supported formats (*.ply *.las *.laz)"
		);
	}

	if (file.isEmpty()) {
		return;
	}
	if (!window_->checkFilePath(file)) {
		return;
	}

	lineEditColorPc->setText(file);
	colorFilePc = file;

	autoFillColor();
}

void widget_startProcess::autoFillColor() {

	QString pathLocation = window_->getFilePath(colorFilePc) +"/";
	//pathLocation = pathLocation.replace('/', '\\');

	//saving path
	lineEditSavingPath_Color->setText(pathLocation);

	//imu
	QStringList filter("*.bag");
	QDir dir(pathLocation);
	QStringList bags = dir.entryList(filter);
	for (QString s : bags) { // filter out slam.bag
		//std::cout << s.toStdString() << std::endl;
		QRegularExpression regex("_SLAM");
		QRegularExpressionMatchIterator i = regex.globalMatch(s);
		//while (i.hasNext()) {
		//	QRegularExpressionMatch match = i.next();

		//	 //std::cout << "Matched string: " << match.captured(0).toStdString()<<std::endl;
		//}
		if (!i.hasNext()) {
			lineEditImu->setText(pathLocation + s);
		}

	}

	// traj
	QStringList filters("*.csv");
	QDir dirs(pathLocation);
	QStringList csvs = dirs.entryList(filters);
	for (QString s : csvs) { // filter out slam.bag
		//std::cout << s.toStdString() << std::endl;
		QRegularExpression regex("_begin");
		QRegularExpressionMatchIterator i = regex.globalMatch(s);
		while (i.hasNext()) {
			QRegularExpressionMatch match = i.next();
			if (match.hasMatch()) {
				lineEditTraj->setText(pathLocation + s);
			}
			//std::cout << "Matched string: " << match.captured(0).toStdString()<<std::endl;
		}
	}


}

void widget_startProcess::_colorProjPath() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Saving Path"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!window_->checkFilePath(dir)) {
		return;
	}
	if (!dir.isEmpty()) {
		lineEditSavingPath_Color->setText(dir);
	}
}
void widget_startProcess::_colorImu() {
	QString file;
	if (colorFileMp4Path != NULL) {
		file = QFileDialog::getOpenFileName(
			this,
			"Original Bag File",
			colorFileMp4Path,
			"Supported formats (*.bag)" // TO BE CHANGED
		);
	}
	else {
		file = QFileDialog::getOpenFileName(
			this,
			"Original Bag File",
			QDir::currentPath(),
			"Supported formats (*.bag)" // TO BE CHANGED
		);
	}
	
	if (file.isEmpty()) {
		return;
	}
	if (!window_->checkFilePath(file)) {
		return;
	}

	lineEditImu->setText(file);
}
void widget_startProcess::_colorTraj() {
	QString file;
	if (colorFileMp4Path != NULL) {
		file = QFileDialog::getOpenFileName(
			this,
			"Csv File",
			colorFileMp4Path,
			"Supported formats (*.csv)" // TO BE CHANGED
		);
	}
	else {
		file = QFileDialog::getOpenFileName(
			this,
			"Csv File",
			QDir::currentPath(),
			"Supported formats (*.csv)" // TO BE CHANGED
		);
	}
	
	if (file.isEmpty()) {
		return;
	}
	if (!window_->checkFilePath(file)) {
		return;
	}

	lineEditTraj->setText(file);
}
void widget_startProcess::_colorCali() {
	QString file;
	if (colorFileMp4Path != NULL) {
		file = QFileDialog::getOpenFileName(
			this,
			"Yaml File",
			colorFileMp4Path,
			"Supported formats (*.yaml)" // TO BE CHANGED
		);
	}
	else {
		file = QFileDialog::getOpenFileName(
			this,
			"Yaml File",
			QDir::currentPath(),
			"Supported formats (*.yaml)" // TO BE CHANGED
		);
	}

	
	if (file.isEmpty()) {
		return;
	}
	if (!window_->checkFilePath(file)) {
		return;
	}

	lineEditCali->setText(file);
}
void widget_startProcess::_colorConfig() {
	
}
// ================================================= RTK ====================================================
void widget_startProcess::_rtk() {
	QString file;
	if (slamBag != NULL) {
		file = QFileDialog::getOpenFileName(
			this,
			"Txt File",
			slamBag,
			"Supported formats (*.txt)"
		);
	}
	else {
		file = QFileDialog::getOpenFileName(
			this,
			"Txt File",
			QDir::currentPath(),
			"Supported formats (*.txt)"
		);
	}
	
	if (file.isEmpty()) {
		std::cout << "file empty" << std::endl;
		return;
	}
	if (!window_->checkFilePath(file)) {
		return;
	}

	lineEditRtk->setText(file);
	rtkFile = file;
}
void widget_startProcess::_rtk_coor() {
	if (!r) {
		//std::cout << "r opened" << std::endl;
		r = new dialog_rtk_coor();
		
		connect(r, SIGNAL(coor_(QString)), this, SLOT(_rtk_coor_output(QString)), Qt::UniqueConnection);
		connect(r, SIGNAL(closed_()), this, SLOT(_rtk_coor_close()), Qt::UniqueConnection);
		r->show();
	}

	//dialog_rtk_coor* r = new dialog_rtk_coor();
	//connect(r, SIGNAL(coor_(QString)), this, SLOT(_rtk_coor_output(QString)), Qt::UniqueConnection);
	//r->show();
}

void widget_startProcess::_rtk_coor_output(QString a) {
	lineEditCoor->setText(a);
	_rtk_coor_close();
}
void widget_startProcess::_rtk_coor_close() {
	if (r) {
		r->close();
		r = nullptr;
		std::cout << "r coor closed" << std::endl;
	}
}
QString widget_startProcess::_rtk_coor_sys(QString _outputCoordinateSystem) {
	QString outputCoordinateSystem;

	if (_outputCoordinateSystem == "WGS 84 / UTM zone 54N")
		outputCoordinateSystem = "+proj=utm +zone=54 +datum=WGS84 +units=m +no_defs";
	else if (_outputCoordinateSystem == "WGS 84 / UTM zone 54S")
		outputCoordinateSystem = "+proj=utm +zone=54 +south +datum=WGS84 +units=m +no_defs";
	else if (_outputCoordinateSystem == "WGS 84 / UTM zone 55N")
		outputCoordinateSystem = "+proj=utm +zone=55 +datum=WGS84 +units=m +no_defs";
	else if (_outputCoordinateSystem == "WGS 84 / UTM zone 55S")
		outputCoordinateSystem = "+proj=utm +zone=55 +south +datum=WGS84 +units=m +no_defs";
	else if (_outputCoordinateSystem == "CGCS2000 / 3-degree Gauss-Kruger CM 108E")
		outputCoordinateSystem = "+proj=tmerc +lat_0=0 +lon_0=108 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs";
	else if (_outputCoordinateSystem == "CGCS2000 / 3-degree Gauss-Kruger CM 111E")
		outputCoordinateSystem = "+proj=tmerc +lat_0=0 +lon_0=111 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs";
	else if (_outputCoordinateSystem == "CGCS2000 / 3-degree Gauss-Kruger CM 114E")
		outputCoordinateSystem = "+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs";
	else if (_outputCoordinateSystem == "CGCS2000 / 3-degree Gauss-Kruger CM 117E")
		outputCoordinateSystem = "+proj=tmerc +lat_0=0 +lon_0=117 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs";
	else if (_outputCoordinateSystem == "CGCS2000 / 3-degree Gauss-Kruger CM 120E")
		outputCoordinateSystem = "+proj=tmerc +lat_0=0 +lon_0=120 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs";
	else if (_outputCoordinateSystem == "GDA2020 / MGA zone 55")
		outputCoordinateSystem = "+proj=utm +zone=55 +south +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "WGS 84 / UTM zone 31N")
		outputCoordinateSystem = "+proj=utm +zone=31 +datum=WGS84 +units=m +no_defs";
	else if (_outputCoordinateSystem == "WGS 84 / UTM zone 32N")
		outputCoordinateSystem = "+proj=utm +zone=32 +datum=WGS84 +units=m +no_defs";
	else if (_outputCoordinateSystem == "WGS 84 / UTM zone 33N")
		outputCoordinateSystem = "+proj=utm +zone=33 +datum=WGS84 +units=m +no_defs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS I")
		outputCoordinateSystem = "+proj=tmerc +lat_0=33 +lon_0=129.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS II")
		outputCoordinateSystem = "+proj=tmerc +lat_0=33 +lon_0=131 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS III")
		outputCoordinateSystem = "+proj=tmerc +lat_0=36 +lon_0=132.166666666667 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS IV")
		outputCoordinateSystem = "+proj=tmerc +lat_0=33 +lon_0=133.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS V")
		outputCoordinateSystem = "+proj=tmerc +lat_0=36 +lon_0=134.333333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS VI")
		outputCoordinateSystem = "+proj=tmerc +lat_0=36 +lon_0=136 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS VII")
		outputCoordinateSystem = "+proj=tmerc +lat_0=36 +lon_0=137.166666666667 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS VIII")
		outputCoordinateSystem = "+proj=tmerc +lat_0=36 +lon_0=138.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS IX")
		outputCoordinateSystem = "+proj=tmerc +lat_0=36 +lon_0=139.833333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS X")
		outputCoordinateSystem = "+proj=tmerc +lat_0=40 +lon_0=140.833333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XI")
		outputCoordinateSystem = "+proj=tmerc +lat_0=44 +lon_0=140.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XII")
		outputCoordinateSystem = "+proj=tmerc +lat_0=44 +lon_0=142.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XIII")
		outputCoordinateSystem = "+proj=tmerc +lat_0=44 +lon_0=144.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XIV")
		outputCoordinateSystem = "+proj=tmerc +lat_0=26 +lon_0=142 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XV")
		outputCoordinateSystem = "+proj=tmerc +lat_0=26 +lon_0=127.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XVI")
		outputCoordinateSystem = "+proj=tmerc +lat_0=26 +lon_0=124 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XVII")
		outputCoordinateSystem = "+proj=tmerc +lat_0=26 +lon_0=131 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XVIII")
		outputCoordinateSystem = "+proj=tmerc +lat_0=20 +lon_0=136 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
	else if (_outputCoordinateSystem == "JGD2011 / Japan Plane Rectangular CS XIX")
		outputCoordinateSystem = "+proj=tmerc +lat_0=26 +lon_0=154 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";



	return outputCoordinateSystem;
}
// =============================================== GCP ============================================================

void widget_startProcess::_gcp() {
	QString file;
	if (slamBag != NULL) {
		file = QFileDialog::getOpenFileName(
			this,
			"Txt File",
			slamBag,
			"Supported formats (*.txt)"
		);
	}
	else {
		file = QFileDialog::getOpenFileName(
			this,
			"Txt File",
			QDir::currentPath(),
			"Supported formats (*.txt)"
		);
	}

	if (file.isEmpty()) {
		return;
	}
	if (!window_->checkFilePath(file)) {
		return;
	}

	lineEditGcp->setText(file);
	gcpFile = file;
}

void widget_startProcess::_checkBox_switch(int b) {
	if (b)
		spinBoxFilterDist_2->setEnabled(true);
	else
		spinBoxFilterDist_2->setDisabled(true);
}
void widget_startProcess::_checkBox_zcoor(int b) {
	if (b) {
		doubleSpinBoxZCoorMax->setEnabled(true);
		doubleSpinBoxZCoorMin->setEnabled(true);
	}
	else {
		doubleSpinBoxZCoorMax->setEnabled(false);
		doubleSpinBoxZCoorMin->setEnabled(false);
	}
}

void widget_startProcess::getCudaCompute(){
	if(!cudaQ){
		cudaQ = new QProcess;
		cudaQ->start("nvidia-smi --query-gpu=compute_cap --format=csv,noheader\n");
		connect(cudaQ, SIGNAL(readyReadStandardOutput()), this, SLOT(cudaCompute()));
	}

}
void widget_startProcess::cudaCompute(){
	QString str = QString(cudaQ->readAllStandardOutput());
    //int index = str.lastIndexOf("compute_cap");
	int index = str.lastIndexOf(".");


    if (index != -1) {
		// std::vector<QString> cc; // computing capability
		// for (QString a : str) {
		// 	cc.emplace_back(a);
		// 	//std::cout << "******** " <<a.toStdString() << std::endl;
    	// }

		

		std::cout<<"cuda computing capability: "<<str.toStdString()<<std::endl;

		QStringList pieces = str.split(".");
		int temp = pieces.at(0).toInt();
		
		if(temp>=5){
			checkBoxCuda->setEnabled(1);
			checkBoxCuda->setChecked(1);

		}else{
			checkBoxCuda->setEnabled(0);
		}

		
        disconnect(cudaQ, 0, 0, 0);
        cudaQ->kill();
        cudaQ = NULL;
		//cc.clear();
    }else{

	}
}

void widget_startProcess::getMemory() {
	if (!cudaQM) {
		cudaQM = new QProcess;
		cudaQM->start("nvidia-smi --query-gpu=memory.total --format=csv,noheader\n");
		connect(cudaQM, SIGNAL(readyReadStandardOutput()), this, SLOT(memCompute()));
	}

}
void widget_startProcess::memCompute() {
	QString str = QString(cudaQM->readAllStandardOutput());
	int res = -9999999;
	QRegularExpression regex("(\\d+) MiB"); // ƥ������
	QRegularExpressionMatchIterator i = regex.globalMatch(str);
	while (i.hasNext()) {
		QRegularExpressionMatch match = i.next();
		res = match.captured(1).toInt();
	}
	std::cout << "res: " << res << " MiB___" << std::endl;


	if (res != -9999999) {

		if (res >= 11000) {
			//Algorithm2
			spinBoxAlgorithm2Frame->setValue(40);
			std::cout << "euqal or more than 11000 MiB___" << std::endl;

		}
		else {
			spinBoxAlgorithm2Frame->setValue(30);
			std::cout << "less than 11000 MiB___" << std::endl;
		}


		disconnect(cudaQM, 0, 0, 0);
		cudaQM->kill();
		cudaQM = NULL;
	}
	else {
	}
}

void widget_startProcess::resetAllParams() {

	// slam
	checkBox_loopClosure->setChecked(false);
	spinBox_intervalFrames->setValue(600);
	checkBox_narrowSpace->setChecked(false);
	doubleSpinBox_intensityFilter->setValue(0.0);
	doubleSpinBox_lidarMaxRange->setValue(50.0);
	doubleSpinBox_lidarMinRange->setValue(1.0);
	doubleSpinBox_lidarMaxXToFilter->setValue(0.0);
	doubleSpinBox_lidarMinXToFilter->setValue(-0.6);
	doubleSpinBox_translationThreshold->setValue(0.25);
	spinBox_icpMaximumIterations->setValue(5);
	comboBoxSigma->setCurrentIndex(0);
	doubleSpinBoxResolution->setValue(0.001);
	doubleSpinBoxVoxel->setValue(1.0);

}