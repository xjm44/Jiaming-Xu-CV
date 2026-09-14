#pragma once

#include "progress.h"
#include "logger.h"
#include <string>

#include <QMainWindow>

//#include <easy3d/util/progress.h>
//#include <easy3d/util/logging.h>
#include <easy3d/core/types.h>
#include <easy3d/renderer/shape.h>

#include <QtWidgets/QMainWindow>
#include "ui_productApp_newClass.h"
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <GL/GLU.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include "../../resources/language/linguist.h"
#include "process/loginWindow.h"
#include "process/processGui.h"

#include "easy3d_inc/viewer.h" 
#include "easy3d_inc/widgets/widget_model_list.h" 
#include "easy3d_inc/widgets/widget_drawable.h" 
#include "easy3d_inc/widgets/widget_drawable_points.h" 
#include "easy3d_inc/widgets/widget_global_setting.h" 
#include "easy3d_inc/widgets/widget_drawable_triangles.h" 
#include "easy3d_inc/dialog/dialog_photo.h"

#include "dialog_gcp_init.h"
#include "dialog_gcp.h"
#include "dialog_gcp_merge.h"
#include "Yuan.h"


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

enum UiStyle {
    Classic,
    Dark
};


class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = nullptr);
    ~mainwindow();


    // for login =============================
    windowLogin login_page;
    void showLoginPage();
    //========================================

    // for process ===========================
    gui* processGui;
    //========================================

    // for multi languages ===================
    linguist* lang;
    //========================================
    
    // for loading pop-up ====================
    progress* prog;
    //========================================

    // for log/console ====================
    logger* log;
    //========================================

    // for log widget
    QListWidget* getLogWidget() { return ui.logWidget; };

    // get viewer
    PaintCanvas* viewer() { return viewer_; }
    
    // ui stuff
    WidgetModelList* widgetModelList() const;
    void activeDrawableChanged(easy3d::Drawable*);

    void updateUi();                            // entire ui: window tile, rendering panel, model panel

    void updateRenderingPanel();                // rendering property panel only

    // for point clicking
    bool pointClick;
    easy3d::vec4 prevBgColor;

    int x = 0;

    // for gcp 
    bool addGcp;                     // add gcp
    easy3d::vec3 newGcpXYZ;          // add a new gcp
    QString current_pointcloud_path;
    QString current_routecloud_path;
    QString current_saving_path;   
    std::vector<Yuan> _gcp_yuan;     // store all xyzrtrxryrz
    void _gcp_addGcp_found();        // add gcp;
    int no_of_gcp;                   // to produce gcp_x
    int route_key;                   // route cloud path: check if present
    void updateList();               // update list if vector<Yuan> changed
    void saveTxt(QString filename);  // save the txt as 'filename'

private:
    Ui::productApp_newClass ui;

    void init();                                // e.g. connect() , linguist , position , etc

    void changeUiStyle(UiStyle);                // ui style 

    void show_hide_Buttons(bool);               // buttons -> grey / original  

    DialogPhoto* photo;                         // photo function

    //gcp function
    dialog_gcp_init* gcp_init;                  // gcp init (input param)
    //gcp* getGcp;                              // extract all gcps
    dialog_gcp* gcp_dialog;                     // ui for rearrange gcps
    dialog_gcp_merge* gcp_merge;                // ui for automatic merging

private:
    PaintCanvas* viewer_;
    easy3d::Model* open(const std::string& file_name);

    QString	curDataDirectory_;
    QStringList recentFiles_;

    WidgetPointsDrawable* widgetPointsDrawable_; // property - points
    WidgetGlobalSetting* widgetGlobalSetting;    // property - global
    WidgetTrianglesDrawable* widgetTrianglesDrawable_; // property - mesh


private Q_SLOTS:
    
    void displayMainwindow();            // this ui
    void slot_timeRemaining(QString);    // the remaing time of the licence
    void on_btnProcess_clicked();        // process btn
    void slot_loginPgLanguage(int x);    // when login page changes language
    void processGui_closed();            // process ui closed

    // tool buttons
    void on_actionAbout_triggered();     // about btn
    void _english_triggered();           // change language: english
    void _chinese_triggered();           // change language: chinese
    void _edl_triggered(bool b);         // edl btn
    void _global_shadow_toggled(bool);   // global toggled shadow -> edl btn
    void _global_ssao_toggled(bool);     // global toggled ssao
    void _global_cplane_toggled(bool);   // global toggled clipping plane
    void _centerPivot_triggered(bool);   // center pivot show/hide
    void _classic_triggered();           // style : white 
    void _dark_triggered();              // style : dark 
    void _open_triggered();              // open button
    bool _save_triggered();              // save button
    void _del_triggered();               // delete button
    void computeHeightFieldX();          // x-axis color
    void computeHeightFieldY();          // y-axis color
    void computeHeightFieldZ();          // z-axis color
    void computeHeightField();           // xyz-axis color
    void _point_triggered(bool);         // point button
    void _trim_triggered(bool);          // segment button
    void _up_triggered();                // pov : up
    void _down_triggered();              // pov : down
    void _front_triggered();             // pov : front
    void _back_triggered();              // pov : back
    void _left_triggered();              // pov : left
    void _right_triggered();             // pov : right
    void _screenshot_triggered();        // screen shot
    void _estNormal_triggered();         // compute normal
    void _actionManual_triggered();      // user guide
    void _sampling_triggered();          // downsampling
    void _mesh_triggered();              // mesh: poisson
    void _photo_triggered();             // click on the point -> show pic
    void _photo_proceed(QString img, QString csv, int a); // begin photo function
    void _photo_cancel();                // end photo function 

    // gcp
    void _gcp_triggered();                      // choose param
    void _gcp_getGcp();                         // get all control points automatically
    void _gcp_addGcp();                         // add gcp manually
    void _gcp_delGcp(int);                      // delete gcp manually
    void _gcp_confirmGcp();                     // confirm btn: output = .txt
    void _gcp_draggedList();                    // drag the list manually
    void _gcp_edittedGcp();                     // save after input rx ry rz
    void _gcp_geographicCoordinateConversion(); // change coordinate
    void _gcp_automaticMerging();               // merge the clouds: show notification
    void _gcp_merge_triggered();                // merge the clouds: using 2 txt
    void _gcp_merging(std::vector<Yuan>);       // merge the clouds: run function and generate new .ply
    void _gcp_eliOffsetError();                 // eliminate offset error

    // for show/hide docks in qmenu
    void processDock_triggered();
    void treeDock_triggered();
    void propertyDock_triggered();
    void consoleDock_triggered();
    void dockProcessClosed(bool);        // true = show, false = hide
    void dockTreeClosed(bool);           // true = show, false = hide
    void dockPropertyClosed(bool);       // true = show, false = hide
    void dockConsoleClosed(bool);        // true = show, false = hide
    void dockConsoleClear();             // clear console contents
    //=============================

    void setBackgroundColor();           // background color

    void updateProgress();               // for the loading pop-up

Q_SIGNALS:
    void to_be_waited();                 // when loading pop-up is needed



protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent*) override;
    void dropEvent(QDropEvent*) override;
};
