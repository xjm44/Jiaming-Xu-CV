#pragma once

#include <QDialog>
#include "ui_progress.h"
#include <QMouseEvent>

class mainwindow;
// the loading ui
class progress : public QDialog
{
    Q_OBJECT

public:
    progress(mainwindow* window);
    ~progress();


private:
    Ui::progressDialog ui;

    // allow user to drag the window ============
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool move1 = false;
    QPoint lastPos;
    //===========================================

private Q_SLOTS:
  


Q_SIGNALS:

};



