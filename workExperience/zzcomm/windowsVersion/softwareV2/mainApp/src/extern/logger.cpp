#include "../include/logger.h"
#include <qmutex.h>

logger::logger()
{
}

logger::~logger()
{
}

void logger::set_log(log_type type, const std::string& msg,QListWidget *widget)
{
    static QMutex mutex;
    mutex.lock();
    switch (type) {
    case log_type::Info:
        widget->addItem(QString::fromStdString("[INFO]  " + msg));
        // set to black will not work if it is dark mode
//            ui->listWidgetLog->item(ui->listWidgetLog->count() - 1)->setForeground(Qt::black);
        break;
    case log_type::Warning:
        widget->addItem(QString::fromStdString("[WARNING]  " + msg));
        widget->item(widget->count() - 1)->setForeground(Qt::darkRed);
        break;
    case log_type::Error:
        widget->addItem(QString::fromStdString("[ERROR]  " + msg));
        widget->item(widget->count() - 1)->setForeground(Qt::red);
        break;
    default: break;
    }

    widget->scrollToBottom();
    mutex.unlock();

}
