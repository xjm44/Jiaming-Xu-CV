#include "../../include/include_gui/terminal.h"


terminal::terminal(QWidget *parent)
	: QWidget(parent)
    , move1(false)
    , maxSize(0)
{
	setupUi(this);
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint); // hide the window's title bar
    connect(btnClear, SIGNAL(clicked()), this, SLOT(btnClear_triggered()), Qt::UniqueConnection);
    connect(btnClose, SIGNAL(clicked()), this, SLOT(btnClose_triggered()), Qt::UniqueConnection);
}

terminal::~terminal()
{
}
void terminal::closeEvent(QCloseEvent* event) {
    emit(_closed());
}

void terminal::btnClose_triggered()
{
    this->close();
}
void terminal::btnClear_triggered()
{
    listWidget->clear();
}

void terminal::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        move1 = true;
        lastPos = event->globalPos() - pos();
    }

    return QWidget::mousePressEvent(event);

}
void terminal::mouseMoveEvent(QMouseEvent* event)
{
    if (move1 && (event->buttons() && Qt::LeftButton) && (event->globalPos() - lastPos).manhattanLength() > QApplication::startDragDistance()) {
        move(event->globalPos() - lastPos);
        lastPos = event->globalPos() - pos();
    }
    return QWidget::mouseMoveEvent(event);
}

void terminal::mouseReleaseEvent(QMouseEvent* event)
{
    move1 = false;
}

void terminal::add(QString s) {

//    static QMutex mutex;
//    mutex.lock();
//    switch (type) {
//    case log_type::Info:
//        widget->addItem(QString::fromStdString("[INFO]  " + msg));
//        // set to black will not work if it is dark mode
////            ui->listWidgetLog->item(ui->listWidgetLog->count() - 1)->setForeground(Qt::black);
//        break;
//    case log_type::Warning:
//        widget->addItem(QString::fromStdString("[WARNING]  " + msg));
//        widget->item(widget->count() - 1)->setForeground(Qt::darkRed);
//        break;
//    case log_type::Error:
//        widget->addItem(QString::fromStdString("[ERROR]  " + msg));
//        widget->item(widget->count() - 1)->setForeground(Qt::red);
//        break;
//    default: break;
//    }
//
//    widget->scrollToBottom();
//    mutex.unlock();


    // add a limit
    if (maxSize < 10000) {
        listWidget->addItem(s);
        listWidget->scrollToBottom();
        maxSize++;
    }
    else {      
        listWidget->clear();
        maxSize = 0;
    }



}