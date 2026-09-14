#pragma once
#include <QMainWindow>
#include <QtWidgets/QMainWindow>
#include "ui_mainApp.h"
#include <QPropertyAnimation>
#include <QScrollArea>

#include "../../../resources/language/linguist.h"
#include "../include_process/loginWindow.h"

// //#include "dialog_gcp_init.h"
// //#include "dialog_gcp.h"
// //#include "dialog_gcp_merge.h"
// //#include "Yuan.h"

#include "widget_newProcess.h"
#include "widget_startProcess.h"
#include "stopWatch.h"

#include "mainwindowSetting.h"
// class QLabel;
// class QProgressBar;
// class QPushButton;



class mainApp : public QMainWindow, public Ui::mainAppClass
{
    Q_OBJECT

public:
    mainApp(QWidget *parent = nullptr);
    ~mainApp();
    
    QProcess* rootCore;


    QString getFileName(QString);
    QString getFilePath(QString a);
    bool checkFilePath(QString); // check for path's validity
//     void setStatus(std::string x){ Ui::mainAppClass::statusBar->showMessage(tr(x.c_str())); }
//     void setStatusWTime(std::string x) { Ui::mainAppClass::statusBar->showMessage(tr(x.c_str()),2000); }

    // for login 
    windowLogin login_page;
    void showLoginPage();

    // undone or done - for running bag in line up
    bool availableForBagRunning;
    void checkAllNewProcess();     
    // check for first colourisation
    bool availableForColourRunning;

    void hideModelProperty(bool); // if no model


    // for multi languages ===================
    linguist* lang;
    // for language storage
    void languageStorage(int);
    int getLanguage();

    // setting
    mainwindowSettingDialog* settings;

    // for edit the newProcess list
    std::vector<widget_newProcess*> getNewProcessList() { return newProcess_list; }
    void removeNewProcess(QString);

protected:
    void setToolBar();     // layout of the tool buttons
    void addSpacer(int i); // for the center buttons in the toolbar
    void setConnection();  // all signals and slots
    void init();           // basic layouts etc.

    

protected:
//     void paintEvent(QPaintEvent*); // positioning
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

    // new process
    std::vector<widget_newProcess*> newProcess_list;    // cannot be killed : store all the new process info : press view - show name, param, log
    QWidget* processList = new QWidget;                            // for scroll area
    QVBoxLayout* processListLayout = new QVBoxLayout(processList); // for scroll area
    widget_newProcess* new_process;    
    int new_process_name;
    // ===============

    // store bag files
    std::vector<QString> bagFiles;

    bool reminderBeforeRunningBag; // reminder for running bag
    QString versionNo;

private Q_SLOTS:
    // closeevent
    void closeApp();
    // login
    void displayMainwindow();            // this ui
    void slot_timeRemaining(QString);    // the remaing time of the licence
    void slot_loginPgLanguage(int x);    // when login page changes language

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


//     //void displayMainwindow();            // this ui
//     //void slot_timeRemaining(QString);    // the remaing time of the licence
//     //void on_btnProcess_clicked();        // process btn
//     //void slot_loginPgLanguage(int x);    // when login page changes language
//     //void processGui_closed();            // process ui closed

    //// tool buttons
    void on_actionAbout_triggered();     // about btn
    void _setting_triggered();           // setting button
    void _setting_closed();


// Q_SIGNALS:
//     //void to_be_waited();                 // when loading pop-up is needed

public:
    stopWatch w;
    void startTimer();
    std::vector<std::string> getTimer(); // seconds only, for each stage/function

    void stopTimer();
    void setVersion(QString a) { versionNo = a; }
};
