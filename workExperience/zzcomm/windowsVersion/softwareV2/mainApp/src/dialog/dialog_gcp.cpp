#include "../include/dialog_gcp.h"
#include "../include/widget_gcp_list.h"
#include "../include/mainwindow.h"
#include <iostream>
#include <QStandardItemModel>

dialog_gcp::dialog_gcp(mainwindow* parent)
    : QWidget(parent), t(nullptr), window_(nullptr), model(nullptr)
{  
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	ui.setupUi(this);
}


dialog_gcp::~dialog_gcp()
{}

void dialog_gcp::init(mainwindow* w, std::vector<float> xyzCoor, std::vector<int> nameIndex) {
    window_ = w;
    t = new gcpTableView(this); // table
    ui.verticalLayout->addWidget(t);
    model = new QStandardItemModel(); // items in the table

    // set property ==============================================================
    //t->horizontalHeader()->hide();	// col-header
    //t->verticalHeader()->hide();	// row-header
    t->verticalHeader()->setDefaultSectionSize(30);
    t->horizontalHeader()->setStretchLastSection(false);
    //t->setEditTriggers(QTableView::NoEditTriggers);		// no edit	
    t->setSelectionBehavior(QTableView::SelectRows);		// select a row per time
    t->setSelectionMode(QTableView::SingleSelection);
    t->setAlternatingRowColors(true);                       
    //t->setShowGrid(false);                                // no grid lines
    t->setFocusPolicy(Qt::NoFocus);                         

    ui.btnAdd->setStyleSheet("QPushButton{border-image:url(:/process/process/maxnew.png);} \
        QPushButton::hover{border-image:url(:/process/process/max.png);}");
    ui.btnRemove->setStyleSheet("QPushButton{border-image:url(:/process/process/minimizenew.png);} \
        QPushButton::hover{border-image:url(:/process/process/minimize.png);}");



    //set headings ==============================================================
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("name")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("x")));
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("y")));
    model->setHorizontalHeaderItem(3, new QStandardItem(tr("z")));
    model->setHorizontalHeaderItem(4, new QStandardItem(tr("r")));
    model->setHorizontalHeaderItem(5, new QStandardItem(tr("real_x")));
    model->setHorizontalHeaderItem(6, new QStandardItem(tr("real_y")));
    model->setHorizontalHeaderItem(7, new QStandardItem(tr("real_z")));
    t->SetModel(model);

    // add initial data
    addData(xyzCoor, nameIndex);

    connect(ui.btnAdd, SIGNAL(clicked()), this, SLOT(_add()), Qt::UniqueConnection);
    connect(ui.btnRemove, SIGNAL(clicked()), this, SLOT(_remove()), Qt::UniqueConnection);
    connect(ui.btnConfirm, SIGNAL(clicked()), this, SLOT(_confirm()), Qt::UniqueConnection);
    connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(_save()), Qt::UniqueConnection);
    connect(ui.btnGcc, SIGNAL(clicked()), this, SLOT(_geographicCoordinateConversion()), Qt::UniqueConnection);
    connect(ui.btnMerge, SIGNAL(clicked()), this, SLOT(_automaticMerging()), Qt::UniqueConnection);
    connect(ui.btnEerror, SIGNAL(clicked()), this, SLOT(_eliminateOffsetErrors()), Qt::UniqueConnection);

}

void dialog_gcp::addData(std::vector<float> xyzCoor, std::vector<int> nameIndex) {
  
    // clear first
    model->removeRows(0, model->rowCount());

    for (int i = 0; i < xyzCoor.size() / 7; i++)
    {
        // name
        model->setItem(i, 0, new QStandardItem(QString("gcp_") + QString::number(nameIndex.at(i))));
        // xyzr
        model->setItem(i, 1, new QStandardItem(QString::number(xyzCoor.at(i * 7 + 0))));
        model->setItem(i, 2, new QStandardItem(QString::number(xyzCoor.at(i * 7 + 1))));
        model->setItem(i, 3, new QStandardItem(QString::number(xyzCoor.at(i * 7 + 2))));
        //QString temp(data[i * 4 + 3]);                                  // dont know why '...' follows behind (if data is vector<qstring>)
        //model->setItem(i, 4, new QStandardItem(QString::number(temp.toFloat())));
        model->setItem(i, 4, new QStandardItem(QString::number(xyzCoor.at(i * 7 + 3))));
        // real x y z
        model->setItem(i, 5, new QStandardItem(QString::number(xyzCoor.at(i * 7 + 4))));
        model->setItem(i, 6, new QStandardItem(QString::number(xyzCoor.at(i * 7 + 5))));
        model->setItem(i, 7, new QStandardItem(QString::number(xyzCoor.at(i * 7 + 6))));
    }

    t->selectRow(0);
    update();
}


void dialog_gcp::closeEvent(QCloseEvent* event)
{
}


// read from txt

//std::vector<QString> dialog_gcp::readTxt()
//{
//    // return vector
//    std::vector<QString> strs;
//
//    QFile file(filePath);
//    if (!file.open(QIODevice::ReadOnly))
//    {
//        return strs;
//    }
//
//    int count = 0;
//    while (!file.atEnd())
//    {
//        
//        QByteArray line = file.readLine();
//        if (count != 0 && count%2==0) {
//            QString temp_str(line);
//            QStringList list = temp_str.split(" ");
//            strs.push_back(list[1]);
//            strs.push_back(list[2]);
//            strs.push_back(list[3]);
//            strs.push_back(list[4]);
//
//            //std::cout << "count: "<<count   <<"  " << std::to_string(list[1].toFloat()) << std::endl;
//            //std::cout << "count: " << count << "  " << list[2].toStdString() << std::endl;
//            //std::cout << "count: " << count << "  " << list[3].toStdString() << std::endl;
//            //std::cout << "count: " << count << "  " << list[4].toStdString() << std::endl;
//
//        }
//        count++;
//
//    }
//
//
//    file.close();
//
//    return strs;
//
//}


void dialog_gcp::_save() {
    t->selectRow(0);
    update();
    QApplication::processEvents(); //wait

    emit(_saved());
    window_->log->set_log(log_type::Info, QString(tr("All input saved")).toStdString(), window_->getLogWidget());
    
}

void dialog_gcp::_add() {

    window_->log->set_log(log_type::Info, QString(tr("Left click on the point cloud, you have chosed, to choose a GCP.")).toStdString(), window_->getLogWidget());
    emit(_chooseGcp());

    //t->getModel()->appendRow(new QStandardItem(QString("gcp_") + QString::number(t->getModel()->rowCount()+1)));
    //window_->log->set_log(log_type::Info, QString(tr("Added a new GCP.")).toStdString(), window_->getLogWidget());
}

void dialog_gcp::_remove() {
    //std::cout<<t->currentIndex().row()<<std::endl;

    QMessageBox::StandardButton check;
    check = QMessageBox::question(this, QString(tr("Confirmation")), QString(tr("Delete this GCP?")), QMessageBox::Yes | QMessageBox::No);
    if (check == QMessageBox::Yes) {

        t->getModel()->removeRow(t->currentIndex().row());
        emit(_delGcp(t->currentIndex().row()));
        window_->log->set_log(log_type::Info, QString(tr("Removed a GCP.")).toStdString(), window_->getLogWidget());
    }  
}

void dialog_gcp::_confirm() {

    //Please input the real_x/real_y/real_z coordinates now.
    window_->log->set_log(log_type::Info, QString(tr("All GCPs are confirmed.")).toStdString(), window_->getLogWidget());
    emit(_confirmGcp());
}

void dialog_gcp::_geographicCoordinateConversion()
{
    QMessageBox::StandardButton check;
    check = QMessageBox::information(this, QString(tr("Confirmation")), QString(tr("Please make sure at least 3 GCPs have inputted real x,y,z coordinates, saved and confirmed before proceed.")), QMessageBox::Yes | QMessageBox::No);
    if (check == QMessageBox::Yes) {
        
        window_->log->set_log(log_type::Info, QString(tr("Geographic Coordinate Conversion starts.")).toStdString(), window_->getLogWidget());
        emit(_gcc());
    }

}

void dialog_gcp::_automaticMerging()
{     
    window_->log->set_log(log_type::Info, QString(tr("Please make sure at least 3 identical GCPs (no need real x/y/z coordinates) have appeared in the 2 point clouds later on.")).toStdString(), window_->getLogWidget());
    emit(_autoMerge());
}

void dialog_gcp::_eliminateOffsetErrors()
{

    QMessageBox::StandardButton check;
    check = QMessageBox::information(this, QString(tr("Confirmation")), QString(tr("Please make sure all GCPs have inputted real x,y,z coordinates and saved. \nNeed to input the first RTK coordinates(realx,y,z) now.")), QMessageBox::Yes | QMessageBox::No);
    if (check == QMessageBox::Yes) {
        emit(_eliError());
        //window_->log->set_log(log_type::Info, QString(tr("Eliminate Offset Errors starts.")).toStdString(), window_->getLogWidget());
    }
}


