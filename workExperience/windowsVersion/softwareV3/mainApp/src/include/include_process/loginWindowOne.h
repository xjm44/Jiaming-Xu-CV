#pragma once

#include <QDialog>
#include "ui_loginWindowOne.h"
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QApplication>
#include <QRect>
#include <QPaintEvent>
#include <QPainter>
#include <QMessageBox>
#include <QProcess>

#include "loginOne.h"

class loginWindowOne : public QDialog, Ui::loginWindowOneClass
{
	Q_OBJECT

public:
	loginWindowOne(QWidget *parent = nullptr);
	~loginWindowOne();

    void swapError(int a); // for login
    bool canShowErr;

    //timer t;
    loginOne login_checker_one; // check authentity

protected:
    void paintEvent(QPaintEvent*);

    // allow user to drag the window ============
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool move1 = false;
    QPoint lastPos;
    //===========================================
    //login login_checker; // check authentity
    
    

private Q_SLOTS:
    void slot_updataUI(); // error to be shown if license is invalid

    void _quit_clicked(); // exit button

    void _btnen_clicked();
    void _btnzh_clicked();

    //void errorMissingEncFile(int);

Q_SIGNALS:
    void enter_signal(); // enter button 
    void languageChanged_signal(int); // language changed
	
};
