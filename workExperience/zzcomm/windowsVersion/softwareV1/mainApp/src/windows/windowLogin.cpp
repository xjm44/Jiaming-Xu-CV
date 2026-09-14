#include "../include/windowLogin.h"



windowLogin::windowLogin(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // hide the window's title bar
    this->setWindowTitle(QString("LICENSE WIZARD"));
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowIcon(QIcon("../resources/images/titlelogo.png"));

    

    //message to be shown if the license is invalid
    datasetlabel = new QLabel;
    datasetlabel->setParent(this);
    datasetlabel->setStyleSheet("color:#ffffff;");
    datasetlabel->setWordWrap(true);
    datasetlabel->setFont(QFont("Arial", 10,20));
    datasetlabel->setGeometry(235, 185, 350, 70);
    // the error image beside the message above
    datasetbtn = new QPushButton;
    datasetbtn->setParent(this);
    datasetbtn->setGeometry(175, 195, 50, 46);
    datasetbtn->setStyleSheet("QPushButton{border:none; }");
    datasetbtn->setEnabled(false);


    connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_updataUI())); // to show if the license entered is invalid
    
    ui.lineEdit->setText(QString::fromStdString(globalzzenter->serialNum)); // serial no.

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


void windowLogin::slot_updataUI()
{

    if (ui.lineEdit_2->text() == "")
    {

        datasetlabel->setText("License is null, please enter the license.");
        datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");

    }
    else if (ui.lineEdit_2->text().size() == 16 && ui.lineEdit->text() == QString::fromStdString(globalzzenter->serialNum))
    {
        globalzzenter->passStr = ui.lineEdit_2->text();
        //QString strwww = globalzzenter->passStr;
        int zeEnterNUM = globalzzenter->dialogcheckcompany();

        //std::cout << "btn  str :" << strwww.toStdString() << std::endl;
        //std::cout << "btn  b :" << zeEnterNUM << std::endl;

        swapError(zeEnterNUM);
    }
    //else if (ui.lineEdit_2->text() == "100") // fake license
    //{
    //    globalzzenter->company100();
    //    ui.lineEdit_2->setText(globalzzenter->passStr);

    //}

    //else if (ui.lineEdit_2->text() == "10") // fake license
    //{
    //    globalzzenter->company10();
    //    ui.lineEdit_2->setText(globalzzenter->passStr);

    //}
    else
    {
        datasetlabel->setText("License is invalid, please enter the correct license.");
        datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");
        //QMessageBox::information(NULL, "a", QString::fromStdString(globalzzenter->serialNum));
    }
}


void windowLogin::swapError(int a) {
    //QMessageBox::information(NULL, "b", QString::number(a));
    switch (a) {
    case 1:
        datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");
        datasetlabel->setText("Unable to proceed. Please contact the supplier. ");
        break;
    case 2:
        datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");
        datasetlabel->setText("Invalid license. Please contact the supplier. ");
        break;
    case 3:
        datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");
        datasetlabel->setText("System time error. Please update the system time. ");
        break;
    case 4:
        datasetbtn->setStyleSheet("QPushButton{border-image: url(../resources/images/icon4.png);border:none;color:rgb(38, 38, 38); }");
        datasetlabel->setText("License expired. Please contact the supplier. ");
        break;
    case 6:
        emit enter_signal();
        break;
    }
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

        if (globalzzenter->set_time(total) && total>0) {
            ui.lineEdit_2->setText(globalzzenter->passStr);
        }
        else {
            QMessageBox::information(NULL, "Error", "Set Failed");
        }
    }

}

windowLogin::~windowLogin()
{
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
    if (str1.contains("mainApp.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("mainApp.exe"));
    }
    if (str1.contains("algorithm1_nrt.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt.exe"));
    }
    if (str1.contains("merge.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
    }

    process.close();

    exit(0);
    qDebug() << "~windowLogin()";
    delete& ui;
}