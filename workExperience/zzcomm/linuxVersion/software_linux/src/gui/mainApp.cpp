#include "../include/include_gui/mainApp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <QLabel>
#include <QPropertyAnimation>
#include <QListWidget>
#include <QScrollBar>
#include <QDropEvent>
#include <QMimeData>
#include <QMutex>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QColorDialog>
#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QFormLayout>
#include <QScrollArea>

// //#include "../include/gcp.h"
// //#include "../include/gcp_transformation.h"

#include "yaml-cpp/yaml.h"


mainApp::mainApp(QWidget* parent)
    : QMainWindow(parent)
    ,rootCore(nullptr)
    ,availableForBagRunning(false)
    ,availableForColourRunning(true)
    ,lang(nullptr)
    ,settings(nullptr)
    ,sideMenuClosing(false)
    ,animationSideMenu(new QPropertyAnimation)
    ,processDetailClosing(false)
    ,start_process(nullptr)
    ,new_process(nullptr)
    ,new_process_name(10)
    ,reminderBeforeRunningBag(true)
    ,versionNo(QString())

{
    this->setWindowTitle(QString("company product App"));
    this->setWindowIcon(QIcon(":/logo/process/logo.png"));

    setupUi(this);
    setToolBar();
    init();
    setConnection();

}

mainApp::~mainApp()
{
    std::cout << "main gui down" << std::endl;
}
void mainApp::closeEvent(QCloseEvent* event) {

    QMessageBox::StandardButton check;
    check = QMessageBox::question(this, QString(tr("Warning")), QString(tr("Close this application?")), QMessageBox::Yes | QMessageBox::No);
    if (check == QMessageBox::Yes) {

        // // clear all process 
        // if (rootCore != NULL) {
        //     disconnect(rootCore, 0, 0, 0);
        //     rootCore->kill();
        //     rootCore = NULL;
        //     std::cout << "root roscore x " << std::endl;
        // }

        
        // std::cout << "all processes closed" << std::endl;


        // kill roscore services
        //QProcess::startDetached("killall roscore");
        QProcess::startDetached("killall algorithm1plus");
        QProcess::startDetached("killall algorithm1plusCuda");
        QProcess::startDetached("killall gcpCp");
        QProcess::startDetached("killall colorization");
        QProcess::startDetached("killall merge");
        QProcess::startDetached("killall mp4_merge");
        QProcess::startDetached("killall algorithm1_nrt");
        QProcess::startDetached("killall algorithm1_ba");
        // QProcess::startDetached("killall algorithm1_nrt_gcpEE");
        // QProcess::startDetached("killall algorithm1_nrt_rtk");

        //QProcess::startDetached("kill -15 app's name");

        login_page.close(); // close login page

        event->accept();
        std::cout << "all processes closed" << std::endl;
    }
    else {
        Ui::mainAppClass::statusBar->showMessage(tr(""));
        event->ignore();
        //event->accept();
    }

}
void mainApp::closeApp(){
    this->close();
}
void mainApp::setToolBar()
{
    int index = 0;

    QWidget* spacer = new QWidget(toolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer); 

    toolBar->addAction(actionPoint);
    addSpacer(index++);
    toolBar->addAction(actionSegment);
    addSpacer(index++);
    toolBar->addAction(actionSampling);
    addSpacer(index++);
    toolBar->addAction(actionMesh);
    addSpacer(index++);
    toolBar->addAction(actionPhoto);
    addSpacer(index++);
    toolBar->addAction(actionScreenshot);
    addSpacer(index++);
    toolBar->addSeparator();
    addSpacer(index++);
    toolBar->addAction(actionUp);
    addSpacer(index++);
    toolBar->addAction(actionDown);
    addSpacer(index++);
    toolBar->addAction(actionLeft);
    addSpacer(index++);
    toolBar->addAction(actionRight);
    addSpacer(index++);
    toolBar->addAction(actionFront);
    addSpacer(index++);
    toolBar->addAction(actionBack);
    addSpacer(index++);
    toolBar->addSeparator();
    addSpacer(index++);
    toolBar->addAction(actionEDL);
    addSpacer(index++);
    toolBar->addAction(actionCenterPivot);
    
    // add spacer for toolBar
    QWidget* spacer1 = new QWidget(toolBar);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer1);
    toolBar->addAction(actionManual);
    toolBar->addAction(actionAbout);
}
void mainApp::addSpacer(int i)
{
    QString name = QString("space%1").arg(i);
    QWidget* s = new QWidget(toolBar);
    s->setMinimumWidth(8);
    s->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    toolBar->addWidget(s);
}
void mainApp::setConnection()
{
    // for login part ==============================================================================================================================
    connect(&login_page, SIGNAL(enter_signal()), this, SLOT(displayMainwindow()), Qt::UniqueConnection); // from login page (activated successfully)
    connect(&login_page.login_checker, SIGNAL(timer_signal(QString)), this, SLOT(slot_timeRemaining(QString)));
    connect(&login_page, SIGNAL(close_()), this, SLOT(closeApp()),Qt::UniqueConnection);
    connect(&login_page, SIGNAL(languageChanged_signal(int)), this, SLOT(slot_loginPgLanguage(int)));
    //==============================================================================================================================================
 
    // arrow button animation
    // left side menu buttons ================================
    connect(leftSideMenuBtn, SIGNAL(clicked()), this, SLOT(_sideMenuButton()), Qt::UniqueConnection);
    connect(animationSideMenu, &QVariantAnimation::valueChanged, this, &mainApp::_sideMenuAnimation);
    //create animation for side menu
    animationSideMenu->setTargetObject(widgetLeftSide);
    animationSideMenu->setPropertyName("geometry"); 
    animationSideMenu->setDuration(100);
    leftSideMenuBtn->setIcon(QIcon(":/icons/icons/menu_left.png"));   
    // show process details
    connect(btnProcessDetails, SIGNAL(clicked()), this, SLOT(_processDetailButton()), Qt::UniqueConnection);
    btnProcessDetails->setIcon(QIcon(":/icons/icons/menu_down.png"));
    // ================================================================================================

    // tab widget =============================================
    connect(btnAddprocess, SIGNAL(clicked()), this, SLOT(_addProcess()), Qt::UniqueConnection);  
        
    // tool bar buttons ================================
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(on_actionAbout_triggered()), Qt::UniqueConnection);
    connect(actionSetting, SIGNAL(triggered()), this, SLOT(_setting_triggered()), Qt::UniqueConnection);
}


void mainApp::init() {

    setAcceptDrops(true);
    //auto handle = CreateSemaphoreA(NULL, 4, 4, "company");

    // set left side menu
    widgetEmpty->hide(); // add spacer for left side menu
    widgetProcessList->hide(); // show/hide processes

    hideModelProperty(true); // hide property menu first
    widgetGcp->hide();

    // set statusbar color
    Ui::mainAppClass::statusBar->setStyleSheet("color: #d9d7d7");

    show_hide_Buttons(false); // hide function buttons
}

// void mainApp::paintEvent(QPaintEvent*)
// {
//     //widgetBottom->setGeometry(0, this->height() * 0.6, this->width(), this->height() * 0.4);
//     //widgetMain->setGeometry(0, 0, this->width(), this->height());
// }

void mainApp::dragEnterEvent(QDragEnterEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void mainApp::dropEvent(QDropEvent* e) { // if drag a folder = no extension = no '.'
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
    //std::cout << e->mimeData()->urls().at(0).toLocalFile().toStdString() << std::endl;
    //std::cout << e->mimeData()->urls().at(0).toString().toStdString() << std::endl;

    // bool check_bag = false;
    const QString& fileName = e->mimeData()->urls().at(0).toLocalFile();
    // const std::string& ext = file_system::extension(fileName.toStdString(), true);
    // if (ext == "bag")
    //     check_bag = true;
    //std::cout<<fileName.toStdString()<<std::endl;
    // if (check_bag) { // bag file
        tabWidget->setCurrentWidget(tabProcess);
        btnAddprocess->click();
        start_process->slamBag = fileName;
        start_process->lineEditSlam->setText(fileName);
        // set output name
        start_process->lineEditSlamName->setText(getFileName(fileName)); // slam
        start_process->lineEditSavingPath->setText(getFilePath(fileName));

        // add a function to find the product.yaml automatically  
        // ********************************************* TO BE DONE *************************************
        start_process->update();
    //}   

}

void mainApp::hideModelProperty(bool b) {
    if (b) {
        widgetRightSide->hide();
        widgetModel->hide();
    }
    else {
        widgetRightSide->show();
        widgetModel->show();
    }
}

void mainApp::show_hide_Buttons(bool b) {
    actionSave->setEnabled(b);
    actionDelete->setEnabled(b);
    actionPoint->setEnabled(b);
    actionSegment->setEnabled(b);
    actionSampling->setEnabled(b);
    actionMesh->setEnabled(b);
    actionPhoto->setEnabled(b);
    
    // for linux
    actionOpen->setEnabled(b);
    actionScreenshot->setEnabled(b);
    actionUp->setEnabled(b);
    actionDown->setEnabled(b);
    actionLeft->setEnabled(b);
    actionRight->setEnabled(b);
    actionFront->setEnabled(b);
    actionBack->setEnabled(b);
    actionEDL->setEnabled(b);
    actionCenterPivot->setEnabled(b);
    actionManual->setEnabled(b);
    actionSetting->setEnabled(!b);

}

void mainApp::on_actionAbout_triggered() { 
    QString title = QMessageBox::tr(
        R"(<p align="center"><span style="font-style:italic;">Version Number:</span><span style="font-style:arial;">&nbsp;&nbsp;%1</span></p>)"
    ).arg(versionNo);
    QString text = QMessageBox::tr(
        "<p><strong>Time remaining : </strong><br> %1 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<t></p>"
        "<p>company's Website:<br>"
        "<a href=\"https://www.company.com/\">https://www.company.com/</a>"
    ).arg(login_page.login_checker.maintitleQstr);

    QMessageBox::about(this, QString(tr("About")), title + text);
}

// ================================================================ Process Bag =========================================================================
// ======================================================================================================================================================
void mainApp::_addProcess() {

    if (!start_process) {
        start_process = new widget_startProcess(this);

        connect(start_process, SIGNAL(_started()), this, SLOT(_startProcess()), Qt::UniqueConnection);
        connect(start_process, SIGNAL(_close()), this, SLOT(_startProcessClose()), Qt::UniqueConnection);
        connect(start_process, SIGNAL(_closeNonEmpty()), this, SLOT(_startProcessClose()), Qt::UniqueConnection);
       
        // reminder before running bag
        if (reminderBeforeRunningBag) {
            QCheckBox* cb = new QCheckBox(this);
            cb->setText(tr("Do not appear again for this time"));
            cb->setStyleSheet("QCheckBox{font-size:16px; font:bold Ubuntu; }");
            //cb->setChecked(1);
            cb->setLayoutDirection(Qt::RightToLeft);
            QMessageBox *msgbox = new QMessageBox(this);
            msgbox->setText(tr("When running the bag, ensure no other applications are running in the back. \nThis is because the processing stage requires a relatively large amount of pc's usage.\n"));
            msgbox->setIcon(QMessageBox::Icon::Information);
            msgbox->addButton(QMessageBox::Ok);
            msgbox->setDefaultButton(QMessageBox::Ok);
            msgbox->setCheckBox(cb);
            msgbox->setStyleSheet("QMessageBox{font-size:16px; }");

            QObject::connect(cb, &QCheckBox::stateChanged, [this](int state) {
                if (static_cast<Qt::CheckState>(state) == Qt::CheckState::Checked) {
                    reminderBeforeRunningBag = false;
                }
                });

            msgbox->exec();
        }
    
        start_process->show();
    }
}

void mainApp::_startProcess() {    
    // start to pass data from start_process  ==========================================
    // add a new process to the side menu                    
    new_process = new widget_newProcess(this); 
    new_process->name = QString::number(new_process_name);
    new_process_name++;
    // set up 
    new_process->init(); // set up
    start_process->setParam(start_process->comboBoxType->currentIndex(), new_process); // pass param + type

    //new_process->test();

    // colourisation: no need bag. Point cloud + mp4 only
    if (new_process->getType() == process_type::Colourisation) {

        if (!processDetailClosing) {
            btnProcessDetails->click();
            processDetailClosing = true;
        }

        if (availableForColourRunning) { // check for first time only
            availableForColourRunning = false;
            if (!availableForBagRunning && bagFiles.empty()) {
                availableForBagRunning = true;
            }
        }

        new_process->labelbagName->setText("Colourisation: ");
        new_process->lineEditBagName->setText(getFileName(start_process->colorFilePc));    
        new_process->setViewProcess();

        new_process->set_colour_mp4(start_process->colorFileMp4);
        new_process->set_colour_pc(start_process->colorFilePc);       
    }
    else {
        // add bag ===================================================
        // click 'down' button if bag's list = empty (show the widget)
        if (!processDetailClosing) {
            btnProcessDetails->click();
            processDetailClosing = true;
        }
        if (bagFiles.empty() && !availableForBagRunning && availableForColourRunning) { // cehck for first time only
            availableForBagRunning = true;
        }
        // add to the bag file list (full path)
        bagFiles.emplace_back(start_process->slamBag);
        //===========================================================

        if (new_process->getType() == process_type::RTK) {
            new_process->setRtkFile(start_process->rtkFile);
            std::cout << new_process->getRtkFile().toStdString() << std::endl;
            std::cout << "^rtk file^" << std::endl;
        }
        if (new_process->getType() == process_type::GCP) {
            new_process->gcpTxtFile = start_process->gcpFile;
            std::cout << new_process->gcpTxtFile.toStdString() << std::endl;
            std::cout << "^gcp file^" << std::endl;
        }

        new_process->setBag(bagFiles.at(bagFiles.size() - 1)); // add bag path + set view_process
 
    }


    // =================================================================================
    // add to the scroll area
    processListLayout->addWidget(new_process);
    processListLayout->setAlignment(Qt::AlignTop);
    scrollAreaProcess->setWidget(processList);

    // store all processes
    newProcess_list.emplace_back(new_process);
    
    // end start process dialog
    start_process->close();
    start_process = nullptr;

    // run all new bags - line up
    checkAllNewProcess();
}

void mainApp::_startProcessClose() {
    if (start_process) {
        start_process->close();
        start_process = nullptr; // kill the start_process
        std::cout << "start_process closed" << std::endl;
    }  
}

void mainApp::checkAllNewProcess() {

    // set no. of waiting =======================
    int waiting = 0;
    for (widget_newProcess* np : newProcess_list) {
        if (np->getStatus() == running_status::running || np->getStatus() == running_status::finished) {
            waiting++;
        }
        else {
            // add to the 'detail'
            if (newProcess_list.size() - waiting == 1 && newProcess_list.size() == 1)
                lineEditWaitingBags->setText(QString::number(0));
            else
                lineEditWaitingBags->setText(QString::number(newProcess_list.size() - waiting));
            break;
        }
    }
    // ==========================================
    if (availableForBagRunning)  {
        int checkForRemaining = 0;
        for (widget_newProcess* np : newProcess_list) {
            if (np->getStatus() == running_status::running || np->getStatus() == running_status::finished) {
                checkForRemaining++;
            }
            else {
                // add to the 'detail'
                if (np->getType() == process_type::Slam || np->getType() == process_type::GCP || np->getType() == process_type::RTK) {
                    //if (checkForRemaining == 0)
                    //    lineEditRunningBag->setText(getFileName(bagFiles.at(0)));
                    //else
                    //    lineEditRunningBag->setText(getFileName(bagFiles.at(checkForRemaining)));
                    lineEditRunningBag->setText(np->lineEditBagName->text());
                    lineEditRunningBag->setStyleSheet("QLineEdit{font-size:16px}");
                    if (newProcess_list.size() - checkForRemaining == 1 && newProcess_list.size() == 1) // first bag || 1 bag only
                        lineEditWaitingBags->setText(QString::number(0));
                    else
                        lineEditWaitingBags->setText(QString::number(newProcess_list.size() - checkForRemaining));
                    lineEditWaitingBags->setStyleSheet("QLineEdit{font-size:16px}");
                    // run process
                    np->run();
                    break;
                }
                else if (np->getType() == process_type::Colourisation) {
                    std::cout << np->lineEditBagName->text().toStdString() << std::endl;
                    lineEditRunningBag->setText(np->lineEditBagName->text());
                    lineEditRunningBag->setStyleSheet("QLineEdit{font-size:16px}");
                    if (newProcess_list.size() - checkForRemaining == 1 && newProcess_list.size() == 1) // first bag || 1 bag only
                        lineEditWaitingBags->setText(QString::number(0));
                    else
                        lineEditWaitingBags->setText(QString::number(newProcess_list.size() - checkForRemaining));
                    lineEditWaitingBags->setStyleSheet("QLineEdit{font-size:16px}");


                    np->runColour();
                    break;
                }
                //else if (np->getType() == process_type::RTK) {

                //}
                //else if (np->getType() == process_type::GCP) {

                //}
              
            }
        }

        // all finished
        if (checkForRemaining == (int)newProcess_list.size()) {
            lineEditRunningBag->setText("");
            lineEditWaitingBags->setText(QString::number(0));
        }
    }
}
// ======================================================================================================================================================
// ======================================================================================================================================================

// button animation =============================================
void mainApp::_sideMenuButton() {
    
    QRect te2_1 = widgetLeftSide->geometry();
    animationSideMenu->setStartValue(te2_1);

    if (!sideMenuClosing) {
        QRect te2_2(0, te2_1.y(), leftSideMenuBtn->width(), te2_1.height());
        animationSideMenu->setEndValue(te2_2);
        sideMenuClosing = true;      
        leftSideMenuBtn->setIcon(QIcon(":/icons/icons/menu_right.png"));

        widgetEmpty->show(); // for occupying
        widgetLeftSideItems->hide();
    }
    else {       
        QRect te2_2(0, te2_1.y(), 350, te2_1.height());
        animationSideMenu->setEndValue(te2_2);
        sideMenuClosing = false;
        leftSideMenuBtn->setIcon(QIcon(":/icons/icons/menu_left.png"));

        widgetEmpty->hide();
        widgetLeftSideItems->show();
    }
      
    animationSideMenu->setEasingCurve(QEasingCurve::InOutCirc);
    animationSideMenu->start();
}
void mainApp::_sideMenuAnimation(const QVariant& val) {
    widgetLeftSide->setMaximumWidth(val.toRect().width());
}
void mainApp::_processDetailButton() {

    if (!processDetailClosing) {
        
        btnProcessDetails->setIcon(QIcon(":/icons/icons/menu_up.png"));
        widgetProcessList->show();
        processDetailClosing = true;
    }
    else {

        btnProcessDetails->setIcon(QIcon(":/icons/icons/menu_down.png"));
        widgetProcessList->hide();
        processDetailClosing = false;
    }

}
// =============================================================
QString mainApp::getFileName(QString a) {
    std::string filename = a.toStdString().substr(a.toStdString().find_last_of("/") + 1);
    return QString::fromStdString(filename);
}
QString mainApp::getFilePath(QString a) {
    QFileInfo p(a);
    return p.path();
}
bool mainApp::checkFilePath(QString a) {
    
    QString pathtocreate = a;
    //pathtocreate = pathtocreate.replace("/", "\\");
    QStringList components = pathtocreate.split("/");
    foreach(QString a, components) {
        if (!QString::fromStdString(a.toStdString()).contains(QRegExp("^[A-Za-z0-9_:.-]*$"))) {
            QMessageBox::warning(this, QString(tr("Warning")), QString(tr("Only letters, numbers, underscores and dashes are allowed in the path.")));
            std::cout << a.toStdString() << std::endl;
            std::cout << "**********" << std::endl;
            return false;
        }
    }
    return true;
}
void mainApp::startTimer() {
    w.Reset();
    w.Start();

}
std::vector<std::string> mainApp::getTimer() {
    std::vector<std::string> list;

    w.Stop();
    double time_used = w.GetElapsed().count();
    double time_used_in_s = time_used/1.00e9;

    list.emplace_back(std::to_string(time_used_in_s)); // to accumulate all seconds.

    char* ch = new char[sizeof(time_used_in_s)];
    std::string res = "";
    if (time_used_in_s < 60){ // in s
        sprintf(ch, "%.2f", time_used_in_s);
        res = ch;
        res += "s";

    }else if(time_used_in_s > 60 && time_used_in_s < 60*60){ // in min
        time_used_in_s = time_used_in_s/60;
        sprintf(ch, "%.2f", time_used_in_s);
        res = ch;
        res += "min";
    }else{ // in hr
        time_used_in_s = time_used_in_s/60/60;
        sprintf(ch, "%.2f", time_used_in_s);
        res = ch;
        res += "hr";
    }

    list.emplace_back(res); // each stage/function
    // return res;
    return list;
}
void mainApp::stopTimer(){ w.Reset(); }
void mainApp::showLoginPage() {

    login_page.show();
}
void mainApp::displayMainwindow() {

    this->show();
    login_page.close();
}
void mainApp::slot_timeRemaining(QString errstr)
{

    if (errstr == QString("S6")) // valid duration
    {
        //std::cout << "remaining: " <<login_page.login_checker.maintitleQstr.toStdString() << std::endl;
    }
    else if (errstr == QString("S3"))
    {
        //if (processGui->isVisible())processGui->quitWhenRunning();
        this->hide();
        login_page.show();
        login_page.datasetbtn->setStyleSheet("QPushButton{border-image: url(:/logo/process/icon.png);border:none;color:rgb(38, 38, 38); }");
        login_page.datasetlabel->setText(QString(tr("System time error. Please update the system time. ")));
    }
    else
    {
        //if (processGui->isVisible())processGui->quitWhenRunning();
        this->hide();
        login_page.show();
        login_page.datasetbtn->setStyleSheet("QPushButton{border-image: url(:/logo/process/icon.png);border:none;color:rgb(38, 38, 38); }");
        login_page.datasetlabel->setText(QString(tr("License expired. Please contact the supplier.")));
    }
}
void mainApp::slot_loginPgLanguage(int x)
{
    switch (x) {
    case 1:
        std::cout << " changed to eng \n";
        lang->changeLanguage(Language::en_);
        retranslateUi(this);

        languageStorage(1);
        break;
    case 2:
        std::cout << " changed to chi \n";
        lang->changeLanguage(Language::zh_);
        retranslateUi(this);

        languageStorage(2);
        break;
    default:
        QMessageBox::information(this, QString(tr("Error")), QString(tr("Failed to change the language")));
        break;
    }
}
void mainApp::languageStorage(int a) {

    //QString langFile = "..\\Lib\\company\\language.yaml";

    //QFile fi(langFile);
    //if (!fi.exists()) { // if file is not existing
    //    QFile create(langFile);
    //    create.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);

    //    YAML::Node write = YAML::LoadFile(langFile.toStdString());
    //    std::ofstream fout(langFile.toStdString());
    //    write["language"]["type"] = 0;
    //    fout << write;
    //}
    

    YAML::Node config = YAML::LoadFile("../Lib/company/language.yaml");
    switch (a)
    {
    case 1:
        config["language"]["type"] = 1;
        break;
    case 2:
        config["language"]["type"] = 2;
        break;
    default:
        break;
    }

    QString output = "../Lib/company/language_temp.yaml";
    QFile file(output); // remove if exists
    file.remove();
    std::ofstream fout(output.toStdString());
    fout << config << std::endl; // dump it back into the file
    fout.close();
}
int mainApp::getLanguage() {
    QString output = "../Lib/company/language_temp.yaml";
    QFile file(output); 
    if (file.exists()) {
        YAML::Node config = YAML::LoadFile(output.toStdString());
        int type = config["language"]["type"].as<int>();
        std::cout << "language type: " << std::to_string(type) << std::endl;
        return type;
    }
    else {
        return 1;
    }

}

void mainApp::_setting_triggered() { // change background color for now

    if (!settings) {
        settings = new mainwindowSettingDialog(this);
        connect(settings, SIGNAL(_closed()), this, SLOT(_setting_closed()), Qt::UniqueConnection);
        settings->show();
    }
    else {
        settings->close();
        settings = nullptr;
    }
}
void mainApp::_setting_closed() { // change background color for now

    if (!settings) {
    }
    else {
        settings->close();
        settings = nullptr;
        std::cout << "setting closed" << std::endl;
    }
}
void mainApp::removeNewProcess(QString n) {

    //std::cout <<"Project's name: " << name.toStdString() << std::endl;
    for (std::size_t i = 0; i < getNewProcessList().size(); i++) {
        QString temp_name = getNewProcessList().at(i)->getName();
        if (temp_name == n) {
            std::cout << "found at index of: " << i << std::endl;
            std::cout << "total size before: " << getNewProcessList().size() << std::endl;

            processListLayout->removeWidget(getNewProcessList().at(i)); // delete from ui
            delete getNewProcessList().at(i);
            //getNewProcessList().at(i)->hide();

            std::vector<widget_newProcess*>::iterator it = newProcess_list.begin(); // delete from vector
            std::advance(it, i);
            newProcess_list.erase(it);
   
            
            std::cout << "total size after: " << getNewProcessList().size() << std::endl;
            //processListLayout->update();
            //processList->update();
            //scrollAreaProcess->update();
            //this->update();
            return;
        }
    }

}