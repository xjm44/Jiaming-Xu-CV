#include "../../include/include_process/loginWindow.h"

windowLogin::windowLogin(QWidget* parent)
    : QDialog(parent)
{

    ui.setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // hide the window's title bar
    this->setWindowTitle(QString("LICENSE WIZARD"));
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowIcon(QIcon(":/company/process/logo.png"));


    //message to be shown if the license is invalid
    datasetlabel = new QLabel;
    datasetlabel->setParent(this);
    datasetlabel->setStyleSheet("color:#ffffff;");
    datasetlabel->setWordWrap(true);
    datasetlabel->setFont(QFont("Arial", 10, 20));
    datasetlabel->setGeometry(235, 185, 350, 70);
    // the error image beside the message above
    datasetbtn = new QPushButton;
    datasetbtn->setParent(this);
    datasetbtn->setGeometry(175, 195, 50, 46);
    datasetbtn->setStyleSheet("QPushButton{border:none; }");
    datasetbtn->setEnabled(false);


    connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_updataUI()), Qt::UniqueConnection); // to show if the license entered is valid
    connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(_quit_clicked()), Qt::UniqueConnection); 
    connect(ui.pushButton_en, SIGNAL(clicked()), this, SLOT(_btnen_clicked()), Qt::UniqueConnection); 
    connect(ui.pushButton_zh, SIGNAL(clicked()), this, SLOT(_btnzh_clicked()), Qt::UniqueConnection); 

    ui.lineEdit->setText(QString::fromStdString(login_checker.serialNum)); // serial no.


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
    pix.load(":/company/process/128.png");
    //pix = pix.scaled(60 * 2, 60 * 2);
    painter1.drawPixmap(20, 40, pix);

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

        datasetlabel->setText(QString(tr("License is null, please enter the license.")));
        datasetbtn->setStyleSheet("QPushButton{border-image: url(:/company/process/icon.png);border:none;color:rgb(38, 38, 38); }");

    }
    else if (ui.lineEdit_2->text().size() == 16 && ui.lineEdit->text() == QString::fromStdString(login_checker.serialNum))
    {
        login_checker.passStr = ui.lineEdit_2->text();
        int zeEnterNUM = login_checker.dialogcheckcompany();
        swapError(zeEnterNUM);
    }
    else
    {
        datasetlabel->setText(QString(tr("License is invalid, please enter the correct license.")));
        datasetbtn->setStyleSheet("QPushButton{border-image: url(:/company/process/icon.png);border:none;color:rgb(38, 38, 38); }");
    }
}


void windowLogin::swapError(int a) {
    switch (a) {
    case 1:
        datasetbtn->setStyleSheet("QPushButton{border-image: url(:/company/process/icon.png);border:none;color:rgb(38, 38, 38); }");
        datasetlabel->setText(QString(tr("Unable to proceed. Please contact the supplier. ")));
        break;
    case 2:
        datasetbtn->setStyleSheet("QPushButton{border-image: url(:/company/process/icon.png);border:none;color:rgb(38, 38, 38); }");
        datasetlabel->setText(QString(tr("Invalid license. Please contact the supplier. ")));
        break;
    case 3:
        datasetbtn->setStyleSheet("QPushButton{border-image: url(:/company/process/icon.png);border:none;color:rgb(38, 38, 38); }");
        datasetlabel->setText(QString(tr("System time error. Please update the system time. ")));
        break;
    case 4:
        datasetbtn->setStyleSheet("QPushButton{border-image: url(:/company/process/icon.png);border:none;color:rgb(38, 38, 38); }");
        datasetlabel->setText(QString(tr("License expired. Please contact the supplier. ")));
        break;
    case 6:
        emit enter_signal();
        break;
    }

}

void windowLogin::_quit_clicked() {
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
    if (str1.contains("mainApp.exe"))                     // ********************* TO BE DONE (NAME MAY BE CHANGED) ***************************
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("mainApp.exe"));

    }

    process.close();

    QApplication* app;
    app->exit(8888888);
}

void windowLogin::_btnen_clicked()
{
    emit languageChanged_signal(1);
    ui.retranslateUi(this);
    ui.lineEdit->setText(QString::fromStdString(login_checker.serialNum)); // serial no.
}

void windowLogin::_btnzh_clicked()
{
    emit languageChanged_signal(2);
    ui.retranslateUi(this);
    ui.lineEdit->setText(QString::fromStdString(login_checker.serialNum)); // serial no.
}

windowLogin::~windowLogin()
{
    std::cout << " login window down " << std::endl;
}