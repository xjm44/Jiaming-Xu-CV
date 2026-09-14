#include "src/include/include_gui/mainApp.h"
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include <qdir.h>
#include <QSettings>
#include <QFile>
#include <QProgressDialog>
#include <QThread>

#include "src/include/include_gui/dialog_manual.h"

#define VERSION_NUMBER "3.6.25.725001"

//std::vector<std::string> splitString(const std::string& str) {
//    std::istringstream iss(str);
//    std::vector<std::string> tokens;
//    std::string token;
//    while (iss >> token) {
//        if (!token.empty()) {
//            tokens.push_back(token);
//        }
//    }
//    return tokens;
//}

int main(int argc, char *argv[])
{

    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QApplication a(argc, argv);


    QString output = "..\\Lib\\company\\setting.ini";
    QSettings setting(output, QSettings::IniFormat);
    //setting.setValue("version_number", "3.6.15.703304"); 
    setting.setValue("version_number", VERSION_NUMBER);
    //setting.setValue("kp", 100110110);
    //std::cout << "kp " << setting.value("kp").toInt() << std::endl;

    // splash screen
    const std::string file = ":/company/process/company_logo.png"; 
    QPixmap pixmap(QString::fromStdString(file));
    //QSplashScreen splash(pixmap.scaledToWidth(300), Qt::WindowStaysOnTopHint);
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    QElapsedTimer splashTimer;
    splashTimer.start();
    splash.show();
    //splash.showMessage("  Starting mainApp Application...",1,Qt::red);
    QApplication::processEvents();


    // start a ros process
    QString aa = "..\\setup.bat";
    QProcess* processCore1 = new QProcess;
    processCore1->start("cmd.exe");
    processCore1->write(QString(aa + "\r\n").toUtf8());
    processCore1->write(QString("roscore \r\n").toUtf8());
    //====================

    mainApp* w = new mainApp;
    w->lang = linguist::getLinguistInstance();
    //w->lang->changeLanguage(Language::en_); // starting with english
    //w->languageStorage(1);
    if (w->getLanguage() == 1) {
        w->lang->changeLanguage(Language::en_);
        w->retranslateUi(w);
    }
    else if (w->getLanguage() == 2) {
        w->lang->changeLanguage(Language::zh_);
        w->retranslateUi(w);
    }

    //w->setVersion(QString("3.6.15.703304"));
    w->setVersion(setting.value("version_number").toString());

    //int check_login = w->login_page.login_checker.checkIdentity();
    int check_login = w->login_page_one.login_checker_one.checkAuthentity();
    std::cout << "t: " << check_login << std::endl;
    std::cout << "r: " << w->login_page_one.login_checker_one.remainingTime << std::endl;



    //we want the splash screen to be visible for a minimum amount of time
    while (splashTimer.elapsed() < 1500) {   // in milliseconds
        splash.raise();
        QApplication::processEvents(); //to let the system breath!
    }
    splash.finish(w);

    if (check_login == 6) {
        w->show();
    }
    else {
        w->showLoginPage();
    }

    return a.exec();

}