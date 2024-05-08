#include "../include/include_gui/colourise.h"
#include "../include/include_gui/widget_newProcess.h"

#include <QFile>
#include <iostream>
#include <QTimer>

colourise::colourise(widget_newProcess* parent)
    : QWidget(parent),
    colourisePc(nullptr)
    ,running(true)

{
    parent_ = dynamic_cast<widget_newProcess*>(parent);
    init();
}

colourise::~colourise()
{}

void colourise::closeEvent(QCloseEvent* event) {

}

void colourise::init()
{


}

void colourise::run()
{
    if (running) {
        parent_->setProgressBar(10);
        parent_->setLog(tr("In progress.."));
        //colorize.exe <project_dir> <point_cloud> <video> <imu_source> <traj_source> <calibration_file>[--config config_file][other options...]
        QString all_params;

        QString uncolorPts = ""; // for including uncolorised pts 
        if (parent_->getUncolorPts())uncolorPts = "true";
        else uncolorPts = "false";

        float pen = -0.001; // for Color penetration strength
        if (parent_->getPenetration().contains("low")) {
        	pen = -0.005;
        }
        else if (parent_->getPenetration().contains("normal")) {
        	pen = -0.001;
        }
        else if (parent_->getPenetration().contains("high")) {
        	pen = -0.0005;
        }
        else if (parent_->getPenetration().contains("very high")) {
        	pen = -0.0001;
        }

        // for manual inputs, to be deleted
        QString inputs;
        if (parent_->config.contains("--")) {
            inputs = parent_->config;
        }
        else {
            inputs = "";
        }


        all_params = QString("../Lib/company/colorization " + parent_->getprojDir() + " " + parent_->getpointCloud() + " " +
            parent_->getVideo() + " " + parent_->getImu() + " " + parent_->getTraj() + " " + parent_->getCali() + " " + 
            parent_->getYaml() + "") // to be changed, if yaml file needed
            + QString(
            "--frame_skip %1 --first_frame %2 --last_frame %3 --visible_range %4 --voxel_size %5 --use_default_color=%6 --hpr_gamma=%7 %8")
            .arg(parent_->getframeSkip()).arg(parent_->getfirstFrame()).arg(parent_->getlastFrame())
            .arg(parent_->getvisibleRange()).arg(parent_->getQuality()).arg(uncolorPts).arg(pen).arg(inputs);

        std::cout << all_params.toStdString() << std::endl;

        colourisePc = new QProcess;
        //colourisePc->start("cmd.exe");
        colourisePc->start(QString(all_params + "\r\n").toUtf8());

        connect(colourisePc, SIGNAL(readyReadStandardOutput()), this, SLOT(_starts()));
    }
}

void colourise::_starts() // ADD PROGRESS *******************
{
    QString str = QString(colourisePc->readAllStandardOutput());
    int check = str.lastIndexOf("Progress: ");
    std::cout << str.toStdString() << std::endl;
    if (check == 0) {
        int indexDuration = str.indexOf(check) + 10;
        QString progress = str.mid(indexDuration);
        progress = progress.simplified().section(' ', 0, 0).simplified();
        int oo = progress.toInt();


        int a = (((float)oo / 100) * (float)(88));
        a = a + 10;
        //std::cout << "progress: "<<oo << std::endl;
        if (a <= 98)
            parent_->setProgressBar(a);

        if (oo == 100) {
            QEventLoop loop;
            QTimer::singleShot(2000, &loop, SLOT(quit()));
            loop.exec();
            finished();
            return;
        }           
    }
}

void colourise::finished()
{
    if (running) {
    if (colourisePc != NULL) {

        disconnect(colourisePc, 0, 0, 0);
        colourisePc->kill();
        colourisePc = NULL;
        std::cout << "color x " << std::endl;
        running = false;
    }

    // when done
    std::vector<std::string> tempList = parent_->getWindowTimer();

    parent_->labelRunning->setText(tr("done in %1").arg(QString::fromStdString(tempList.at(1))));
    parent_->setProgressBar(100);
    parent_->setLog(tr("done in %1").arg(QString::fromStdString(tempList.at(1))));
    //parent_->doneColor();
    parent_->setStatus(running_status::finished);
    parent_->windowCanRunBag();
    
    }
}

void colourise::closeBtnPressed() {
    running = false;

    QProcess::startDetached("killall colorization");

    std::cout << "colorize closed" << std::endl;

    if (colourisePc != NULL) {

        disconnect(colourisePc, 0, 0, 0);
        colourisePc->kill();
        colourisePc = NULL;
        std::cout << "color x " << std::endl;

        
    }

    parent_->labelRunning->setText(tr("canceling.."));
    QApplication::processEvents(); // show loading dialog
    QEventLoop loop;
    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();

    parent_->stopWindowTimer();
    parent_->labelRunning->setText(tr("canceled"));
    parent_->setLog(tr("canceled"));
    parent_->setStatus(running_status::finished);
    parent_->windowCanRunBag();
    this->close();
}