#ifndef WINDOWLOGIN_H
#define WINDOWLOGIN_H

#include <QDialog>
#include "ui_login.h"
#include <QPushButton>
#include <QLabel>
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
#include "../control/tool.h"
#include "../include/login.h"
#include <iostream>
#include <QtWidgets/qmessagebox.h>
#include <QProcess>

#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>



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


    // about license==========================
    QLabel* datasetlabel; // error message
    QPushButton* datasetbtn; // error image
    //========================================

    void swapError(int a); // for login


private:
    Ui::Dialog ui;


public slots:
    void slot_updataUI(); // error to be shown if license is invalid

    void slot_setTime(); // set user duration - for testing only


Q_SIGNALS:
    void enter_signal(); // liscene is valid

};


#endif // WINDOWLOGIN_H