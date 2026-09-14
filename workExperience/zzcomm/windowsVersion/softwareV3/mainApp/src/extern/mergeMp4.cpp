#include "../include/include_gui/mergeMp4.h"
#include "../include/include_gui/widget_newProcess.h"

#include <QFile>
#include <QDir>
#include <iostream>
#include <qfileinfo.h>
#include <QTimer>
#include <QProcess>

mergeMp4::mergeMp4(widget_newProcess* parent)
    : QWidget(parent),
    processMp4(nullptr)
    , running(false)
{
    parent_ = dynamic_cast<widget_newProcess*>(parent);
    init();
}

mergeMp4::~mergeMp4()
{}

void mergeMp4::closeEvent(QCloseEvent* event) {

}

void mergeMp4::init()
{


}

void mergeMp4::startMerging(QStringList l)
{
    //parent_->running = true;
    running = true;
    parent_->setStatus(running_status::running);
    parent_->setLog(tr("Checking/Merging video file(s).."));
    parent_->setProgressBar(5);
    parent_->labelRunning->setText("running..");
    parent_->startWindowTimerP(); // for timing

    if (l.size() == 1) {
        parent_->setFinalMp4(l.at(0));
        emit(_startColourise());
        return;
    }


    //if (!QDir("..\\temp").exists()) {
    //    QDir().mkdir("..\\temp");
    //}
    //QString output = "..\\temp\\output_mp4.mp4";   
    //QFile file(output); // remove if exists
    //file.remove();

    QString forOutputPath = parent_->mp4FilePath;
    std::cout << forOutputPath.toStdString() << std::endl;
    QString output = forOutputPath + "/output_mp4.mp4";
    output = output.replace('/', '\\');
    QString outputForMerge = parent_->addQuote(output.toStdString());
    std::cout << output.toStdString() << std::endl;


    QFile fi(output);
    if (fi.exists()) {
        fi.remove();
        std::cout << "duplicated output mp4 file removed" << std::endl;
    }


    QString allFiles;
    for (QString a : l) {
        allFiles += parent_->addQuote(a.toStdString()) + " ";
    }

    processMp4 = new QProcess;
    //processMp4->start("cmd.exe");
    //processMp4->write(QString("..\\Lib\\company\\mp4_join.exe " + allFiles + output + "\r\n").toUtf8());

    QFileInfo fapp("..\\Lib\\company\\mp4_join.exe");
    std::string app = fapp.absoluteFilePath().toStdString();

    processMp4->start(QString(parent_->addQuote(app) + " " + allFiles + outputForMerge) + "\n");

    std::cout << QString(parent_->addQuote(app) + " " + allFiles + outputForMerge + "\r\n").toStdString() << std::endl;

    //QFileInfo fi("..\\temp\\output_mp4.mp4");
    //QString absPath = fi.absoluteFilePath().replace('/', '\\');
    std::cout << "merged mp4 abs path: " << outputForMerge.toStdString() << std::endl;
    parent_->setFinalMp4(output);
    connect(processMp4, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(_Mp4ExitCode(int, QProcess::ExitStatus)));
    connect(processMp4, SIGNAL(readyReadStandardOutput()), this, SLOT(_mp4result()));
    

}
void mergeMp4::_Mp4ExitCode(int a, QProcess::ExitStatus e) { // always return 0
    //if (e == QProcess::ExitStatus::NormalExit)std::cout << "byebye" << std::endl;
    //if (e == QProcess::ExitStatus::CrashExit)std::cout << "gg" << std::endl;
    //std::cout << "exit no.: " << std::to_string(a) << std::endl;
    //if (e == QProcess::ExitStatus::NormalExit && a == 0) {
    //    std::cout << "merge mp4 successfully exits" << std::endl;
    //    //parent->windowAddToCmd(QString(tr("algorithm1 done")));

    //    QEventLoop loop;
    //    QTimer::singleShot(1000, &loop, SLOT(quit()));
    //    loop.exec();

    //    if (processMp4 != NULL) {

    //        disconnect(processMp4, 0, 0, 0);
    //        processMp4->kill();
    //        processMp4 = NULL;
    //        std::cout << "mp4 x " << std::endl;
    //    }
    //    if (running) {
    //        emit(_startColouriseMultiMp4());
    //        running = false;
    //    }
    //}
    //else {
    //    std::cout << "algorithm(merge mp4) in error." << std::endl;
    //    if (processMp4 != NULL) {

    //        disconnect(processMp4, 0, 0, 0);
    //        processMp4->kill();
    //        processMp4 = NULL;
    //        std::cout << "mp4 x " << std::endl;
    //    }
    //    running = false;

    //    //parent->windowAddToCmd(QString(tr("algorithm1 in error.")));
    //    parent_->setColorErr("merge"); 
    //    parent_->colorCrash();
    //}

    if (a == 1) {

        std::cout << "algorithm(merge mp4) in error." << std::endl;
        if (processMp4 != NULL) {

            disconnect(processMp4, 0, 0, 0);
            processMp4->kill();
            processMp4 = NULL;
            std::cout << "mp4 x " << std::endl;
        }
        running = false;

        //parent->windowAddToCmd(QString(tr("algorithm1 in error.")));
        parent_->setColorErr("merge"); 
        parent_->colorCrash();
    }

}
void mergeMp4::_mp4result()
{
    QString str = QString(processMp4->readAllStandardOutput());
    std::cout << "xxx "<< str.toStdString() << std::endl;
    int check = str.indexOf("Merge done:");
    int checkerr = str.indexOf("Merge error:");
    if (check < 0) {

    }
    else {
        if (processMp4 != NULL) {

            disconnect(processMp4, 0, 0, 0);
            processMp4->kill();
            processMp4 = NULL;
            std::cout << "mp4 x " << std::endl;
        }
        if (running) {
            std::cout << "merge mp4 successfully exits" << std::endl;
            parent_->windowAddToCmd(QString(tr("merge mp4 done")));

            emit(_startColouriseMultiMp4());
            running = false;  
        }
    }


    if (checkerr < 0) {

    }
    else {
        if (processMp4 != NULL) {

            disconnect(processMp4, 0, 0, 0);
            processMp4->kill();
            processMp4 = NULL;
            std::cout << "mp4 x " << std::endl;
        }
        if (running) {
            std::cout << "algorithm(merge mp4) in error." << std::endl;
            if (processMp4 != NULL) {

                disconnect(processMp4, 0, 0, 0);
                processMp4->kill();
                processMp4 = NULL;
                std::cout << "mp4 x " << std::endl;
            }
            running = false;

            //parent->windowAddToCmd(QString(tr("algorithm1 in error.")));
            parent_->setColorErr("merge");
            parent_->colorCrash();
        }
    }
}

void mergeMp4::closeBtnPressed() {

    running = false;

    QProcess process;
    QString cmdd = "tasklist";
    process.start(cmdd);
    process.waitForFinished();                   
    QString str1 = process.readAllStandardOutput();
    if (str1.contains("mp4_join.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("mp4_join.exe"));
    }
    process.close();
    std::cout << "mp4_joins closed" << std::endl;

    if (processMp4 != NULL) {
        disconnect(processMp4, 0, 0, 0);
        processMp4->kill();
        processMp4 = NULL;
        std::cout << "mp4 x " << std::endl;
    }
    this->close();

}