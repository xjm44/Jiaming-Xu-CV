#ifndef GUI_H
#define GUI_H

#include "loginWindow.h"
#include <QWidget>
#include <QMouseEvent>
#include <QMainWindow>
#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qframe.h>
#include "ui_gui.h"
#include <QDragEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QEventLoop>
#include <QSettings>
#include <QDir>
#include <QTextStream>
#include <QTreeWidgetItem>
#include <QPixmap>
#include <thread>
#include <QDesktopWidget>
#include <chrono>
#include <boost/winapi/semaphore.hpp>
#include "rtk.h"
#include "setting.h"


// for process ui
class gui : public QFrame
{
    Q_OBJECT

public:
    gui(QWidget* parent = nullptr);
    ~gui();


    void deleteProcesses(); // when whole application closes
    QProcess* processCore1 = NULL; // double check if roscore started, used in main.cpp

    void bringFront(); // bring certain widgets to the front
    void setUpProUi();// to set up the Process Ui
    void readHistoricalData(); // history of the process of the .bag files

    void quitWhenRunning(); // close window or time expired

    void setGcpChecked(bool);

protected:

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

    void setGeo();// set the geometry of each widget

    void on_btnOpen_clicked(QString a, int x); // 3 open buttons

    QString filename = ""; // .bag file

    // for the 3 steps ====
    void slot_startBag(); // 1/3
    void mergeDeal();  // 3/3
    void delBagFile(); // delete unnecessary files
    void closeAll(); // delete unnecessary files after algorithm1_nrt
    void slot_roscore_output(); // check roscore
    void checkbag(); // rosbag info
    void checkbagC(); // rosbag check
    //=====================

    QString DatatimeDir;  //current time -> for mkdir
    QString DatatimeabsoDir = "";  // current time -> for mkdir -> valid path

    ////history of the datasets=======
    QString HistoricalDir1 = "";
    QString HistoricalDir2 = "";
    QString HistoricalDir3 = "";
    QString currentpath;
    QDir dir;
    ////==============================

    // for the echo -> echo twice means current stage is completed
    int playbagIndex = 0;
    int paramIndex = 0;
    // ============================================================


    QString aa; // for ros (setup.bat)

    QFile logFile; // for algorithm1 log

    rtk Rtk; // rtk
    QString rtkTxt;
    bool rtkRunning;
    bool disableDrop;
    bool gcpChecked;

    settingParam* setParams; // ros param setting

private Q_SLOTS:


    // menu bar's buttons ========
    void quit_mainwindow();
    void minimize_mainwindow();
    void max_mainwindow();
    //===========================

    // Process page, open btn, open file after RUN
    void _btnOpen_clicked1();
    void _btnOpen_clicked2();
    void _btnOpen_clicked3();

    void _btnRun_clicked(); // run button.
    void _btnRun_Close_clicked(); // run->close button.

    // for the 3 steps ===============
    void slot_baginfo_output(); // rosbag info
    void slot_bagcheck_output(); // rosbag check
    void slot_startBag_output(); // algorithm1_nrt + rosbag record
    void slot_D3_merge_output(); // merge.exe
    void slot_roscore();  // roscore
    void slot_checkYaml(); // rosparam
    // ===============================

    void _btnRtk_clicked(); // rtk
    void _rtk_ready_(QString, QString); // rtk to go
    void _rtk_close_(); // change rtk button

    void _btnSetting_clicked(); // setting params
    void _btnSetting_close(); // setting params

private:
    Ui::Form ui;
    QProcess* processCore = NULL; // start roscore
    QProcess* processCheck = NULL; // 1/3
    QProcess* processCheckC = NULL; // 1/3
    QProcess* processStart = NULL; // 2/3
    QProcess* processRecord = NULL; //2/3
    QProcess* processM = NULL;     //3/3
    QProcess* processD = NULL;  // del
    QProcess* processCl = NULL; // close
    QProcess* processMkdir = NULL; // create folder
    QProcess* cmd_yaml = NULL; // .yaml

    bool running = false; // check if data is running

    int count = 1; // stage 3/3

    int frame_size = 0; // each bag's total frame no.

    // for max-min btn=========
    int prev_width;
    int prev_height;
    bool max_pressed = false;
    //==========================

    bool close_btn; // prevent app crash when close the UI when process is runnning

protected:
    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);
    void closeEvent(QCloseEvent* event) override;
Q_SIGNALS:
    void _closed();                 // when loading pop-up is needed

public:
    void setParams_fromSetting(int no_of_cores,
            bool loopClosure,
            int loopClosureIntervalFrames,
            bool narrowSpace,
            float minIntensityFilter,
            float lidarMaxRange,
            float lidarMinRange,
            float lidarMaxXToFilter,
            float lidarMinXToFilter,
            float translationThreshold,
            int icpMaximumIterations
            );


private:
    int no_of_cores;
    bool loopClosure;
    int loopClosureIntervalFrames;
    bool narrowSpace;
    float minIntensityFilter;
    float lidarMaxRange;
    float lidarMinRange;
    float lidarMaxXToFilter;
    float lidarMinXToFilter;
    float translationThreshold;
    int icpMaximumIterations;
    QString sensor;
    bool csv;
    bool las;
    bool rotatingFrames;

    bool change_yaml;
    void changeYaml();
    void set_init_Yaml();
};

#endif // GUI_H