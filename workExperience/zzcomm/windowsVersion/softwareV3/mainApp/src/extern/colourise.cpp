#include "../include/include_gui/colourise.h"
#include "../include/include_gui/widget_newProcess.h"

#include <QFile>
#include <iostream>
#include <QTimer>
#include <QFileDialog>
#include <iomanip>

colourise::colourise(widget_newProcess* parent)
    : QWidget(parent),
    colourisePc(nullptr)
    , running(true)
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


        double pen = -0.001; // for Color penetration strength
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

        // for panoramic
        QString mask;
        if (!parent_->getMask().isEmpty()) {
            mask = QString("--mask=")+ parent_->addQuote(parent_->getMask().toStdString());
            //QString cali = "..\\Lib\\company\\camera-calibPa.yaml";   // each camera has its own cali-file *********
            //QFileInfo fi(cali);
            //QString caliPath = fi.absoluteFilePath().replace('/', '\\');
            //parent_->setCali(caliPath);
        }
        else {
            mask = "";
        }

        // add std::quoted() to all commands
        /*std::string app = "..\\Lib\\company\\color\\colorize.exe";*/
        QFileInfo fapp("..\\Lib\\company\\color\\colorize.exe");
        std::string app = fapp.absoluteFilePath().toStdString();
        all_params = QString(parent_->addQuote(app) + " " +
            parent_->addQuote(parent_->getprojDir().toStdString()) + " " +
            parent_->addQuote(parent_->getpointCloud().toStdString()) + " " +
            parent_->addQuote(parent_->getVideo().toStdString()) + " " +
            parent_->addQuote(parent_->getImu().toStdString()) + " " +
            parent_->addQuote(parent_->getTraj().toStdString()) + " " +
            parent_->addQuote(parent_->getCali().toStdString()) + " " +
            parent_->addQuote(parent_->getYaml().toStdString()) + " ") // to be changed, if yaml file needed
            + 
            QString(
                "--frame_skip %1 --first_frame %2 --last_frame %3 --visible_range %4 --voxel_size %5 --use_default_color=%6 --hpr_gamma=%7 %8 %9")
            .arg(parent_->getframeSkip())
            .arg(parent_->getfirstFrame())
            .arg(parent_->getlastFrame())
            .arg(parent_->getvisibleRange())
            .arg(parent_->getQuality())
            .arg(uncolorPts)
            .arg(pen)
            .arg(inputs)
            .arg(mask);



        //all_params = QString("..\\Lib\\company\\color\\colorize.exe " + parent_->getprojDir() + " " + parent_->getpointCloud() + " " +
        //    parent_->getVideo() + " " + parent_->getImu() + " " + parent_->getTraj() + " " + parent_->getCali() + " " +
        //    parent_->getYaml() + "") // to be changed, if yaml file needed
        //    + QString(
        //        "--frame_skip %1 --first_frame %2 --last_frame %3 --visible_range %4 --voxel_size %5 --use_default_color=%6 --hpr_gamma=%7 %8 %9")
        //    .arg(parent_->getframeSkip()).arg(parent_->getfirstFrame()).arg(parent_->getlastFrame())
        //    .arg(parent_->getvisibleRange()).arg(parent_->getQuality()).arg(uncolorPts).arg(pen).arg(inputs)
        //    .arg(mask);

        std::cout << all_params.toStdString() << std::endl;

        colourisePc = new QProcess;
        //colourisePc->start("cmd.exe");
        //colourisePc->write(QString(all_params + "\r\n").toUtf8());
        colourisePc->start(all_params + "\n");

        connect(colourisePc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(_ColorExitCode(int, QProcess::ExitStatus)));
        connect(colourisePc, SIGNAL(readyReadStandardOutput()), this, SLOT(_starts()));
    }
}

void colourise::_ColorExitCode(int a, QProcess::ExitStatus e) { // always return 0
    //if (e == QProcess::ExitStatus::NormalExit)std::cout << "byebye" << std::endl;
    //if (e == QProcess::ExitStatus::CrashExit)std::cout << "gg" << std::endl;
    //std::cout << "exit no.: " << std::to_string(a) << std::endl;
    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "colorization successfully exits" << std::endl;
        parent_->windowAddToCmd(QString(tr("colorization done")));

        QEventLoop loop;
        QTimer::singleShot(1000, &loop, SLOT(quit()));
        loop.exec();
        colorFinished();
    }
    else {
        std::cout << "algorithm(colorization) in error." << std::endl;

        running = false;

        //parent->windowAddToCmd(QString(tr("algorithm1 in error.")));
        parent_->setColorErr("colorization"); 
        parent_->colorCrash();
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

            return;
        }           
    }
    parent_->windowAddToCmd(str);
}

void colourise::colorFinished()
{
    if (running) {
        if (colourisePc != NULL) {

            disconnect(colourisePc, 0, 0, 0);
            colourisePc->kill();
            colourisePc = NULL;
            std::cout << "color x " << std::endl;

            running = false;
        }

        //// when done
        //parent_->labelRunning->setText(tr("done in %1").arg(QString::fromStdString(parent_->getWindowTimer())));
        //parent_->setProgressBar(100);
        //parent_->setLog(tr("done in %1").arg(QString::fromStdString(parent_->getWindowTimer())));
        ////parent_->doneColor();
        //parent_->windowCanRunBag();


        // when done
        std::vector<std::string> tempList = parent_->getWindowTimerP();

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

    QProcess process;
    QString cmdd = "tasklist";
    process.start(cmdd);
    process.waitForFinished();                   
    QString str1 = process.readAllStandardOutput();

    if (str1.contains("colorize.exe"))
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("colorize.exe"));
    }
    process.close();

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

    parent_->stopWindowTimerP();
    parent_->labelRunning->setText(tr("canceled"));
    parent_->setLog(tr("Canceled"));
    parent_->setStatus(running_status::finished);
    parent_->windowCanRunBag();
    this->close();
}