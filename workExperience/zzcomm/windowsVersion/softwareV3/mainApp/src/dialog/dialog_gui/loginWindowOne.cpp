#include "../../include/include_process/loginWindowOne.h"
#include "../../include/include_process/timer.h"
#include <iostream>

loginWindowOne::loginWindowOne(QWidget *parent)
	: QDialog(parent)
    , canShowErr(false)
{
	setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // hide the window's title bar
    this->setWindowTitle(QString("LICENSE WIZARD"));
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowIcon(QIcon(":/company/process/logo.png"));


    connect(pushButtonActivate, SIGNAL(clicked()), this, SLOT(slot_updataUI()), Qt::UniqueConnection); // to show if the license entered is valid
    connect(pushButtonQuit, SIGNAL(clicked()), this, SLOT(_quit_clicked()), Qt::UniqueConnection); 
    connect(pushButtonEn, SIGNAL(clicked()), this, SLOT(_btnen_clicked()), Qt::UniqueConnection);
    connect(pushButtonZh, SIGNAL(clicked()), this, SLOT(_btnzh_clicked()), Qt::UniqueConnection);


}

loginWindowOne::~loginWindowOne()
{}

void loginWindowOne::paintEvent(QPaintEvent* event)
{

    //make rounded corners of the login page
    QPainter painter1(this);
    painter1.setRenderHint(QPainter::Antialiasing); // Anti-aliasing;
    painter1.setBrush(QBrush(QColor(43, 43, 43)));
    painter1.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter1.drawRoundedRect(rect, 20, 20);

    // top left logo
    QPixmap pixmap(":/company/process/100.png");
    labelLogo->setPixmap(pixmap);


    if (canShowErr) {
        labelError->show();
        labelErrorImage->show();
    }
    else {
        labelError->hide();
        labelErrorImage->hide();
    }

}
void loginWindowOne::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        move1 = true;
        lastPos = event->globalPos() - pos();
    }

    return QDialog::mousePressEvent(event);

}

void loginWindowOne::mouseMoveEvent(QMouseEvent* event)
{
    if (move1 && (event->buttons() && Qt::LeftButton) && (event->globalPos() - lastPos).manhattanLength() > QApplication::startDragDistance()) {
        move(event->globalPos() - lastPos);
        lastPos = event->globalPos() - pos();
    }
    return QDialog::mouseMoveEvent(event);
}

void loginWindowOne::mouseReleaseEvent(QMouseEvent* event)
{
    move1 = false;
}
void loginWindowOne::_quit_clicked() {
    std::cout << " quit clicked " << std::endl;


    QProcess process;
    QString cmdd = "tasklist";
    process.start(cmdd);
    process.waitForFinished();                   
    QString str1 = process.readAllStandardOutput();

    if (str1.contains("roscore.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("roscore.exe"));
    }
    if (str1.contains("rosmaster.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("rosmaster.exe"));
    }
    if (str1.contains("rosout.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("rosout.exe"));
    }
    if (str1.contains("python.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("python.exe"));
    }
    if (str1.contains("mainApp.exe"))                      // ********************* TO BE DONE (NAME MAY BE CHANGED) ***************************
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("mainApp.exe"));

    }

    process.close();

    QCoreApplication::quit();
}

void loginWindowOne::_btnen_clicked()
{
    emit languageChanged_signal(1);
    retranslateUi(this);
    canShowErr = false;
}

void loginWindowOne::_btnzh_clicked()
{
    emit languageChanged_signal(2);
    retranslateUi(this);
    canShowErr = false;
}

void loginWindowOne::slot_updataUI()
{
    std::cout << " activate clicked " << std::endl;
    

    if (lineEdit->text().size() == 20) {

        // check the passcode
        // first time login OR has expired before
        int res = login_checker_one.checkAuthentity(lineEdit->text(),true);    
        swapError(res);


    }
    else {
        swapError(2);

    }

    

}

void loginWindowOne::swapError(int a) {
    
    QPixmap pixmap1(":/company/process/iconS.png");
    switch (a) {
    case 1:
        labelError->setText(QString(tr("Missing files. Please contact the supplier. ")));
        labelErrorImage->setPixmap(pixmap1);
        canShowErr = true;
        break;
    case 2:
        labelError->setText(QString(tr("Invalid license. Please contact the supplier. ")));   
        labelErrorImage->setPixmap(pixmap1);
        canShowErr = true;
        break;
    case 3:
        labelError->setText(QString(tr("License expired. Please contact the supplier. ")));
        labelErrorImage->setPixmap(pixmap1);
        canShowErr = true;
        break;
    case 4:
        labelError->setText(QString(tr("Failed to decrypt. Please contact the supplier. ")));
        labelErrorImage->setPixmap(pixmap1);
        canShowErr = true;
        break;
    case 6:
        canShowErr = false;
        emit enter_signal();
        break;
    }

}
