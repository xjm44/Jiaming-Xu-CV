#include "src/include/mainwindow.h"
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QElapsedTimer>

#include "src/include/gcp_transformation.h"
#include "src/include/gcp.h"

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);
    //// splash screen
    //const std::string file = "aa2.png";
    //QPixmap pixmap(QString::fromStdString(file));
    //QSplashScreen splash(pixmap.scaledToWidth(200), Qt::WindowStaysOnTopHint);
    //QElapsedTimer splashTimer;
    //splashTimer.start();
    //splash.show();
    //splash.showMessage("  Starting Main Application...");
    //QApplication::processEvents();

    
    mainwindow* w = new mainwindow;


    // start a ros process
    QString aa = "..\\setup.bat";
    //w->processGui->processCore1 = new QProcess;
    //w->processGui->processCore1->start("cmd.exe");
    //w->processGui->processCore1->write(QString(aa + "\r\n").toUtf8());
    //w->processGui->processCore1->write(QString("roscore \r\n").toUtf8());
    QProcess* processCore1 = new QProcess;
    processCore1->start("cmd.exe");
    processCore1->write(QString(aa + "\r\n").toUtf8());
    processCore1->write(QString("roscore \r\n").toUtf8());
    //====================



    ////we want the splash screen to be visible for a minimum amount of time
    //while (splashTimer.elapsed() < 2500) {   // in milliseconds
    //    splash.raise();
    //    QApplication::processEvents(); //to let the system breath!
    //}


    w->lang = linguist::getLinguistInstance();
    w->lang->changeLanguage(Language::en_); // starting with english
    int check_login = w->login_page.login_checker.checkIdentity();
    //splash.finish(w);

    if (check_login == 6) {
        w->show();
    }
    else {
        w->showLoginPage();
    }

    return a.exec();
}
