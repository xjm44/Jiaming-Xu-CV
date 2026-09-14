#include "../../include/process/processGui.h"
#include "../../include/process/rtk.h"
#include "../../include/process/setting.h"


gui::gui(QWidget* parent)
    : QFrame(parent),
    prev_height(0), prev_width(0),
    btnClose(new QPushButton("")), btnMax(new QPushButton("")), btnMin(new QPushButton("")),
    close_btn(true), Rtk(this), rtkRunning(false), disableDrop(false), gcpChecked(false), setParams(new settingParam(this)),
    change_yaml(false)

{
    ui.setupUi(this);

    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    this->setWindowTitle(QString("company"));
    this->setWindowIcon(QIcon(":/process/process/logo.png"));
    this->setAttribute(Qt::WA_DeleteOnClose);
    setAcceptDrops(true); // drag file

    //ui.checkBox_2->hide();
    ui.checkBoxLas->setEnabled(0);
}

void gui::quitWhenRunning() { 

        ui.dataInput1->setText(QString(tr("Waiting..")));

        if (cmd_yaml != NULL) {

            disconnect(cmd_yaml, 0, 0, 0);
            //cmd_yaml->terminate();
            cmd_yaml->kill();
            //cmd_yaml->close();
            cmd_yaml = NULL;
            std::cout << "rosparam x " << std::endl;
        }
        if (processCore != NULL) {

            disconnect(processCore, 0, 0, 0);
            processCore->kill();
            processCore = NULL;
            std::cout << "roscore x " << std::endl;
        }
        if (processCheck != NULL) {

            disconnect(processCheck, 0, 0, 0);
            processCheck->kill();
            processCheck = NULL;
            std::cout << "rosbag info x " << std::endl;
        }
        if (processCheckC != NULL) {

            disconnect(processCheckC, 0, 0, 0);
            processCheckC->kill();
            processCheckC = NULL;
            std::cout << "rosbag check x " << std::endl;
        }
        if (processStart != NULL) {

            disconnect(processStart, 0, 0, 0);
            processStart->kill();
            processStart = NULL;
            std::cout << "algorithm1 x " << std::endl;
        }
        if (processM != NULL) {

            disconnect(processM, 0, 0, 0);
            processM->kill();
            processM = NULL;
            std::cout << "merge x " << std::endl;
        }
        if (processRecord != NULL) {

            disconnect(processRecord, 0, 0, 0);
            processRecord->kill();
            processRecord = NULL;
            std::cout << "record x " << std::endl;
            closeAll(); // waiting for shutting down rosbag record

        }
        if (processMkdir != NULL) {

            disconnect(processMkdir, 0, 0, 0);
            processMkdir->kill();
            processMkdir = NULL;
            std::cout << "mkdir x " << std::endl;
        }

        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished(-1);                    
        QString str1 = process.readAllStandardOutput();

        if (str1.contains("algorithm1_nrt.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt.exe"));
        }
        if (str1.contains("merge.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
        }
        if (str1.contains("algorithm1_nrt_loopclosure.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_loopclosure.exe"));
        }
        if (str1.contains("algorithm1_nrt_algorithm2.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_algorithm2.exe"));
        }
        if (str1.contains("merge_loopclosure.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("merge_loopclosure.exe"));
        }
        if (str1.contains("algorithm1_nrt_rtk.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk.exe"));
        }
        if (str1.contains("algorithm1_nrt_rtk_eoe.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk_eoe.exe"));
        }

        QEventLoop loop;
        QTimer::singleShot(3000, &loop, SLOT(quit()));
        loop.exec();

        std::cout << " \n";
        QFileInfo file1;
        file1 = QString("%1").arg(DatatimeabsoDir);
        if (file1.exists() == true) {

            QDir dir(DatatimeabsoDir);

            std::cout << std::boolalpha << dir.removeRecursively() + " \n" << std::endl;

            std::cout << DatatimeabsoDir.toStdString() + " deleted \n";
        }
        process.close();
        std::cout << "closed \n" << std::endl;
        this->close(); 
}


void gui::quit_mainwindow()
{

    if (running) {
        QMessageBox::StandardButton check;
        check = QMessageBox::information(NULL, QString(tr("Process is running ")), QString(tr("Are you sure to quit ?")), QMessageBox::Yes, QMessageBox::No);
        if (check == QMessageBox::Yes) {
            if (close_btn) {
                quitWhenRunning();
            }
            else {
                QMessageBox::information(NULL, QString(tr("Error ")), QString(tr("Please try again ")));
            }
        }
    }
    else {
        this->close();
    }

}

// ===================================================== RUNNING BAG ===========================================================================================================
// =============================================================================================================================================================================
void gui::_btnRun_clicked() {
    std::cout << "bag file: "<<filename.toStdString() << std::endl;

    running = true;
    ui.stage->setText("1/3"); // the label for running the .bag file input
    ui.progressBar->setValue(0);
    ui.dataInput1->setText(QString(tr("Preparing...(if the progressive bar doesn't move for 2min, please exit and retry.)")));
    ui.dataInput1->setCursorPosition(0);

    ui.btnProgress->setEnabled(false);
    ui.btnProgress->hide();
    ui.btnStopProgress->raise();
    ui.btnStopProgress->setVisible(true);
    ui.btnStopProgress->setEnabled(true);

    QString lidarBag =  filename;
    int lidarbagIndex = 8; // remove file:/.......

    lidarBag = lidarBag.mid(lidarbagIndex).simplified();
    lidarbagIndex = lidarBag.lastIndexOf("/");

    if (lidarbagIndex > 0)
    {

        ui.progressBar->setValue(20);

        // start ros========================================== 
        processCore = new QProcess;
        processCore->start("cmd.exe");
        processCore->write(QString(aa + "\r\n").toUtf8());
        processCore->write(QString("roscore \r\n").toUtf8());

        connect(processCore, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_roscore()));


        std::cout << "\n"<<std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "roscore starting" << std::endl;
        std::cout << "============================" << std::endl;


        // for log
        logFile.setFileName("..\\Lib\\company\\algorithm1_Log.txt");
        logFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);

    }
    else if (lidarbagIndex < 0)
    {
        ui.dataInput1->setText(QString(tr("Denied! Invalid .bag file.")));
        return;
    }
}

void gui::slot_roscore() {
    if (running) {
        QString str = QString(processCore->readAllStandardOutput()); //roscore need run 2 times to get output
        int index = str.lastIndexOf("NODES");

        if (index > -1) {
            slot_roscore_output();
            return;
            //mergeDeal();
        }
        else {
            QEventLoop loop1;
            QTimer::singleShot(1000, &loop1, SLOT(quit()));
            loop1.exec();
        }
    }
}

void gui::slot_roscore_output() {

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "roscore started" << std::endl;
    std::cout << "============================" << std::endl;

    if (running) {
        ui.progressBar->setValue(40);
        ui.dataInput1->setText(QString(tr("Configuration..(if the progressive bar doesn't move for 1min, please exit and retry.)")));
        ui.dataInput1->setCursorPosition(0);

        cmd_yaml = new QProcess;
        cmd_yaml->start("cmd.exe");
        QString _startParams;

        // ==========================================================================================================================================================================
        if (rtkRunning) { // rtk

            if (!gcpChecked) {
                std::cout << "rtk \n";
                _startParams = "rosparam load ..\\Lib\\company\\params_rtk.yaml && echo CFYcfy"; 
            }
            else {
                std::cout << "rtk + gcp \n";
                _startParams = "rosparam load ..\\Lib\\company\\params_rtk_eoe.yaml && echo CFYcfy";            
            }      
        }
        else {

            if (ui.checkBox_2->checkState() == Qt::Checked) { // algorithm2
                std::cout << "algorithm2 \n";
                _startParams = "rosparam load ..\\Lib\\company\\params_algorithm2.yaml && echo CFYcfy";
            }
            else {
                std::cout << "new algorithm1 \n";
                change_yaml = true;
                _startParams = "rosparam load ..\\Lib\\company\\params_comb.yaml && echo CFYcfy";
                  
            }
        }

        cmd_yaml->write(QString(aa + "\r\n").toUtf8());
        cmd_yaml->write(_startParams.toLocal8Bit() + '\r' + '\n');
        connect(cmd_yaml, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_checkYaml()));
        // ==========================================================================================================================================================================
        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "rosparam starting" << std::endl;
        std::cout << "============================" << std::endl;
    }
}

void gui::slot_checkYaml() {
    if (running) {
        QString str = QString(cmd_yaml->readAllStandardOutput());
        int index = str.lastIndexOf("cfy"); // will echo twice

        if (index > 1) {
            paramIndex++;
            if (paramIndex > 1) {

                if (change_yaml) {
                    changeYaml();
                }
                else {
                    checkbagC();
                    //checkbag();
                }
                
                paramIndex = 0;
            }
        }
    }
}

void gui::changeYaml() {

    ui.dataInput1->setText(QString(tr("Setting parameters..")));

    // gui page
    this->sensor = ui.comboBoxSensor->currentText();
    this->csv = ui.checkBoxCsv->isChecked();
    this->las = ui.checkBoxLas->isChecked();
    this->rotatingFrames = ui.checkBoxRotation->isChecked();

    //// setting 
    //this->loopClosure = loopClosure;
    //this->loopClosureIntervalFrames = loopClosureIntervalFrames;
    //this->narrowSpace = narrowSpace;
    //this->minIntensityFilter = minIntensityFilter;
    //this->lidarMaxRange = lidarMaxRange;
    //this->lidarMinRange = lidarMinRange;
    //this->lidarMaxXToFilter = lidarMaxXToFilter;
    //this->lidarMinXToFilter = lidarMinXToFilter;
    //this->translationThreshold = translationThreshold;
    //this->icpMaximumIterations = icpMaximumIterations;



    QString _indiParams;
    QProcess* _yamlParam = new QProcess();
    _yamlParam->start("cmd.exe");
    _yamlParam->write(QString(aa + "\r\n").toUtf8());

    // no of cores
    _indiParams = QString("rosparam set /algorithm1/numberOfCores %1").arg(no_of_cores);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QEventLoop loop;
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();

    // loop closure
    if (loopClosure) 
        _indiParams = QString("rosparam set /algorithm1/useLoopClosure true");    
    
    else 
        _indiParams = QString("rosparam set /algorithm1/useLoopClosure false");
    
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();

    // loop closure interval frames
    if (loopClosure) {
        _indiParams = QString("rosparam set /algorithm1/loopClosureIntervalFrames %1").arg(loopClosureIntervalFrames);
        _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
        //QApplication::processEvents();
        QTimer::singleShot(500, &loop, SLOT(quit()));
        loop.exec();
    }

    // narrow space
    if (narrowSpace)
        _indiParams = QString("rosparam set /algorithm1/isNarrowSpace true");

    else
        _indiParams = QString("rosparam set /algorithm1/isNarrowSpace false");

    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();

    // csv
    if (csv)
        _indiParams = QString("rosparam set /algorithm1/savePoseToCSV true");

    else
        _indiParams = QString("rosparam set /algorithm1/savePoseToCSV false");

    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();

    // las
    if (las)
        _indiParams = QString("rosparam set /algorithm1/saveMapToLAS true");

    else
        _indiParams = QString("rosparam set /algorithm1/saveMapToLAS false");

    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();

    //sensor
    _indiParams = QString("rosparam set /algorithm1/sensor %1").arg(sensor);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 
    
    // min intensity filter
    _indiParams = QString("rosparam set /algorithm1/minIntensity %1").arg(minIntensityFilter);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 

    // lidar max range
    _indiParams = QString("rosparam set /algorithm1/lidarMaxRange %1").arg(lidarMaxRange);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 

    // lidar min range
    _indiParams = QString("rosparam set /algorithm1/lidarMinRange %1").arg(lidarMinRange);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
   // QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 

    // lidar max x
    _indiParams = QString("rosparam set /algorithm1/lidarMaxXToFilter %1").arg(lidarMaxXToFilter);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 

    // lidar min x
    _indiParams = QString("rosparam set /algorithm1/lidarMinXToFilter %1").arg(lidarMinXToFilter);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 

    // rotating frames
    if (rotatingFrames) 
        _indiParams = QString("rosparam set /algorithm1/onlyRotationFramesUsedForMapping true");

    else
        _indiParams = QString("rosparam set /algorithm1/onlyRotationFramesUsedForMapping false");

    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 

    // translation threshold
    _indiParams = QString("rosparam set /algorithm1/translationThreshold %1").arg(translationThreshold);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
   // QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 

    // icp iteration
    _indiParams = QString("rosparam set /algorithm1/icpMaximumIterations %1").arg(icpMaximumIterations);
    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QApplication::processEvents();
    QTimer::singleShot(500, &loop, SLOT(quit())); 
    loop.exec(); 

    checkbagC();

}

void gui::checkbagC() {

    if (running) {
        change_yaml = false;

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "rosparam started" << std::endl;
        std::cout << "============================" << std::endl;

        QString lidarBag = filename;
        QString path1 = "";
        QString path2 = "";
        QString  D3_checkbagStr = "";
        processCheckC = new QProcess; // preparation
        processCheckC->start("cmd.exe");

        int lidarbagIndex = 8; // remove file:/.......

        lidarBag = lidarBag.mid(lidarbagIndex).simplified();
        lidarbagIndex = lidarBag.lastIndexOf("/");

        if (lidarbagIndex > 0)
        {
            path1 = lidarBag.replace('/', '\\');
            D3_checkbagStr = QString("rosbag check %1").arg(path1);

            // write to terminal
            processCheckC->write(QString(aa + "\r\n").toUtf8());
            processCheckC->write(QString(D3_checkbagStr + "\r\n").toUtf8());
            ui.dataInput1->setText(QString(tr("Checking bag...(if the progress bar doesn't move for 10mins, please check your bag or ask supplier)")));
            ui.dataInput1->setCursorPosition(0);//���ù�������
            ui.progressBar->setValue(60);

            connect(processCheckC, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_bagcheck_output()));

            std::cout << "\n" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "rosbag check checking" << std::endl;
            std::cout << "============================" << std::endl;

        }
        else if (lidarbagIndex < 0)
        {
            ui.dataInput1->setText(QString(tr("Denied! Invalid .bag file.")));
            return;
        }
    }
}

void gui::slot_bagcheck_output()
{
    if (running) {
        QString str = QString(processCheckC->readAllStandardOutput());
        int check = str.indexOf("Bag file does not need any migrations");

        if (check < 0)
        {
            std::cout << "\n" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "rosbag check failed OR still checking" << std::endl;
            std::cout << "============================" << std::endl;

        }
        else 
        {
            //====================================================================================
            QEventLoop loop1;
            QTimer::singleShot(500, &loop1, SLOT(quit()));  
            loop1.exec(); 
            //====================================================================================

            checkbag();
            return;
        }
    }
}

void gui::checkbag() {

    if (running) {
        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "rosbag check checked" << std::endl;
        std::cout << "============================" << std::endl;

        QString lidarBag = filename;

        QString path1 = "";
        QString path2 = "";
        QString  D3_checkbagStr = "";
        processCheck = new QProcess; // preparation
        processCheck->start("cmd.exe");

        int lidarbagIndex = 8; // remove file:/.......

        lidarBag = lidarBag.mid(lidarbagIndex).simplified();
        lidarbagIndex = lidarBag.lastIndexOf("/");

        if (lidarbagIndex > 0)
        {
            path1 = lidarBag.replace('/', '\\');
            D3_checkbagStr = QString("rosbag info %1").arg(path1);

            // write to terminal
            processCheck->write(QString(aa + "\r\n").toUtf8());
            processCheck->write(QString(D3_checkbagStr + "\r\n").toUtf8());
            ui.dataInput1->setText(QString(tr("Checking topics...(if the progress bar doesn't move for 5mins, please check your bag or ask supplier)")));
            ui.dataInput1->setCursorPosition(0);
            ui.progressBar->setValue(90);

            connect(processCheck, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_baginfo_output()));


            std::cout << "\n" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "rosbag info checking" << std::endl;
            std::cout << "============================" << std::endl;
        }
        else if (lidarbagIndex < 0)
        {
            ui.dataInput1->setText(QString(tr("Denied! Invalid .bag file.")));
            return;
        }
    }
}

void gui::slot_baginfo_output()
{
    if (running) {
        QString str = QString(processCheck->readAllStandardOutput());
        int imuIndex = str.indexOf("/imu/data");
        int encIndex = str.indexOf("/ENCread");
        int hesaiIndex = str.indexOf("/hesai/pandar");                      // TO BE DONE ********* IF CHANGED TO VELODYNE
        int end = str.indexOf("msgs    : sensor_msgs/PointCloud2");


        if (imuIndex < 0 && encIndex < 0 && hesaiIndex < 0)
        {
            std::cout << "\n" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "rosbag info failed OR still checking" << std::endl;
            std::cout << "============================" << std::endl;

        }
        else // check the 3 topics and included in the bag
        {
            //====================================================================================

            QString get_total_frame = "";
            for (int i = hesaiIndex; i < end; i++) {
                get_total_frame += str.at(i);
            }
            QStringList l = get_total_frame.split(" ");
            frame_size = l.at(l.size() - 2).toInt(); // get frame number
            QEventLoop loop1;
            QTimer::singleShot(1000, &loop1, SLOT(quit()));   yuan 85000
            loop1.exec(); 
            //====================================================================================

            ui.btnStopProgress->setEnabled(false);
            close_btn = false;
            std::cout << "\n" << std::endl;
            std::cout << "Stop button not enable" << std::endl;
            std::cout << "" << std::endl;
            slot_startBag();
        }
    }
}

void gui::slot_startBag() // play and record simultaneously
{
    if (running) {
        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "rosbag info checked" << std::endl;
        std::cout << "============================" << std::endl;

        processStart = new QProcess;
        processStart->start("cmd.exe");
        QString path1 = "";
        QString path2 = "";
        QString  _algorithm1BagStr = "";
        QString lidarBag = filename;
        int lidarbagIndex = 8;
        lidarBag = lidarBag.mid(lidarbagIndex).simplified();
        lidarbagIndex = lidarBag.lastIndexOf("/");


        if (lidarbagIndex > 0)
        {
            path1 = lidarBag.replace('/', '\\');
            path2 = lidarBag.left(lidarbagIndex + 1);

        }

        // ==========================================================================================================================================================================
        if (rtkRunning) {

            if (!gcpChecked) {
                std::cout << "rtk\n";
                // run the bag
                _algorithm1BagStr = QString("..\\Lib\\company\\algorithm1_nrt_rtk.exe %1 %2 __name:=bagplay && echo CFYplaybagendcfy").arg(path1).arg(rtkTxt);
            }
            else {
                std::cout << "rtk + gcp\n";
                // run the bag
                _algorithm1BagStr = QString("..\\Lib\\company\\algorithm1_nrt_rtk_eoe.exe %1 %2 __name:=bagplay && echo CFYplaybagendcfy").arg(path1).arg(rtkTxt);
            }
        }
        else {
            
            if (ui.checkBox_2->checkState() == Qt::Checked) { // algorithm2
                std::cout << "algorithm2 \n";
                // run the bag
                _algorithm1BagStr = QString("..\\Lib\\company\\algorithm1_nrt_algorithm2.exe %1 __name:=bagplay && echo CFYplaybagendcfy").arg(path1);
                
            }
            else {                      // **************** RECORD TO BE DONE + MERGE *************************
                //(record)=================================================================================
                DatatimeDir = (QDateTime::currentDateTime()).toString("yyyy.MM.dd_hh:mm:ss");
                DatatimeDir = DatatimeDir.replace('.', '_');
                DatatimeDir = DatatimeDir.replace(':', '_');
                DatatimeabsoDir = path2 + DatatimeDir;

                //===============================================================================
                processMkdir = new QProcess;
                processMkdir->start("cmd.exe");
                QString D3_mkdirBagFile = QString("md %1").arg(DatatimeabsoDir);
                processMkdir->write(D3_mkdirBagFile.toLocal8Bit() + '\n');
                QEventLoop loop1;
                QTimer::singleShot(1000, &loop1, SLOT(quit()));   yuan 85000
                loop1.exec(); 
                // ================================================================================================================

                std::cout << "new algorithm1 \n";

                processRecord = new QProcess;
                processRecord->start("cmd.exe");
                QString D3_RecordBagStr = QString("rosbag record -O %1\\filename.bag -b 4096 /algorithm1/mapping/odom /algorithm1/mapping/pose_begin /algorithm1/mapping/pose_end __name:=record_data").arg(DatatimeabsoDir);  //   /hilbert_h/mapping/cloud_registered_raw  /velodyne_points
                ////QString D3_RecordBagStr = QString("rosbag record -O %1\\filename.bag -b 4096 /algorithm1/mapping/cloud_registered_raw __name:=record_data").arg(DatatimeabsoDir); 
                //QString D3_RecordBagStr = QString("rosbag record -O %1\\filename.bag -b 4096 /algorithm1/mapping/cloud_registered_raw /algorithm1/lc/mapping /algorithm1/lc/path_begin /algorithm1/lc/path_end /algorithm1/lc/loop_closure_constraints /algorithm1/lc/cur_cloud_key /algorithm1/lc/pre_cloud_key /algorithm1/lc/correct_cloud_key __name:=record_data").arg(DatatimeabsoDir);
                processRecord->write(QString(aa + "\r\n").toUtf8());
                processRecord->write(D3_RecordBagStr.toLocal8Bit() + '\n');
                QEventLoop loop11;
                QTimer::singleShot(1000, &loop11, SLOT(quit()));   yuan 85000
                loop11.exec(); 


                // run the bag
                _algorithm1BagStr = QString("..\\Lib\\company\\algorithm1_nrt_comb.exe %1 __name:=bagplay && echo CFYplaybagendcfy").arg(path1);

                }
            }

            processStart->write(QString(aa + "\r\n").toUtf8());
            processStart->write(_algorithm1BagStr.toLocal8Bit() + '\n');
            connect(processStart, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_startBag_output()));
    }
        // ==========================================================================================================================================================================

        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished();                    
        QString str1 = process.readAllStandardOutput();

        if (!str1.contains("record.exe") && ui.progressBar->value() == 0)
        {
            std::cout << "" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "rosbag record started" << std::endl;
            std::cout << "============================" << std::endl;

        }

        ui.progressBar->setValue(0);
        ui.stage->setText("2/3");
        ui.dataInput1->setText(QString(tr("Recording..(starts soon)")));

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "rosbag record starting" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "";

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "algorithm1_nrt.exe starting" << std::endl;
        std::cout << "============================" << std::endl;


        //std::cout << "\n" << std::endl;
        //std::cout << "============================--------------------------------------------------------" << std::endl;
        //std::cout << path1.toStdString() << std::endl;
        //std::cout << "============================---------------------------------------------------------" << std::endl;
}

void gui::slot_startBag_output()
{
    if (running) {

        QString str = QString(processStart->readAllStandardOutput());
        int index = str.lastIndexOf("playbagendcfy"); // will echo twice

        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished();                  
        QString str1 = process.readAllStandardOutput();

        if (!str1.contains("algorithm1_nrt") && ui.progressBar->value() == 0)
        {
            std::cout << "\n" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "algorithm1_nrt.exe not responding yet (remember to check if this is running in the task manager)" << std::endl;
            std::cout << "============================" << std::endl;

        }

        if (index > 1)
        {
            playbagIndex = playbagIndex + 1;
            std::cout << "" << std::endl;
            std::cout << "Stop button enabled" << std::endl;

            ui.btnStopProgress->setEnabled(true);
            close_btn = true;

            if (playbagIndex > 1)
            {

                closeAll(); // close play, record

                QEventLoop loop;
                QTimer::singleShot(2000, &loop, SLOT(quit())); 
                loop.exec(); 
                ui.progressBar->setValue(100);

                mergeDeal();
                playbagIndex = 0;

            }
        }

        int check = str.lastIndexOf("failed rate");
        if (check > 0) {
            int indexDuration = str.indexOf(" | ") + 2;
            QString progress = str.mid(indexDuration);
            progress = progress.simplified().section(' ', 0, 0).simplified();
            int current_prog = (progress.toInt() * 100 / frame_size);
            ui.progressBar->setValue(current_prog);
            ui.dataInput1->setText(QString(tr("Processing..(this will take a bit longer)")));

            std::cout << "\n" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "algorithm1_nrt.exe running" << std::endl;
            std::cout << "============================" << std::endl;

            // for log
            QTextStream stream(&logFile);
            stream << str;
        }
    }
}

void gui::mergeDeal()
{
    if (running) {
        ui.stage->setText("3/3");
        ui.progressBar->setValue(0);

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "algorithm1_nrt.exe and record finished" << std::endl;
        std::cout << "============================" << std::endl;

        //DatatimeabsoDir = "C:\\Users\\company\\Desktop\\2022_09_01_10_38_58";
        ui.dataInput1->setText(QString(tr("Merging..(starts soon)")));
        processM = new QProcess;
        processM->start("cmd.exe");

        // ==========================================================================================================================================================================
        if (rtkRunning) { // rtk
            std::cout << "rtk / rtk + gcp\n";
            delBagFile();
        }
        else {

            if (ui.checkBox_2->checkState() == Qt::Checked) { // algorithm2
                std::cout << "algorithm2 \n";
                delBagFile();

            }
            else {
                std::cout << "new algorithm1 \n";
                //QString D3_merge_str = QString("..\\Lib\\company\\merge.exe %1\\filename.bag /algorithm1/mapping/cloud_registered_raw").arg(DatatimeabsoDir);
                //QString D3_merge_str = QString("..\\Lib\\company\\merge_loopclosure.exe %1\\filename.bag /algorithm1/lc/mapping").arg(DatatimeabsoDir);

                //processM->write(QString(aa + "\r\n").toUtf8());
                //processM->write(D3_merge_str.toLocal8Bit() + '\r' + '\n');

                //connect(processM, SIGNAL(readyReadStandardOutput()), this, SLOT(slot_D3_merge_output()));
               
                //std::cout << "\n" << std::endl;
                //std::cout << "============================" << std::endl;
                //std::cout << "merge.exe running (remember to check if this is running in the task manager)" << std::endl;
                //std::cout << "============================" << std::endl;

                delBagFile();
            }
        }

        
        // ==========================================================================================================================================================================
    }
}

void gui::slot_D3_merge_output()
{
    if (running) {
        QString str = QString(processM->readAllStandardOutput());
        int index = str.lastIndexOf("success");

        if (count > 1) { // set progress bar's value
            if (ui.progressBar->value() > 40) {
                ui.dataInput1->setText(QString(tr("Please wait a bit longer..")));
            }
            else {
                ui.dataInput1->setText(QString(tr("Merging..")));
            }
        }

        if (index < 0) {

            if (str.lastIndexOf("\r\n") > 0) {
                if (count * 7 < 100) {
                    ui.progressBar->setValue(count * 7);
                    count++;
                }
            }
        }
        else {
            count = 1;
            delBagFile();

            std::cout << "\n" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "merge.exe finished" << std::endl;
            std::cout << "============================" << std::endl;
            return;
        }
    }
}

void gui::delBagFile()
{
    // log done
    logFile.flush();
    logFile.close();

    processD = new QProcess;
    processD->start("cmd.exe");


    QString path1 = "";
    QString path2 = "";
    QString lidarBag = filename;
    int lidarbagIndex = 8;
    lidarBag = lidarBag.mid(lidarbagIndex).simplified();
    lidarbagIndex = lidarBag.lastIndexOf("/");
    if (lidarbagIndex > 0)
    {
        path1 = lidarBag.replace('/', '\\');
        path2 = lidarBag.left(lidarbagIndex + 1);
    }

    if (rtkRunning) { // rtk
        ui.progressBar->setValue(50);
        //HistoricalDir3 = HistoricalDir2;
        //HistoricalDir2 = HistoricalDir1;
        //HistoricalDir1 = path2;
    }
    else {
        if (ui.checkBox_2->checkState() == Qt::Checked) { // algorithm2
            ui.progressBar->setValue(50);

        }
        else { // algorithm1 / closeloop
            //QString D3_delBagFile_str = QString("del/f/s/q %1\\filename.bag").arg(DatatimeabsoDir);     //.arg(DatatimeabsoDir);  //&& cd ~/%1 && rm -rf pcd0/ && rm -f filename.bag
            //processD->write(D3_delBagFile_str.toLocal8Bit() + '\n');  //************NEED UNCOMMENTED
            ui.progressBar->setValue(50);
        }
    }

    HistoricalDir3 = HistoricalDir2;
    HistoricalDir2 = HistoricalDir1;
    HistoricalDir1 = path2;

    QEventLoop loop;
    QTimer::singleShot(2000, &loop, SLOT(quit())); 
    loop.exec(); 

    ui.progressBar->setValue(100);
    ui.dataInput1->setText(QString(tr("Done, thanks for waiting.")));
    ui.btnProgress->setVisible(true);
    ui.btnStopProgress->hide();
    ui.btnStopProgress->setEnabled(false);


    ui.dataline1->setText(HistoricalDir1);
    ui.dataline2->setText(HistoricalDir2);
    ui.dataline3->setText(HistoricalDir3);

    QFile file("./historical_data.txt");  //new file
    bool fileok = file.open(QIODevice::WriteOnly);
    if (fileok == true)
    {
        QString str = QString("%1\r\n%2\r\n%3").arg(HistoricalDir1).arg(HistoricalDir2).arg(HistoricalDir3);
        file.write(str.toStdString().data());
    }
    file.close();

    if (cmd_yaml != NULL)cmd_yaml->close();
    if (processCore != NULL)processCore->close();
    if (processCheck != NULL)processCheck->close();
    if (processCheckC != NULL)processCheckC->close();
    if (processStart != NULL)processStart->close();
    if (processMkdir != NULL)processMkdir->close();
    if (processRecord != NULL)processRecord->close();
    if (processM != NULL)processM->close();
    if (processD != NULL)processD->close();
    if (processCl != NULL)processCl->close();

    filename = "";
    running = false;
    rtkRunning = false;
    rtkTxt = QString();

}
void gui::closeAll()
{
    processCl = new QProcess;
    processCl->start("cmd.exe");
    processCl->write(QString(aa + "\r\n").toUtf8());
    QString D3_closeAll_str3 = "rosnode kill record_data";
    processCl->write(D3_closeAll_str3.toLocal8Bit() + '\n');
    QString D3_closeAll_str1 = "rosnode kill /ekf_gps";
    processCl->write(D3_closeAll_str1.toLocal8Bit() + '\n');
    QString D3_closeAll_str2 = "rosnode kill bagplay";
    processCl->write(D3_closeAll_str2.toLocal8Bit() + '\n');

}

// =============================================================================================================================================================================
// ================================================================== BAG FINISHED =============================================================================================



void gui::setUpProUi() {
    // three buttons on the title bar =========================================================
    btnMin->setStyleSheet("QPushButton{border-image:url(:process/process/minimizenew.png);} QPushButton::hover{ border-image:url(:process/process/minimize.png);}");
    btnMax->setStyleSheet("QPushButton{border-image:url(:process/process/maxnew.png); }QPushButton::hover{ border-image:url(:process/process/max.png);} ");
    btnClose->setStyleSheet("QPushButton{border-image:url(:process/process/closenew.png);}QPushButton::hover{ border-image:url(:process/process/close.png);}");
    btnClose->setParent(this);
    btnMin->setParent(this);
    btnMax->setParent(this);
    connect(btnClose, SIGNAL(clicked()), this, SLOT(quit_mainwindow()), Qt::UniqueConnection);
    connect(btnMin, SIGNAL(clicked()), this, SLOT(minimize_mainwindow()), Qt::UniqueConnection);
    connect(btnMax, SIGNAL(clicked()), this, SLOT(max_mainwindow()), Qt::UniqueConnection);
    //=============================================================================================

    // OPEN btn in the Process page=============================================
    connect(ui.btnopen1, SIGNAL(clicked()), this, SLOT(_btnOpen_clicked1()), Qt::UniqueConnection);
    connect(ui.btnopen2, SIGNAL(clicked()), this, SLOT(_btnOpen_clicked2()), Qt::UniqueConnection);
    connect(ui.btnopen3, SIGNAL(clicked()), this, SLOT(_btnOpen_clicked3()), Qt::UniqueConnection);
    //==========================================================================

    // RUN AND CLOSE BUTTONS
    connect(ui.btnProgress, SIGNAL(clicked()), this, SLOT(_btnRun_clicked()), Qt::UniqueConnection);
    connect(ui.btnStopProgress, SIGNAL(clicked()), this, SLOT(_btnRun_Close_clicked()), Qt::UniqueConnection);

    // rtk
    connect(ui.btnRtk, SIGNAL(clicked()), this, SLOT(_btnRtk_clicked()), Qt::UniqueConnection);
    connect(&Rtk, SIGNAL(rtk_ready(QString,QString)), this, SLOT(_rtk_ready_(QString,QString)), Qt::UniqueConnection);
    connect(&Rtk, SIGNAL(rtk_close()), this, SLOT(_rtk_close_()), Qt::UniqueConnection);

    // setting
    connect(ui.btnSetting, SIGNAL(clicked()), this, SLOT(_btnSetting_clicked()), Qt::UniqueConnection);
    connect(setParams, SIGNAL(param_close()), this, SLOT(_btnSetting_close()), Qt::UniqueConnection);
    
}

void gui::paintEvent(QPaintEvent*)
{
    setGeo(); // widgets' positioning

    // tab bar's background
    QPainter painter(this);
    QPixmap pix;
    pix.load(":process/process/background1.png");
    painter.drawPixmap(0, 29, this->width(), 45, pix);
    // Process page's background
    pix.load(":process/process/background2.png");
    ui.bkgd1->setPixmap(pix);
    ui.bkgd1->setGeometry(this->width() * 0.01 + 10, this->height() * 0.45 + 32, this->width() - 40, this->height() * 0.385 - 6);
    pix.load(":process/process/background3.png");
    ui.bkgd2->setPixmap(pix);
    ui.bkgd2->setGeometry(this->width() * 0.01 + 24, this->height() * 0.45 + 43, this->width() - 64, this->height() * 0.385 - 28);

    // logo ================================================================
    pix.load(":process/process/title.png"); // top left
    //p = pix.scaled(QSize(120, 20));
    ui.logo->setPixmap(pix);
    ui.logo->raise();
    ui.logo->setGeometry(0, 0, 180, 28);

    //pix.load(":process/process/logo.png"); // bottom right
    //ui.btmImg->setPixmap(pix);
    //ui.btmImg->raise();
    //ui.btmImg->setGeometry((ui.bkgd2->x() + ui.bkgd2->width()) * 0.78, ui.bkgd2->y() , ui.bkgd2->width() , ui.bkgd2->height());
    //======================================================================
    ui.groupBox->setGeometry((ui.btnopen1->x() + 200 ) , ui.btnopen1->y(), 200, ui.btnopen3->y()+30- ui.btnopen1->y());


}

void gui::setGeo() {

    // title bar 3 buttons
    btnClose->setGeometry(this->width() - 37, -3.5, 32, 32);
    btnMax->setGeometry(this->width() - 77, -3.5, 32, 32);
    btnMin->setGeometry(this->width() - 117, -3, 32, 32);
    // to set individuals' position (varies with the size of the UI) ========================================================================
    // tab bar titles
    ui.btnProcess->setGeometry(20, 28, 110, 50);
    ui.btnRtk->setGeometry(130, 28, 110, 50);
    ui.btnSetting->setGeometry(this->width() - 130, 28, 110, 50);
    //to set the individual UIs
    ui.stackedWidget->setGeometry(0, 73, this->width(), this->height());
    // for data sets: to display/open
    ui.label->setGeometry(60, this->height() * 0.578 + 5, 240, 30);
    ui.label_2->setGeometry(60, this->height() * 0.578 + 65, 240, 30);
    ui.label_3->setGeometry(60, this->height() * 0.578 + 125, 240, 30);
    ui.dataline1->setGeometry(150, this->height() * 0.578 + 5, this->width() * 0.31, 30);
    ui.dataline2->setGeometry(150, this->height() * 0.578 + 65, this->width() * 0.31, 30);
    ui.dataline3->setGeometry(150, this->height() * 0.578 + 125, this->width() * 0.31, 30);
    ui.btnopen1->setGeometry(this->width() * 0.325 + 150, this->height() * 0.578 + 5, 80, 30);
    ui.btnopen2->setGeometry(this->width() * 0.325 + 150, this->height() * 0.578 + 65, 80, 30);
    ui.btnopen3->setGeometry(this->width() * 0.325 + 150, this->height() * 0.578 + 125, 80, 30);

    // data/file to be dragged into
    ui.dataInput->setGeometry(20, 30, this->width() - 40, this->height() * 0.197);
    ui.dataInput1->setGeometry(50, this->height() * 0.0655, this->width() - 100, this->height() * 0.135);
    // progress
    ui.progressBar->setGeometry(20, 50 + this->height() * 0.197, (this->width() - 40) - this->height() * 0.197 - 10, this->height() * 0.197);
    ui.stage->setGeometry((this->width() - 40) - this->height() * 0.25 - 5, 50 + this->height() * 0.197, 50, this->height() * 0.197);
    // the 'run' button
    ui.btnProgress->setGeometry((this->width() - 40) - this->height() * 0.197 + 20, 50 + this->height() * 0.197, this->height() * 0.197, this->height() * 0.197);
    ui.btnStopProgress->setGeometry((this->width() - 40) - this->height() * 0.197 + 20, 50 + this->height() * 0.197, this->height() * 0.197, this->height() * 0.197);
    // ======================================================================================================================================
}


void gui::bringFront() {
    ui.label->raise();
    ui.label_2->raise();
    ui.label_3->raise();
    ui.dataline1->raise();
    ui.dataline2->raise();
    ui.dataline3->raise();
    ui.btnopen1->raise();
    ui.btnopen2->raise();
    ui.btnopen3->raise();
    ui.dataInput1->raise();
    ui.btnStopProgress->hide();
}


void gui::readHistoricalData()
{
    currentpath = dir.currentPath();
    aa = "..\\setup.bat";
    QString fileName = currentpath + "/historical_data.txt";

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream txtInput(&f);
    QString lineStr;
    int Noi = -1;
    while (!txtInput.atEnd())
    {
        if (Noi == -1)
        {
            Noi = 0;
        }
        lineStr = txtInput.readLine();
        if (Noi == 0)
        {
            HistoricalDir1 = lineStr;
            ui.dataline1->setText(lineStr);
        }
        if (Noi == 1)
        {
            HistoricalDir2 = lineStr;
            ui.dataline2->setText(lineStr);
        }
        if (Noi == 2)
        {
            HistoricalDir3 = lineStr;
            ui.dataline3->setText(lineStr);
        }

        Noi++;
    }
    f.close();

    set_init_Yaml(); // params 
}

void gui::on_btnOpen_clicked(QString a, int x) {

    if (a != "")
    {

        QString dlgTitle = "Choose File"; 
        QString filter = "All Files(*.*)"; 
        QString aFileName = QFileDialog::getOpenFileName(this, dlgTitle, a, filter);
        if (!aFileName.isEmpty()) {
            if (x == 1) {
                ui.dataline1->setText(aFileName);
            }
            else if (x == 2) {
                ui.dataline2->setText(aFileName);
            }
            else {
                ui.dataline3->setText(aFileName);
            }
        }
    }
    else {
        QMessageBox::information(NULL, QString(tr("Error")), QString(tr("Invalid file path.")));
    }
}

void gui::_btnOpen_clicked1()
{
    on_btnOpen_clicked(ui.dataline1->text(), 1);
}
void gui::_btnOpen_clicked2()
{
    on_btnOpen_clicked(ui.dataline2->text(), 2);
}
void gui::_btnOpen_clicked3()
{
    on_btnOpen_clicked(ui.dataline3->text(), 3);
}


static bool removeDir(const QString& dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists()) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = QDir().rmdir(dirName);
    }
    return result;
}

void gui::_btnRun_Close_clicked() {

    QMessageBox::StandardButton check;
    check = QMessageBox::information(NULL, QString(tr("The process is running")), QString(tr("Are you sure to stop the process?")), QMessageBox::Yes, QMessageBox::No);
    if (check == QMessageBox::Yes) {

        running = false;
        rtkRunning = false;
        rtkTxt = QString();

        ui.dataInput1->setText(QString(tr("Waiting..")));

        if (cmd_yaml != NULL) {

            disconnect(cmd_yaml, 0, 0, 0);
            //cmd_yaml->terminate();
            cmd_yaml->kill();
            cmd_yaml = NULL;
            std::cout << "rosparam x "<< std::endl;
        }
        if (processCore != NULL) {

            disconnect(processCore, 0, 0, 0);
            processCore->kill();
            processCore = NULL;
            std::cout << "roscore x "<< std::endl;
        }
        if (processCheck != NULL) {

            disconnect(processCheck, 0, 0, 0);
            processCheck->kill();
            processCheck = NULL;
            std::cout << "rosbag info x "<< std::endl;
        }
        if (processCheckC != NULL) {

            disconnect(processCheckC, 0, 0, 0);
            processCheckC->kill();
            processCheckC = NULL;
            std::cout << "rosbag check x "<< std::endl;
        }
        if (processStart != NULL) {

            disconnect(processStart, 0, 0, 0);
            processStart->kill();
            processStart = NULL;
            std::cout << "algorithm1 x "<< std::endl;
        }
        if (processM != NULL) {

            disconnect(processM, 0, 0, 0);
            processM->kill();
            processM = NULL;
            std::cout << "merge x "<< std::endl;
        }
        if (processRecord != NULL) {

            disconnect(processRecord, 0, 0, 0);
            processRecord->kill();
            processRecord = NULL;
            std::cout << "record x "<< std::endl;
            closeAll(); // waiting for shutting down rosbag record

        }
        if (processMkdir != NULL) {

            disconnect(processMkdir, 0, 0, 0);
            processMkdir->kill();
            processMkdir = NULL;
            std::cout << "mkdir x "<< std::endl;

        }


        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished(-1);                    
        QString str1 = process.readAllStandardOutput();

        if (str1.contains("algorithm1_nrt.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt.exe"));
        }
        if (str1.contains("merge.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
        }
        if (str1.contains("algorithm1_nrt_loopclosure.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_loopclosure.exe"));
        }
        if (str1.contains("algorithm1_nrt_algorithm2.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_algorithm2.exe"));
        }
        if (str1.contains("merge_loopclosure.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("merge_loopclosure.exe"));
        }
        if (str1.contains("algorithm1_nrt_rtk.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk.exe"));
        }
        if (str1.contains("algorithm1_nrt_rtk_eoe.exe"))
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk_eoe.exe"));
        }

        QEventLoop loop;
        QTimer::singleShot(3000, &loop, SLOT(quit())); 
        loop.exec(); 

        std::cout << " \n";
        QFileInfo file1;
        file1 = QString("%1").arg(DatatimeabsoDir);
        if (file1.exists() == true) {

            QDir dir(DatatimeabsoDir);

            std::cout << std::boolalpha << dir.removeRecursively() + " \n" << std::endl;
            //std::cout << std::boolalpha << removeDir(DatatimeabsoDir) + " \n" << std::endl;

            std::cout << DatatimeabsoDir.toStdString() + " deleted \n";
        }


        playbagIndex = 0; // for 2/3
        count = 1; // for 3/3

        process.close();
        std::cout << "cancelled \n"<< std::endl;
        ui.dataInput1->setText(QString(tr("Cancelled, drag a .bag file to begin.")));
        ui.btnProgress->setVisible(true);
        ui.btnStopProgress->hide();
        ui.btnStopProgress->setEnabled(false);

    }
}

void gui::dragEnterEvent(QDragEnterEvent* e)
{
    if (disableDrop) {
        return;
    }

    if (ui.stackedWidget->currentIndex() == 0) {
        if ((e->pos().y()) > 100.0 && (e->pos().y()) < 300.0)
        {
            e->acceptProposedAction();	
        }
    }
}

void gui::dropEvent(QDropEvent* e)
{
    if (ui.stackedWidget->currentIndex() == 0) {
        if (running == true) { // if data is running
            QMessageBox::information(NULL, QString(tr("The process is running")), QString(tr("Please wait for the current file to be processed.")), QMessageBox::Ok);
        }
        else { // first time or finished
            filename = e->mimeData()->urls().first().toString();
            if (filename.isEmpty()) { ui.dataInput1->setText(QString(tr("Empty File! Please Retry."))); }
            else {
                if (filename.toStdString().substr(filename.toStdString().find_last_of(".") + 1) == "bag") {
                    std::string draggedfilename = filename.toStdString().substr(filename.toStdString().find_last_of("/") + 1);
                    QString qdraggedfilename = QString::fromStdString(draggedfilename);
                    ui.dataInput1->setText(QString(tr("\"%1\" received. Please press 'Run' button to continue!")).arg(qdraggedfilename));
                    ui.btnProgress->setEnabled(true);
                    ui.stage->setText("0/3"); // the label for running the .bag file input
                    ui.progressBar->setValue(0);
                }
                else {
                    ui.dataInput1->setText(QString(tr("Denied! Please drag .bag file only.")));
                }
                ui.dataInput1->setCursorPosition(0);
            }
        }
    }
}

void gui::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) { // can drag at anywhere
        move1 = true;
        lastPos = event->globalPos() - pos();
    }
    return QFrame::mousePressEvent(event);
}

void gui::mouseMoveEvent(QMouseEvent* event)
{
    if (move1 && (event->buttons() && Qt::LeftButton) && (event->globalPos() - lastPos).manhattanLength() > QApplication::startDragDistance()) {
        move(event->globalPos() - lastPos);
        lastPos = event->globalPos() - pos();
    }
    return QFrame::mouseMoveEvent(event);
}

void gui::mouseReleaseEvent(QMouseEvent* event)
{
    move1 = false;
}

void gui::minimize_mainwindow() {
    this->setWindowState(Qt::WindowMinimized);
}

void gui::max_mainwindow() {

    if (!max_pressed) {
        prev_width = geometry().width();
        prev_height = geometry().height();

        QDesktopWidget* desktop = QApplication::desktop();
        QRect desktopGeometry = desktop->availableGeometry();
        int desktopHeight = desktopGeometry.height();
        int desktopWidth = desktopGeometry.width();
        resize(desktopWidth, desktopHeight);

        int padx = (frameGeometry().width() - geometry().width()) / 2;
        move(-padx, 0);

        max_pressed = true;
    }
    else {
        QDesktopWidget* desktop = QApplication::desktop();
        QRect desktopGeometry = desktop->availableGeometry();

        int desktopHeight1 = prev_height;
        int desktopWidth1 = prev_width;
        resize(desktopWidth1, desktopHeight1);

        int padx = (desktopGeometry.width() - prev_width) / 2;
        int pady = (desktopGeometry.height() - prev_height) / 2;
        move(+padx, +pady);

        max_pressed = false;
    }
}
void gui::closeEvent(QCloseEvent* event) {
    emit(_closed());
}
gui::~gui() {
    Rtk.close();
    setParams->close();
    std::cout << " process gui down " << std::endl;
}


void gui::deleteProcesses()
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
    //if (str1.contains("mainApp.exe")) 
    //{
    //    QProcess::startDetached("taskkill -t  -f /IM " + QString("mainApp.exe"));
    //}
    if (str1.contains("algorithm1_nrt.exe")) 
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt.exe"));
    }
    if (str1.contains("merge.exe")) 
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
    }
    if (str1.contains("algorithm1_nrt_loopclosure.exe")) 
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_loopclosure.exe"));
    }
    if (str1.contains("algorithm1_nrt_algorithm2.exe")) 
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_algorithm2.exe"));
    }
    if (str1.contains("merge_loopclosure.exe")) 
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("merge_loopclosure.exe"));
    }
    if (str1.contains("algorithm1_nrt_rtk.exe")) 
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk.exe"));
    }
    if (str1.contains("algorithm1_nrt_rtk_eoe.exe")) 
    {
        QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk_eoe.exe"));
    }

    if(cmd_yaml!=NULL)cmd_yaml->close();
    if (processCore != NULL)processCore->close();
    if (processCore1 != NULL)processCore1->close();
    if (processCheck != NULL)processCheck->close();
    if (processCheckC != NULL)processCheckC->close();
    if (processStart != NULL)processStart->close();
    if (processMkdir != NULL)processMkdir->close();
    if (processRecord != NULL)processRecord->close();
    if (processM != NULL)processM->close();
    if (processD != NULL)processD->close();
    if (processCl != NULL)processCl->close();
    process.close();

}
void gui::_btnRtk_clicked() {

    if (!ui.btnRtk->isChecked()) {
        Rtk.close();
        disableDrop = false;
    }
    else {
        Rtk.show();
        disableDrop = true;
    }
   
}

void gui::_rtk_ready_(QString bag, QString txt) {
    //std::cout << bag.toStdString() << std::endl;
    std::cout << "gcp check: " << gcpChecked << std::endl;

    disableDrop = false;
    ui.dataInput1->setText(QString(tr("You may start the RTK process by pressing the Run button.")));
    ui.btnProgress->setEnabled(true);
    ui.stage->setText("0/3"); // the label for running the .bag file input
    ui.progressBar->setValue(0);

    rtkTxt = txt;
    filename = bag;
    rtkRunning = true;
}

void gui::_rtk_close_() {
    disableDrop = false;
    ui.btnRtk->setChecked(false);
    Rtk.close();
}

void gui::setGcpChecked(bool b) {
    gcpChecked = b;
}

void gui::_btnSetting_clicked() {
    if (!ui.btnSetting->isChecked()) {
        setParams->close();

    }
    else {
        setParams->show();

    }


}

void gui::_btnSetting_close() {

    ui.btnSetting->setChecked(false);
    setParams->close();
}

void gui::setParams_fromSetting(int no_of_cores, bool loopClosure, int loopClosureIntervalFrames, bool narrowSpace, float minIntensityFilter, float lidarMaxRange, float lidarMinRange, float lidarMaxXToFilter, float lidarMinXToFilter, float translationThreshold, int icpMaximumIterations)
{
    //// gui page
    //this->sensor = ui.comboBoxSensor->currentText();
    //this->csv = ui.checkBoxCsv->isChecked();
    //this->las = ui.checkBoxLas->isChecked();
    //this->rotatingFrames = ui.checkBoxRotation->isChecked();

    // setting 
    this->no_of_cores = no_of_cores;
    this->loopClosure = loopClosure;
    this->loopClosureIntervalFrames = loopClosureIntervalFrames;
    this->narrowSpace = narrowSpace;
    this->minIntensityFilter = minIntensityFilter;
    this->lidarMaxRange = lidarMaxRange;
    this->lidarMinRange = lidarMinRange;
    this->lidarMaxXToFilter = lidarMaxXToFilter;
    this->lidarMinXToFilter = lidarMinXToFilter;
    this->translationThreshold = translationThreshold;
    this->icpMaximumIterations = icpMaximumIterations;


    std::cout << "gui ---" << std::endl;
    //std::cout << "---  --- from gui" << std::endl;
    //std::cout << "sensor: " << sensor.toStdString() << std::endl;
    //std::cout << "csv: " << std::to_string(csv) << std::endl;
    //std::cout << "las: " << std::to_string(las) << std::endl;
    //std::cout << "rotatingFrames: " << std::to_string(rotatingFrames) << std::endl;
    std::cout << "---  --- from setting" << std::endl;
    std::cout << "core: " << std::to_string(no_of_cores) << std::endl;
    std::cout << "loopClosure: " << std::to_string(loopClosure) << std::endl;
    std::cout << "loopClosureIntervalFrames: " << std::to_string(loopClosureIntervalFrames) << std::endl;
    std::cout << "narrowSpace: " << std::to_string(narrowSpace) << std::endl;
    std::cout << "minIntensityFilter: " << std::to_string(minIntensityFilter) << std::endl;
    std::cout << "lidarMaxRange: " << std::to_string(lidarMaxRange) << std::endl;
    std::cout << "lidarMinRange: " << std::to_string(lidarMinRange) << std::endl;
    std::cout << "lidarMaxXToFilter: " << std::to_string(lidarMaxXToFilter) << std::endl;
    std::cout << "lidarMinXToFilter: " << std::to_string(lidarMinXToFilter) << std::endl;
    std::cout << "translationThreshold: " << std::to_string(translationThreshold) << std::endl;
    std::cout << "icpMaximumIterations: " << std::to_string(icpMaximumIterations) << std::endl;
    std::cout << "------------------------" << std::endl;

}

void gui::set_init_Yaml() {

    // gui page
    this->sensor = ui.comboBoxSensor->currentText();
    ui.checkBoxCsv->setChecked(false);
    this->csv = ui.checkBoxCsv->isChecked();
    ui.checkBoxLas->setChecked(true);
    this->las = ui.checkBoxLas->isChecked();
    ui.checkBoxRotation->setChecked(true);
    this->rotatingFrames = ui.checkBoxRotation->isChecked();

    // setting 
    this->no_of_cores = 24;
    this->loopClosure = false;
    this->loopClosureIntervalFrames = 600;
    this->narrowSpace = false;
    this->minIntensityFilter = 1.0;
    this->lidarMaxRange = 50.0;
    this->lidarMinRange = 1.0;
    this->lidarMaxXToFilter = 0.0;
    this->lidarMinXToFilter = -0.6;
    this->translationThreshold = 0.25;
    this->icpMaximumIterations = 5;

    /* rosparam set / algorithm1 / numberOfCores 24
 rosparam set / algorithm1 / useLoopClosure true
 rosparam set / algorithm1 / loopClosureIntervalFrames  600
 rosparam set / algorithm1 / isNarrowSpace false
 rosparam set / algorithm1 / savePoseToCSV false
 rosparam set / algorithm1 / saveMapToLAS true
 rosparam set / algorithm1 / sensor pandar
 rosparam set / algorithm1 / minIntensity 1.0
 rosparam set / algorithm1 / lidarMaxRange 50.0
 rosparam set / algorithm1 / lidarMinRange 1.0
 rosparam set / algorithm1 / lidarMaxXToFilter 0.0
 rosparam set / algorithm1 / lidarMinXToFilter - 0.6
 rosparam set / algorithm1 / onlyRotationFramesUsedForMapping true
 rosparam set / algorithm1 / translationThreshold 1000
 rosparam set / algorithm1 / icpMaximumIterations 5*/

}