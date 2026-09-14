#ifndef WINDOWLOGIN_H
#define WINDOWLOGIN_H

#include "login.h"
#include <QDialog>
#include "ui_login.h"
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QApplication>
#include <QRect>
#include <QPaintEvent>
#include <QPainter>
//#include "../../control_authentity/tool.h"
#include <QMessageBox>
#include <QProcess>

// for login
class windowLogin : public QDialog
{
    Q_OBJECT

public:
    windowLogin(QWidget* parent = nullptr);
    ~windowLogin();

    void paintEvent(QPaintEvent*);

    // allow user to drag the window ============
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool move1 = false;
    QPoint lastPos;
    //===========================================

    login login_checker; // check authentity

    // about license==========================
    QLabel* datasetlabel; // error message
    QPushButton* datasetbtn; // error image
    //========================================

    void swapError(int a); // for login




private:
    Ui::Dialog ui;



private Q_SLOTS:
    void slot_updataUI(); // error to be shown if license is invalid

    //void slot_setTime(); // set user duration - for testing only
    void _quit_clicked(); // exit button

    void _btnen_clicked();
    void _btnzh_clicked();


Q_SIGNALS:
    void enter_signal(); // enter button 
    void languageChanged_signal(int); // language changed
    void close_(); // quit 

};


#endif // WINDOWLOGIN_H