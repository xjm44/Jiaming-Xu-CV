
#include "../../include/include_gui/widget_viewProcess.h"
#include "../../include/include_gui/widget_newProcess.h"
#include "../../include/include_gui/mainApp.h"

#include <QFileDialog>
#include <QGuiApplication>
#include <QScreen>

widget_viewProcess::widget_viewProcess(widget_newProcess*parent)
	: QDialog(parent),
	paramClicked(false)
{
	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	//this->setMinimumHeight(100);
	this->setMinimumWidth(500);
	setupUi(this);
	init();
	this->parent = parent;
	adjustSize();

}

widget_viewProcess::~widget_viewProcess()
{}

void widget_viewProcess::init() {
	frame_2->hide();
	connect(btnParam, SIGNAL(clicked()), this, SLOT(_viewParam()), Qt::UniqueConnection);

	// gcp
	connect(btnCoor, SIGNAL(clicked()), this, SLOT(_coor()), Qt::UniqueConnection);
	connect(btnPc, SIGNAL(clicked()), this, SLOT(_pc()), Qt::UniqueConnection);
	connect(btnPcPath, SIGNAL(clicked()), this, SLOT(_pcPath()), Qt::UniqueConnection);
	connect(btnContinue, SIGNAL(clicked()), this, SLOT(_continue()), Qt::UniqueConnection);

	btnContinue->setEnabled(0);

	label_6->hide();spinBoxGcps->hide(); //gcp = noofgcps

	// for accuracy report
	connect(pushButtonAccuracy, SIGNAL(clicked()), this, SLOT(_gcpReportBtn()), Qt::UniqueConnection);
	pushButtonAccuracy->hide();
}

void widget_viewProcess::_viewParam() {
	
	if (!paramClicked) {

		frame_2->show();
		paramClicked = true;

		QSize size = QGuiApplication::primaryScreen()->size();
		std::cout << size.height() << std::endl;
		std::cout << size.width() << std::endl;
		this->setMinimumHeight(int(size.height() * 0.75));
		this->setMinimumWidth(500);
	}
	else {

		frame_2->hide();
		paramClicked = false;
		this->setMinimumHeight(20);
		this->setMinimumWidth(500);
	}
	adjustSize();
	update();

}

void widget_viewProcess::_showParam(process_type type) {

	QStringList slam = parent->getSlamParam();
	QStringList rtk = parent->getRtkParam();
	QFileInfo colourYaml(QString("..\\Lib\\company\\mp4_join.yaml"));

	switch (type) {
	case process_type::Slam:
		// to be deleted
		labelBaIter->setText(QString::number(parent->baIter));
		labelBaVoxel->setText(QString::number(parent->baVoxel));
		labelBaCB->setText(parent->baSplit);

		setSlamParam(slam);


		// tab
		tabSetting->removeTab(3);
		tabSetting->removeTab(2);
		tabSetting->removeTab(1);
		tabSetting->removeTab(0);
		tabSetting->insertTab(0, frameSlamSetting, "Slam"); // restore tab

		break;
	case process_type::Colourisation:

		lineEditSavingPath_Color_2->setText(parent->getprojDir());
		lineEditImu_2->setText(parent->getImu());
		lineEditTraj_2->setText(parent->getTraj());
		lineEditCali_2->setText(parent->getCali());
		spinBoxFrameSkip_2->setValue(parent->getframeSkip());
		spinBoxFrameStart_2->setValue(parent->getfirstFrame());
		spinBoxFrameEnd_2->setValue(parent->getlastFrame());
		doubleSpinBoxVisibleRange_2->setValue(parent->getvisibleRange());
		//QString("..\\Lib\\company\\mp4_join.yaml")
		lineEditColourYaml->setText("");
		doubleSpinBoxQuality->setValue(parent->getQuality());
		if (parent->getUncolorPts()) {
			labelUnclorPts->setText("true");
		}else{
			labelUnclorPts->setText("false");
		}
		labelPenetration->setText(parent->getPenetration());
		
		// tab
		tabSetting->removeTab(3);
		tabSetting->removeTab(2);
		tabSetting->removeTab(1);
		tabSetting->removeTab(0);
		tabSetting->insertTab(0, frameColourSetting, "Colourisation"); // restore tab

		break;
	case process_type::RTK:

		if (rtk.at(0) == "true")
			labelGeoid->setText("true");
		else
			labelGeoid->setText("false");

		doubleSpinBoxXRtk->setValue(rtk.at(2).toFloat());
		doubleSpinBoxYRtk->setValue(rtk.at(3).toFloat());
		doubleSpinBoxZRtk->setValue(rtk.at(4).toFloat());
		spinBoxGpsFreq->setValue(rtk.at(5).toInt());
		lineEditHemi->setText(rtk.at(6));

		if(rtk.at(1) == "+proj=utm +zone=54 +datum=WGS84 +units=m +no_defs")
			outputCoordinateSystem->setText("WGS 84 / UTM zone 54N");
		if (rtk.at(1) == "+proj=utm +zone=54 +south +datum=WGS84 +units=m +no_defs")
			outputCoordinateSystem->setText("WGS 84 / UTM zone 54S");
		if (rtk.at(1) == "+proj=utm +zone=55 +datum=WGS84 +units=m +no_defs")
			outputCoordinateSystem->setText("WGS 84 / UTM zone 55N");
		if (rtk.at(1) == "+proj=utm +zone=55 +south +datum=WGS84 +units=m +no_defs")
			outputCoordinateSystem->setText("WGS 84 / UTM zone 55S");
		if (rtk.at(1) == "+proj=tmerc +lat_0=0 +lon_0=108 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs")
			outputCoordinateSystem->setText("CGCS2000 / 3-degree Gauss-Kruger CM 108E");
		if (rtk.at(1) == "+proj=tmerc +lat_0=0 +lon_0=111 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs")
			outputCoordinateSystem->setText("CGCS2000 / 3-degree Gauss-Kruger CM 111E");
		if (rtk.at(1) == "+proj=tmerc +lat_0=0 +lon_0=114 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs")
			outputCoordinateSystem->setText("CGCS2000 / 3-degree Gauss-Kruger CM 114E");
		if (rtk.at(1) == "+proj=tmerc +lat_0=0 +lon_0=117 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs")
			outputCoordinateSystem->setText("CGCS2000 / 3-degree Gauss-Kruger CM 117E");
		if (rtk.at(1) == "+proj=tmerc +lat_0=0 +lon_0=120 +k=1 +x_0=500000 +y_0=0 +ellps=GRS80 +units=m +no_defs")
			outputCoordinateSystem->setText("CGCS2000 / 3-degree Gauss-Kruger CM 120E");
		if (rtk.at(1) == "+proj=utm +zone=55 +south +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("GDA2020 / MGA zone 55");
		if (rtk.at(1) == "+proj=utm +zone=31 +datum=WGS84 +units=m +no_defs")
			outputCoordinateSystem->setText("WGS 84 / UTM zone 31N");
		if (rtk.at(1) == "+proj=utm +zone=32 +datum=WGS84 +units=m +no_defs")
			outputCoordinateSystem->setText("WGS 84 / UTM zone 32N");
		if (rtk.at(1) == "+proj=utm +zone=33 +datum=WGS84 +units=m +no_defs")
			outputCoordinateSystem->setText("WGS 84 / UTM zone 33N");
		if (rtk.at(1) == "+proj=tmerc +lat_0=33 +lon_0=129.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS I");
		if (rtk.at(1) == "+proj=tmerc +lat_0=33 +lon_0=131 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS II");
		if (rtk.at(1) == "+proj=tmerc +lat_0=36 +lon_0=132.166666666667 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS III");
		if (rtk.at(1) == "+proj=tmerc +lat_0=33 +lon_0=133.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS IV");
		if (rtk.at(1) == "+proj=tmerc +lat_0=36 +lon_0=134.333333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS V");
		if (rtk.at(1) == "+proj=tmerc +lat_0=36 +lon_0=136 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS VI");
		if (rtk.at(1) == "+proj=tmerc +lat_0=36 +lon_0=137.166666666667 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS VII");
		if (rtk.at(1) == "+proj=tmerc +lat_0=36 +lon_0=138.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS VIII");
		if (rtk.at(1) == "+proj=tmerc +lat_0=36 +lon_0=139.833333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS IX");
		if (rtk.at(1) == "+proj=tmerc +lat_0=40 +lon_0=140.833333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS X");
		if (rtk.at(1) == "+proj=tmerc +lat_0=44 +lon_0=140.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XI");
		if (rtk.at(1) == "+proj=tmerc +lat_0=44 +lon_0=142.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XII");
		if (rtk.at(1) == "+proj=tmerc +lat_0=44 +lon_0=144.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XIII");
		if (rtk.at(1) == "+proj=tmerc +lat_0=26 +lon_0=142 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XIV");
		if (rtk.at(1) == "+proj=tmerc +lat_0=26 +lon_0=127.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XV");
		if (rtk.at(1) == "+proj=tmerc +lat_0=26 +lon_0=124 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XVI");
		if (rtk.at(1) == "+proj=tmerc +lat_0=26 +lon_0=131 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XVII");
		if (rtk.at(1) == "+proj=tmerc +lat_0=20 +lon_0=136 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XVIII");
		if (rtk.at(1) == "+proj=tmerc +lat_0=26 +lon_0=154 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs")
			outputCoordinateSystem->setText("JGD2011 / Japan Plane Rectangular CS XIX");

		//slam param
		setSlamParam(slam);
		// ==================
		// tab
		tabSetting->removeTab(3);
		tabSetting->removeTab(2);
		tabSetting->removeTab(1);
		tabSetting->removeTab(0);
		tabSetting->insertTab(0, frameSlamSetting, "Slam"); // restore tab
		tabSetting->insertTab(1, frameRTKSetting, "RTK"); // restore tab

		break;
	case process_type::GCP:


		if (parent->getManualCheck())
			labelManualCheck->setText(tr("true"));
		else
			labelManualCheck->setText(tr("false"));
		spinBoxGcps->setValue(parent->getNoOfGcp());
		doubleSpinBoxRadmax->setValue(parent->getRadMax());
		doubleSpinBoxRadmin->setValue(parent->getRadMin());
		doubleSpinBoxIntensitymax->setValue(parent->getIntensityMax());
		doubleSpinBoxIntensitymin->setValue(parent->getIntensityMin());
		spinBoxMinpoints->setValue(parent->getCirclePts());
		spinBoxIteration->setValue(parent->getIteration());
		if (parent->getFilterDist())
			labelFilterDist->setText(tr("true"));
		else
			labelFilterDist->setText(tr("false"));
		spinBoxFilterDist->setValue(parent->getFilterDistance());
		if(parent->getFilterZCoor())
			labelZCoor->setText(tr("true"));
		else
			labelZCoor->setText(tr("false"));
		spinBoxZCoorMin->setValue(parent->getFilterZCoorMin());
		spinBoxZCoorMax->setValue(parent->getFilterZCoorMax());

		//slam param
		setSlamParam(slam);
		// ==================

		// tab
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
	widget_setting->update();
	update();
}

//slam
void widget_viewProcess::setSlamParam(QStringList slam) {
	spinBox_noOfCores->setValue(slam.at(0).toInt());
	if (slam.at(1) == "true")
		labelBa->setText("true");
	else
		labelBa->setText("false");
	if (slam.at(2) == "true")
		labelcsv->setText("true");
	else
		labelcsv->setText("false");
	if (slam.at(3) == "true")
		labelrfo->setText("true");
	else
		labelrfo->setText("false");
	if (slam.at(4) == "true") {
		labellc->setText("true");
		spinBox_intervalFrames->setValue(slam.at(5).toInt());
	}
	else {
		labellc->setText("false");
		spinBox_intervalFrames->setValue(0);
	}

	if (slam.at(6) == "true")
		labelns->setText("true");
	else
		labelns->setText("false");
	doubleSpinBox_intensityFilter->setValue(slam.at(7).toFloat());
	doubleSpinBox_lidarMaxRange->setValue(slam.at(8).toFloat());;
	doubleSpinBox_lidarMinRange->setValue(slam.at(9).toFloat());;
	doubleSpinBox_lidarMaxXToFilter->setValue(slam.at(10).toFloat());;
	doubleSpinBox_lidarMinXToFilter->setValue(slam.at(11).toFloat());;
	doubleSpinBox_translationThreshold->setValue(slam.at(12).toFloat());;
	spinBox_icpMaximumIterations->setValue(slam.at(13).toInt());
	lineEditName->setText(slam.at(14));
	lineEditPath->setText(slam.at(15));
	if (slam.at(20) == "true")
		labelCuda->setText("true");
	else
		labelCuda->setText("false");
	if (slam.at(16) == "true")
		labelAlgorithm1->setText("true");
	else {
		labelAlgorithm1->setText("false");
		labelCuda->setText("false");
	}
	labellosssigma->setText(slam.at(17));
	if (slam.at(18) == "true")
		labeltolaz->setText("true");
	else
		labeltolaz->setText("false");
	doubleSpinBoxResolution->setValue(slam.at(19).toFloat());;
	doubleSpinBoxVoxel->setValue(slam.at(21).toFloat());
	lineEditProductParams->setText(slam.at(22));
	labelAlgorithm2Frame->setText(slam.at(23));
	labelAlgorithm2Uphold->setText(slam.at(24));

	std::cout<<"23 "<<slam.at(23).toStdString()<<std::endl;
	std::cout<<"24 "<<slam.at(24).toStdString()<<std::endl;
	// to be deleted
	labelBaIter->setText(QString::number(parent->baIter));
	labelBaVoxel->setText(QString::number(parent->baVoxel));
}

// gcp
void widget_viewProcess::_coor() {
	QString file = QFileDialog::getOpenFileName(
		this,
		"Csv File",
		QDir::currentPath(),
		"Supported formats (*.csv)"
	);
	if (file.isEmpty()) {
		return;
	}

	lineEditCoor->setText(file);
}
void widget_viewProcess::_pc() {
	QString file = QFileDialog::getOpenFileName(
		this,
		"Bag File",
		QDir::currentPath(),
		"Supported formats (*.bag)"
	);
	if (file.isEmpty()) {
		return;
	}

	lineEditPc->setText(file);
}
void widget_viewProcess::_pcPath() {
	QString file = QFileDialog::getOpenFileName(
		this,
		"Point Cloud's Path File",
		QDir::currentPath(),
		"Supported formats (*.ply)"
	);
	if (file.isEmpty()) {
		return;
	}

	lineEditPcPath->setText(file);
}
void widget_viewProcess::_continue() {
	// start eliminate error
	
	emit(_gcpEE());
	btnContinue->setEnabled(0);

}
void widget_viewProcess::onAccuracyReport() {
	if (parent->getType() == process_type::GCP) {
		if (parent->getStatus() == running_status::finished) {
			pushButtonAccuracy->show();
			pushButtonAccuracy->click();
		}
	}

}
void widget_viewProcess::_gcpReportBtn() {
	std::cout << "report pressed" << std::endl;
	emit(_gcpReport());
}