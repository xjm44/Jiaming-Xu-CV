

#include "src/include/windowLogin.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QFile>
#include "src/include/login.h"
#include "src/control/tool.h"
#include <stdio.h>
#include <QMessageBox>


bool suanfaBOOL;
login* globalzzenter;

int main(int argc, char* argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);


    QApplication a(argc, argv);
    //suanfaBOOL = false;
    login zEnter;
    globalzzenter = &zEnter;
    //globalzzenter->suanfaBOOL = suanfaBOOL;
    //int b = globalzzenter->checkIdentity();

    //gui mainPage;
    ////QMessageBox::information(NULL, "a", QString::number(b));

    //mainPage.readHistoricalData();
    //if (b == 6)
    //{
    //    mainPage.show();
    //}
    //else // if login is needed
    //{
    //    mainPage.displayDialog();
    //}
    windowLogin l;
    l.show();

    return a.exec();
}



























