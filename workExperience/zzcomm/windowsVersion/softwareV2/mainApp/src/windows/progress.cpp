#include "../include/progress.h"
#include "../include/mainwindow.h"
#include <iostream>

progress::progress(mainwindow* window)
    : QDialog(window)
{

    ui.setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // hide the window's title bar
    this->setWindowIcon(QIcon(":/process/process/logo.png"));


}


progress::~progress() {
    std::cout << " progress window down " << std::endl;
}

void progress::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        move1 = true;
        lastPos = event->globalPos() - pos();
    }

    return QDialog::mousePressEvent(event);

}

void progress::mouseMoveEvent(QMouseEvent* event)
{
    if (move1 && (event->buttons() && Qt::LeftButton) && (event->globalPos() - lastPos).manhattanLength() > QApplication::startDragDistance()) {
        move(event->globalPos() - lastPos);
        lastPos = event->globalPos() - pos();
    }
    return QDialog::mouseMoveEvent(event);
}

void progress::mouseReleaseEvent(QMouseEvent* event)
{
    move1 = false;
}