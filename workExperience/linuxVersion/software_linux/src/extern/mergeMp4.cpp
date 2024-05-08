#include "../include/include_gui/mergeMp4.h"
#include "../include/include_gui/widget_newProcess.h"

#include <QFile>
#include <QDir>
#include <iostream>
#include <qfileinfo.h>

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
    parent_->setLog(tr("Checking/Merging Mp4 file(s).."));
    parent_->setProgressBar(5);
    parent_->labelRunning->setText("running..");
    parent_->startWindowTimer(); // for timing

    if (l.size() == 1) {
        parent_->setFinalMp4(l.at(0));
        emit(_startColourise());
        return;
    }

    //if (!QDir("..\\temp").exists()) {
    //    QDir().mkdir("..\\temp");
    //}
    // QString output = "../temp/output_mp4.mp4";   
    // QFile file(output); // remove if exists
    // file.remove();

    QString forOutputPath = parent_->mp4FilePath;
    std::cout << forOutputPath.toStdString() << std::endl;
    QString output = forOutputPath + "/output_mp4.mp4";
    //output = output.replace('/', '\\');
    std::cout << output.toStdString() << std::endl;

    QFile fi(output);
    if (fi.exists()) {
        fi.remove();
        std::cout << "duplicated output mp4 file removed" << std::endl;
    }


    QString allFiles;
    for (QString a : l) {
        allFiles += a + " ";
    }

    processMp4 = new QProcess;
    //processMp4->start("cmd.exe");
    processMp4->start(QString("../Lib/company/mp4_merge " + allFiles +" "+ output + "\r\n").toUtf8());
    std::cout << QString("../Lib/company/mp4_merge " + allFiles +" "+ output + "\r\n").toStdString() << std::endl;

    // QFileInfo fi("../temp/output_mp4.mp4");
    // QString absPath = fi.absoluteFilePath();
    std::cout << "merged mp4 abs path: " << output.toStdString() << std::endl;
    parent_->setFinalMp4(output);
    connect(processMp4, SIGNAL(readyReadStandardOutput()), this, SLOT(_mp4result()));

}

void mergeMp4::_mp4result()
{
    QString str = QString(processMp4->readAllStandardOutput());
    int check = str.indexOf("Merge done:");
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
            emit(_startColouriseMultiMp4());
            running = false;
        }
    }
}

void mergeMp4::closeBtnPressed() {

    running = false;

    QProcess::startDetached("killall mp4_merge");
    std::cout << "mp4_merge closed" << std::endl;

    if (processMp4 != NULL) {
        disconnect(processMp4, 0, 0, 0);
        processMp4->kill();
        processMp4 = NULL;
        std::cout << "mp4 x " << std::endl;
    }
    this->close();

}