#include "../include/mainwindow.h"

#include <string>
#include <iostream>

#include <QMutex>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include <QSettings>
#include <QMessageBox>
#include <QColorDialog>
#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

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
#include <easy3d/util/stop_watch.h>
#include <easy3d/util/line_stream.h>
#include <easy3d/util/version.h>

#include "../include/easy3d_inc/dialog/dialog_snapshot.h"
#include "../include/easy3d_inc/dialog/dialog_point_cloud_normal_estimation.h"
#include "../include/easy3d_inc/dialog/dialog_point_cloud_simplification.h"
#include "../include/easy3d_inc/dialog/dialog_poisson_reconstruction.h"

#include "../include/gcp.h"
#include "../include/gcp_transformation.h"
#include <QFormLayout>

using namespace easy3d;

easy3d::StopWatch w;

mainwindow::mainwindow(QWidget* parent)
    : QMainWindow(parent), processGui(nullptr),
    prog(new progress(this)),
    log(new logger), pointClick(false), prevBgColor(easy3d::vec4())
    , photo(nullptr)
    , gcp_init(nullptr), gcp_dialog(nullptr), addGcp(false), current_pointcloud_path(QString()), current_routecloud_path(QString()), current_saving_path(QString()), no_of_gcp(0), gcp_merge(nullptr) //,getGcp(nullptr)
    
{
    ui.setupUi(this);
    this->setWindowTitle(QString("company product"));
    this->setWindowIcon(QIcon(":/process/process/logo.png"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    //this->setWindowState(Qt::WindowMaximized);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
    auto handle = CreateSemaphoreA(NULL, 4, 4, "company");

    init(); 
    show_hide_Buttons(false);


    // hide buttons (to be deleted)
    ui.actionMerge->setEnabled(0);
    ui.actionMove->setEnabled(0);
    ui.actionNoise->setEnabled(0);
    ui.actionFix->setEnabled(0);
    ui.actionEdgeExtract->setEnabled(0);
    ui.actionReport->setEnabled(0);
    //=============

    
    ui.frame->hide();
}
    
void mainwindow::updateProgress()
{
    prog->show();    
}

void mainwindow::updateUi() {
    ui.treeWidgetModels->updateModelList();
    updateRenderingPanel();
}

void mainwindow::updateRenderingPanel() {

    widgetTrianglesDrawable_->updatePanel();
    //widgetLinesDrawable_->updatePanel();
    widgetPointsDrawable_->updatePanel();

    if(!viewer_->currentModel())
        show_hide_Buttons(false);
    else {
        
        if (dynamic_cast<easy3d::PointCloud*>(viewer()->currentModel())) {
            if (ui.actionPoint->isChecked()) {
                show_hide_Buttons(false);
                ui.actionOpen->setEnabled(false);
                ui.treeWidgetModels->setEnabled(false);
                ui.actionPoint->setEnabled(true);
            }
            else if (ui.actionSegment->isChecked()) {
                show_hide_Buttons(false);
                ui.actionOpen->setEnabled(false);
                ui.treeWidgetModels->setEnabled(false);
                ui.actionSegment->setEnabled(true);
                ui.actionDelete->setEnabled(true);

            }
            else if (ui.actionPhoto->isChecked()) {
                show_hide_Buttons(false);
                ui.actionOpen->setEnabled(false);
                ui.treeWidgetModels->setEnabled(false);
                ui.actionPhoto->setEnabled(true);
            }
            else {
                ui.treeWidgetModels->setEnabled(true);
                show_hide_Buttons(true);
                ui.actionOpen->setEnabled(true);
            }
        }
        else if (dynamic_cast<SurfaceMesh*>(viewer()->currentModel())) {
            show_hide_Buttons(true);
            ui.actionPoint->setEnabled(0);
            ui.actionSampling->setEnabled(0);
        }
    }
}

mainwindow::~mainwindow()
{
    if(processGui)
        processGui->deleteProcesses();
    std::cout << " mainwindow down " << std::endl;
}

void mainwindow::init() {

    // opengl window
    viewer_ = new PaintCanvas(this);

    QHBoxLayout* lay = new QHBoxLayout(ui.widget);
    lay->setSpacing(0);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(viewer_);

    setCentralWidget(viewer_);
    




    // tree dock
    ui.treeWidgetModels->init(this);

    // ============= rendering panel ==================
    widgetTrianglesDrawable_ = new WidgetTrianglesDrawable(this);
    widgetTrianglesDrawable_->setEnabled(false);
    QScrollArea* scrollAreaM = new QScrollArea(ui.tabTrianglesDrawable);    //create a scroll area
    scrollAreaM->setWidget(widgetTrianglesDrawable_);
    ui.verticalLayoutTrianglesDrawable->addWidget(scrollAreaM);
    scrollAreaM->setMinimumWidth(310);
    scrollAreaM->setFrameShape(QFrame::NoFrame);

    //widgetLinesDrawable_ = new WidgetLinesDrawable(this);
    //ui->verticalLayoutLinesDrawable->addWidget(widgetLinesDrawable_);
    //widgetLinesDrawable_->setEnabled(false);

    widgetPointsDrawable_ = new WidgetPointsDrawable(this);
    widgetPointsDrawable_->setEnabled(false);
    QScrollArea* scrollAreaP = new QScrollArea(ui.tabPointsDrawable);    //create a scroll area
    scrollAreaP->setWidget(widgetPointsDrawable_);
    ui.verticalLayoutPointsDrawable->addWidget(scrollAreaP);
    scrollAreaP->setMinimumWidth(310);
    scrollAreaP->setFrameShape(QFrame::NoFrame);


    widgetGlobalSetting = new WidgetGlobalSetting(this);
    QScrollArea* scrollAreaG = new QScrollArea(ui.tabPageGlobal);    //create a scroll area
    scrollAreaG->setWidget(widgetGlobalSetting);
    ui.verticalLayoutGlobalSetting->addWidget(scrollAreaG);
    scrollAreaG->setMinimumWidth(310);
    scrollAreaG->setFrameShape(QFrame::NoFrame);
    connect(widgetGlobalSetting, SIGNAL(shadow_toggled(bool)), this, SLOT(_global_shadow_toggled(bool)), Qt::UniqueConnection);
    connect(widgetGlobalSetting, SIGNAL(ssao_on(bool)), this, SLOT(_global_ssao_toggled(bool)), Qt::UniqueConnection);
    connect(widgetGlobalSetting, SIGNAL(clipping_plane_on(bool)), this, SLOT(_global_cplane_toggled(bool)), Qt::UniqueConnection);
    
    //// communication between widgets
    //widgetGlobalSetting->widgetTrianglesDrawable_ = widgetTrianglesDrawable_;
    // =================================================

    // docks in qmenu 'File'
    connect(ui.actionProcessDock, SIGNAL(triggered()), this, SLOT(processDock_triggered()), Qt::UniqueConnection);
    connect(ui.actionTreeDock, SIGNAL(triggered()), this, SLOT(treeDock_triggered()), Qt::UniqueConnection);
    connect(ui.actionPropertyDock, SIGNAL(triggered()), this, SLOT(propertyDock_triggered()), Qt::UniqueConnection);
    connect(ui.actionConsoleDock, SIGNAL(triggered()), this, SLOT(consoleDock_triggered()), Qt::UniqueConnection);
    connect(ui.processDock, SIGNAL(visibilityChanged(bool)), SLOT(dockProcessClosed(bool)), Qt::UniqueConnection);
    connect(ui.treeDock, SIGNAL(visibilityChanged(bool)), SLOT(dockTreeClosed(bool)), Qt::UniqueConnection);
    connect(ui.propertyDock, SIGNAL(visibilityChanged(bool)), SLOT(dockPropertyClosed(bool)), Qt::UniqueConnection);
    connect(ui.consoleDock, SIGNAL(visibilityChanged(bool)), SLOT(dockConsoleClosed(bool)), Qt::UniqueConnection);
    connect(ui.actionClear_Console, SIGNAL(triggered()), SLOT(dockConsoleClear()), Qt::UniqueConnection);
    // =====================
    
    //// process button ===============================
    //QWidget* lTitleBar = ui.processDock->titleBarWidget();
    //QWidget* lEmptyWidget = new QWidget();
    //ui.processDock->setTitleBarWidget(lEmptyWidget);
    //delete lTitleBar;
    //// =============================================

    // for login part ==============================================================================================================================
    connect(&login_page, SIGNAL(enter_signal()), this, SLOT(displayMainwindow()), Qt::UniqueConnection); // from login page (activated successfully)
    connect(&login_page.login_checker, SIGNAL(timer_signal(QString)), this, SLOT(slot_timeRemaining(QString)));
    connect(&login_page, SIGNAL(languageChanged_signal(int)), this, SLOT(slot_loginPgLanguage(int)));
    //==============================================================================================================================================

    // mainwindow signals and slots ============================================================================
    connect(ui.btnProcess, SIGNAL(clicked()), this, SLOT(on_btnProcess_clicked()), Qt::UniqueConnection);
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(on_actionAbout_triggered()), Qt::UniqueConnection);
    connect(ui.actionEnglish, SIGNAL(triggered()), this, SLOT(_english_triggered()), Qt::UniqueConnection);
    connect(ui.actionChinese, SIGNAL(triggered()), this, SLOT(_chinese_triggered()), Qt::UniqueConnection);
    connect(ui.actionClassic, SIGNAL(triggered()), this, SLOT(_classic_triggered()), Qt::UniqueConnection);
    connect(ui.actionDark, SIGNAL(triggered()), this, SLOT(_dark_triggered()), Qt::UniqueConnection);
    connect(ui.actionPerspective_Orthographic, SIGNAL(toggled(bool)), viewer_, SLOT(_PersOrtho_triggered(bool)));
    connect(ui.actionmanual, SIGNAL(triggered()), this, SLOT(_actionManual_triggered()), Qt::UniqueConnection);
    //===========================================================================================================

    // loading ui ==========================
    connect(this, SIGNAL(to_be_waited()), this, SLOT(updateProgress()));
    //======================================

    // set background colour
    connect(ui.actionBackground_Colour, SIGNAL(triggered()), this, SLOT(setBackgroundColor()));
    // =====================


    // process button
    ui.btnProcess->setStyleSheet("QPushButton{border-image:url(658.png);} \
        QPushButton::hover{border-image:url(658(1).png);} \
        QPushButton::pressed{border-image:url(658.png);}");


    // tool bar buttons ================================
    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(_open_triggered()), Qt::UniqueConnection);
    connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(_save_triggered()), Qt::UniqueConnection);
    connect(ui.actionDelete, SIGNAL(triggered()), this, SLOT(_del_triggered()), Qt::UniqueConnection);
    connect(ui.actionPoint, SIGNAL(toggled(bool)), this, SLOT(_point_triggered(bool)), Qt::UniqueConnection);
    connect(ui.actionSegment, SIGNAL(toggled(bool)), this, SLOT(_trim_triggered(bool)), Qt::UniqueConnection);
    connect(ui.actionScreenshot, SIGNAL(triggered()), this, SLOT(_screenshot_triggered()), Qt::UniqueConnection);
    connect(ui.actionEDL, SIGNAL(toggled(bool)), this, SLOT(_edl_triggered(bool)), Qt::UniqueConnection);
    connect(ui.actionCenterPivot, SIGNAL(toggled(bool)), this, SLOT(_centerPivot_triggered(bool)), Qt::UniqueConnection);
    ui.actionCenterPivot->toggle();
    connect(ui.actionX, SIGNAL(triggered()), this, SLOT(computeHeightFieldX()), Qt::UniqueConnection);
    connect(ui.actionY, SIGNAL(triggered()), this, SLOT(computeHeightFieldY()), Qt::UniqueConnection);
    connect(ui.actionZ, SIGNAL(triggered()), this, SLOT(computeHeightFieldZ()), Qt::UniqueConnection);
    connect(ui.actionAll_of_above, SIGNAL(triggered()), this, SLOT(computeHeightField()), Qt::UniqueConnection);
    connect(ui.actionUp, SIGNAL(triggered()), this, SLOT(_up_triggered()), Qt::UniqueConnection);
    connect(ui.actionDown, SIGNAL(triggered()), this, SLOT(_down_triggered()), Qt::UniqueConnection);
    connect(ui.actionFront, SIGNAL(triggered()), this, SLOT(_front_triggered()), Qt::UniqueConnection);
    connect(ui.actionBack, SIGNAL(triggered()), this, SLOT(_back_triggered()), Qt::UniqueConnection);
    connect(ui.actionLeft, SIGNAL(triggered()), this, SLOT(_left_triggered()), Qt::UniqueConnection);
    connect(ui.actionRight, SIGNAL(triggered()), this, SLOT(_right_triggered()), Qt::UniqueConnection);
    connect(ui.actionEstimate_Normal, SIGNAL(triggered()), this, SLOT(_estNormal_triggered()), Qt::UniqueConnection);
    connect(ui.actionSampling, SIGNAL(triggered()), this, SLOT(_sampling_triggered()), Qt::UniqueConnection);
    connect(ui.actionMesh, SIGNAL(triggered()), this, SLOT(_mesh_triggered()), Qt::UniqueConnection);
    connect(ui.actionPhoto, SIGNAL(triggered()), this, SLOT(_photo_triggered()), Qt::UniqueConnection);
    connect(ui.actionGround_Control_Point, SIGNAL(triggered()), this, SLOT(_gcp_triggered()), Qt::UniqueConnection);
    connect(ui.actionAutomatic_Merging_GCP, SIGNAL(triggered()), this, SLOT(_gcp_merge_triggered()), Qt::UniqueConnection);
    
    // =================================================

    // ui style
    changeUiStyle(UiStyle::Classic);
}


void mainwindow::setBackgroundColor() {
    const vec4& c = viewer_->backGroundColor();
    QColor orig(static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255), static_cast<int>(c.a * 255));
    const QColor& color = QColorDialog::getColor(orig, this);
    if (color.isValid()) {
        const vec4 newColor(
            static_cast<float>(color.redF()),
            static_cast<float>(color.greenF()),
            static_cast<float>(color.blueF()),
            static_cast<float>(color.alphaF())
        );
        viewer_->setBackgroundColor(newColor);
        viewer_->update();
    }
}


void mainwindow::closeEvent(QCloseEvent* event) {

    ui.statusBar->showMessage(tr("Closing.."));
    if (!processGui) {
       event->accept();
    }
    else {
        QMessageBox::StandardButton check;
        check = QMessageBox::question(this, QString(tr("Warning")), QString(tr("Process page is currently online, confirm close this application?")), QMessageBox::Yes | QMessageBox::No);
        if (check == QMessageBox::Yes) {
            event->accept();  
            processGui->~gui();
        }
        else {
            ui.statusBar->showMessage(tr(""));
            event->ignore();
        }
    }
}


void mainwindow::dragEnterEvent(QDragEnterEvent* e) {
    if (e->mimeData()->hasUrls()) {
        
        e->acceptProposedAction();
    }
}


void mainwindow::dropEvent(QDropEvent* e) {
    if (e->mimeData()->hasUrls()) 
        e->acceptProposedAction();
    
    int count = 0;
    foreach(const QUrl & url, e->mimeData()->urls()) {
        const QString& fileName = url.toLocalFile();
        if (open(fileName.toStdString()))
            ++count;
    }

    if (count > 0)
        viewer_->update();
}


Model* mainwindow::open(const std::string& file_name) {
    emit(to_be_waited());
    QApplication::processEvents(); // show loading dialog
    w.restart();

    QString temp;
    if (QString::fromStdString(file_name).contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        temp = QString(tr("Only English Characters allowed. Load point cloud failed: \"%1\"").arg(QString::fromStdString(file_name)));
        log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
    }
    else {
        auto models = viewer_->models();
        for (auto m : models) {
            if (m->name() == file_name) {
                prog->close();
                QString temp;
                temp = QString(tr("model already loaded: \"%1\" ").arg(QString::fromStdString(file_name)));
                log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
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

            model->set_name(file_name);
            viewer_->addModel(model);

            ui.treeWidgetModels->addModel(model, true);

            //setCurrentFile(QString::fromStdString(file_name));

            /*const auto keyframe_file = file_system::replace_extension(model->name(), "kf");
            if (file_system::is_file(keyframe_file)) {
                if (viewer_->walkThrough()->interpolator()->read_keyframes(keyframe_file)) {
                    LOG(INFO) << "model has an accompanying animation file \'"
                        << file_system::simple_name(keyframe_file) << "\' (loaded)";
                    viewer_->walkThrough()->set_scene({ model });
                }
            }*/

            temp = QString(tr("\"%1\" loaded successfully, time spent %2.").arg(QString::fromStdString(file_name)).arg(QString::fromStdString(w.time_string())));
            log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        }
        else {
            temp = QString(tr("Unknown file format: \"%1\" or Failed to read this point cloud. Load point cloud failed: \"%2\"").arg(QString::fromStdString(ext)).arg(QString::fromStdString(file_name)));
            log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
        }

        prog->close();
        return model;

    }
    prog->close();
    return nullptr;
}


WidgetModelList* mainwindow::widgetModelList() const {
    return ui.treeWidgetModels;
}

void mainwindow::activeDrawableChanged(Drawable* d) {       // sub item in treewidget
    if (!d)
        return;


    switch (d->type()) {
    case Drawable::DT_POINTS:
        ui.tabWidget->setCurrentWidget(ui.tabPointsDrawable);
        dynamic_cast<WidgetDrawable*>(widgetPointsDrawable_)->setActiveDrawable(d);
        
        break;
    case Drawable::DT_LINES:
        //ui->toolBox->setCurrentWidget(ui->toolBoxLinesDrawable);
        //dynamic_cast<WidgetDrawable*>(widgetLinesDrawable_)->setActiveDrawable(d);
        
        break;
    case Drawable::DT_TRIANGLES:
        ui.tabWidget->setCurrentWidget(ui.tabTrianglesDrawable);
        dynamic_cast<WidgetDrawable*>(widgetTrianglesDrawable_)->setActiveDrawable(d);
        
        break;
    }
}


// ================================================================== Tool Buttons Actions =============================================================================
// =====================================================================================================================================================================
void mainwindow::_actionManual_triggered() {
    QString res = tr("\n"
            " User Manual:                                                     \n"
            " ------------------------------------------------------------------\n"
            "  Left drag:                                                       Rotate the camera                           \n"
            "  Right drag:                                                     Move the camera                             \n"
            "  Middle or Wheel:                                            Zoom in/out                                 \n"
            " ------------------------------------------------------------------\n"
            "  'f':                                                                  Fit screen (all models)                     \n"
            "  'space':                                                          Fit screen (current model only)             \n"
            "  Shift + left-click / double click left button :        Define a target point on model              \n"
            "  'z' + left-click/right click:                                  Show current model size                     \n"
            "  '-'/'=':                                                             Decrease/Increase point size                \n"
            "  'k' + left-click:                                                             for photo function                \n"
            " ------------------------------------------------------------------\n"
            "  Clipping plane:                                               alt + left-click rotate , alt + right-click translate				\n"
            " ------------------------------------------------------------------\n");
    log->set_log(log_type::Info, res.toStdString(), ui.logWidget);
}

void mainwindow::on_actionAbout_triggered() {
    QString title = QMessageBox::tr(
        R"(<p align="center"><span style="font-style:italic;">product Application</span></p>)"
    );
    QString text = QMessageBox::tr(
        "<p><strong>Time remaining : </strong><br> %1 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<t></p>  "
        "<p>company's Website:<br>"
        "<a href=\"https://www.company.com/\">https://www.company.com/</a>"
    ).arg(login_page.login_checker.maintitleQstr);

    QMessageBox::about(this, QString(tr("About")), title + text);
}

void mainwindow::_english_triggered()
{
    if (lang->current_language == Language::zh_)
    {
        lang->changeLanguage(Language::en_);
        ui.retranslateUi(this);
        //widgetPointsDrawable_->changeLanguage(Language::en_);
        widgetGlobalSetting->changeLanguage(Language::en_);
    }
    else {
        QMessageBox::information(this, QString(tr("Changing language")), QString(tr("Failed changing to English. Already in English.")));
    }
}

void mainwindow::_chinese_triggered()
{
    if (lang->current_language == Language::en_)
    {
        lang->changeLanguage(Language::zh_);
        ui.retranslateUi(this);
        //widgetPointsDrawable_->changeLanguage(Language::zh_);
        widgetGlobalSetting->changeLanguage(Language::zh_);
    }
    else {
        QMessageBox::information(this, QString(tr("Changing language")), QString(tr("Failed changing to Mandarin. Already in Mandarin.")));
    }
}

void mainwindow::_edl_triggered(bool b) {
    ui.frame->show();
    QString temp;
    if (b)
        temp = QString(tr("EDL enabled."));
    else
        temp = QString(tr("EDL disabled"));

    log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
    widgetGlobalSetting->setEyeDomeLighting(b);
}

void mainwindow::_global_shadow_toggled(bool b) {
    QString temp;
    if (b) {
        if (ui.actionEDL->isChecked()) {
            widgetGlobalSetting->setEyeDomeLighting(!b);
            ui.actionEDL->setChecked(!b);
        }
        temp = QString(tr("Shadow enabled."));
    }
    else {
        temp = QString(tr("Shadow disabled."));
    }
    log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
}

void mainwindow::_global_ssao_toggled(bool b) {
    QString temp;
    if (b)
        temp = QString(tr("SSAO enabled."));

    else
        temp = QString(tr("SSAO disabled."));

    log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
}

void mainwindow::_global_cplane_toggled(bool b) {
    QString temp;
    if (b)
        temp = QString(tr("Clipping plane enabled."));

    else
        temp = QString(tr("Clipping plane disabled."));

    log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
}

void mainwindow::_centerPivot_triggered(bool b) {
    QString temp;
    if (b) {

        temp = QString(tr("Center pivot enabled."));
    }

    else
        temp = QString(tr("Center pivot disabled."));

    viewer()->setCenterPivot(b);
    log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
}

void mainwindow::_classic_triggered() {
    changeUiStyle(UiStyle::Classic);
}
void mainwindow::_dark_triggered() {
    changeUiStyle(UiStyle::Dark);
}

void mainwindow::_open_triggered() {
    const QStringList& fileNames = QFileDialog::getOpenFileNames(
        this,
        "Open file(s)",
        QDir::currentPath(),
        "Supported formats (*.ply *.obj *.off *.stl *.sm *.geojson *.trilist *.bin *.las *.laz *.xyz *.bxyz *.vg *.bvg *.ptx *.plm *.pm *.mesh)\n"
        "Surface Mesh (*.ply *.obj *.off *.stl *.sm *.geojson *.trilist)\n"
        "Point Cloud (*.ply *.bin *.ptx *.las *.laz *.xyz *.bxyz *.vg *.bvg *.ptx)\n"
        "Polyhedral Mesh (*.plm *.pm *.mesh)\n"
        "Graph (*.ply)\n"
        "All formats (*.*)"
    );

    QString temp;
    if (fileNames.empty()) {
        temp = QString(tr("Opening file(s) cancelled."));
        log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        prog->close();
    }
    else {
        for (const auto& name : fileNames) {
            open(name.toStdString());
        }
    }
}

bool mainwindow::_save_triggered() {
    QString temp;
    const Model* model = viewer_->currentModel();
    if (!model) {
        temp = QString(tr("No model exists/selects."));
        log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
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
        temp = QString(tr("Saving file(s) cancelled."));
        log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        return false;
    }

    emit(to_be_waited());
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

    if (saved) {
        temp = QString(tr("Model successfully saved to: %1, time spent %2.")).arg(fileName).arg(QString::fromStdString(w.time_string()));
        log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        prog->close();
        return true;
    }
    
    temp = QString(tr("Model failed to save."));
    log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
    prog->close();
    return false;

}

void mainwindow::_del_triggered() {
    QString temp;
    auto tool_manager = viewer()->tool_manager();
    if(tool_manager->current_tool()){
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

            temp = QString(tr("Selected points deleted: %1 points.")).arg(QString::number(count));
            log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
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

            temp = QString(tr("Selected faces deleted: %1 faces.")).arg(QString::number(count));
            log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        }
    }
    else {
        ui.treeWidgetModels->deleteFromMainwindow();
    }
   
}

void mainwindow::computeHeightFieldX() {
    QString temp;
    auto model = viewer_->currentModel();
    if (!model) {
        temp = QString(tr("No point cloud is selected."));
        log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
    }
    else {
        // add scalar fields defined on vertices only.
        if (dynamic_cast<easy3d::PointCloud*>(model)) {
            auto cloud = dynamic_cast<easy3d::PointCloud*>(model);

            auto v_height_x = cloud->vertex_property<float>("v:height_x");

            for (auto v : cloud->vertices()) {
                const auto& p = cloud->position(v);
                v_height_x[v] = p.x;
            }

            temp = QString(tr("X scalar field computed for model %1. Added in 'Coloring'.")).arg(QString::fromStdString(model->name()));
            log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        }
        else {
            temp = QString(tr("Failed to computed for the model. Make sure point cloud is selected."));
            log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
        }

        model->renderer()->update();
        viewer()->update();
        updateUi();
    }
}

void mainwindow::computeHeightFieldY() {
    QString temp;
    auto model = viewer_->currentModel();
    if (!model) {
        temp = QString(tr("No point cloud is selected."));
        log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
    }
    else {
        // add scalar fields defined on vertices only.
        if (dynamic_cast<easy3d::PointCloud*>(model)) {
            auto cloud = dynamic_cast<easy3d::PointCloud*>(model);

            auto v_height_y = cloud->vertex_property<float>("v:height_y");

            for (auto v : cloud->vertices()) {
                const auto& p = cloud->position(v);
                v_height_y[v] = p.y;
            }

            temp = QString(tr("Y scalar field computed for model %1. Added in 'Coloring'.")).arg(QString::fromStdString(model->name()));
            log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        }
        else {
            temp = QString(tr("Failed to computed for the model. Make sure point cloud is selected."));
            log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
        }

        model->renderer()->update();
        viewer()->update();
        updateUi();
    }
}

void mainwindow::computeHeightFieldZ() {
    QString temp; auto model = viewer_->currentModel();
    if (!model) {
        temp = QString(tr("No point cloud is selected."));
        log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
    }
    else {
        // add scalar fields defined on vertices only.
        if (dynamic_cast<easy3d::PointCloud*>(model)) {
            auto cloud = dynamic_cast<easy3d::PointCloud*>(model);

            auto v_height_z = cloud->vertex_property<float>("v:height_z");
            for (auto v : cloud->vertices()) {
                const auto& p = cloud->position(v);
                v_height_z[v] = p.z;
            }

            temp = QString(tr("Z scalar field computed for model %1. Added in 'Coloring'.")).arg(QString::fromStdString(model->name()));
            log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        }
        else {
            temp = QString(tr("Failed to computed for the model. Make sure point cloud is selected."));
            log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
        }

        model->renderer()->update();
        viewer()->update();
        updateUi();
    }
}

void mainwindow::computeHeightField() {
    QString temp;

    auto model = viewer_->currentModel();
    if (!model) {
        temp = QString(tr("No point cloud is selected."));
        log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
    }
    else {
        // add scalar fields defined on vertices only.
        if (dynamic_cast<easy3d::PointCloud*>(model)) {
            auto cloud = dynamic_cast<easy3d::PointCloud*>(model);

            auto v_height_x = cloud->vertex_property<float>("v:height_x");
            auto v_height_y = cloud->vertex_property<float>("v:height_y");
            auto v_height_z = cloud->vertex_property<float>("v:height_z");
            for (auto v : cloud->vertices()) {
                const auto& p = cloud->position(v);
                v_height_x[v] = p.x;
                v_height_y[v] = p.y;
                v_height_z[v] = p.z;
            }

            temp = QString(tr("X, Y, Z scalar field computed for model %1. Added in 'Coloring'.")).arg(QString::fromStdString(model->name()));
            log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
        }
        else {
            temp = QString(tr("Failed to computed for the model. Make sure point cloud is selected."));
            log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
        }

        model->renderer()->update();
        viewer()->update();
        updateUi(); 
    }
}

void mainwindow::_point_triggered(bool b) {
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
        temp = QString(tr("Point selection disabled"));
        // return to prev color
        widgetPointsDrawable_->WidgetDrawable::setColorScheme(widgetPointsDrawable_->color_scheme_name);

        pointClick = false; 
        // dist btw two points
        viewer_->show_line = false;
        viewer_->to_show_line.clear();
        viewer_->lines.clear();


        //viewer_->setBackgroundColor(prevBgColor);
        viewer()->tool_manager()->set_tool(tools::ToolManager::EMPTY_TOOL);
        viewer()->update();
    }

    log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
}

void mainwindow::_trim_triggered(bool b) {
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
        temp = QString(tr("Segment disabled"));

        if (dynamic_cast<easy3d::PointCloud*>(viewer()->currentModel()))
            widgetPointsDrawable_->WidgetDrawable::setColorScheme(widgetPointsDrawable_->color_scheme_name);
        if (dynamic_cast<SurfaceMesh*>(viewer()->currentModel()))
            widgetTrianglesDrawable_->WidgetDrawable::setColorScheme(widgetTrianglesDrawable_->color_scheme_name);

        viewer_->setBackgroundColor(prevBgColor);
        viewer()->tool_manager()->set_tool(tools::ToolManager::EMPTY_TOOL);
        viewer()->update();
    }
    log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
}

void mainwindow::_up_triggered() {
    viewer_->pov(0);
}
void mainwindow::_down_triggered() {
    viewer_->pov(1);
}
void mainwindow::_front_triggered() {
    viewer_->pov(2);
}
void mainwindow::_back_triggered() {
    viewer_->pov(3);
}
void mainwindow::_left_triggered() {
    viewer_->pov(4);
}
void mainwindow::_right_triggered() {
    viewer_->pov(5);
}

void mainwindow::_screenshot_triggered() {
    DialogSnapshot dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        dialog.saveSnapshot();
}

void mainwindow::_estNormal_triggered() {
    auto cloud = dynamic_cast<easy3d::PointCloud*>(viewer()->currentModel());
    if (!cloud) {
        QString temp = QString(tr("No point cloud is selected."));
        log->set_log(log_type::Warning, temp.toStdString(), ui.logWidget);
        return;
    } 

    DialogPointCloudNormalEstimation dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        emit(to_be_waited());
        QString temp;
        temp = QString(tr("building kd_tree and estimating normals.."));
        log->set_log(log_type::Info, temp.toStdString(), getLogWidget());
        QApplication::processEvents(); // show loading dialog
        w.restart();

        unsigned int k = dlg.lineEditNeighborSize->text().toUInt();
        PointCloudNormals::estimate(cloud, k);
        cloud->renderer()->update();
        viewer()->update();
        updateUi();

        temp = QString(tr("done in %1")).arg(QString::fromStdString(w.time_string()));
        log->set_log(log_type::Info, temp.toStdString(), getLogWidget());
        prog->close();
    }
}

void mainwindow::_sampling_triggered() {
    DialogPointCloudSimplification* dialog = nullptr;
    if (!dialog)
        dialog = new DialogPointCloudSimplification(this);
    dialog->show();
}

void mainwindow::_mesh_triggered() {
    DialogPoissonReconstruction* dialog = nullptr;
    if (!dialog)
        dialog = new DialogPoissonReconstruction(this);
    dialog->show();
}

void mainwindow::_photo_triggered() {

    QString temp;
    if (ui.actionPhoto->isChecked()) {
        temp = QString(tr("Photo function enabled. Choose a valid image path and .csv file before continue."));
        photo = new DialogPhoto(this);
        connect(photo, SIGNAL(_proceed(QString, QString,int)), this, SLOT(_photo_proceed(QString, QString,int)), Qt::UniqueConnection);
        connect(photo, SIGNAL(_cancel()), this, SLOT(_photo_cancel()), Qt::UniqueConnection);
        photo->show();

        log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
    }
    else {
        photo->close();
    }

    updateRenderingPanel();
}
void mainwindow::_photo_proceed(QString imgPath, QString csvFile, int a) {

    QString temp;
    if (a!=2 ){
        QMessageBox::information(this, tr("Info"), tr("Please choose all."));
        return;
    }

    photo->hide();

    std::vector<vec3> all_coor; // xyz
    std::vector<int> picName;   // frame id
    QFile file(csvFile);
    if (!file.open(QIODevice::ReadOnly)) {
        temp = QString(tr("Current file cannot be read."));
        log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
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
            temp = QString(tr("Invalid file."));
            log->set_log(log_type::Error, temp.toStdString(), ui.logWidget);
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
    log->set_log(log_type::Info, temp.toStdString(), ui.logWidget);
}

void mainwindow::_photo_cancel() {
    ui.actionPhoto->setChecked(0);
    ui.actionPhoto->setCheckable(1);
    if (photo) {
        photo->close();
        photo = nullptr;
    }
    log->set_log(log_type::Info, QString(tr("Photo function disabled.")).toStdString(), ui.logWidget);
    viewer_->photoPath = QString();
    viewer_->showPhoto = false;
    viewer_->cameraPoint = 0;
    viewer_->all_coor.clear();
    viewer_->picName.clear();
    viewer()->update();
    updateRenderingPanel();

}


void mainwindow::_gcp_triggered() {
    // reset input param
    current_pointcloud_path = QString();
    current_routecloud_path = QString();
    current_saving_path = QString();

    gcp_init = new dialog_gcp_init(this);
    gcp_init->show();
    connect(gcp_init, SIGNAL(_proceed()), this, SLOT(_gcp_getGcp()), Qt::UniqueConnection);
}

void mainwindow::_gcp_getGcp() {

    QString pointCloud = gcp_init->pointCloud;
    QString routeCloud = gcp_init->routeCloud;
    QString savingPath = gcp_init->savingPath+'/';
    
    if (routeCloud.isEmpty())
        route_key = 0;
    else
        route_key = 1;

    gcp_init->close();
    gcp_init = nullptr;

    // add stopwatch: show loading screen and log
    log->set_log(log_type::Info, QString(tr("Extracting all GCPs..this may take some minutes.")).toStdString(), getLogWidget());
    emit(to_be_waited());
    QApplication::processEvents(); // show loading dialog
    w.restart();
  
    //std::cout << pointCloud.toStdString()<<" " << savingPath.toStdString() << " " << no_of_gcp << " " << routeCloud.toStdString() << std::endl;

    gcp* getGcp = new gcp();
    //getGcp->get_gcp(pointCloud.toStdString(), savingPath.toStdString(), routeCloud.toStdString(), no_of_gcp);
    getGcp->get_gcp(pointCloud.toStdString(), savingPath.toStdString(), routeCloud.toStdString());
    
    _gcp_yuan = getGcp->get_yuan(); // initial vector yuan

    no_of_gcp = _gcp_yuan.size()+1; // gcp_x

    std::vector<float> for_dialog_gcp; // store all xyzr
    std::vector<int> for_dialog_gcp_name; // store all name index

    for (Yuan yy : _gcp_yuan) {

        //std::cout << "x: " << std::to_string(yy.x) << std::endl;
        //std::cout << "y: " << std::to_string(yy.y) << std::endl;
        //std::cout << "z: " << std::to_string(yy.z) << std::endl;
        //std::cout << "r: " << std::to_string(yy.r) << std::endl;
        for_dialog_gcp_name.push_back(yy.name); 
        for_dialog_gcp.push_back(yy.x);
        for_dialog_gcp.push_back(yy.y);
        for_dialog_gcp.push_back(yy.z);
        for_dialog_gcp.push_back(yy.r);
        for_dialog_gcp.push_back(yy.rx);
        for_dialog_gcp.push_back(yy.ry);
        for_dialog_gcp.push_back(yy.rz);
        //std::cout << "__________" << std::endl;
    }

    ////for (int i = 0; i < for_dialog_gcp.size()/4; i++) {
    ////    std::cout << "x: " << std::to_string(for_dialog_gcp.at(i * 4 + 0)) << std::endl;
    ////    std::cout << "y: " << std::to_string(for_dialog_gcp.at(i * 4 + 1)) << std::endl;
    ////    std::cout << "z: " << std::to_string(for_dialog_gcp.at(i * 4 + 2)) << std::endl;
    ////    std::cout << "r: " << std::to_string(for_dialog_gcp.at(i * 4 + 3)) << std::endl;
    ////    std::cout << "__________" << std::endl;
    ////}

    prog->close();
    log->set_log(log_type::Info, QString(tr("Finished extracting, time spent %1.")).arg(QString::fromStdString(w.time_string())).toStdString(), getLogWidget());
    log->set_log(log_type::Info, QString(tr("Please follow the steps: \n  1. Add/Remove a GCP, drag the GCPs in the list manually (if needed) \n  2. Confirm: the output will be saved. \n  3. Save: when inputted realx/realy/realz values.")).toStdString(), getLogWidget());

    gcp_dialog = new dialog_gcp(this);
    gcp_dialog->init(this, for_dialog_gcp, for_dialog_gcp_name);
    gcp_dialog->show(); 


    connect(gcp_dialog, SIGNAL(_chooseGcp()), this, SLOT(_gcp_addGcp()),Qt::UniqueConnection);
    connect(gcp_dialog, SIGNAL(_delGcp(int)), this, SLOT(_gcp_delGcp(int)), Qt::UniqueConnection);
    connect(gcp_dialog, SIGNAL(_confirmGcp()), this, SLOT(_gcp_confirmGcp()), Qt::UniqueConnection);
    connect(gcp_dialog->getTable(), SIGNAL(_dragged()), this, SLOT(_gcp_draggedList()), Qt::UniqueConnection);
    connect(gcp_dialog, SIGNAL(_saved()), this, SLOT(_gcp_edittedGcp()), Qt::UniqueConnection);
    connect(gcp_dialog, SIGNAL(_gcc()), this, SLOT(_gcp_geographicCoordinateConversion()), Qt::UniqueConnection);
    connect(gcp_dialog, SIGNAL(_autoMerge()), this, SLOT(_gcp_automaticMerging()), Qt::UniqueConnection);
    connect(gcp_dialog, SIGNAL(_eliError()), this, SLOT(_gcp_eliOffsetError()), Qt::UniqueConnection);
}

void mainwindow::saveTxt(QString filename) {
    // update vector yuan from list
    QStandardItemModel* model = gcp_dialog->getModel();
    std::vector<Yuan> temp_vec_yuan;
    for (int i = 0; i < model->rowCount(); i++) {

        // get name id from list
        QStringList pieces = model->item(i, 0)->text().split("_");
        int nameIndex = pieces.at(1).toInt();

        // use list's name id to get real Yuan
        for (Yuan yy : _gcp_yuan) {
            if (yy.name == nameIndex) {
                temp_vec_yuan.emplace_back(yy);
                break;
            }
        }
    }
    // vector yuan updated
    _gcp_yuan = temp_vec_yuan;

    gcp* gcp_ = new gcp();
    gcp_->gcp_save(route_key, _gcp_yuan, filename.toStdString());
    //std::cout << std::to_string(route_key) << std::endl;

    //// for testing saving to txt
    //std::vector<Yuan> temp_vec;
    //temp_vec = gcp_->gcp_load (savePathTxt.toStdString());
    //gcp_->gcp_save(route_key, temp_vec, savePathTxt.toStdString());
    //std::vector<Yuan> temp_vec1;
    //temp_vec1 = gcp_->gcp_load(savePathTxt.toStdString());
    //for (Yuan yy : temp_vec1) {
    //    std::cout << "name: " << std::to_string(yy.name) << " x: " << std::to_string(yy.x) << " y: " << std::to_string(yy.y) << " z: " << std::to_string(yy.z) << " r: " << std::to_string(yy.r) << " t: " << std::to_string(yy.t) << " rx: " << std::to_string(yy.rx) << " ry: " << std::to_string(yy.ry) << " rz: " << std::to_string(yy.rz) << std::endl;
    //}
    log->set_log(log_type::Info, QString(tr("Output has saved as %1 in the saved folder.").arg(filename)).toStdString(), getLogWidget());
}
void mainwindow::_gcp_confirmGcp() {
    
    QString txtName;
    txtName = current_pointcloud_path;
    std::string _filename = txtName.toStdString().substr(txtName.toStdString().find_last_of("/") + 1);
    QString savePathTxt = current_saving_path + '/' + QString(QString::fromStdString(_filename + ".txt"));
    saveTxt(savePathTxt);

}

void mainwindow::updateList() {

    // update list
    std::vector<float> for_dialog_gcp; // store all xyzr
    std::vector<int> for_dialog_gcp_name; // store all name index
    for (Yuan yy : _gcp_yuan) {
        for_dialog_gcp_name.push_back(yy.name);
        for_dialog_gcp.push_back(yy.x);
        for_dialog_gcp.push_back(yy.y);
        for_dialog_gcp.push_back(yy.z);
        for_dialog_gcp.push_back(yy.r);
        for_dialog_gcp.push_back(yy.rx);
        for_dialog_gcp.push_back(yy.ry);
        for_dialog_gcp.push_back(yy.rz);
    }
   
    gcp_dialog->addData(for_dialog_gcp, for_dialog_gcp_name);

}

void mainwindow::_gcp_addGcp() {  
    addGcp = true;                 
}

void mainwindow::_gcp_addGcp_found() {

    addGcp = false; // for viewer: mouse pressed

    float x, y, z;
    x = newGcpXYZ.x;
    y = newGcpXYZ.y;
    z = newGcpXYZ.z;

    /*std::cout << "x: " << std::to_string(x) << "y: " << std::to_string(y) << "z: " << std::to_string(z) << std::endl;
    std::cout << "pc: " << current_pointcloud_path.toStdString() << "sp: " << current_saving_path.toStdString() << "r: " << current_routecloud_path.toStdString() << std::endl;*/

    gcp* getGcpCoor = new gcp();
    
    log->set_log(log_type::Info, QString(tr("Finding..")).toStdString(), getLogWidget());
    emit(to_be_waited());
    QApplication::processEvents(); // show loading dialog
    w.restart();

    Yuan yp;
    
    if (current_routecloud_path.isEmpty())
        yp = getGcpCoor->add(x, y, z, current_pointcloud_path.toStdString(),(current_saving_path + '/').toStdString(), no_of_gcp);
    else
        yp = getGcpCoor->add(x, y, z, current_pointcloud_path.toStdString(), (current_saving_path + '/').toStdString(), no_of_gcp, current_routecloud_path.toStdString());


    if (yp.n > 0)// if added
    {
        
        _gcp_yuan.emplace_back(yp);
        //sort(_gcp_yuan.begin(), _gcp_yuan.end(), cmpt1);   //sort the vector

        updateList();

        no_of_gcp++;

        log->set_log(log_type::Info, QString(tr("Done in %1. A new gcp has been saved to the saved path.")).arg(QString::fromStdString(w.time_string())).toStdString(), getLogWidget());  
    }
    else {
        log->set_log(log_type::Warning, QString(tr("Done in %1. Not Found.")).arg(QString::fromStdString(w.time_string())).toStdString(), ui.logWidget);
    }

    prog->close();
}

void mainwindow::_gcp_delGcp(int index) {

    _gcp_yuan.erase(_gcp_yuan.begin() + index);
    updateList();
}

void mainwindow::_gcp_edittedGcp() {  
    // update vector yuan from list
    QStandardItemModel* model = gcp_dialog->getModel();
    for (int i = 0; i < model->rowCount(); i++) {
        // get name id from list
        QStringList pieces = model->item(i, 0)->text().split("_");
        int nameIndex = pieces.at(1).toInt();
        // use list's name id to get real Yuan

        int index = 0;
        for (Yuan yy : _gcp_yuan) {
            if (yy.name == nameIndex) {

                //update realxyz
                QString realx = model->item(i, 5)->text();
                QString realy = model->item(i, 6)->text();
                QString realz = model->item(i, 7)->text();
                
                //std::cout << "rx: " << realx.toStdString() << " ry: " << realy.toStdString() << " rz: " << realz.toStdString() << std::endl;

                yy.rx = realx.toFloat();
                yy.ry = realy.toFloat();
                yy.rz = realz.toFloat();

                _gcp_yuan.erase(_gcp_yuan.begin() + index); // remove prev
                _gcp_yuan.insert(_gcp_yuan.begin() + index, yy); // add new
            }
            index++;
        }
    }
    updateList();
}

void mainwindow::_gcp_draggedList() {

    // update vector yuan from list
    QStandardItemModel* model = gcp_dialog->getModel();
    std::vector<Yuan> temp_vec_yuan;
    for (int i = 0; i < model->rowCount(); i++) {

        // get name id from list
        QStringList pieces = model->item(i, 0)->text().split("_");
        int nameIndex = pieces.at(1).toInt();

        // use list's name id to get real Yuan
        for (Yuan yy : _gcp_yuan) {
            if (yy.name == nameIndex) {
                temp_vec_yuan.emplace_back(yy);
                break;
            }
        }
    }

    // vector yuan updated
    _gcp_yuan = temp_vec_yuan;

    ////for testing 
    //for (Yuan yy : _gcp_yuan) {
    //    std::cout << "name: " << std::to_string(yy.name) << "x: " << std::to_string(yy.x) << "y: " << std::to_string(yy.y) << "z: " << std::to_string(yy.z) << "r: " << std::to_string(yy.r) << std::endl;
    //}

    updateList();
}

void mainwindow::_gcp_geographicCoordinateConversion() {

    gcp_trans trans;
    std::vector<Yuan> with_realxyz;
    QString txtName;
    txtName = current_pointcloud_path;
    std::string _filename = txtName.toStdString().substr(txtName.toStdString().find_last_of("/") + 1);

    log->set_log(log_type::Info, QString(tr("Converting..")).toStdString(), getLogWidget());
    emit(to_be_waited());
    QApplication::processEvents(); // show loading dialog
    w.restart();


    // use vector yuan, get all gcps with real xyz
    for (Yuan yy : _gcp_yuan) {
        if (yy.rx != 0 || yy.ry != 0 || yy.rz != 0) {
            with_realxyz.emplace_back(yy);
        }
    }

    //// use txt
    //QString confirmedTxt = "E:\\200\\test\\t.txt";
    //QFile file(confirmedTxt);
    //if (!file.open(QIODevice::ReadOnly)) {
    //    log->set_log(log_type::Info, QString(tr("Current file cannot be read.")).toStdString(), ui.logWidget);
    //    return;
    //}

    //int prevent_first_line = 0;
    //while (!file.atEnd()) { // store all coord.

    //    QByteArray line = file.readLine();
    //    prevent_first_line++;

    //    if (prevent_first_line != 1) { // begin with 2nd line

    //        // if real != 0, create new Yuan.  6=rx 7=ry 8=rz
    //        if (line.split(' ')[6].toFloat() != 0 || line.split(' ')[7].toFloat() != 0 || line.split(' ')[8].toFloat() != 0) {
    //            
    //            // create new Yuan
    //            float x, y, z, r, t,rx,ry,rz;
    //            x = line.split(' ')[1].toFloat();
    //            y = line.split(' ')[2].toFloat();
    //            z = line.split(' ')[3].toFloat();
    //            r = line.split(' ')[4].toFloat();
    //            t = line.split(' ')[5].toFloat();
    //            rx = line.split(' ')[6].toFloat();
    //            ry = line.split(' ')[7].toFloat();
    //            rz = line.split(' ')[8].toFloat();

    //            Yuan temp_yuan;
    //            temp_yuan.name = line.split(' ')[0].toInt();
    //            temp_yuan.x = x;
    //            temp_yuan.y = y;
    //            temp_yuan.z = z;
    //            temp_yuan.r = r;
    //            temp_yuan.t = t;
    //            temp_yuan.rx = rx;
    //            temp_yuan.ry = ry;
    //            temp_yuan.rz = rz;

    //            with_realxyz.emplace_back(temp_yuan);

    //           /* std::cout << "name: " << std::to_string(line.split(' ')[0].toInt()) << " x: " << std::to_string(x) << " y : " << std::to_string(y) << " z : " << std::to_string(z) << " r : " << std::to_string(r) << std::endl;
    //            std::cout << " rx: " << std::to_string(rx) << " ry: " << std::to_string(ry) << " rz: " << std::to_string(rz) << std::endl;
    //            std::cout << " _____________" << std::endl;*/
    //        }    
    //    }
    //}

    // point cloud w new coordinates
    if(trans.trans(with_realxyz, current_pointcloud_path.toStdString(), (current_saving_path).toStdString()+"/"+ _filename+"_trans.ply") == -99)
        log->set_log(log_type::Error, QString(tr("Done in %1. Conversion failed. Please ensure everything has followed the correct procedure.")).arg(QString::fromStdString(w.time_string())).toStdString(), getLogWidget());
    else
        log->set_log(log_type::Info, QString(tr("Done in %1. Output cloud has been saved to the saved path.")).arg(QString::fromStdString(w.time_string())).toStdString(), getLogWidget());

    prog->close();
}

void mainwindow::_gcp_automaticMerging() {

    // notification + save as txt
    QMessageBox::StandardButton check;
    check = QMessageBox::information(this, QString(tr("Information")), QString(tr("Please remove all non-identical control points first. The list will be saved in the saving path. \nGo to 'Point Cloud' - 'Automatic Merging (GCP)' to merge two clouds.")), QMessageBox::Ok | QMessageBox::Cancel);
    if (check == QMessageBox::Ok) {

        QString txtName;
        txtName = current_pointcloud_path;
        std::string _filename = txtName.toStdString().substr(txtName.toStdString().find_last_of("/") + 1);
        QString savePathTxt = current_saving_path + '/' + QString(QString::fromStdString(_filename + "_toMerge.txt"));
        saveTxt(savePathTxt);
    }

}
void mainwindow::_gcp_merge_triggered() {

    gcp_merge = new dialog_gcp_merge(this);
    gcp_merge->show();
    
    // give a noti about how to get the 2 txt files
    log->set_log(log_type::Warning, QString(tr("Make sure the two input txt files are generated and confirmed from the function 'Ground Control Point'.")).toStdString(), getLogWidget());

    connect(gcp_merge, SIGNAL(_toMerge(std::vector<Yuan>)), this, SLOT(_gcp_merging(std::vector<Yuan>)), Qt::UniqueConnection);
}

void mainwindow::_gcp_merging(std::vector<Yuan> v) {
    
    QMessageBox::information(this, QString(tr("Choose the point cloud")), QString(tr("Choose the point cloud relative to the first(left) txt file imported. This cloud will be merged.")));
    // get first cloud
    QString file = QFileDialog::getOpenFileName(
        this,
        "Ply File",
        QDir::currentPath(),
        "Supported formats (*.ply)"
    );
    if (file.isEmpty()) {
        log->set_log(log_type::Warning, QString(tr("Invalid file")).toStdString(), getLogWidget());
        return;
    }

    // set a random name for .ply
    QString filename;
    filename = "_merged.ply";

    // get trans()
    gcp_trans trans;

    log->set_log(log_type::Info, QString(tr("Merging..")).toStdString(), getLogWidget());
    emit(to_be_waited());
    QApplication::processEvents(); // show loading dialog
    w.restart();
    // point cloud w new coordinates
    if (trans.trans(v, file.toStdString(), (file).toStdString() + "_" + filename.toStdString()) == -99)
        log->set_log(log_type::Error, QString(tr("Done in %1. Conversion failed. Please ensure everything has followed the correct procedure.")).arg(QString::fromStdString(w.time_string())).toStdString(), getLogWidget());
    else
        log->set_log(log_type::Info, QString(tr("Done in %1. Output cloud has been saved to the same folder as the chosen cloud's.")).arg(QString::fromStdString(w.time_string())).toStdString(), getLogWidget());
    
    prog->close();
}


void mainwindow::_gcp_eliOffsetError() {

    // output name for the txt
    QString txtName;
    txtName = current_pointcloud_path;
    std::string _filename = txtName.toStdString().substr(txtName.toStdString().find_last_of("/") + 1);
    QString savePathTxt = current_saving_path + '/' + QString(QString::fromStdString(_filename + "_Rtk.txt"));


    log->set_log(log_type::Info, QString(tr("Input the first RTK coordinates(realx,y,z).")).toStdString(), getLogWidget());



    // add new ui to input line 1 realxyz only
    QDialog dialog(gcp_dialog);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);
    // Add info
    form.addRow(new QLabel("The values are obtained from the starting RTK point."));
    // Add the realxyz labels
    QList<QLineEdit*> fields;
    QLineEdit* lineEdit = new QLineEdit(&dialog);
    QLineEdit* lineEdit1 = new QLineEdit(&dialog);
    QLineEdit* lineEdit2 = new QLineEdit(&dialog);

    // set float only
    //QRegExp regx("[-+]?[0-9]*\.?[0-9]+");
    //QValidator* validator = new QRegExpValidator(regx, lineEdit);
    //lineEdit->setValidator(validator);
    lineEdit->setValidator(new QDoubleValidator(-FLT_MAX, FLT_MAX, 100, this));
    lineEdit1->setValidator(new QDoubleValidator(-FLT_MAX, FLT_MAX, 100, this));
    lineEdit2->setValidator(new QDoubleValidator(-FLT_MAX, FLT_MAX, 100, this));

    QString label = QString("real x: ");
    form.addRow(label, lineEdit);
    fields << lineEdit;
    QString label1 = QString("real y: ");
    form.addRow(label1, lineEdit1);
    fields << lineEdit1;
    QString label2 = QString("real z: ");
    form.addRow(label2, lineEdit2);
    fields << lineEdit2;

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept())); 
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    bool canSave = true;
    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        // check if all float
        QRegExp checkdouble("-?\\d*\\.?\\d*");  // a digit (\d), zero or more times (*)
        foreach(QLineEdit * lineEdit, fields) {
            if (!checkdouble.exactMatch(lineEdit->text()) || lineEdit->text().isEmpty()) {
                QMessageBox::warning(this, tr("Warning"), tr("Invalid input. Please re-enter."));
                canSave = false;
                break;
            }
        }

        // all valid float
        if (canSave) {
            std::vector<float> first_real;
            foreach(QLineEdit* lineEdit, fields) {
                first_real.emplace_back(lineEdit->text().toFloat()); // save the 3 realxyz
            }

            // update vector yuan from list
            QStandardItemModel* model = gcp_dialog->getModel();
            std::vector<Yuan> temp_vec_yuan;
            for (int i = 0; i < model->rowCount(); i++) {
                // get name id from list
                QStringList pieces = model->item(i, 0)->text().split("_");
                int nameIndex = pieces.at(1).toInt();
                // use list's name id to get real Yuan
                for (Yuan yy : _gcp_yuan) {
                    if (yy.name == nameIndex) {
                        temp_vec_yuan.emplace_back(yy);
                        break;
                    }
                }
            }
            // vector yuan updated
            _gcp_yuan = temp_vec_yuan;

            std::ofstream dataFile;
            dataFile.open(savePathTxt.toStdString(), ofstream::app);
            fstream file(savePathTxt.toStdString(), ios::out);

            dataFile << std::fixed << setprecision(10) << first_real.at(0) << " " << first_real.at(1) << " " << first_real.at(2) << " " << 0 << " " << 0 << " " << 0 << " " << 0 << std::endl;
            for (int i = 0; i < _gcp_yuan.size(); i++) {

                dataFile << std::fixed << setprecision(10) << _gcp_yuan[i].rx << " " << _gcp_yuan[i].ry << " " << _gcp_yuan[i].rz << " " << _gcp_yuan[i].x << " " << _gcp_yuan[i].y << " " << _gcp_yuan[i].z << " " << std::fixed << setprecision(6) << _gcp_yuan[i].t << std::endl;
            }


            log->set_log(log_type::Info, QString(tr("Output has saved as %1 in the saved folder.").arg(savePathTxt)).toStdString(), getLogWidget());
            log->set_log(log_type::Info, QString(tr("You may proceed to 'Process' - 'RTK' and use this .txt file and corresponding .bag file to continue the function.")).toStdString(), getLogWidget());
        }
        else {
            _gcp_eliOffsetError();
        }
    }

}

// for login part & remaining time ========================================================================================================================
void mainwindow::slot_loginPgLanguage(int x)
{
    switch (x) {
    case 1:
        std::cout << " changed to eng \n";
        lang->changeLanguage(Language::en_);
        ui.retranslateUi(this);

        break;
    case 2:
        std::cout << " changed to chi \n";
        lang->changeLanguage(Language::zh_);
        ui.retranslateUi(this);
        break;
    default:
        QMessageBox::information(this, QString(tr("Error")), QString(tr("Failed to change the language")));
        break;
    }
}

void mainwindow::showLoginPage() {

    login_page.show();
}
void mainwindow::displayMainwindow() {

    this->show();
    login_page.close();
}
void mainwindow::slot_timeRemaining(QString errstr)
{

    if (errstr == QString("S6")) // valid duration
    {}
    else if (errstr == QString("S3"))                                          
    {
        if (processGui->isVisible())processGui->quitWhenRunning();
        this->hide();
        login_page.show();
        login_page.datasetbtn->setStyleSheet("QPushButton{border-image: url(:/process/process/icon.png);border:none;color:rgb(38, 38, 38); }");
        login_page.datasetlabel->setText(QString(tr("System time error. Please update the system time. ")));
    }
    else
    {
        if (processGui->isVisible())processGui->quitWhenRunning();
        this->hide();
        login_page.show();
        login_page.datasetbtn->setStyleSheet("QPushButton{border-image: url(:/process/process/icon.png);border:none;color:rgb(38, 38, 38); }");
        login_page.datasetlabel->setText(QString(tr("License expired. Please contact the supplier.")));
    }
}
// =======================================================================================================================================================


// for the docks =============================
void mainwindow::processDock_triggered() {
    if (ui.actionProcessDock->isChecked()) {
        ui.processDock->show();
    }
    else {
        ui.processDock->close();
    }
}
void mainwindow::treeDock_triggered(){
    if (ui.actionTreeDock->isChecked()) {
        ui.treeDock->show();
    }
    else {
        ui.treeDock->close();
    }
}
void mainwindow::propertyDock_triggered(){
    if (ui.actionPropertyDock->isChecked()) {
        ui.propertyDock->show();
    }
    else {
        ui.propertyDock->close();
    }
}
void mainwindow::consoleDock_triggered(){
    if (ui.actionConsoleDock->isChecked()) {
        ui.consoleDock->show();
    }
    else {
        ui.consoleDock->close();
    }
}
void mainwindow::dockProcessClosed(bool x) {
    if (x) {
        ui.actionProcessDock->setChecked(true);
    }
    else {
        ui.actionProcessDock->setChecked(false);
    }
}
void mainwindow::dockTreeClosed(bool x) {
    if (x) {
        ui.actionTreeDock->setChecked(true);
    }
    else {
        ui.actionTreeDock->setChecked(false);
    }
}
void mainwindow::dockPropertyClosed(bool x) {
    if (x) {
        ui.actionPropertyDock->setChecked(true);
    }
    else {
        ui.actionPropertyDock->setChecked(false);
    }
}
void mainwindow::dockConsoleClosed(bool x) {
    if (x) {
        ui.actionConsoleDock->setChecked(true);
    }
    else {
        ui.actionConsoleDock->setChecked(false);
    }
}

void mainwindow::dockConsoleClear() {
    ui.logWidget->clear();
}
// ===============================================


void mainwindow::changeUiStyle(UiStyle style) {
    QString a;
    switch (style) {
    case UiStyle::Classic:

        a = "QSlider::handle:horizontal {background: #FAAB43;} \
                QCheckBox::indicator{ border: 1px solid #c4c4c4; border-radius: 2px;} \
                QCheckBox::indicator:checked{ background-color:#FAAB43; }\
                QCheckBox::indicator:disabled{ background-color:#D9D9D9; } \
                    QComboBox QAbstractItemView{ selection-background-color: #faa028; } \
                        QDoubleSpinBox{selection-background-color: #faa028;} \
                            QHeaderView::section { border:1px;}";

       widgetPointsDrawable_->setStyleSheet(a);
       widgetGlobalSetting->setStyleSheet(a);
       widgetGlobalSetting->setSphereColor(0);
       widgetTrianglesDrawable_->setStyleSheet(a);
       

       a = "QWidget{} \
                QListWidget::item:selected{background-color: #faa028;}\
                    QLineEdit{selection-background-color: #faa028;}\
                        QCheckBox::indicator{ border: 1px solid #c4c4c4; border-radius: 2px;} \
                        QCheckBox::indicator:checked{ background-color:#FAAB43; }\
                        QCheckBox::indicator:disabled{ background-color:#D9D9D9; } \
                            QDoubleSpinBox{selection-background-color: #faa028;} \
                            QSpinBox{selection-background-color: #faa028;} \
                                QMenuBar::item:selected{background-color:#faa028; }\
                                QMenu::item:selected{background-color:#faa028; }\
                                QMenu::item {padding: 3px 30px 3px 10px;}\
                                QMenu::separator {height: 2px;margin: 2px 5px 2px 4px;}\
                                QMenu::indicator {width: 20px;height: 13px;}\
                    ";
       this->setStyleSheet(a);
       break;


    case UiStyle::Dark:

        a = "QSlider::handle:horizontal {background: #FAAB43;} \
                QCheckBox::indicator{ border: 1px solid #c4c4c4; border-radius: 2px;} \
                QCheckBox::indicator:checked{ background-color:#FAAB43; }\
                QCheckBox::indicator:disabled{ background-color:#D9D9D9; } \
                    QComboBox QAbstractItemView { selection-background-color:#faa028;selection-color:#ffffff; } \
                    QComboBox{ selection-background-color:#faa028;selection-color:#ffffff; } \
                        QDoubleSpinBox{selection-background-color: #faa028; border:1px solid #121212}";
                                
        widgetPointsDrawable_->setStyleSheet(a);
        widgetGlobalSetting->setStyleSheet(a);
        widgetGlobalSetting->setSphereColor(1);
        widgetTrianglesDrawable_->setStyleSheet(a);

        //#faa028 orange

        a = "QWidget{color:#ffffff; background-color:#1e1e1e;}\
                QToolBar{border:none;} \
                    QDockWidget:title{ background-color:#121212; border:none; } \
                        WidgetModelList{color:#ffffff;border:1px solid #121212} \
                            QMenuBar{background-color:#121212;}\
                            QMenuBar::item:selected{background-color:#faa028; }\
                            QMenu::item:selected{background-color:#faa028; }\
                            QMenu::item {padding: 3px 30px 3px 10px;}\
                            QMenu::separator {height: 2px;margin: 2px 5px 2px 4px;}\
                            QMenu::indicator {width: 20px;height: 13px;}\
                                QTabWidget::pane{border:1px solid #121212} \
                                QTabWidget QTabBar::tab{background-color: #1e1e1e;}\
                                QTabWidget QTabBar::tab:selected{background-color: #121212;} \
                                    QListWidget::item:selected{background-color: #ffffff; color:#faa028}\
                                        QHeaderView::section {background-color:#1e1e1e; border:1px solid #121212; border-bottom: 0px}\
                        QLineEdit{selection-background-color: #faa028;}\
                        QCheckBox::indicator{ border: 1px solid #c4c4c4; border-radius: 2px;} \
                        QCheckBox::indicator:checked{ background-color:#FAAB43; }\
                        QCheckBox::indicator:disabled{ background-color:#D9D9D9; } \
                        QDoubleSpinBox{selection-background-color: #faa028;} \
                        QSpinBox{selection-background-color: #faa028;} \
            ";
        this->setStyleSheet(a);

        break;


    default:
        break;
    }
}

void mainwindow::show_hide_Buttons(bool b) {
    ui.actionSave->setEnabled(b);
    ui.actionDelete->setEnabled(b);
    ui.actionPoint->setEnabled(b);
    ui.actionSegment->setEnabled(b);
    ui.actionSampling->setEnabled(b);
    ui.actionMesh->setEnabled(b);
    ui.actionPhoto->setEnabled(b);
}

// process ui ==========================
void mainwindow::on_btnProcess_clicked() {
    if (!processGui) {
        processGui = new gui;

        processGui->setUpProUi();
        processGui->bringFront();
        processGui->readHistoricalData();

        std::cout << "============================" << std::endl;
        std::cout << "waiting to run" << std::endl;
        std::cout << "============================" << std::endl;

        processGui->show();
        connect(processGui, SIGNAL(_closed()), this, SLOT(processGui_closed()), Qt::UniqueConnection);
    }
}

void mainwindow::processGui_closed() {
    std::cout << "gui closed" << std::endl;
    processGui = nullptr;
}
//=======================================