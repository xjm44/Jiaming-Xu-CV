#include "../include/include_gui/mainApp.h"

#include <iostream>
#include <string>
#include <QLabel>
#include <QPropertyAnimation>
#include <QListWidget>
#include <QScrollBar>
#include <QDropEvent>
#include <QMimeData>
#include <boost/winapi/semaphore.hpp>
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

#include <easy3d/core/surface_mesh.h>
#include <easy3d/core/graph.h>
#include <easy3d/core/point_cloud.h>
#include <easy3d/core/poly_mesh.h>
#include <easy3d/core/random.h>
#include <easy3d/core/surface_mesh_builder.h>
#include <easy3d/renderer/camera.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/renderer/clipping_plane.h>
#include <easy3d/renderer/drawable_lines.h>
#include <easy3d/renderer/drawable_points.h>
#include <easy3d/renderer/key_frame_interpolator.h>
#include <QScrollArea>
#include <easy3d/renderer/drawable_triangles.h>
#include <easy3d/renderer/manipulator.h>
#include <easy3d/renderer/transform.h>
#include <easy3d/fileio/point_cloud_io.h>
#include <easy3d/fileio/graph_io.h>
#include <easy3d/fileio/surface_mesh_io.h>
#include <easy3d/fileio/poly_mesh_io.h>
#include <easy3d/fileio/ply_reader_writer.h>
#include <easy3d/fileio/point_cloud_io_ptx.h>
#include <easy3d/fileio/translator.h>
#include <easy3d/algo/point_cloud_normals.h>
#include <easy3d/algo/surface_mesh_components.h>
#include <easy3d/algo/surface_mesh_topology.h>
#include <easy3d/algo/surface_mesh_triangulation.h>
#include <easy3d/algo/surface_mesh_tetrahedralization.h>
#include <easy3d/algo/surface_mesh_subdivision.h>
#include <easy3d/algo/surface_mesh_geodesic.h>
#include <easy3d/algo/surface_mesh_stitching.h>
#include <easy3d/algo/surface_mesh_enumerator.h>
#include <easy3d/algo/surface_mesh_polygonization.h>
#include <easy3d/algo/surface_mesh_geometry.h>
#include <easy3d/algo_ext/surfacer.h>
#include <easy3d/algo/delaunay_2d.h>
#include <easy3d/algo/delaunay_3d.h>
#include <easy3d/util/logging.h>
#include <easy3d/util/file_system.h>
#include <easy3d/util/line_stream.h>
#include <easy3d/util/version.h>

#include "../include/include_easy3d/dialog/dialog_snapshot.h"
//#include "../include/easy3d_inc/dialog/dialog_point_cloud_normal_estimation.h"
#include "../include/include_easy3d/dialog/walk_through.h"
#include "../include/include_easy3d/dialog/dialog_walk_through.h"

//#include "../include/gcp.h"
//#include "../include/gcp_transformation.h"
#include "../include/include_gui/dialog_manual.h"

#include "yaml-cpp/yaml.h"
#include <QStandardPaths>

using namespace easy3d;

easy3d::StopWatch w;

mainApp::mainApp(QWidget* parent)
    : QMainWindow(parent),
    sideMenuClosing(false), processDetailClosing(false),
    animationSideMenu(new QPropertyAnimation),
    new_process(nullptr), start_process(nullptr), start_folder(nullptr),
    availableForBagRunning(false), availableForColourRunning(true),
    samp_(nullptr),
    pointClick(false),
    photo(nullptr),
    mesh(nullptr),
    lang(nullptr),
    settings(nullptr),
    reminderBeforeRunningBag(true),
    new_process_name(10),
    versionNo(QString())
    , cmdAvailable(false)
    , cmd_(nullptr)
    , gcpManual(false)
    //, dialogGcp(nullptr)
    //,man(this)
{
    this->setWindowTitle(QString("company product App"));
    this->setWindowIcon(QIcon(":/company/process/logo.png"));

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
    check = QMessageBox::question(0, QString(tr("Warning")), QString(tr("Close this application?")), QMessageBox::Yes | QMessageBox::No);
    if (check == QMessageBox::Yes) {
        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished();                    
        QString str1 = process.readAllStandardOutput();

        if (str1.contains("roscore.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("roscore.exe"));
        }
        if (str1.contains("rosmaster.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("rosmaster.exe"));
        }
        if (str1.contains("rosout.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("rosout.exe"));
        }
        if (str1.contains("python.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("python.exe"));
        }
        //if (str1.contains("algorithm1_nrt_rtk.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk.exe"));
        //}
        //if (str1.contains("algorithm1_nrt_gcpEE.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_gcpEE.exe"));
        //}
        if (str1.contains("algorithm1_nrt_comb.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_comb.exe"));
        }
        if (str1.contains("algorithm1plus.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1plus.exe"));
        }
        if (str1.contains("algorithm1plusCuda.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1plusCuda.exe"));
        }
        if (str1.contains("merge.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
        }
        if (str1.contains("mp4_join.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("mp4_join.exe"));
        }
        if (str1.contains("colorize.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("colorize.exe"));
        }
        if (str1.contains("gcpCp.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("gcpCp.exe"));
        }
        if (str1.contains("algorithm1_ba.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_ba.exe"));
        }
        process.close();


        delete viewer_;

        event->accept();
        std::cout << "all processes closed" << std::endl;
    }
    else {
        Ui::mainAppClass::statusBar->showMessage(tr(""));
        event->ignore();
        //event->accept();
    }
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
    //connect(&login_page, SIGNAL(enter_signal()), this, SLOT(displayMainwindow()), Qt::UniqueConnection); // from login page (activated successfully)
    //connect(&login_page.login_checker, SIGNAL(timer_signal(QString)), this, SLOT(slot_timeRemaining(QString)));
    //connect(&login_page, SIGNAL(languageChanged_signal(int)), this, SLOT(slot_loginPgLanguage(int)));

    connect(&login_page_one, SIGNAL(languageChanged_signal(int)), this, SLOT(slot_loginPgLanguage(int)));
    connect(&login_page_one.login_checker_one, SIGNAL(timer_signal(int)), this, SLOT(slot_timeRemaining(int)));
    connect(&login_page_one, SIGNAL(enter_signal()), this, SLOT(displayMainwindow()), Qt::UniqueConnection); // from login page (activated successfully)
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
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(_open_triggered()), Qt::UniqueConnection);
    connect(actionSave, SIGNAL(triggered()), this, SLOT(_save_triggered()), Qt::UniqueConnection);
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(_del_triggered()), Qt::UniqueConnection);
    connect(actionEDL, SIGNAL(toggled(bool)), this, SLOT(_edl_triggered(bool)), Qt::UniqueConnection);
    connect(actionCenterPivot, SIGNAL(toggled(bool)), this, SLOT(_centerPivot_triggered(bool)), Qt::UniqueConnection);
    actionCenterPivot->toggle();
    connect(actionUp, SIGNAL(triggered()), this, SLOT(_up_triggered()), Qt::UniqueConnection);
    connect(actionDown, SIGNAL(triggered()), this, SLOT(_down_triggered()), Qt::UniqueConnection);
    connect(actionFront, SIGNAL(triggered()), this, SLOT(_front_triggered()), Qt::UniqueConnection);
    connect(actionBack, SIGNAL(triggered()), this, SLOT(_back_triggered()), Qt::UniqueConnection);
    connect(actionLeft, SIGNAL(triggered()), this, SLOT(_left_triggered()), Qt::UniqueConnection);
    connect(actionRight, SIGNAL(triggered()), this, SLOT(_right_triggered()), Qt::UniqueConnection);
    connect(actionSetting, SIGNAL(triggered()), this, SLOT(_setting_triggered()), Qt::UniqueConnection);
    connect(actionSampling, SIGNAL(triggered()), this, SLOT(_sampling_triggered()), Qt::UniqueConnection);

    connect(actionPoint, SIGNAL(toggled(bool)), this, SLOT(_point_triggered(bool)), Qt::UniqueConnection);
    connect(actionSegment, SIGNAL(toggled(bool)), this, SLOT(_trim_triggered(bool)), Qt::UniqueConnection);
    connect(actionScreenshot, SIGNAL(triggered()), this, SLOT(_screenshot_triggered()), Qt::UniqueConnection);
    connect(actionMesh, SIGNAL(triggered()), this, SLOT(_mesh_triggered()), Qt::UniqueConnection);
    connect(actionPhoto, SIGNAL(triggered()), this, SLOT(_photo_triggered()), Qt::UniqueConnection);

    connect(actionManual, SIGNAL(triggered()), this, SLOT(_manual_triggered()), Qt::UniqueConnection);

}


void mainApp::init() {

    setAcceptDrops(true);
    auto handle = CreateSemaphoreA(NULL, 4, 4, "company");

    // set left side menu
    widgetEmpty->hide(); // add spacer for left side menu
    widgetProcessList->hide(); // show/hide processes

    // set opengl window
    viewer_ = new PaintCanvas(this);

    //// to be deleted
    //viewer_->walkThrough()->set_path_visible(true);
    //viewer_->walkThrough()->set_cameras_visible(true);
    //viewer_->adjustSceneRadius();
    //// ==========

    QHBoxLayout* lay = new QHBoxLayout(widgetCentral);
    lay->setSpacing(0);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(viewer_);

    // tree dock
    treeWidgetModels->init(this);

    // ============= rendering panel ==================
    widgetTrianglesDrawable_ = new WidgetTrianglesDrawable(this);
    widgetTrianglesDrawable_->setEnabled(false);
    verticalLayoutTrianglesDrawable->addWidget(widgetTrianglesDrawable_);

    widgetPointsDrawable_ = new WidgetPointsDrawable(this);
    widgetPointsDrawable_->setEnabled(false);
    verticalLayoutPointsDrawable->addWidget(widgetPointsDrawable_);

    widgetGlobalSetting = new WidgetGlobalSetting(this);
    verticalLayoutGlobalSetting->addWidget(widgetGlobalSetting);

    connect(widgetGlobalSetting, SIGNAL(clipping_plane_on(bool)), this, SLOT(_global_cplane_toggled(bool)), Qt::UniqueConnection);

    hideModelProperty(true); // hide property menu first
    widgetGcp->hide();

    // set statusbar color
    Ui::mainAppClass::statusBar->setStyleSheet("color: #d9d7d7");

    show_hide_Buttons(false); // hide function buttons

    btnGcp->hide(); // hide btn GCP


    // create a folder in roaming for a file to stored all pw in a machine ========================
    //std::cout << "doc: " << QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0).toStdString() << std::endl;
    //for (QString a : QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)) {
    //    std::cout << "aa: " << a.toStdString() << std::endl;
    //}
    //std::cout << "aa: " << QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0).toStdString() << std::endl;
    QString dir_path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
    if (!QDir(dir_path).exists()) {
        QDir().mkdir(dir_path);
        std::cout << dir_path.toStdString() << std::endl;
        //std::fstream fin;
        //std::string a = dir_path.toStdString() + "\\companyEs.txt";
        ////fin.open(a, std::fstream::in);
        //fin.open(a, std::fstream::out | std::fstream::trunc);
        //fin.close();
    }
    // ===============================
}

void mainApp::paintEvent(QPaintEvent*)
{
    //widgetBottom->setGeometry(0, this->height() * 0.6, this->width(), this->height() * 0.4);
    //widgetMain->setGeometry(0, 0, this->width(), this->height());
}

void mainApp::dragEnterEvent(QDragEnterEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void mainApp::dropEvent(QDropEvent* e) { // if drag a folder = no extension = no '.'
    //for (QUrl s : e->mimeData()->urls()) {
    //    std::cout <<"1 " << s.toString().toStdString() << std::endl;
    //}

    if (!gcpManual) {
        if (e->mimeData()->hasUrls())
            e->acceptProposedAction();
        //std::cout << e->mimeData()->urls().at(0).toLocalFile().toStdString() << std::endl;
        //std::cout << e->mimeData()->urls().at(0).toString().toStdString() << std::endl;

        // =========== check folder =============
        bool check_folder = false;
        QString temp = e->mimeData()->urls().at(0).toLocalFile();
        std::cout << "dragged: " << temp.toStdString() << std::endl;
        QDir d(temp);
        if (d.exists()) {
            std::cout << "folder dragged" << std::endl;
            if(checkFilePath(temp))
                check_folder = true;
        }
        // ======================================

        bool check_bag = false;

        const QString& fileName = e->mimeData()->urls().at(0).toLocalFile();
        const std::string& ext = file_system::extension(fileName.toStdString(), true);
        if (ext == "bag")
            check_bag = true;

        if (check_bag) { // bag file
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
        }
        else if(check_folder) {
            if (!start_folder)
                folder_process(d);
            else
                QMessageBox::warning(0, QString(tr("Warning")), QString(tr("A folder has been dragged previously.")));
        }
        else { // point cloud file(s)
            int count = 0;
            foreach(const QUrl & url, e->mimeData()->urls()) {
                const QString& fileName = url.toLocalFile();
                if (open(fileName.toStdString()))
                    ++count;
            }
            if (count > 0) {
                tabWidget->setCurrentWidget(tab3dview);
                viewer_->update();
            }
        }
    }
    else {
        QMessageBox::warning(0, QString(tr("Warning")), QString(tr("Adding control points manually in process.")));
    }
}

void mainApp::on_actionAbout_triggered() {
    
    QString title = QMessageBox::tr(
        R"(<p align="center"><span style="font-style:italic;">Version Number:</span><span style="font-style:arial;">&nbsp;&nbsp;%1</span></p>)"
    ).arg(versionNo);
    QString text = QMessageBox::tr(
        "<p><strong>Time remaining : </strong><br> %1 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<t></p>"
        "<p>company's Website:<br>"
        "<a href=\"https://www.company.com/\">https://www.company.com/</a>"
    ).arg(QString::fromStdString(login_page_one.login_checker_one.remainingTime));
    //arg(login_page.login_checker.maintitleQstr);
    //arg(QString::fromStdString(login_page_one.login_checker_one.remainingTime));
    QMessageBox about(nullptr);
    //(0, QString(tr("About")), title + text);
    about.setWindowTitle("About");
    about.setWindowIcon(QIcon(":/company/process/logo.png"));
    QPixmap qp(":/company/process/100.png");
    about.setIconPixmap(qp);
    about.setText(title + text);
    about.exec();
}

void mainApp::_manual_triggered() {
    dialog_manual* man = new dialog_manual(this);
    man->init();
    man->show();



}

// ================================================================ POINT CLOUD =============================================================
// ==========================================================================================================================================

void mainApp::updateUi() {
    treeWidgetModels->updateModelList();
    updateRenderingPanel();
}

void mainApp::updateRenderingPanel() {

    widgetTrianglesDrawable_->updatePanel();
    widgetPointsDrawable_->updatePanel();

    if (!viewer_->currentModel()){
        frameProperty->hide();
        frameMesh->hide();
        show_hide_Buttons(false);
    }
    else {
        if (dynamic_cast<SurfaceMesh*>(viewer()->currentModel())) {

            frameMesh->show();
            frameProperty->hide();
            show_hide_Buttons(true);
            actionPoint->setEnabled(0);
            actionSampling->setEnabled(0);
        }

        else if (dynamic_cast<easy3d::PointCloud*>(viewer()->currentModel())) {

            frameProperty->show();
            frameMesh->hide();
            if (actionPoint->isChecked()) {
                show_hide_Buttons(false);
                actionOpen->setEnabled(false);
                treeWidgetModels->setEnabled(false);
                actionPoint->setEnabled(true);
            }
            else if (actionSegment->isChecked()) {
                show_hide_Buttons(false);
                actionOpen->setEnabled(false);
                treeWidgetModels->setEnabled(false);
                actionSegment->setEnabled(true);
                actionDelete->setEnabled(true);

            }
            else if (actionPhoto->isChecked()) {
                show_hide_Buttons(false);
                actionOpen->setEnabled(false);
                treeWidgetModels->setEnabled(false);
                actionPhoto->setEnabled(true);
            }
            else {
                treeWidgetModels->setEnabled(true);
                show_hide_Buttons(true);
                actionOpen->setEnabled(true);
            }
        }     
    }
}

WidgetModelList* mainApp::widgetModelList() const {
    return treeWidgetModels;
}

void mainApp::activeDrawableChanged(Drawable* d) {       // sub item in treewidget
    if (!d)
        return;
    switch (d->type()) {
    case Drawable::DT_POINTS:
        //ui.tabWidget->setCurrentWidget(ui.tabPointsDrawable);
        dynamic_cast<WidgetDrawable*>(widgetPointsDrawable_)->setActiveDrawable(d);

        break;
    case Drawable::DT_LINES:
        //ui->toolBox->setCurrentWidget(ui->toolBoxLinesDrawable);
        //dynamic_cast<WidgetDrawable*>(widgetLinesDrawable_)->setActiveDrawable(d);

        break;
    case Drawable::DT_TRIANGLES:
        //ui.tabWidget->setCurrentWidget(ui.tabTrianglesDrawable);
        dynamic_cast<WidgetDrawable*>(widgetTrianglesDrawable_)->setActiveDrawable(d);
        break;
    }
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
}

Model* mainApp::open(const std::string& file_name) {
    //emit(to_be_waited());
    Ui::mainAppClass::statusBar->showMessage(tr("Opening file(s).."));
    QApplication::processEvents(); // show loading dialog
    w.restart();

    QString temp;
    if (QString::fromStdString(file_name).contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        temp = QString(tr("Only English Characters allowed. Load point cloud failed: \"%1\"").arg(QString::fromStdString(file_name)));
        //log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
        Ui::mainAppClass::statusBar->showMessage(temp, 2000);
    }
    else {
        auto models = viewer_->models();
        for (auto m : models) {
            if (m->name() == file_name) {
                //prog->close();
                temp = QString(tr("model already loaded: \"%1\" ").arg(QString::fromStdString(file_name)));
                //log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
                Ui::mainAppClass::statusBar->showMessage(temp,2000);
                return nullptr;
            }
        }


        const std::string& ext = file_system::extension(file_name, true);
        bool is_ply_mesh = false;
        if (ext == "ply")
            is_ply_mesh = (easy3d::io::PlyReader::num_instances(file_name, "face") > 0);


        Model* model = nullptr;
        if ((ext == "ply" && is_ply_mesh) || ext == "obj" || ext == "off" || ext == "stl" || ext == "sm" ||
            ext == "plg") { // mesh
            model = SurfaceMeshIO::load(file_name);
        }
        else if (ext == "ply" && easy3d::io::PlyReader::num_instances(file_name, "edge") > 0) {
            model = GraphIO::load(file_name);
        }
        else if (ext == "plm" || ext == "pm" || ext == "mesh") {
            model = PolyMeshIO::load(file_name);
        }
        else { // point cloud

            if (ext == "ptx") {
                easy3d::io::PointCloudIO_ptx serializer(file_name);
                while (auto cloud = serializer.load_next()) {
                    viewer_->addModel(cloud);
                    //ui->treeWidgetModels->addModel(cloud, true);
                }
            }
            else
                model = PointCloudIO::load(file_name);

        }

        if (model) {

            tabWidget->setCurrentWidget(tab3dview);

            model->set_name(file_name);
            viewer_->addModel(model);

            treeWidgetModels->addModel(model, true);


            /*const auto keyframe_file = file_system::replace_extension(model->name(), "kf");
            if (file_system::is_file(keyframe_file)) {
                if (viewer_->walkThrough()->interpolator()->read_keyframes(keyframe_file)) {
                    LOG(INFO) << "model has an accompanying animation file \'"
                        << file_system::simple_name(keyframe_file) << "\' (loaded)";
                    viewer_->walkThrough()->set_scene({ model });
                }
            }*/

            temp = QString(tr("\"%1\" loaded successfully, time spent %2.").arg(QString::fromStdString(file_name)).arg(QString::fromStdString(w.time_string())));
            //log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
            //Ui::mainAppClass::statusBar->showMessage(temp, 2000);
        }
        else {
            temp = QString(tr("Unknown file format: \"%1\" or Failed to read this point cloud. Load point cloud failed: \"%2\"").arg(QString::fromStdString(ext)).arg(QString::fromStdString(file_name)));
            //log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
            //Ui::mainAppClass::statusBar->showMessage(temp, 2000);
        }
        Ui::mainAppClass::statusBar->showMessage(temp, 2000);
        //prog->close();
    

        return model;
    }
    //prog->close();
    return nullptr;
}

void mainApp::_global_cplane_toggled(bool b) {
    if (b)
        Ui::mainAppClass::statusBar->showMessage(QString(tr("Clipping plane enabled.")), 2000);
    else
        Ui::mainAppClass::statusBar->showMessage(QString(tr("Clipping pivot disabled.")), 2000);
}

void mainApp::_centerPivot_triggered(bool b) {
    if (b)
        Ui::mainAppClass::statusBar->showMessage(QString(tr("Center pivot enabled.")), 2000);
    else
        Ui::mainAppClass::statusBar->showMessage(QString(tr("Center pivot disabled.")), 2000);
    viewer_->setCenterPivot(b);
}

void mainApp::_edl_triggered(bool b) {
    if (b)
        Ui::mainAppClass::statusBar->showMessage(QString(tr("EDL enabled.")), 2000);
    else
        Ui::mainAppClass::statusBar->showMessage(QString(tr("EDL disabled.")), 2000);

    widgetGlobalSetting->setEyeDomeLighting(b);
}

void mainApp::_up_triggered() {
    //// to be deleted
    //_animation();
    //new_process = new widget_newProcess(this);
    //new_process->init();
    //new_process->manualGcpShow("E:\\vsTest\\2023_12_20_17_52_48\\111test.bag.laz");
    
    viewer_->pov(0);
}
void mainApp::_down_triggered() {
    viewer_->pov(1);
}
void mainApp::_front_triggered() {
    viewer_->pov(2);
}
void mainApp::_back_triggered() {
    viewer_->pov(3);
}
void mainApp::_left_triggered() {
    viewer_->pov(4);
}
void mainApp::_right_triggered() {
    viewer_->pov(5);
}
void mainApp::_setting_triggered() { // change background color for now

    if (!settings) {
        settings = new mainwindowSettingDialog(this);
        connect(settings, SIGNAL(_closed()), this, SLOT(_setting_closed()), Qt::UniqueConnection);
        cmdAvailable = true;
        connect(settings, SIGNAL(_cmd()), this, SLOT(slot_cmd()));
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

void mainApp::_open_triggered() {
    const QStringList& fileNames = QFileDialog::getOpenFileNames(
        this,
        "Open file(s)",
        QDir::currentPath(),
        //"Supported formats (*.ply *.las *.laz )\n"
        "Supported formats (*.ply *.obj *.off *.stl *.sm *.geojson *.trilist *.bin *.las *.laz *.xyz *.bxyz *.vg *.bvg *.ptx *.plm *.pm *.mesh)\n"
        "Surface Mesh (*.ply *.obj *.off *.stl *.sm *.geojson *.trilist)\n"
        "Point Cloud (*.ply *.bin *.ptx *.las *.laz *.xyz *.bxyz *.vg *.bvg *.ptx)\n"
        "Polyhedral Mesh (*.plm *.pm *.mesh)\n"
        "Graph (*.ply)\n"
        "All formats (*.*)" 
    );

    //QString temp;
    if (fileNames.empty()) {
        //temp = QString(tr("Opening file(s) cancelled."));
        //log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        //prog->close();
    }
    else {
        for (const auto& name : fileNames) {
            open(name.toStdString());
        }
    }
}

bool mainApp::_save_triggered() {
    //QString temp;
    const Model* model = viewer_->currentModel();
    if (!model) {
        //temp = QString(tr("No model exists/selects."));
        //log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
        return false;
    }

    std::string default_file_name = model->name();
    if (file_system::extension(default_file_name).empty()) // no extension?
        default_file_name += ".ply"; // default to ply

    const QString& fileName = QFileDialog::getSaveFileName(
        this,
        "Save file",
        QString::fromStdString(default_file_name),
        "Supported formats (*.ply *.obj *.off *.stl *.sm *.bin *.las *.laz *.xyz *.bxyz *.vg *.bvg *.plm *.pm *.mesh)\n"
        "Surface Mesh (*.ply *.obj *.off *.stl *.sm)\n"
        "Point Cloud (*.ply *.bin *.ptx *.las *.laz *.xyz *.bxyz *.vg *.bvg)\n"
        "Polyhedral Mesh (*.plm *.pm *.mesh)\n"
        "Graph (*.ply)\n"
        "All formats (*.*)"
    );

    if (fileName.isEmpty()) {
        //temp = QString(tr("Saving file(s) cancelled."));
        //log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        return false;
    }

    Ui::mainAppClass::statusBar->showMessage(tr("Saving.."));
    //emit(to_be_waited());
    QApplication::processEvents(); // show loading dialog
    w.restart();


    bool saved = false;
    if (dynamic_cast<const easy3d::PointCloud*>(model)) {
        const auto cloud = dynamic_cast<const easy3d::PointCloud*>(model);
        saved = PointCloudIO::save(fileName.toStdString(), cloud);
    }
    else if (dynamic_cast<const SurfaceMesh*>(model)) {
        const auto mesh = dynamic_cast<const SurfaceMesh*>(model);
        saved = SurfaceMeshIO::save(fileName.toStdString(), mesh);
    }
    else if (dynamic_cast<const Graph*>(model)) {
        const auto graph = dynamic_cast<const Graph*>(model);
        saved = GraphIO::save(fileName.toStdString(), graph);
    }
    else if (dynamic_cast<const PolyMesh*>(model)) {
        const auto mesh = dynamic_cast<const PolyMesh*>(model);
        saved = PolyMeshIO::save(fileName.toStdString(), mesh);
    }

    QString temp;

    if (saved) {
        temp = QString(tr("Model successfully saved to: %1, time spent %2.")).arg(fileName).arg(QString::fromStdString(w.time_string()));
        //log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        //prog->close();
        Ui::mainAppClass::statusBar->showMessage(temp, 2000);
        return true;
    }

    temp = QString(tr("Model failed to save."));
    //log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
    //prog->close();
    Ui::mainAppClass::statusBar->showMessage(temp, 2000);
    return false;

}

void mainApp::_del_triggered() {
    //QString temp;
    auto tool_manager = viewer()->tool_manager();
    if (tool_manager->current_tool()) {
        auto cloud = dynamic_cast<easy3d::PointCloud*>(viewer()->currentModel());
        auto mesh = dynamic_cast<SurfaceMesh*>(viewer()->currentModel());
        //PointCloud* newCloud = new PointCloud();

        if (cloud) {

            auto select_vertices = cloud->vertex_property<bool>("v:select");

            std::size_t count(0);
            for (auto v : cloud->vertices()) {
                if (select_vertices[v]) {
                    //newCloud->add_vertex(cloud->position(v));
                    cloud->delete_vertex(v);
                    ++count;
                }
            }
            cloud->collect_garbage();
            cloud->manipulator()->reset();
            cloud->renderer()->update();


            //Model* model = nullptr;
            //model = newCloud;
            //model->set_name("file_name111");
            //viewer_->addModel(model);
            //ui.treeWidgetModels->addModel(model, true);


            updateUi();
            viewer_->update();

            //temp = QString(tr("Selected points deleted: %1 points.")).arg(QString::number(count));
            //log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        }

        if (mesh) {

            auto select_faces = mesh->face_property<bool>("f:select");
            std::size_t count(0);
            for (auto f : mesh->faces()) {
                if (select_faces[f]) {
                    mesh->delete_face(f);
                    ++count;
                }
            }
            mesh->collect_garbage();
            mesh->manipulator()->reset();
            mesh->renderer()->update();

            updateUi();
            viewer_->update();

            //temp = QString(tr("Selected faces deleted: %1 faces.")).arg(QString::number(count));
            //log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        }
    }
    else {
        treeWidgetModels->deleteFromMainwindow();
    }

}

void mainApp::_sampling_triggered() {
    if (!samp_) {
        samp_ = new sampling(this);
        connect(samp_, SIGNAL(_closed()), this, SLOT(_samplingClosed()), Qt::UniqueConnection);
        samp_->show();
    }
    else {
        samp_->close();
        samp_ = nullptr;
    }

    
}

void mainApp::_samplingClosed() {
    if (samp_) {
        samp_->close();
        samp_ = nullptr;
    }
}

void mainApp::_point_triggered(bool b) {
    updateRenderingPanel();
    QString temp;
    if (b) {
        temp = QString(tr("Point selection enabled. Left Click: select / Right Click: remove red point. Make sure current imposter: plain."));
        auto tool_manager = viewer()->tool_manager();
        if (dynamic_cast<easy3d::PointCloud*>(viewer()->currentModel())) {
            tool_manager->set_tool(tools::ToolManager::SELECT_POINT_CLOUD_CLICK_TOOL);

            pointClick = true;
            //prevBgColor = viewer_->getBackgroundColor();
        }
    }
    else {
        temp = QString(tr(""));
        // return to prev color
        widgetPointsDrawable_->WidgetDrawable::setColorScheme(widgetPointsDrawable_->color_scheme_name);

        pointClick = false;
        // dist btw two points
        viewer_->show_line = false;
        viewer_->to_show_line.clear();
        viewer_->lines.clear();

        // gcp manual
        viewer_->startGcp = false;

        viewer()->tool_manager()->set_tool(tools::ToolManager::EMPTY_TOOL);
        viewer()->update();
    }

    Ui::mainAppClass::statusBar->showMessage(temp);

    if (gcpManual) {
        treeWidgetModels->setEnabled(false);
        actionOpen->setEnabled(false);
        actionSave->setEnabled(false);
        actionDelete->setEnabled(false);
        actionSegment->setEnabled(false);
        actionSampling->setEnabled(false);
        actionMesh->setEnabled(false);
        actionPhoto->setEnabled(false);
    }
}

void mainApp::_trim_triggered(bool b) {
    updateRenderingPanel();
    QString temp;
    if (b) {

        auto tool_manager = viewer()->tool_manager();
        if (dynamic_cast<easy3d::PointCloud*>(viewer()->currentModel())) {
            tool_manager->set_tool(tools::ToolManager::SELECT_POINT_CLOUD_LASSO_TOOL);

            prevBgColor = viewer_->getBackgroundColor();

            temp = QString(tr("Segment enabled. Left Click + Drag: select / Right Click + Drag: deselect. Make sure current imposter: plain."));
        }
        else if (dynamic_cast<SurfaceMesh*>(viewer()->currentModel())) {

            tool_manager->set_tool(tools::ToolManager::SELECT_SURFACE_MESH_FACE_LASSO_TOOL);

            prevBgColor = viewer_->getBackgroundColor();

            temp = QString(tr("Segment enabled. Left Click + Drag: select / Right Click + Drag: deselect."));
        }
    }
    else {
        temp = QString(tr(""));

        if (dynamic_cast<easy3d::PointCloud*>(viewer()->currentModel()))
            widgetPointsDrawable_->WidgetDrawable::setColorScheme(widgetPointsDrawable_->color_scheme_name);
        if (dynamic_cast<SurfaceMesh*>(viewer()->currentModel()))
            widgetTrianglesDrawable_->WidgetDrawable::setColorScheme(widgetTrianglesDrawable_->color_scheme_name);

        viewer_->setBackgroundColor(prevBgColor);
        viewer()->tool_manager()->set_tool(tools::ToolManager::EMPTY_TOOL);
        viewer()->update();
    }
    Ui::mainAppClass::statusBar->showMessage(temp);
}

void mainApp::_screenshot_triggered() {
    DialogSnapshot dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        dialog.saveSnapshot();
}

void mainApp::_mesh_triggered() {

    if (!mesh) {
        mesh = new DialogPoissonReconstruction(this);
        connect(mesh, SIGNAL(_closed()), this, SLOT(_mesh_closed()), Qt::UniqueConnection);
        mesh->show();
    }
    else {
        mesh->close();
        mesh = nullptr;
    }
}

void mainApp::_mesh_closed() {
    if (mesh) {
        mesh->close();
        mesh = nullptr;
    }
}

void mainApp::_photo_triggered() {
    QString temp;
    if (actionPhoto->isChecked()) {
        temp = QString(tr("Photo function enabled. Choose a valid image path and .csv file before continue."));
        photo = new DialogPhoto(this);
        connect(photo, SIGNAL(_proceed(QString, QString, int)), this, SLOT(_photo_proceed(QString, QString, int)), Qt::UniqueConnection);
        connect(photo, SIGNAL(_cancel()), this, SLOT(_photo_cancel()), Qt::UniqueConnection);
        photo->show();

        Ui::mainAppClass::statusBar->showMessage(temp);
        
    }
    else {
        photo->close();
    }

    updateRenderingPanel();
}

void mainApp::_photo_proceed(QString imgPath, QString csvFile, int a) {

    QString temp;
    if (a != 2) {
        QMessageBox::information(0, tr("Info"), tr("Please choose all."));
        return;
    }
    photo->hide();

    std::vector<vec3> all_coor; // xyz
    std::vector<int> picName;   // frame id
    QFile file(csvFile);
    if (!file.open(QIODevice::ReadOnly)) {
        temp = QString(tr("Current file cannot be read."));
        QMessageBox::information(0, tr("Information"), temp);
        photo->close();
        return;
    }

    QStringList xList, yList, zList;
    QStringList nameList;
    int prevent_first_line = 0;
    bool check = false; // row 0 col 0 == frame_id
    while (!file.atEnd()) { // store all coord.

        QByteArray line = file.readLine();
        prevent_first_line++;

        if (prevent_first_line == 1)
            check = true;
        else
            check = false;

        if (line.split(',').first() != "frame_id" && check) { // check validity of the csv file
            temp = QString(tr("Invalid csv file."));
            QMessageBox::information(0, tr("Information"), temp);
            photo->close();
            return;
        }


        if (prevent_first_line != 1) { // begin with 2nd line

            xList.append(line.split(',')[2]);
            yList.append(line.split(',')[3]);
            zList.append(line.split(',')[4]);

            vec3 temp(xList.at(0).toFloat(), yList.at(0).toFloat(), zList.at(0).toFloat());
            all_coor.push_back(temp);

            nameList.append(line.split(',').first());
            picName.push_back(nameList.at(0).toInt());

            xList.clear(); yList.clear(); zList.clear(); nameList.clear();
        }
    }

    QApplication::processEvents(); // show loading dialog


    temp = QString(tr("Press 'K' and left-click on the points to see the real scene picture."));


    viewer_->photoPath = imgPath;
    viewer_->all_coor = all_coor;
    viewer_->picName = picName;
    viewer_->showPhoto = true;

    Ui::mainAppClass::statusBar->showMessage(temp);
    //log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);

}

void mainApp::_photo_cancel() {
    actionPhoto->setChecked(0);
    actionPhoto->setCheckable(1);
    if (photo) {
        photo->close();
        photo = nullptr;
    }
    Ui::mainAppClass::statusBar->showMessage("");
    //log->set_log(log_type::Info, QString(tr("Photo function disabled.")).toStdString(), ui.logWidget);
    viewer_->photoPath = QString();
    viewer_->showPhoto = false;
    viewer_->cameraPoint = 0;
    viewer_->all_coor.clear();
    viewer_->picName.clear();
    viewer()->update();
    updateRenderingPanel();

}
// ================================================================ Process Bag =========================================================================
// ======================================================================================================================================================
void mainApp::folder_process(QDir d) {
    //for (QString a : d.entryList()) {
    //    std::cout << "el "<< a.toStdString() << std::endl;
    //}
    if (!start_folder) {
        start_folder = new startFolder(this);
        connect(start_folder, SIGNAL(_project(QString)), this, SLOT(_folder_triggered(QString)), Qt::UniqueConnection);
        connect(start_folder, SIGNAL(_exit()), this, SLOT(_folder_exit()), Qt::UniqueConnection);
        
        start_folder->setDir(d);
        start_folder->show();
    }


}
void mainApp::_folder_triggered(QString str) {
    if (start_folder->checkDir(str)) {

        std::cout << str.toStdString() << std::endl;
        _addProcess();
        if (str == "slam") {
            
            if (start_process) {
                std::cout << "^start process^" << std::endl;
                // set bag
                start_process->slamBag = start_folder->getSlamBag();
                start_process->lineEditSlam->setText(start_folder->getSlamBag());
                // set yaml
                start_process->lineEditproductParams->setText(start_folder->getSlamYaml());
                // set output name
                start_process->lineEditSlamName->setText(getFileName(start_folder->getSlamBag()));
                // set saving path
                start_process->lineEditSavingPath->setText(start_folder->getProjPath());
            }

        }
        else if (str == "color") {
            //QStringList getMp4File() { return mp4Files; }
            //QStringList getPanoFile() { return panoFile; }
            //QString getCaliGp() { return colorCaliGp; }
            //QString getCaliPa() { return colorCaliPa; }
            //QString getMask() { return colorMask; }
            //QString getPc() { return pointCloud; }
            //QString getCsv() { return colorCsv; }
            if (start_process) {
                std::cout << "^start process^" << std::endl;
                //set video files
                if (start_folder->getMp4File().size() > 0) {
                    // video
                    QString temp;
                    for (const auto& name : start_folder->getMp4File()) {
                        QString name1 = name + QString(" ");
                        temp += name1;
                    }
                    start_process->lineEditColorMp4->setText(temp);
                    start_process->colorFileMp4 = start_folder->getMp4File();
                    start_process->colorFileMp4Path = getFilePath(start_folder->getMp4File().at(0)) + "/";
                    start_process->colorFileMp4Path = start_process->colorFileMp4Path.replace('/', '\\');
                    // set yaml
                    start_process->lineEditCali->setText(start_folder->getCaliGp());

                }
                else if (start_folder->getPanoFile().size() > 0) {

                    QString temp;
                    for (const auto& name : start_folder->getPanoFile()) {
                        QString name1 = name + QString(" ");
                        temp += name1;
                    }
                    
                    start_process->lineEditColorMp4->setText(temp);
                    start_process->colorFileMp4 = start_folder->getPanoFile();
                    start_process->colorFileMp4Path = getFilePath(start_folder->getPanoFile().at(0)) + "/";
                    start_process->colorFileMp4Path = start_process->colorFileMp4Path.replace('/', '\\');
                    // set yaml
                    start_process->lineEditCali->setText(start_folder->getCaliPa());
                    // set mask
                    start_process->lineEditMask->setText(getFileName(start_folder->getMask()));
                }

                // set point cloud
                start_process->lineEditColorPc->setText(start_folder->getPc());
                start_process->colorFilePc = start_folder->getPc();
                // set bag/imu
                start_process->lineEditImu->setText(start_folder->getSlamBag());
                // set csv
                start_process->lineEditTraj->setText(start_folder->getCsv());
                // set saving path
                start_process->lineEditSavingPath_Color->setText(start_folder->getProjPath());
                start_process->comboBoxType->setCurrentIndex(1);
            }


        }
        else if (str == "rtk") {

            if (start_process) {
                std::cout << "^start process^" << std::endl;
                // set bag
                start_process->slamBag = start_folder->getSlamBag();
                start_process->lineEditSlam->setText(start_folder->getSlamBag());
                // set yaml
                start_process->lineEditproductParams->setText(start_folder->getSlamYaml());
                // set output name
                start_process->lineEditSlamName->setText(getFileName(start_folder->getSlamBag()));
                // set saving path
                start_process->lineEditSavingPath->setText(start_folder->getProjPath());
                //// rtk txt
                //start_process->lineEditRtk->setText(start_folder->getRtkTxt());
                //start_process->rtkFile = start_folder->getRtkTxt();
                start_process->comboBoxType->setCurrentIndex(2);
            }

        }
        else if (str == "gcp") {

            if (start_process) {
                std::cout << "^start process^" << std::endl;
                // set bag
                start_process->slamBag = start_folder->getSlamBag();
                start_process->lineEditSlam->setText(start_folder->getSlamBag());
                // set yaml
                start_process->lineEditproductParams->setText(start_folder->getSlamYaml());
                // set output name
                start_process->lineEditSlamName->setText(getFileName(start_folder->getSlamBag()));
                // set saving path
                start_process->lineEditSavingPath->setText(start_folder->getProjPath());
                //// gcp txt
                //start_process->lineEditGcp->setText(start_folder->getGcpTxt());
                //start_process->gcpFile = start_folder->getGcpTxt();
                start_process->comboBoxType->setCurrentIndex(3);
            }

        }


        _folder_exit();

    }
    else {
        if (str == "color") {

        }
        else {
            if (str == "slam") {
                QMessageBox::warning(nullptr, tr("Warning"), tr("Invalid files in this folder.\nEnsure .bag file and folder params are present."));

            }
            else if (str == "rtk" || str == "gcp") {
                QMessageBox::warning(nullptr, tr("Warning"), tr("Invalid files in this folder.\nEnsure .bag file and folder params are present."));

            }
            
        }
    }


}
void mainApp::_folder_exit() {
    start_folder->close();
    start_folder = nullptr;
}
void mainApp::_addProcess() {

    if (!start_process) {
        start_process = new widget_startProcess(this);

        connect(start_process, SIGNAL(_started()), this, SLOT(_startProcess()), Qt::UniqueConnection);
        connect(start_process, SIGNAL(_close()), this, SLOT(_startProcessClose()), Qt::UniqueConnection);
        connect(start_process, SIGNAL(_closeNonEmpty()), this, SLOT(_startProcessClose()), Qt::UniqueConnection);
       
        // reminder before running bag
        if (reminderBeforeRunningBag) {
            QCheckBox* cb = new QCheckBox(0);
            cb->setText(tr("Do not appear again for this time"));
            cb->setStyleSheet("QCheckBox{font-size:16px; font:bold Arial; }");
            //cb->setChecked(1);
            cb->setLayoutDirection(Qt::RightToLeft);
            QMessageBox *msgbox = new QMessageBox(nullptr);
            msgbox->setText("<font size = '5'>"+ tr("When running the bag, ensure no other applications are running in the back. \nThis is because the processing stage requires a relatively large amount of pc's usage.\n") + "</font>");
            msgbox->setIcon(QMessageBox::Icon::Information);
            msgbox->addButton(QMessageBox::Ok);
            msgbox->setDefaultButton(QMessageBox::NoButton);
            msgbox->setCheckBox(cb);
            //msgbox->setStyleSheet("QLabel{font-size:16px; }\
                                        QPushbutton{}");


            //QFont font;
            //font.setBold(true);
            //msgbox->setFont(font);
            //msgbox->button(QMessageBox::Ok)->setFont(font);


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
        //if (np->running) {
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
            //if (np->running) {
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
        if (checkForRemaining == newProcess_list.size()) {
            lineEditRunningBag->setText("");
            lineEditWaitingBags->setText(QString::number(0));
        }
    }
}

void mainApp::removeNewProcess(QString n) {

    //std::cout <<"Project's name: " << name.toStdString() << std::endl;
    for (int i = 0; i < getNewProcessList().size(); i++) {
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
    pathtocreate = pathtocreate.replace("/", "\\");
    QStringList components = pathtocreate.split("\\");
    foreach(QString a, components) {
        if (!QString::fromStdString(a.toStdString()).contains(QRegExp("^[A-Za-z0-9_:.- ]*$"))) {
            QMessageBox::warning(0, QString(tr("Warning")), QString(tr("Only letters, numbers, underscores, spaces and dashes are allowed in the path.")));
            std::cout << a.toStdString() << std::endl;
            std::cout << "**********" << std::endl;
            return false;
        }
    }
    return true;
}
void mainApp::startTimer() {
    w.restart();
}
std::string mainApp::getTimer() {
    return w.time_string();
}
void mainApp::startTimerP() {
    wP.Reset();
    wP.Start();

}
std::vector<std::string> mainApp::getTimerP() {
    std::vector<std::string> list;

    wP.Stop();
    double time_used = wP.GetElapsed().count();
    double time_used_in_s = time_used / 1.00e9;

    list.emplace_back(std::to_string(time_used_in_s)); // to accumulate all seconds.

    char* ch = new char[sizeof(time_used_in_s)];
    std::string res = "";
    if (time_used_in_s < 60) { // in s
        sprintf(ch, "%.2f", time_used_in_s);
        res = ch;
        res += "s";
        // time_used_in_s = std::ceil(time_used_in_s * 100.0) / 100.0;
        // res = std::to_string(time_used_in_s) + "s";

    }
    else if (time_used_in_s > 60 && time_used_in_s < 60 * 60) { // in min
        time_used_in_s = time_used_in_s / 60;
        sprintf(ch, "%.2f", time_used_in_s);
        res = ch;
        res += "min";
        // time_used_in_s = std::ceil(time_used_in_s * 100.0) / 100.0;
        // res = std::to_string(time_used_in_s) + "min";
    }
    else { // in hr
        time_used_in_s = time_used_in_s / 60 / 60;
        sprintf(ch, "%.2f", time_used_in_s);
        res = ch;
        res += "hr";
        // time_used_in_s = std::ceil(time_used_in_s * 100.0) / 100.0;
        // res = std::to_string(time_used_in_s) + "hr";
    }

    list.emplace_back(res); // each stage/function
    // return res;
    return list;
}
void mainApp::stopTimerP(){ wP.Reset(); }
void mainApp::showLoginPage() {

    //login_page.show();
    login_page_one.show();
}
void mainApp::displayMainwindow() {

    this->show();
    //login_page.close();
    login_page_one.close();
}
void mainApp::slot_timeRemaining(QString errstr)
{

    //if (errstr == QString("S6")) // valid duration
    //{
    //    //std::cout << login_page.login_checker.maintitleQstr.toStdString() << std::endl;
    //    //std::cout << login_page_one.login_checker_one.remainingTime << std::endl;
    //}
    //else if (errstr == QString("S3"))
    //{   
    //    //login_page_one.swapError(x);
    //    // 
    //    //if (processGui->isVisible())processGui->quitWhenRunning();
    //    this->hide();
    //    login_page.show();
    //    login_page.datasetbtn->setStyleSheet("QPushButton{border-image: url(:/company/process/icon.png);border:none;color:rgb(38, 38, 38); }");
    //    login_page.datasetlabel->setText(QString(tr("System time error. Please update the system time. ")));
    //}
    //else
    //{
    //    //if (processGui->isVisible())processGui->quitWhenRunning();
    //    this->hide();
    //    login_page.show();
    //    login_page.datasetbtn->setStyleSheet("QPushButton{border-image: url(:/company/process/icon.png);border:none;color:rgb(38, 38, 38); }");
    //    login_page.datasetlabel->setText(QString(tr("License expired. Please contact the supplier.")));
    //}
}
void mainApp::slot_timeRemaining(int err)
{

    if (err == 6) // valid duration
    {
        //std::cout << login_page.login_checker.maintitleQstr.toStdString() << std::endl;
        //std::cout << login_page_one.login_checker_one.remainingTime << std::endl;
    }
    else if (err == 3)
    {
        login_page_one.swapError(3);
        this->hide();
        login_page_one.show();

        // stop all processes
        for (widget_newProcess* np : newProcess_list) {
            np->setStatus(running_status::finished);
        }
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
        QMessageBox::information(0, QString(tr("Error")), QString(tr("Failed to change the language")));
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

    YAML::Node config = YAML::LoadFile("..\\Lib\\company\\language.yaml");
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

    QString output = "..\\Lib\\company\\language_temp.yaml";
    QFile file(output); // remove if exists
    file.remove();
    std::ofstream fout("..\\Lib\\company\\language_temp.yaml");
    fout << config << std::endl; // dump it back into the file
    fout.close();
}
int mainApp::getLanguage() {
    QString output = "..\\Lib\\company\\language_temp.yaml";
    QFile file(output); 
    if (file.exists()) {
        YAML::Node config = YAML::LoadFile("..\\Lib\\company\\language_temp.yaml");
        int type = config["language"]["type"].as<int>();
        std::cout << "language type: " << std::to_string(type) << std::endl;
        return type;
    }
    else {
        return 1;
    }
}

void mainApp::_animation() {
    static DialogWalkThrough* dialog = nullptr;
    if (!dialog)
        dialog = new DialogWalkThrough(this);

    viewer_->walkThrough()->set_scene(viewer_->models());

    //// don't allow model picking when creating camera paths.
    //ui->actionSelectModel->setChecked(false);
    //for (auto m : viewer_->models())
    //    m->renderer()->set_selected(false);
    dialog->show();
}

void mainApp::slot_cmd() {

    if (!cmd_) {
        cmd_ = new terminal(this);
        cmdAvailable = false;
        std::cout << "terminal opened." << std::endl;
        connect(cmd_, SIGNAL(_closed()), this, SLOT(_cmd_closed()), Qt::UniqueConnection);
        cmd_->setGeometry(this->x() + this->width() - cmd_->width(), this->y() + this->height() - cmd_->height(), cmd_->width(), cmd_->height());
        cmd_->show();
    }
    else {
        //std::cout << "failed." << std::endl;
    }
}

void mainApp::_cmd_closed() {
    if (!cmd_) {
    }
    else {
        cmd_->close();
        cmd_ = nullptr;
        cmdAvailable = true;
        std::cout << "terminal closed." << std::endl;
    }
}
void mainApp::addToCmd(QString s) {
    
    if (cmd_) {
        cmd_->add(s);
    }
    else {
        std::cout << "terminal is closed." << std::endl;
    }

}
void mainApp::prepareManualGcp(QString a) {

    if (a.contains("start")) {
        std::cout << "gcp - manual starts." << std::endl;
        gcpManual = true;
        treeWidgetModels->setEnabled(false);
        actionOpen->setEnabled(false);
        actionSave->setEnabled(false);
        actionDelete->setEnabled(false);
        actionSegment->setEnabled(false);
        actionSampling->setEnabled(false);
        actionMesh->setEnabled(false);
        actionPhoto->setEnabled(false);

        widgetPointsDrawable_->setPointSize(1.0);
        widgetPointsDrawable_->setColorScheme("scalar - v:intensity");
        widgetPointsDrawable_->setScalarFieldClamp(false);
        widgetPointsDrawable_->toggleClamp(false);
    }
    else if (a.contains("end")){
        //std::cout << "gcp - manual ends." << std::endl;

        gcpManual = false;
        treeWidgetModels->setEnabled(true);
        actionOpen->setEnabled(true);

        if (actionPoint->isChecked()) {
            actionPoint->toggle();
            std::cout << "actionPoint button unclicked" << std::endl;
        }
        updateRenderingPanel();
    }
}
void mainApp::toAddxyzGcp(double x, double y, double z) {
    emit _toAddxyzGcp(x, y, z);
}