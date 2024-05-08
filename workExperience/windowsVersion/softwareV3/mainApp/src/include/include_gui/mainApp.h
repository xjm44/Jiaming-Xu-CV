#pragma once
#include <QMainWindow>
#include <QtWidgets/QMainWindow>
#include "ui_mainApp.h"
#include <QPropertyAnimation>
#include <QScrollArea>
#include <easy3d/core/types.h>
#include <easy3d/renderer/shape.h>
#include <easy3d/util/stop_watch.h>

#include <QtWidgets/QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <GL/GLU.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <QDir>

#include "../../resources/language/linguist.h"
//#include "../include_process/loginWindow.h"
#include "../include_process/loginWindowOne.h"


#include "../include_easy3d/viewer.h" 
#include "../include_easy3d/widgets/widget_model_list.h" 
#include "../include_easy3d/widgets/widget_drawable.h" 
#include "../include_easy3d/widgets/widget_drawable_points.h" 
#include "../include_easy3d/widgets/widget_global_setting.h" 
#include "../include_easy3d/dialog/sampling.h"
#include "../include_easy3d/widgets/widget_drawable_triangles.h" 
#include "../include_easy3d/dialog/dialog_photo.h"
#include "../include_easy3d/dialog/dialog_poisson_reconstruction.h"
//
//#include "dialog_gcp_init.h"
//#include "dialog_gcp.h"
//#include "dialog_gcp_merge.h"
//#include "Yuan.h"
//#include "dialog_manual.h"

#include "widget_newProcess.h"
#include "widget_startProcess.h"
#include "stopWatch.h"
#include "terminal.h"
//#include "dialog_gcp.h"

#include "mainwindowSetting.h"
#include "startFolder.h"

class QLabel;
class QProgressBar;
class QPushButton;

class PaintCanvas;
class WidgetPointsDrawable;
class WidgetLinesDrawable;
class WidgetTrianglesDrawable;
class WidgetModelList;


namespace easy3d {
    class Model;
    class Drawable;
}

class mainApp : public QMainWindow, public Ui::mainAppClass
{
    Q_OBJECT

public:
    mainApp(QWidget *parent = nullptr);
    ~mainApp();
    
    QString getFileName(QString);
    QString getFilePath(QString);
    bool checkFilePath(QString); // check for path's validity

    void setStatus(std::string x){ Ui::mainAppClass::statusBar->showMessage(tr(x.c_str())); }
    void setStatusWTime(std::string x) { Ui::mainAppClass::statusBar->showMessage(tr(x.c_str()),2000); }

    // for login 
    //windowLogin login_page;
    void showLoginPage();
    loginWindowOne login_page_one;


    // undone or done - for running bag in line up
    bool availableForBagRunning;
    void checkAllNewProcess();     
    // check for first colourisation
    bool availableForColourRunning;

    // for opengl
    // get viewer
    PaintCanvas* viewer() { return viewer_; }
    void updateRenderingPanel();  // rendering property panel only
    WidgetModelList* widgetModelList() const;
    void activeDrawableChanged(easy3d::Drawable*);

    void hideModelProperty(bool); // if no model

    void updateUi();                            // entire ui: window tile, rendering panel, model panel

    // for point clicking
    bool pointClick;
    easy3d::vec4 prevBgColor;

    // for multi languages ===================
    linguist* lang;

    // setting
    mainwindowSettingDialog* settings;

    // for language storage
    void languageStorage(int);
    int getLanguage();

    // for edit the newProcess list
    std::vector<widget_newProcess*> getNewProcessList() { return newProcess_list; }
    void removeNewProcess(QString);

    // for terminal
    bool checkCmd() { return cmdAvailable; }
    void addToCmd(QString);


    // for gcp manual add 
    void prepareManualGcp(QString);
    bool gcpManual;
    void toAddxyzGcp(double,double,double);

protected:
    void setToolBar();     // layout of the tool buttons
    void addSpacer(int i); // for the center buttons in the toolbar
    void setConnection();  // all signals and slots
    void init();           // basic layouts etc.

//private:
//    // for login 
//    windowLogin login_page;
    

protected:
    void paintEvent(QPaintEvent*); // positioning
    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);
    void closeEvent(QCloseEvent* event);


private:
    // animation
    bool sideMenuClosing; // check side menu if clicked
    QPropertyAnimation *animationSideMenu; // side menu animation
    bool processDetailClosing; // check process detail if clicked
    // ==========

    void show_hide_Buttons(bool);               // buttons -> grey / original 

    // PROCESS  
    // start process
    widget_startProcess* start_process; // can be killed : emit param, bag, files only
    startFolder* start_folder;
    void folder_process(QDir);

    // new process
    std::vector<widget_newProcess*> newProcess_list;    // cannot be killed : store all the new process info : press view - show name, param, log
    QWidget* processList = new QWidget;                            // for scroll area
    QVBoxLayout* processListLayout = new QVBoxLayout(processList); // for scroll area
    widget_newProcess* new_process;   
    int new_process_name;
    // ===============

    // store bag files
    std::vector<QString> bagFiles;

    // sampling
    sampling* samp_;

    // photo
    DialogPhoto* photo;

    //mesh 
    DialogPoissonReconstruction* mesh;

    // terminal 
    terminal* cmd_;
    bool cmdAvailable; // for cmd

private Q_SLOTS:

    // login
    void displayMainwindow();            // this ui
    void slot_timeRemaining(QString);    // the remaing time of the licence
    void slot_loginPgLanguage(int x);    // when login page changes language
    void slot_timeRemaining(int);       // the remaing time of the licence - NEW
    // animation
    void _sideMenuButton();                     // left side menu button
    void _sideMenuAnimation(const QVariant& val);
    void _processDetailButton();                // process details button
    //==========

    // add process
    void _addProcess();     // add a process (in the tab widget)


    // start process
    void _startProcess();     // start btn pressed
    void _startProcessClose();// start_process closed
    //void _startProcessCloseNonEmpty(); // start_process closed - non empty input


    //void displayMainwindow();            // this ui
    //void slot_timeRemaining(QString);    // the remaing time of the licence
    //void on_btnProcess_clicked();        // process btn
    //void slot_loginPgLanguage(int x);    // when login page changes language
    //void processGui_closed();            // process ui closed

    //// tool buttons
    void on_actionAbout_triggered();     // about btn
    //void _english_triggered();           // change language: english
    //void _chinese_triggered();           // change language: chinese
    void _edl_triggered(bool b);         // edl btn
    //void _global_shadow_toggled(bool);   // global toggled shadow -> edl btn
    //void _global_ssao_toggled(bool);     // global toggled ssao
    void _global_cplane_toggled(bool);   // global toggled clipping plane
    void _centerPivot_triggered(bool);   // center pivot show/hide
    //void _classic_triggered();           // style : white 
    //void _dark_triggered();              // style : dark 
    void _open_triggered();              // open button
    bool _save_triggered();              // save button
    void _del_triggered();               // delete button
    //void computeHeightFieldX();          // x-axis color
    //void computeHeightFieldY();          // y-axis color
    //void computeHeightFieldZ();          // z-axis color
    //void computeHeightField();           // xyz-axis color
    void _point_triggered(bool);         // point button
    void _trim_triggered(bool);          // segment button
    void _up_triggered();                // pov : up
    void _down_triggered();              // pov : down
    void _front_triggered();             // pov : front
    void _back_triggered();              // pov : back
    void _left_triggered();              // pov : left
    void _right_triggered();             // pov : right
    void _screenshot_triggered();        // screen shot
    //void _estNormal_triggered();         // compute normal
    void _manual_triggered();      // user guide
    void _sampling_triggered();          // downsampling
    void _samplingClosed();
    void _mesh_triggered();              // mesh: poisson
    void _mesh_closed();
    void _photo_triggered();             // click on the point -> show pic
    void _photo_proceed(QString img, QString csv, int a); // begin photo function
    void _photo_cancel();                // end photo function 
    void _setting_triggered();           // setting button
    //// gcp
    //void _gcp_triggered();                      // choose param
    //void _gcp_getGcp();                         // get all control points automatically
    //void _gcp_addGcp();                         // add gcp manually
    //void _gcp_delGcp(int);                      // delete gcp manually
    //void _gcp_confirmGcp();                     // confirm btn: output = .txt
    //void _gcp_draggedList();                    // drag the list manually
    //void _gcp_edittedGcp();                     // save after input rx ry rz
    //void _gcp_geographicCoordinateConversion(); // change coordinate
    //void _gcp_automaticMerging();               // merge the clouds: show notification
    //void _gcp_merge_triggered();                // merge the clouds: using 2 txt
    //void _gcp_merging(std::vector<Yuan>);       // merge the clouds: run function and generate new .ply
    //void _gcp_eliOffsetError();                 // eliminate offset error

    //// for show/hide docks in qmenu
    //void processDock_triggered();
    //void treeDock_triggered();
    //void propertyDock_triggered();
    //void consoleDock_triggered();
    //void dockProcessClosed(bool);        // true = show, false = hide
    //void dockTreeClosed(bool);           // true = show, false = hide
    //void dockPropertyClosed(bool);       // true = show, false = hide
    //void dockConsoleClosed(bool);        // true = show, false = hide
    //void dockConsoleClear();             // clear console contents
    ////=============================

    //void setBackgroundColor();           // background color

    //void updateProgress();               // for the loading pop-up

    // setting dialog
    void _setting_closed(); 

    //// animation
    //void _animation();

    // terminal 
    void slot_cmd();
    void _cmd_closed();

    // drag folder
    void _folder_triggered(QString);
    void _folder_exit();

Q_SIGNALS:
    //void to_be_waited();                 // when loading pop-up is needed
    void _toAddxyzGcp(double x, double y, double z); // gcp manual

private:
    PaintCanvas* viewer_;
    
    WidgetPointsDrawable* widgetPointsDrawable_; // property - points
    WidgetGlobalSetting* widgetGlobalSetting;    // property - global
    WidgetTrianglesDrawable* widgetTrianglesDrawable_; // property - mesh

    //easy3d::StopWatch w;
    // animation - TO BE DELETED ***
    void _animation();

    bool reminderBeforeRunningBag; // reminder for running bag

    QString versionNo;




public:
    void startTimer();
    std::string getTimer();
    easy3d::Model* open(const std::string& file_name);

    stopWatch wP;
    void startTimerP();
    std::vector<std::string> getTimerP(); // seconds only, for each stage/function
    void stopTimerP();

    void setVersion(QString a) { versionNo = a; }
};
