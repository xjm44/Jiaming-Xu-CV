
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
#include "login.h"
#include <iostream>
#include <QtWidgets/qmessagebox.h>



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

    login l;


private:
    Ui::Dialog ui;


public slots:

    void slot_setTime(); // set user duration - for testing only


Q_SIGNALS:
    void enter_signal(); // liscene is valid

};


#endif // WINDOWLOGIN_H