#include <QApplication>
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include <QDir>
#include <QProcess>
#include <iostream>
#include <QObject>
#include <QSettings>
#include <QFile>

#define VERSION_NUMBER "3.6.25.725001"

#include "src/include/include_gui/mainApp.h"
#include "src/include/include_process/timer.h"
#include "src/control_authentity/company.hpp"

int main(int argc, char *argv[])
{

    //QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication a(argc, argv);
    //Process::startDetached("roscore");
    mainApp* w = new mainApp;

    // start a ros process
    w->rootCore = new QProcess;
    // w->rootCore->start("sh");
    // w->rootCore->write(QString("roscore\n").toUtf8());
    w->rootCore->start("roscore\n");
    
    QString output = "../Lib/company/setting.ini";
    QSettings setting(output, QSettings::IniFormat);
    //setting.setValue("version_number", "3.6.15.703304"); 
    setting.setValue("version_number", VERSION_NUMBER);

    // splash screen ============
    const std::string file = ":/logo/process/company_logo.png";
    QPixmap pixmap(QString::fromStdString(file));
    //QSplashScreen splash(pixmap.scaledToWidth(200), Qt::WindowStaysOnTopHint);
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    QElapsedTimer splashTimer;
    splashTimer.start();
    splash.show();
    //splash.showMessage(QObject::tr("Starting mainApp.."));
    QApplication::processEvents();

    // be visible for a minimum amount of time
    while (splashTimer.elapsed() < 1500) {   // in milliseconds
        splash.raise();
        QApplication::processEvents(); //to let the system breath
    }
    splash.finish(nullptr);
    //====================

    w->lang = w->lang->getLinguistInstance();
    if (w->getLanguage() == 1) {
        w->lang->changeLanguage(Language::en_);
        w->retranslateUi(w);
    }
    else if (w->getLanguage() == 2) {
        w->lang->changeLanguage(Language::zh_);
        w->retranslateUi(w);
    }
    w->setVersion(setting.value("version_number").toString());
    int check_login = w->login_page.login_checker.checkIdentity();
    if (check_login == 6) {
         w->show();
    }
    else {
        w->showLoginPage();
    }


    return a.exec();
}