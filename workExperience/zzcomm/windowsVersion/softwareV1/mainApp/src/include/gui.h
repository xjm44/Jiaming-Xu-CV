#ifndef GUI_H
#define GUI_H

#include "windowLogin.h"
#include "findGeode.h"
#include "colormap.h"
#include "picture.h"
#include <QWidget>
#include <QMouseEvent>
#include <QByteArray>
#include <QMargins>
#include <QPushButton>
#include <QLabel>
#include <QIcon>
#include <QLineEdit>
#include <QProgressBar>
#include <QMouseEvent>
#include <QPoint>
#include <QApplication>
#include <QBitmap>
#include <QRect>
#include <QPaintEvent>
#include <QPainterPath>
#include <QColor>
#include <QDebug>
#include <QPainter>
#include <QMainWindow>
#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qframe.h>
#include "ui_gui.h"
#include <QScreen>
#include <iostream>
#include <QMessageBox>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QDropEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <string>
#include <QProcess>
#include <QEventLoop>
#include <QTimer>
#include <QSettings>
//#include <stdio.h>
#include <QDir>
#include <QTextStream>
#include <QTreeWidgetItem>
#include <QPixmap>
#include <thread>
#include <QWheelEvent>

#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <osgGA/Export>
#include <osgGA/GUIEventAdapter>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/CompositeViewer>
#include <osg/ShapeDrawable>
#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/TexGen>
#include <osg/MatrixTransform>
#include <osgQOpenGL/osgQOpenGLWidget>
#include <osg/Node>
#include <osgGA/FirstPersonManipulator>


#include "../include/osgview.h"
#include <QDesktopWidget>
#include "../include/pickHandler.h"
#include "../include/pickPhoto.h"
#include "../include/osgPlaneExtraction.h"
#include "../include/osgTrim.h"
#include "../include/treatment.h"
#include "../include/osgScreenShot.h"
#include "../include/osgCapture.h"
#include "../include/report.h"
#include "../include/osgLod.h"
#include <chrono>
#include <thread>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QThread>

class gui : public QFrame
{
    Q_OBJECT

public:
    gui(QWidget *parent = nullptr);
    ~gui();

    void paintEvent(QPaintEvent*); // positioning

    // allow user to drag the window ============
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool move1 = false;
    QPoint lastPos;
    //===========================================

    // menu bar's buttons
    QPushButton* btnClose;
    QPushButton* btnMin;
    QPushButton* btnMax;

    void bringFront(); // bring certain widgets to the front

    void setUpProUi();// to set up the Process Ui
    void setGeo();// set the geometry of each widget

    void displayDialog(); //show login page

    windowLogin dlg; 
    picture pic; // for point cloud

    void on_btnOpen_clicked(QString a,int x); // 3 open buttons
    void on_btnDisplay_clicked(QString a); // 3 display buttons

    QString filename=""; // .bag file


    // for the 3 steps ====
    void slot_startBag(); // 1/3
    void mergeDeal();  // 3/3
    void delBagFile(); // delete unnecessary files
    void closeAll(); // delete unnecessary files after rosbag play
    void slot_roscore_output();
    void checkbag();  // info
    void checkbagC(); // check
    //=====================

    QString DatatimeDir;  //current time -> for mkdir
    QString DatatimeabsoDir;  // current time -> for mkdir -> valid path

    //history of the datasets=======
    QString HistoricalDir1 = ""; 
    QString HistoricalDir2 = "";
    QString HistoricalDir3 = "";
    QString currentpath;
    QDir dir;
    void readHistoricalData();
    //==============================

    // for the echo -> echo twice means current stage is completed
    int playbagIndex = 0;
    int paramIndex = 0;
    // ============================================================

    time_t now;

    QString aa; // for ros
    //QString aa = "..\\setup.bat";


    void showCloudProperties();

    treatment treat;
    report* reportPDF;

private Q_SLOTS:

    
    // menu bar's buttons ========
    void quit_mainwindow();
    void minimize_mainwindow();
    void max_mainwindow();
    //===========================

    // switching between the UIs===
    void on_process_clicked();
    void on_3dview_clicked();
    void btnprocess_clicked();
    void btn3dview_clicked();
    //=============================

    //void on_btnAbout_clicked(); //About page
    void displayMainwindow(); // show main window

    void slot_timeError(QString); // when time is gone

    // Process page, open btn, open file after RUN
    void on_btnOpen_clicked1();
    void on_btnOpen_clicked2();
    void on_btnOpen_clicked3();
    
    // Process page, display btn, file to be shown in the 3d view page
    void on_btnDisp_clicked1();
    void on_btnDisp_clicked2();
    void on_btnDisp_clicked3();


    void on_btnRun_clicked(); // run button.
    void on_btnRun_Close_clicked(); // stop button

    // for the 3 steps ===============
    void slot_baginfo_output(); // rosbag info
    void slot_bagcheck_output(); // rosbag check
    void slot_startBag_output(); // rosbag play + rosbag record
    void slot_D3_merge_output(); // hilbert_h_merge.exe
    void slot_roscore();
    void slot_checkYaml();
    // ===============================

    // 3d view =================================================
    void initOSG(); // show point cloud
    void on_setBg_changed(); // backgound color
    void on_selectCloud_clicked(QTreeWidgetItem* a , int b); // click the cloud on the treewidget
    void on_btndelCloud_clicked(); // delete btn
    void on_btnView1Cloud_clicked(); // restore clouds pos
    void on_btnView2Cloud_clicked(); // restore clouds pos
    void on_btnView3Cloud_clicked(); // restore clouds pos
    void on_btnView4Cloud_clicked(); // restore clouds pos
    void on_btnView5Cloud_clicked(); // restore clouds pos
    void on_btnView6Cloud_clicked(); // restore clouds pos
    void on_point_clicked(); // click point(s) from the cloud
    void on_photo_clicked(); // show 3 pics
    void slot_pointsize(QString); // point size
    void slot_colormap(int); // point size
    void slot_color(int); // point size
    void on_mesh_clicked();
    void on_rnoise_clicked();
    void on_trim_clicked();
    void on_move_clicked();
    void on_mclose_clicked();
    void on_target_clicked();
    void on_rclose_clicked();
    void on_rplane_clicked();
    void on_rplanesave_clicked();
    void on_rtrim_clicked();
    void on_treat_clicked();
    void treat_cloud(QString);
    void start_treat_cloud(QString);
    void on_meclose_clicked();
    void on_meposs_clicked();
    void on_meaf_clicked();
    void on_mess_clicked();
    void on_merge_clicked();
    void on_help_clicked();
    void on_tclose_clicked();
    void on_screenshot_clicked();
    void on_screenshotclose_clicked();
    void on_output_clicked();
    void on_oclose_clicked();
    void on_shader_clicked();
    //==========================================================

    //void slot_spd(QString); // play speed
    


private:
    Ui::Form ui;
    QProcess* processCore = NULL; // start roscore
    QProcess* processCore1 = NULL; // double check if roscore started
    QProcess* processCheck = NULL; // 1/3
    QProcess* processCheckC = NULL; // 1/3
    QProcess* processStart = NULL; // 2/3
    QProcess* processRecord = NULL; //2/3
    QProcess* processM = NULL;     //3/3
    QProcess* processD = NULL;  // del
    QProcess* processCl = NULL; // close
    QProcess* processMkdir = NULL; // create folder
    QProcess* cmd_yaml = NULL;

    bool running = false; // check if data is running

    int count = 1; // stage 3/3

    int frame_size = 0; // each bag's total frame no.

    // for 3d view ===================
    osgQOpenGLWidget* osgWidget;
    osgview* osgView;
    void showCoor();
    void showDist();
    QTreeWidgetItem* prev_cloud=NULL;
    void de_select_cloud(); // when not clicking on the cloud from the treewidget
    void show_select_cloud(); // clicked the cloud from the treewidget
    void different_view(int x); // x: 1 to 6 == front back top bot left right
    int current_color = 0;
    osg::Geode* temp_geode;
    void move_cloud(bool x);
    findNode* finder;
    void planeextraction();
    bool check_plane_extraction = false; // if plane is selected
    void add_widgettree(QTreeWidgetItem* parent, QString name);
    void get_pov(osg::Vec3d viewDirection, osg::Vec3d upDirection, osg::ref_ptr<osg::Geode> geode1, float dist);
    //QString treat_path;
    void to_mesh(int index); // Mesh after one the of 3 btns is clicked
    int mesh_index;
    void set_cloud_prop(QString name, int points); // set new cloud property
    screenshotHandler* screenshotHandlerObj; // screen shot
    // ===============================

    // for max-min btn=========
    int prev_width;
    int prev_height;
    bool max_pressed = false;
    //==========================


    // loading (x function is running)
    QLabel* loadinglabel;
    void initLoading();
    void startLoading();
    void stopLoading();
    //===========================

    string getTempFolder();

    osgLodHandler* lodHandler;

protected:
    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);

    int aa1 = 0;

};

#endif // GUI_H