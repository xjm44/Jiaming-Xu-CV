#include "../include/windowLogin.h"



windowLogin::windowLogin(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // hide the window's title bar
    this->setWindowTitle(QString("LICENSE WIZARD"));
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowIcon(QIcon("../resources/images/titlelogo.png"));

    //editTime* et = new editTime(this);
    connect(ui.btnset, SIGNAL(clicked()), this, SLOT(slot_setTime()));



}
void windowLogin::paintEvent(QPaintEvent* event)
{
    
    //make rounded corners of the login page
    QPainter painter1(this);
    painter1.setRenderHint(QPainter::Antialiasing); // Anti-aliasing;
    painter1.setBrush(QBrush(QColor(38, 38, 38)));
    painter1.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter1.drawRoundedRect(rect, 20, 20);

    // Logo at the top left side
    QPixmap pix;
    pix.load("../resources/images/titlelogo.png");
    pix = pix.scaled(46.5 * 2, 52.5 * 2);
    painter1.drawPixmap(31.9, 38.2, pix);

}


void windowLogin::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        move1 = true;
        lastPos = event->globalPos() - pos();
    }

    return QDialog::mousePressEvent(event);

}

void windowLogin::mouseMoveEvent(QMouseEvent* event)
{
    if (move1 && (event->buttons() && Qt::LeftButton) && (event->globalPos() - lastPos).manhattanLength() > QApplication::startDragDistance()) {
        //&& (event->globalPos()-lastPos).manhattanLength() > QApplication::startDragDistance()
        move(event->globalPos() - lastPos);
        lastPos = event->globalPos() - pos();
    }
    return QDialog::mouseMoveEvent(event);
}

void windowLogin::mouseReleaseEvent(QMouseEvent* event)
{
    move1 = false;
}


void windowLogin::slot_setTime() {

    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (!(re.exactMatch(ui.editdays->text()) && re.exactMatch(ui.edithours->text()) && re.exactMatch(ui.editminutes->text()))) {
        QMessageBox::information(NULL, "Error", "Only Integers");
    }
    else {

        int days_to_sec = ((ui.editdays->text()).toInt() * 24);
        int hours_to_sec = ((ui.edithours->text().toInt() + days_to_sec) * 60);
        int mins_to_sec = (ui.editminutes->text().toInt() + hours_to_sec) * 60;
        int total = days_to_sec + mins_to_sec + days_to_sec;

        if (globalzzenter->set_time(total,ui.lineEdit->text()) && total>0) {
            ui.lineEdit_2->setText(globalzzenter->passStr);

        }
        else {
            QMessageBox::information(NULL, "Error", "Set Failed");
        }
    }

}

windowLogin::~windowLogin()
{
    qDebug() << "~windowLogin()";
    //delete &ui;
}