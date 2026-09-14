#include "../../include/include_gui/dialog_rtk_coor.h"
#include <QStandardItemModel>
#include <iostream>
#include <QStringListModel>
#include <QMessageBox>

dialog_rtk_coor::dialog_rtk_coor(QWidget *parent)
	: QDialog(parent)
{
	
	
	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	this->setAttribute(Qt::WA_QuitOnClose,false);
	setupUi(this);

	init();


}

dialog_rtk_coor::~dialog_rtk_coor()
{}

void dialog_rtk_coor::closeEvent(QCloseEvent* event) {
	emit closed_();
}

void dialog_rtk_coor::init()
{
	listViewL->setSelectionBehavior(QListView::SelectRows);		// select a row per time
	listViewL->setSelectionMode(QListView::SingleSelection);
	listViewL->setEditTriggers(QListView::NoEditTriggers);
	//t->setShowGrid(false);                                // no grid lines
	listViewL->setFocusPolicy(Qt::NoFocus);
	listViewC->setSelectionBehavior(QListView::SelectRows);		// select a row per time
	listViewC->setSelectionMode(QListView::SingleSelection);
	listViewC->setEditTriggers(QListView::NoEditTriggers);
	//listViewC->setAlternatingRowColors(true);                       
	//t->setShowGrid(false);                                // no grid lines
	listViewC->setFocusPolicy(Qt::NoFocus);
	


	connect(pushButtonNext, SIGNAL(clicked()), this, SLOT(next_triggered()), Qt::UniqueConnection);
	connect(pushButtonBack, SIGNAL(clicked()), this, SLOT(back_triggered()), Qt::UniqueConnection);
	connect(pushButtonCon, SIGNAL(clicked()), this, SLOT(continue_triggered()), Qt::UniqueConnection);

	pushButtonCon->setEnabled(0);
	stackedWidget->setCurrentIndex(0);

	firstPageInfo();
}


void dialog_rtk_coor::next_triggered() {
	pushButtonCon->setEnabled(1);
	stackedWidget->setCurrentIndex(1);

	std::cout <<"1st selection: " << listViewL->currentIndex().data().toString().toStdString() << std::endl;
	secondPageInfo();

}
void dialog_rtk_coor::back_triggered() {
	pushButtonCon->setEnabled(0);
	stackedWidget->setCurrentIndex(0);
	listViewC->setModel(new QStringListModel);
}
void dialog_rtk_coor::continue_triggered(){

	// output to startParam
	if (listViewC->currentIndex().data().toString().isEmpty()) {
		QMessageBox::warning(0, tr("Warning"), tr("No coordinate system is selected."));
	}
	else {
		emit coor_(listViewC->currentIndex().data().toString());
		//this->close();
		//emit closed_();
	}
}


void dialog_rtk_coor::firstPageInfo() {

	//QStandardItemModel* pModel = new QStandardItemModel();
	//QStandardItem* pItem = new QStandardItem;
	//pItem->setText("China");
	//pModel->appendRow(pItem);
	//pModel->insertRow(0, pItem);
	//QStandardItem* pItem1 = new QStandardItem;
	//pItem->setText("Australia");
	//pModel->appendRow(pItem1);
	//QStandardItem* pItem2 = new QStandardItem;
	//pItem->setText("Germany");
	//pModel->appendRow(pItem2);
	//QStandardItem* pItem3 = new QStandardItem;
	//pItem->setText("Japan");
	//pModel->appendRow(pItem3);
	////QStandardItem* pItem4 = new QStandardItem;
	////pItem->setText("yoyoyo");
	////pModel->appendRow(pItem4);
	//listViewL->setModel(pModel);


	QStringList list;                               
	list.append("China");
	list.append("Australia");
	list.append("Germany");
	list.append("Japan");


	QStringListModel* listmodel = new QStringListModel(list);
	listViewL->setModel(listmodel);                  


}

void dialog_rtk_coor::secondPageInfo() {

	QString loc = listViewL->currentIndex().data().toString();

	QStringList list;                               


	if (loc == QString("China")) {
		list.append("CGCS2000 / 3-degree Gauss-Kruger CM 108E");
		list.append("CGCS2000 / 3-degree Gauss-Kruger CM 111E");
		list.append("CGCS2000 / 3-degree Gauss-Kruger CM 114E");
		list.append("CGCS2000 / 3-degree Gauss-Kruger CM 117E");
		list.append("CGCS2000 / 3-degree Gauss-Kruger CM 120E");


	}
	else if (loc == QString("Australia")) {
		list.append("WGS 84 / UTM zone 54S");
		list.append("WGS 84 / UTM zone 55S");
		list.append("GDA2020 / MGA zone 55");

	}
	else if (loc == QString("Germany")) {		
		list.append("WGS 84 / UTM zone 31N");
		list.append("WGS 84 / UTM zone 32N");
		list.append("WGS 84 / UTM zone 33N");

	}
	else if (loc == QString("Japan")) {
		list.append("JGD2011 / Japan Plane Rectangular CS I");
		list.append("JGD2011 / Japan Plane Rectangular CS II");
		list.append("JGD2011 / Japan Plane Rectangular CS III");
		list.append("JGD2011 / Japan Plane Rectangular CS IV");
		list.append("JGD2011 / Japan Plane Rectangular CS V");
		list.append("JGD2011 / Japan Plane Rectangular CS VI");
		list.append("JGD2011 / Japan Plane Rectangular CS VII");
		list.append("JGD2011 / Japan Plane Rectangular CS VIII");
		list.append("JGD2011 / Japan Plane Rectangular CS IX");
		list.append("JGD2011 / Japan Plane Rectangular CS X");
		list.append("JGD2011 / Japan Plane Rectangular CS XI");
		list.append("JGD2011 / Japan Plane Rectangular CS XII");
		list.append("JGD2011 / Japan Plane Rectangular CS XIII");
		list.append("JGD2011 / Japan Plane Rectangular CS XIV");
		list.append("JGD2011 / Japan Plane Rectangular CS XV");
		list.append("JGD2011 / Japan Plane Rectangular CS XVI");
		list.append("JGD2011 / Japan Plane Rectangular CS XVII");
		list.append("JGD2011 / Japan Plane Rectangular CS XVIII");
		list.append("JGD2011 / Japan Plane Rectangular CS XIX");

	}


	QStringListModel* listmodel = new QStringListModel(list);
	listViewC->setModel(listmodel);                 

}

// #include "../../include/include_gui/dialog_rtk_coor.h"
// #include <QStandardItemModel>
// #include <iostream>
// #include <QStringListModel>
// #include <QMessageBox>

// #include "../../include/include_process/coordinate.h"

// dialog_rtk_coor::dialog_rtk_coor(QWidget *parent)
// 	: QDialog(parent)
// {
	
	
// 	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
// 	this->setAttribute(Qt::WA_QuitOnClose,false);
// 	setupUi(this);

// 	init();


// }

// dialog_rtk_coor::~dialog_rtk_coor()
// {}

// void dialog_rtk_coor::closeEvent(QCloseEvent* event) {
// 	emit closed_();
// }

// void dialog_rtk_coor::init()
// {
// 	listViewL->setSelectionBehavior(QListView::SelectRows);		// select a row per time
// 	listViewL->setSelectionMode(QListView::SingleSelection);
// 	listViewL->setEditTriggers(QListView::NoEditTriggers);
// 	//t->setShowGrid(false);                                // no grid lines
// 	listViewL->setFocusPolicy(Qt::NoFocus);
// 	listViewC->setSelectionBehavior(QListView::SelectRows);		// select a row per time
// 	listViewC->setSelectionMode(QListView::SingleSelection);
// 	listViewC->setEditTriggers(QListView::NoEditTriggers);
// 	//listViewC->setAlternatingRowColors(true);                       //�м��ɫ
// 	//t->setShowGrid(false);                                // no grid lines
// 	listViewC->setFocusPolicy(Qt::NoFocus);
	


// 	connect(pushButtonNext, SIGNAL(clicked()), this, SLOT(next_triggered()), Qt::UniqueConnection);
// 	connect(pushButtonBack, SIGNAL(clicked()), this, SLOT(back_triggered()), Qt::UniqueConnection);
// 	connect(pushButtonCon, SIGNAL(clicked()), this, SLOT(continue_triggered()), Qt::UniqueConnection);

// 	pushButtonCon->setEnabled(0);
// 	stackedWidget->setCurrentIndex(0);

// 	firstPageInfo();
// }


// void dialog_rtk_coor::next_triggered() {
// 	pushButtonCon->setEnabled(1);
// 	stackedWidget->setCurrentIndex(1);

// 	std::cout <<"1st selection: " << listViewL->currentIndex().data().toString().toStdString() << std::endl;
// 	secondPageInfo();

// }
// void dialog_rtk_coor::back_triggered() {
// 	pushButtonCon->setEnabled(0);
// 	stackedWidget->setCurrentIndex(0);
// 	listViewC->setModel(new QStringListModel);
// }
// void dialog_rtk_coor::continue_triggered(){

// 	// output to startParam
// 	if (listViewC->currentIndex().data().toString().isEmpty()) {
// 		QMessageBox::warning(0, tr("Warning"), tr("No coordinate system is selected."));
// 	}
// 	else {

		
// 		for (auto node : m_listCoordinate)
// 		{
// 			if (node.first == listViewC->currentIndex().data().toString())
// 			{
// 				emit coor_(node.first, node.second);
// 				return;
// 			}
// 		}
// 		//this->close();
// 		//emit closed_();
// 		emit coor_(QString(), QString());
// 	}
// }


// void dialog_rtk_coor::firstPageInfo() {

// 	//QStandardItemModel* pModel = new QStandardItemModel();
// 	//QStandardItem* pItem = new QStandardItem;
// 	//pItem->setText("China");
// 	//pModel->appendRow(pItem);
// 	//pModel->insertRow(0, pItem);
// 	//QStandardItem* pItem1 = new QStandardItem;
// 	//pItem->setText("Australia");
// 	//pModel->appendRow(pItem1);
// 	//QStandardItem* pItem2 = new QStandardItem;
// 	//pItem->setText("Germany");
// 	//pModel->appendRow(pItem2);
// 	//QStandardItem* pItem3 = new QStandardItem;
// 	//pItem->setText("Japan");
// 	//pModel->appendRow(pItem3);
// 	////QStandardItem* pItem4 = new QStandardItem;
// 	////pItem->setText("yoyoyo");
// 	////pModel->appendRow(pItem4);
// 	//listViewL->setModel(pModel);


// 	QStringList list = coordinate::GetInstance().GetCountryList();                                //����������ʾ�б�

// 	//list.append("China");
// 	//list.append("Australia");
// 	//list.append("Germany");
// 	//list.append("Japan");
// 	//list.append("Hungary");

// 	//ʹ�������б�����������ʾģ��
// 	QStringListModel* listmodel = new QStringListModel(list);
// 	listViewL->setModel(listmodel);                   //����ģ�͵�listview��


// }

// void dialog_rtk_coor::secondPageInfo() {

// 	QString loc = listViewL->currentIndex().data().toString();

// 	QStringList list;                                //����������ʾ�б�

// 	m_listCoordinate = coordinate::GetInstance().GetCoordinateList(loc);
// 	for (auto node : m_listCoordinate)
// 	{
// 		list.append(node.first);
// 	}

// 	//if (loc == QString("China")) {
// 	//	list.append("CGCS2000 / 3-degree Gauss-Kruger CM 108E");
// 	//	list.append("CGCS2000 / 3-degree Gauss-Kruger CM 111E");
// 	//	list.append("CGCS2000 / 3-degree Gauss-Kruger CM 114E");
// 	//	list.append("CGCS2000 / 3-degree Gauss-Kruger CM 117E");
// 	//	list.append("CGCS2000 / 3-degree Gauss-Kruger CM 120E");
// 	//
// 	//}
// 	//else if (loc == QString("Australia")) {
// 	//	list.append("WGS 84 / UTM zone 54S");
// 	//	list.append("WGS 84 / UTM zone 55S");
// 	//	list.append("GDA2020 / MGA zone 55");

// 	//}
// 	//else if (loc == QString("Germany")) {		
// 	//	list.append("x");
// 	//	list.append("x");
// 	//	list.append("x");

// 	//}
// 	//else if (loc == QString("Japan")) {
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS I");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS II");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS III");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS IV");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS V");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS VI");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS VII");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS VIII");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS IX");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS X");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XI");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XII");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XIII");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XIV");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XV");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XVI");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XVII");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XVIII");
// 	//	list.append("JGD2011 / Japan Plane Rectangular CS XIX");

// 	//}
// 	//else if (loc == QString("Hungary"))
// 	//{
// 	//	list.append("HD72"); // 4237
// 	//	list.append("HD72 / EOV"); // 23700
// 	//}

// 	//ʹ�������б�����������ʾģ��
// 	QStringListModel* listmodel = new QStringListModel(list);
// 	listViewC->setModel(listmodel);                   //����ģ�͵�listview��





// }