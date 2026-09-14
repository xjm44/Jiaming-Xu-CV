#include <iomanip>
#include "../../include/include_process/processBag.h"
#include "../../include/include_gui/widget_newProcess.h"
#include <QTimer>
#include <iostream>
#include <QFileInfo>
#include <fstream>
#include <sstream>
#include <QDateTime>
#include <QDir>
#include "yaml-cpp/yaml.h"

processBag::processBag(widget_newProcess* parent)
    : QWidget(parent)
    , paramIndex(0), algorithm1Index(0)
    , algorithm2Index(0), algorithm2_size(0), algorithm2Remaining(0)
    , gcpCp2ndRound(false), gcpRemaining(0), gcpParamIndex(0)
    ,rtkRemaining(0)
    ,rtkIndex(0)   
    , baRemaining(0)
    , type(process_type::Slam),    
    frame_size(0)
    , total_time(0.0),
    process_running(false)
    
{

    init();
	this->parent = parent;
}
processBag::~processBag() {
}
void processBag::init() {
    
    ros_setup = " ";
}

void processBag::startTheRun(QString bagPath) {

#ifdef __linux__
#endif

    parent->labelRunning->setText("running..");
    //parent->running = true;
    parent->setStatus(running_status::running);
    parent->startWindowTimer(); // for timing
    process_running = true;

    std::cout << bagPath.toStdString() << std::endl; // eg. E:/2023-04-26-03-33-48.bag
    this->bagPath = bagPath;
    //std::cout << ros_setup.toStdString() << std::endl;

    parent->setLog(tr("starting.."));



                                                                                                         
    // ***************************************************************************
    // QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // env.insert("LD_LIBRARY_PATH" , "../ros/noetic/lib");
    // env.insert("CMAKE_PREFIX_PATH" , "../ros/noetic");
    // env.insert("ROS_PACKAGE_PATH" , "../ros/noetic/share"); 
    // processStart->setProcessEnvironment(env);
    // processStart->start(QString(_algorithm1BagStr));
    // ***************************************************************************


    processCore = new QProcess;
    QString _coreStr = QString("source ../ros/noetic/setup.bash && roscore\n");
    processCore->start("bash", QStringList() << "-c" << _coreStr);


    connect(processCore, SIGNAL(readyReadStandardOutput()), this, SLOT(_roscoreStarts()));

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "starting" << std::endl;
    std::cout << "============================" << std::endl;
    
}
void processBag::_roscoreStarts() {      
    QString str = QString(processCore->readAllStandardOutput()); //roscore need run 2 times to get output
    int index = str.lastIndexOf("NODES");
    //std::cout << str.toStdString() << std::endl;

    if (index > -1) {
        //_roscore_output();
        setYaml();
        return;
    }
    else {
        //QEventLoop loop1;
        //QTimer::singleShot(1000, &loop1, SLOT(quit()));
        //loop1.exec();
    }

}
void processBag::_roscore_output() {
    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "roscore started" << std::endl;
    std::cout << "============================" << std::endl;
    parent->setProgressBar(2);
    parent->setLog(tr("core services started."));

    cmd_yaml = new QProcess;
    QString _startParams = "";

    // ==========================================================================================================================================================================

    if (parent->getType() == process_type::RTK) {
        _startParams = "rosparam load ../Lib/company/params_rtk.yaml && echo CFYcfy";
    }
    else { // slam type
        //if (parent->getSlamParam().at(17) == "true") {
        //    _startParams = "rosparam load ..\\Lib\\company\\params_plus.yaml && echo CFYcfy";
        //}
        //else {
            _startParams = tr("rosparam load ../Lib/company/params_comb.yaml && echo CFYcfy");
        //}
    }

    cmd_yaml->start(_startParams+"\n");
    connect(cmd_yaml, SIGNAL(readyReadStandardOutput()), this, SLOT(_checkYaml()));
    // ==========================================================================================================================================================================
    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "rosparam starting" << std::endl;
    std::cout << "============================" << std::endl;
    
}
void processBag::_checkYaml() {
    QString str = QString(cmd_yaml->readAllStandardOutput());
    int index = str.lastIndexOf("cfy"); // will echo twice
    //std::cout << str.toStdString() << std::endl;
    if (index > 1) {
        paramIndex++;
        if (paramIndex > 1) {

            if (parent->getType() == process_type::RTK) {
                setYaml_Rtk();
                std::cout << "set rtk param" << std::endl;
            }
            else { // slam type
                //if (parent->getSlamParam().at(17) == "true") { // algorithm2
                //    //checkbagCheck();
                //    checkbagInfo();
                //}
                //else { // algorithm1
                    setYaml();
                    std::cout << "set algorithm1 param" << std::endl;
                //}
            }
            paramIndex = 0;
        }
    }
}
void processBag::setYaml(){
    parent->setProgressBar(2);
    parent->setLog(tr("configuration in progress.."));

    YAML::Node config = YAML::LoadFile("../Lib/company/params_comb.yaml");

    // method to gcp
    config["algorithm1"]["methodToUse"] = 0; // normal algorithm1

    // no of cores
    config["algorithm1"]["numberOfCores"] = parent->getSlamParam().at(0).toInt();
    // las
    // if (parent->getSlamParam().at(16) == "true")  // gcp no need las // parent->getType() == process_type::GCP
    //     config["algorithm1"]["saveMapToLAS"] = false;
    // else 
        config["algorithm1"]["saveMapToLAS"] = true; 
    // csv
    if (parent->getSlamParam().at(2) == "true")
        config["algorithm1"]["savePoseToCSV"] = true;
    else
        config["algorithm1"]["savePoseToCSV"] = false;
    // rotating frames
    if (parent->getSlamParam().at(3) == "true")
        config["algorithm1"]["onlyRotationFramesUsedForMapping"] = true;
    else
        config["algorithm1"]["onlyRotationFramesUsedForMapping"] = false;
    // loop closure
    if (parent->getType() == process_type::GCP || parent->getType() == process_type::RTK) { // gcp no need loop closure
        config["algorithm1"]["useLoopClosure"] = false;
    }
    else {
        if (parent->getSlamParam().at(4) == "true") {
            config["algorithm1"]["useLoopClosure"] = true;
            config["algorithm1"]["loopClosureIntervalFrames"] = parent->getSlamParam().at(5).toInt(); // loop closure interval frames
        }
        else
            config["algorithm1"]["useLoopClosure"] = false;
    }
    
    // narrow space
    if (parent->getSlamParam().at(6) == "true")
        config["algorithm1"]["isNarrowSpace"] = true;
    else
        config["algorithm1"]["isNarrowSpace"] = false;
    // min intensity filter
    config["algorithm1"]["minIntensity"] = parent->getSlamParam().at(7).toFloat();
    // lidar max range
    config["algorithm1"]["lidarMaxRange"] = parent->getSlamParam().at(8).toFloat();
    // lidar min range
    config["algorithm1"]["lidarMinRange"] = parent->getSlamParam().at(9).toFloat();
    // lidar max x
    config["algorithm1"]["lidarMaxXToFilter"] = parent->getSlamParam().at(10).toFloat();
    // lidar min x
    config["algorithm1"]["lidarMinXToFilter"] = parent->getSlamParam().at(11).toFloat();
    // translation threshold
    config["algorithm1"]["translationThreshold"] = parent->getSlamParam().at(12).toFloat();
    // icp iteration
    config["algorithm1"]["icpMaximumIterations"] = parent->getSlamParam().at(13).toInt();
    // loss sigma
    config["algorithm1"]["lossSigma"] = parent->getSlamParam().at(17).toFloat();
    // to laz
    if (parent->getSlamParam().at(18) == "true")
        config["algorithm1"]["compressLas"] = true;
    else
        config["algorithm1"]["compressLas"] = false;
    // map resolution
    config["algorithm1"]["mapResolution"] = parent->getSlamParam().at(19).toFloat();
    // algorithm2: save bag
    //if (parent->getSlamParam().at(17) == "true" || parent->getType() == process_type::GCP) // gcp need this bag for merge: 
    config["algorithm1"]["saveSLAMToBag"] = true;
    //else
        //config["algorithm1"]["saveSLAMToBag"] = false; 
    // voxel size
    config["algorithm1"]["voxelSize"] = parent->getSlamParam().at(21).toFloat();

    //====================================================================================

    if (parent->getType() == process_type::GCP) {
        //// method to gcp
        //config["algorithm1"]["methodToUse"] = 2;
        // useGTSAMOptimizer
        config["algorithm1"]["usecompanyOptimizer"] = true;
        // saveGTSAMPath
        config["algorithm1"]["savecompanyPath"] = true;
           
    }


    if (parent->getType() == process_type::RTK) {
        // method to gcp
        config["algorithm1"]["methodToUse"] = 1;
        // use geoid
        if (parent->getRtkParam().at(0) == "true")
            config["algorithm1"]["useGeoid"] = true;
        else
            config["algorithm1"]["useGeoid"] = false;
        // outputCoordinateSystem   
        QString temp = parent->getRtkParam().at(1);
        // std::string x = temp.toStdString();
        // temp = QString("%1").arg(temp);
        config["algorithm1"]["outputCoordinateSystem"] = temp.toStdString();
        // gpsPositionOffset_x
        config["algorithm1"]["gpsPositionOffset_x"] = parent->getRtkParam().at(2).toFloat();
        // gpsPositionOffset_y
        config["algorithm1"]["gpsPositionOffset_y"] = parent->getRtkParam().at(3).toFloat();
        // gpsPositionOffset_z
        config["algorithm1"]["gpsPositionOffset_z"] = parent->getRtkParam().at(4).toFloat();
        // gpsFrequence
        config["algorithm1"]["gpsFrequency"] = parent->getRtkParam().at(5).toInt();

        // hemisphere: only southern and northern for now
        if (parent->getRtkParam().at(7) == "true")
            config["algorithm1"]["inSouthernHemisphere"] = true;
        else
            config["algorithm1"]["inSouthernHemisphere"] = false;
        // useGTSAMOptimizer
        config["algorithm1"]["usecompanyOptimizer"] = true;

        // gpsElevation
        config["algorithm1"]["useGpsElevation"] = true;
        // walkBackAndForth
        config["algorithm1"]["walkBackAndForthMode"] = false;
    }


    QString output = "../Lib/company/params.yaml";
    QFile file(output); // remove if exists
    file.remove();
    std::ofstream fout("../Lib/company/params.yaml");
    fout << config << std::endl; // dump it back into the file
    fout.close();


    //checkbagCheck();
    //checkbagInfo();
    std::cout<<"done params"<<std::endl;
    _startBag();

}

void processBag::setYaml_Rtk() {
    parent->setProgressBar(2);
    parent->setLog(tr("configuration in progress.."));

    YAML::Node config = YAML::LoadFile("../Lib/company/params_rtk.yaml");

    //====================================================================================
    // sensor
    config["algorithm1"]["sensor"] = parent->getRtkParam().at(0).toStdString(); 
    // no of cores
    config["algorithm1"]["numberOfCores"] = parent->getRtkParam().at(1).toInt(); 
    // las (TRUE FOR NOW)
    //if (parent->getSlamParam().at(16) == "true")
        //config["algorithm1"]["saveMapToLAS"] = false; 
    //else
        config["algorithm1"]["saveMapToLAS"] = true; 
    // csv
    if (parent->getRtkParam().at(3) == "true")
        config["algorithm1"]["savePoseToCSV"] = true; 
    else
        config["algorithm1"]["savePoseToCSV"] = false; 
    // rotating frames
    if (parent->getRtkParam().at(4) == "true")
        config["algorithm1"]["onlyRotationFramesUsedForMapping"] = true; 
    else
        config["algorithm1"]["onlyRotationFramesUsedForMapping"] = false; 
    // narrow space
    if (parent->getRtkParam().at(5) == "true")
        config["algorithm1"]["useGeoid"] = true;
    else
        config["algorithm1"]["useGeoid"] = false;
    // min intensity filter
    config["algorithm1"]["minIntensity"] = parent->getRtkParam().at(6).toFloat();
    // lidar max range
    config["algorithm1"]["lidarMaxRange"] = parent->getRtkParam().at(7).toFloat();
    // lidar min range
    config["algorithm1"]["lidarMinRange"] = parent->getRtkParam().at(8).toFloat();
    // lidar max x
    config["algorithm1"]["lidarMaxXToFilter"] = parent->getRtkParam().at(9).toFloat();
    // lidar min x
    config["algorithm1"]["lidarMinXToFilter"] = parent->getRtkParam().at(10).toFloat();
    // translation threshold
    config["algorithm1"]["translationThreshold"] = parent->getRtkParam().at(11).toFloat();
    // icp iteration
    config["algorithm1"]["icpMaximumIterations"] = parent->getRtkParam().at(12).toInt();
    // outputCoordinateSystem   
    QString temp = parent->getRtkParam().at(16);
    // std::string x = temp.toStdString();
    // temp = QString("%1").arg(temp);
    config["algorithm1"]["outputCoordinateSystem"] = temp.toStdString(); 
    // gpsPositionOffset_x
    config["algorithm1"]["gpsPositionOffset_x"] = parent->getRtkParam().at(17).toFloat();
    // gpsPositionOffset_y
    config["algorithm1"]["gpsPositionOffset_y"] = parent->getRtkParam().at(18).toFloat();
    // gpsPositionOffset_z
    config["algorithm1"]["gpsPositionOffset_z"] = parent->getRtkParam().at(19).toFloat();
    // gpsFrequence
    config["algorithm1"]["Frequency"] = parent->getRtkParam().at(20).toInt();
    // algorithm2: save bag
    config["algorithm1"]["saveSLAMToBag"] = true; 
    // hemisphere: only southern and northern for now
    if (parent->getRtkParam().at(23) == "true")
        config["algorithm1"]["inSouthernHemisphere"] = true;
    else
        config["algorithm1"]["inSouthernHemisphere"] = false;
    //====================================================================================

    //checkbagCheck();
    //checkbagInfo();
    QString output = "./temp.yaml";   
    QFile file(output); // remove if exists
    file.remove();
    std::ofstream fout("./temp.yaml"); 

    fout << config<< std::endl; // dump it back into the file
    fout.close();
    std::cout<<"done rtk"<<std::endl;
    _startBag();

}
void processBag::checkbagCheck() {

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "rosparam started" << std::endl;
    std::cout << "============================" << std::endl;
    parent->setProgressBar(10);
    parent->setLog(tr("parameters checked."));

    QString lidarBag = bagPath;
    QString path1 = "";
    QString _checkbagCheck = "";

    processCheckC = new QProcess; // preparation
    processCheckC->start("cmd.exe");

    path1 = lidarBag.replace('/', '\\');
    _checkbagCheck = QString("rosbag check %1").arg(path1);

    std::cout << "bag path:   " << _checkbagCheck.toStdString() << std::endl;


    // write to terminal
    processCheckC->write(QString(ros_setup + "\r\n").toUtf8());
    processCheckC->write(QString(_checkbagCheck + "\r\n").toUtf8());


    connect(processCheckC, SIGNAL(readyReadStandardOutput()), this, SLOT(_bagcheck_output()));

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "rosbag check checking" << std::endl;
    std::cout << "============================" << std::endl;
}
void processBag::_bagcheck_output() {
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
        QTimer::singleShot(1000, &loop1, SLOT(quit()));
        loop1.exec();
        //====================================================================================
        checkbagInfo();
        return;
    }
}
void processBag::checkbagInfo() {

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "rosbag check checked" << std::endl;
    std::cout << "============================" << std::endl;
    parent->setProgressBar(10);
    parent->setLog(tr("checking bag.."));


    QString lidarBag = bagPath;
    QString path1 = "";
    QString  _checkbagInfo = "";
    processCheck = new QProcess; // preparation
    
    path1 = lidarBag;
    _checkbagInfo = QString("rosbag info %1").arg(path1);

    // write to terminal
    processCheck->start(_checkbagInfo + "\n");
    connect(processCheck, SIGNAL(readyReadStandardOutput()), this, SLOT(_baginfo_output()));

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "rosbag info checking" << std::endl;
    std::cout << "============================" << std::endl;

}
void processBag::_baginfo_output() {

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
        ////====================================================================================
        QString get_total_frame = "";
        for (int i = hesaiIndex; i < end; i++) {
            get_total_frame += str.at(i);
        }
        QStringList l = get_total_frame.split(" ");
        frame_size = l.at(l.size() - 2).toInt(); // get frame number
        QEventLoop loop1;
        QTimer::singleShot(1000, &loop1, SLOT(quit()));
        loop1.exec();
        ////====================================================================================
        _startBag();
    }
}
void processBag::_startBag() {
    if (process_running) {
    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "algorithm1 starting" << std::endl;
    std::cout << "============================" << std::endl;
    parent->setProgressBar(3);
    parent->setLog(tr("algorithm1 in processing.."));

    processStart = new QProcess;
    //processStart->start("sh");

    QString path1 = "";
    QString  _algorithm1BagStr = "";
    QString lidarBag = bagPath;

    //// get current dir
    //QString path2 = "";
    //int lidarbagIndex = lidarBag.lastIndexOf("/");
    //path2 = lidarBag.left(lidarbagIndex + 1);

    QString path = parent->getSlamParam().at(15) + "/";
    QString Fpath = path;

    QString name = parent->getSlamParam().at(14);
    QString Fname = name;

    path1 = lidarBag;

    if (parent->getType() == process_type::RTK) {
        QString rtkFile = parent->getRtkFile();
        if(rtkFile.isEmpty())rtkFile="void";
        QString rtkPath = rtkFile;

        //_algorithm1BagStr = QString("LD_LIBRARY_PATH='../ros/noetic/lib':$LD_LIBRARY_PATH:'../Lib/company' ../Lib/company/algorithm1_nrt_rtk %1 ./temp.yaml %2 %3 %4 && echo CFYplaybagendcfy").arg(path1).arg(rtkPath).arg(Fpath).arg(name);
        //_algorithm1BagStr = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib':'../Lib/company' ../Lib/company/algorithm1_nrt_rtk %1 ./temp.yaml %2 %3 %4 && echo CFYplaybagendcfy").arg(path1).arg(rtkPath).arg(Fpath).arg(name);
        _algorithm1BagStr = QString("source ../ros/noetic/setup.bash && ../ros/noetic/bin/algorithm1_nrt -bag=%1 -algorithm1=../Lib/company/params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 -log_path=../Log/\n").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname).arg(rtkPath);
             
    }
    else { // slam type OR gcp (for merging)

        // for gcp
        if (parent->getType() == process_type::GCP) {
            // new folder for ply file(s)
            gcpPlyFolder = (QDateTime::currentDateTime()).toString("yyyy.MM.dd_hh:mm:ss");
            gcpPlyFolder = gcpPlyFolder.replace('.', '_');
            gcpPlyFolder = gcpPlyFolder.replace(':', '_');
            gcpPlyFolder = QString("controlPts1st");
            gcpPlyFolder = parent->getSlamParam().at(15) + "/"+gcpPlyFolder;
            //bag =================================================================================
            QProcess* processMkdir = new QProcess;
            QString D3_mkdirBagFile = QString("mkdir %1").arg(gcpPlyFolder);
            processMkdir->start(D3_mkdirBagFile+"\n");
            std::cout << "creating a new folder for ply file(s)" << std::endl;
            QEventLoop loop1;
            QTimer::singleShot(1000, &loop1, SLOT(quit()));
            loop1.exec();
            // ================================================================================================================
    
            //_algorithm1BagStr = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib' ../ros/noetic/bin/algorithm1_nrt %1 ./temp.yaml %2 %3 && echo CFYplaybagendcfy").arg(path1).arg(gcpPlyFolder).arg(Fname);

            _algorithm1BagStr = QString("source ../ros/noetic/setup.bash && ../ros/noetic/bin/algorithm1_nrt -bag=%1 -algorithm1=../Lib/company/params.yaml -product=%2 -save_path=%3 -save_name=%4 -log_path=../Log/\n").arg(path1).arg(parent->getSlamParam().at(22)).arg(gcpPlyFolder).arg(Fname);
                 
        }
        else {
            // run the bag

            // start + write
            //_algorithm1BagStr = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib' ../ros/noetic/bin/algorithm1_nrt -bag=%1 -algorithm1=../Lib/company/params.yaml -product=%2 -save_path=%3 -save_name=%4 && echo CFYplaybagendcfy").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname);
            
            // with qprocess env setting
            //_algorithm1BagStr = QString("../ros/noetic/bin/algorithm1_nrt -bag=%1 -algorithm1=../Lib/company/params.yaml -product=%2 -save_path=%3 -save_name=%4\n").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname);

            // bash + source
            _algorithm1BagStr = QString("source ../ros/noetic/setup.bash && ../ros/noetic/bin/algorithm1_nrt -bag=%1 -algorithm1=../Lib/company/params.yaml -product=%2 -save_path=%3 -save_name=%4 -log_path=../Log/\n").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname);
                                                                                                          
        }       
    }
    std::cout << _algorithm1BagStr.toStdString() << std::endl;
    // ***************************************************************************
    // QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // env.insert("LD_LIBRARY_PATH" , "../ros/noetic/lib");
    // env.insert("CMAKE_PREFIX_PATH" , "../ros/noetic");
    // env.insert("ROS_PACKAGE_PATH" , "../ros/noetic/share"); 
    // processStart->setProcessEnvironment(env);
    // processStart->start(QString(_algorithm1BagStr));
    // ***************************************************************************

    processStart->start("bash", QStringList() << "-c" << _algorithm1BagStr);

    //connect(processStart, SIGNAL(readyReadStandardOutput()), this, SLOT(_startBag_output()));
    //connect(processStart, SIGNAL(readyReadStandardError()), this, SLOT(_startBag_output()));
    connect(processStart, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotAlgorithm1ExitCode(int, QProcess::ExitStatus)));
    connect(processStart, SIGNAL(readyReadStandardError()), this, SLOT(_startBag_output()));

    }
}
void processBag::slotAlgorithm1ExitCode(int a, QProcess::ExitStatus e) {

    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "algorithm1 successfully exits" << std::endl;

        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();

        algorithm1Done();
    }
    else {
        std::cout << "algorithm(algorithm1) in error." << std::endl;
        errInfo = "algorithm1";
        if (parent->progressBar->value() == 3) {
            errInfo = "algorithm1Egm";
        }else if (parent->progressBar->value() > 50) {
            errInfo = "algorithm1Space";
        }
        whenCrashed();
    }

    //if (e == QProcess::ExitStatus::NormalExit)std::cout << "byebye" << std::endl;
    //if (e == QProcess::ExitStatus::CrashExit)std::cout << "gg" << std::endl;
    //std::cout << "exit no.: " << std::to_string(a) << std::endl;
}
void processBag::_startBag_output() {
    //QString str = QString(processStart->readAllStandardOutput());
    QString err = QString(processStart->readAllStandardError());
    //QString str1 = QString(processStart->readAllStandardError());
    //std::cout << "---: " << str1.toStdString() << std::endl;
    //int index = str.lastIndexOf("playbagendcfy"); // will echo twice

    // // ===============================================
    // int index1 = str.lastIndexOf("[ Lidar ]");
    // int index2 = str.lastIndexOf(")  ---- /hesai/pandar");
    // if (index2 > -1 && index1 > -1) {

    //     QString text = str;
    //     int frame = 0;
    //     //QString text = "[ Lidar ]:  [0.049316 ~ 0.100096] avg: 0.050039 ( 1264)  ---- /hesai/pandar";
    //     QRegularExpression regex("\\((.*)\\)  ---- /hesai/pandar");
    //     QRegularExpressionMatchIterator i = regex.globalMatch(text);
    //     while (i.hasNext()) {
    //         QRegularExpressionMatch match = i.next();
    //         frame = match.captured(1).toInt();
    //         // std::cout << "Matched string: " << match.captured(0).toStdString()<<std::endl;
    //         // std::cout << "Start position: " << match.capturedStart()<<std::endl;
    //         // std::cout << "End position: " << match.capturedEnd()<<std::endl;
    //     }
    //     std::cout<< "frame size: " <<frame<< " ___" <<std::endl;
    //     frame_size = frame;
    //     // std::cout << "l: "<<index <<std::endl;
    //     // std::cout << "h: "<<index1 <<std::endl;
    //     // QString getFrames = str.mid(index,index1).simplified().section('(', 1, -1).simplified();
    //     // getFrames = getFrames.section(')', 0, 0).simplified();
    //     // std::cout<< "____: " <<getFrames.toStdString()<< " ___" <<std::endl;      
    // }

    // int check = str.lastIndexOf("Failure rate");
    // if (check > 0) {
    //     int indexDuration = str.indexOf(" | ") + 2;
    //     QString progress = str.mid(indexDuration);
    //     progress = progress.simplified().section(' ', 0, 0).simplified();
    //     int current_prog = (progress.toInt() * 100 / frame_size);

    //     if (parent->getType() == process_type::RTK) {
    //         parent->setProgressBar(3 + (current_prog * 0.2)); // total = 3 + 20 %
    //     }
    //     else { // slam / slam + gcp
    //         if (parent->getType() == process_type::GCP) {
    //             parent->setProgressBar(3 + (current_prog * 0.25)); // total = 3 + 25 %
    //         }
    //         else {
    //             if (parent->getSlamParam().at(16) == "true" ) // algorithm2
    //                 parent->setProgressBar(3 + (current_prog * 0.5)); // total = 3 + 50 %
    //             else // slam
    //                 parent->setProgressBar(3 + (current_prog * 0.90)); // total = 3 + 90 %
    //         }
    //     }

    //     std::cout << str.toStdString() << std::endl;
    // }
    //// ====================================

    // ================================================ algorithm1 WITH GLOG ================================================
    // get frame no. for later purposes
    int index = err.lastIndexOf("Valid LiDAR size:");
    //std::cout << "***** " << index << std::endl;
    if (index > -1) {

        QString text = err;
        int frame = 0;
        //[I20240105 10:04:51.723225 18456 Ba: 454] Staring shrink LiDAR messages ...
        //[I20240105 10:04 : 51.723225 18456 Ba : 464] Valid LiDAR size : 140
        //    [I20240105 10:04 : 51.723225 18456 Ba : 131] Trying to check timestamp ...
        //    [I20240105 10:04 : 51.723225 18456 Ba : 286]  IMU : the first timestamp from queue is 1677570281.2571361065 and last timestamp is 1677570288.247058000  
        QRegularExpression regex("Valid LiDAR size: (\\d+)"); // ƥ   
        QRegularExpressionMatchIterator i = regex.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            frame = match.captured(1).toInt();
        }
        std::cout << "frame size: " << frame << " ___" << std::endl;
        frame_size = frame;  
    }

    // for progress bar
    int check = err.lastIndexOf("Progress:");
    if (check > -1) {
        //[I20240105 09:45:13.509135 23752 Ma: 129] Progress: 0.7143% ( 1 / 140 )  Failure rate: 0% (0 / 140 )
        QRegularExpression regex("Progress: (([0-9]*[.])?[0-9]+)%"); // ƥ   
        QRegularExpressionMatchIterator i = regex.globalMatch(err);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            double current_percentile = match.captured(1).toDouble();
            int current_prog = static_cast<int>(current_percentile);

            if (parent->getType() == process_type::RTK) {
                parent->setProgressBar(3 + (current_prog * 0.2)); // total = 3 + 20 %
            }
            else { // slam / slam + gcp
                if (parent->getType() == process_type::GCP) {
                    parent->setProgressBar(3 + (current_prog * 0.25)); // total = 3 + 25 %
                }
                else {
                    if (parent->getSlamParam().at(16) == "true") // algorithm2
                        parent->setProgressBar(3 + (current_prog * 0.5)); // total = 3 + 50 %
                    else // slam
                        parent->setProgressBar(3 + (current_prog * 0.90)); // total = 3 + 90 %
                }
            }
        }
        std::cout << err.toStdString() << std::endl;
        
    }
}
void processBag::algorithm1Done() {
    if (process_running) {
    //// log done
    //logFile.flush();
    //logFile.close();
    if (parent->getType() == process_type::GCP) {
        _startGcpMerge(); 

    }else if (parent->getType() == process_type::RTK) {
        //allDone();
        std::vector<std::string> tempList = parent->getWindowTimer();
        // total_time += std::stod(tempList.at(0));
        // parent->setLog(tr("rtk done in %1").arg(QString::fromStdString(tempList.at(1))));
        // parent->startWindowTimer();
        // startalgorithm2();

        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("rtk's first stage done in %1").arg(QString::fromStdString(tempList.at(1))));
        parent->setLog(tr("rtk's final stage starts."));
        parent->startWindowTimer();
        start2ndRtk();
    }
    else {
        if (parent->getSlamParam().at(1) == "true") {
                std::vector<std::string> tempList = parent->getWindowTimer();
                total_time += std::stod(tempList.at(0));
                parent->setLog(tr("algorithm1 done in %1").arg(QString::fromStdString(tempList.at(1))));
                parent->startWindowTimer();
                startBa();

            }
        else if (parent->getSlamParam().at(16) == "true"){
            std::vector<std::string> tempList = parent->getWindowTimer();
            total_time += std::stod(tempList.at(0));
            parent->setLog(tr("algorithm1 done in %1").arg(QString::fromStdString(tempList.at(1))));
            parent->startWindowTimer();
            startalgorithm2();
        }
        else{
            std::vector<std::string> tempList = parent->getWindowTimer();
            total_time += std::stod(tempList.at(0));
            parent->setLog(tr("algorithm1 done in %1").arg(QString::fromStdString(tempList.at(1))));
            allDone();
        }
    }
    }
}

void processBag::startalgorithm2() {
    if (process_running) {
    parent->setLog(tr("starting algorithm1 plus.."));
    algorithm2Remaining = parent->progressBar->value();

    algorithm2Bag = parent->getSlamParam().at(15) + "/" + parent->getSlamParam().at(14) + QString("_SLAM.bag");

    
    if (parent->getSlamParam().at(1) == "true") { // after hba
        algorithm2Bag = parent->getSlamParam().at(15) + "/" + parent->getSlamParam().at(14) + QString("_SLAM.bag_optimized.bag");
    }

    if (parent->getType() == process_type::GCP) {
        if (gcpCp2ndRound) {
            algorithm2Bag = gcpPlyFolder2nd + "/" + parent->getSlamParam().at(14) + QString("_SLAM.bag");
        }
    }

    //algorithm2Bag = "/home/xjm/testBag/algorithm2/2023-06-15-14-32-38.bag";
    std::cout <<"algorithm1 plus bag: " << algorithm2Bag.toStdString() << std::endl;
    QFile fi(algorithm2Bag);
    if (fi.exists())
        std::cout << "algorithm1 plus starts" << std::endl;
    else
        std::cout << "slam bag not found" << std::endl;


    algorithm2_size = frame_size;
    std::cout << "algorithm1 plus frame: " << std::to_string(algorithm2_size) << std::endl;
    startalgorithm2Run();
    }
}
void processBag::_getalgorithm2Frames() {
    QString str = QString(algorithm2Frame->readAllStandardOutput());
    int end = str.indexOf("msgs    : sensor_msgs/PointCloud2");
    int hesaiIndex = str.indexOf("/algorithm1/lc/mapping");
    if (end > 0) {
        ////====================================================================================
        QString get_total_frame = "";
        for (int i = hesaiIndex; i < end; i++) {
            get_total_frame += str.at(i);
        }
        QStringList l = get_total_frame.split(" ");
        algorithm2_size = l.at(l.size() - 2).toInt(); // get frame number
        QEventLoop loop1;
        QTimer::singleShot(1500, &loop1, SLOT(quit()));
        loop1.exec();
        ////====================================================================================
        std::cout << "algorithm1 plus frame: " << std::to_string(algorithm2_size) << std::endl;
        parent->setProgressBar(algorithm2Remaining+1);
        parent->setLog(tr("algorithm1 plus in process.."));
        startalgorithm2Run();
    }
}
void processBag::startalgorithm2Run() {
    if (process_running) {
    QString algorithm2_path;
    QString algorithm2bag = algorithm2Bag;
    QString name;
    bool cuda = false;
    name = parent->getSlamParam().at(14);

    if (parent->getSlamParam().at(20) == "true"){
        cuda = true;
        std::cout << "cuda is checked " << std::endl;
    }
    else {
        cuda = false;
        std::cout << "cuda is not checked " << std::endl;
    }

    QString Fname = name; 
    if(cuda){       
        //algorithm2_path = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib' ../Lib/company/algorithm1plusCuda %1 %2").arg(algorithm2bag).arg(Fname);
        algorithm2_path = QString("source ../ros/noetic/setup.bash && ../Lib/company/algorithm1plusCuda %1 %2 %3 %4\n").arg(algorithm2bag).arg(parent->getSlamParam().at(24)).arg(Fname).arg(parent->getSlamParam().at(23));
        std::cout << "using cuda boost" << std::endl;
    }
    else{     
        //algorithm2_path = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib' ../Lib/company/algorithm1plus %1 %2").arg(algorithm2bag).arg(Fname);
        algorithm2_path = QString("source ../ros/noetic/setup.bash && ../Lib/company/algorithm1plus %1 %2\n").arg(algorithm2bag).arg(Fname); 
        std::cout << "w/o cuda boost" << std::endl;
    }

    algorithm2 = new QProcess;
    // algorithm2->start("sh");
    // algorithm2->write(algorithm2_path.toLocal8Bit()+ '\n');
    // connect(algorithm2, SIGNAL(readyReadStandardOutput()), this, SLOT(_startalgorithm2Output()));

    algorithm2->start("bash", QStringList() << "-c" << algorithm2_path);
    connect(algorithm2, SIGNAL(readyReadStandardOutput()), this, SLOT(_startalgorithm2Output()));
    connect(algorithm2, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotalgorithm2ExitCode(int, QProcess::ExitStatus)));

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "algorithm1 plus starting" << std::endl;
    std::cout << "============================" << std::endl;
    }
}
void processBag::slotalgorithm2ExitCode(int a, QProcess::ExitStatus e) {

    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "algorithm1 plus successfully exits" << std::endl;

        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();
        algorithm2Done();
    }
    else {
        std::cout << "algorithm(algorithm1 plus) in error." << std::endl;
        errInfo = "algorithm1Plus";
        whenCrashed();
    }
}
void processBag::_startalgorithm2Output() {
    QString str = QString(algorithm2->readAllStandardOutput());

    int check = str.lastIndexOf("Current frame is: ");

    if (check > -1) {
        
        QString text = str;
        int frame = 0;
        //QString text = "[ Lidar ]:  [0.049316 ~ 0.100096] avg: 0.050039 ( 1264)  ---- /hesai/pandar";
        //QRegularExpression regex("\\((.*)\\)  ---- /hesai/pandar"); 
        QRegularExpression regex("Current frame is: (.*)."); 
        QRegularExpressionMatchIterator i = regex.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            frame = match.captured(1).toInt();
        }

        int oo = frame;
        int a = (((float)(oo * 100) / algorithm2_size) / 100) * (float)(100 - algorithm2Remaining+1 );
        //std::cout << a << std::endl;
        if((algorithm2Remaining+1 + a)<=98)
            parent->setProgressBar(algorithm2Remaining+1 +a);

        std::cout << "algorithm1 plus is running.." << std::endl;
    }
}
void processBag::algorithm2Done() {
    std::cout << "algorithm1 plus done" << std::endl;
    // if (parent->getType() == process_type::GCP ){

    // }else{
    //     parent->setLog(tr("algorithm1 plus done."));
    // }
    
    //.arg(QString::fromStdString(parent->getWindowTimer())));
    std::vector<std::string> tempList = parent->getWindowTimer();
    total_time += std::stod(tempList.at(0));
    parent->setLog(tr("algorithm1 plus done in %1").arg(QString::fromStdString(tempList.at(1))));
    allDone();
}

void processBag::allDone() {

    shutAll(0);
    total_time = 0.00;
}

void processBag::closeAll()
{
    // processCl = new QProcess;

    // QString D3_closeAll_str3 = "rosnode kill record_data";
    // processCl->start(D3_closeAll_str3+ '\n');
    // QString D3_closeAll_str1 = "rosnode kill /ekf_gps";
    // processCl->start(D3_closeAll_str1+ '\n');
    // QString D3_closeAll_str2 = "rosnode kill bagplay";
    // processCl->start(D3_closeAll_str2+ '\n');
}
void processBag::start2ndRtk() {
    if (process_running) {
        rtkRemaining = parent->progressBar->value();

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "rtk 2nd stage starting" << std::endl;
        std::cout << "============================" << std::endl;

        rtk_2nd = new QProcess;

        QString path1 = "";
        QString  _algorithm1BagStr = "";
        QString lidarBag = bagPath;

        path1 = lidarBag;

        QString path = parent->getSlamParam().at(15) + "/";
        QString Fpath = path;

        QString name = parent->getSlamParam().at(14);
        QString Fname = name;

        QString rtkFile = parent->getRtkFile();
        if (rtkFile.isEmpty())rtkFile = "void";
        QString rtkPath = rtkFile;

        _algorithm1BagStr = QString("source ../ros/noetic/setup.bash && ../ros/noetic/bin/algorithm1_nrt -bag=%1 -algorithm1=../Lib/company/params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 -log_path=../Log/\n").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname).arg(rtkPath);
            
        std::cout << _algorithm1BagStr.toStdString() << std::endl;

        //rtk_2nd->write(QString(ros_setup + "\r\n").toUtf8());
        //rtk_2nd->write(_algorithm1BagStr.toLocal8Bit() + '\n');
        //connect(rtk_2nd, SIGNAL(readyReadStandardOutput()), this, SLOT(_startRtk2nd_output()));

        rtk_2nd->start("bash", QStringList() << "-c" << _algorithm1BagStr);
        connect(rtk_2nd, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotRtkExitCode(int, QProcess::ExitStatus)));
        connect(rtk_2nd, SIGNAL(readyReadStandardError()), this, SLOT(_startRtk2nd_output()));       
        //connect(rtk_2nd, SIGNAL(readyReadStandardOutput()), this, SLOT(_startRtk2nd_output()));
    }
}
void processBag::slotRtkExitCode(int a, QProcess::ExitStatus e) {
    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "algorithm1(rtk) successfully exits" << std::endl;

        std::vector<std::string> tempList = parent->getWindowTimer();
        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("Rtk's final stage done in %1").arg(QString::fromStdString(tempList.at(1))));
        parent->startWindowTimer();
        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();
        startalgorithm2();

    }
    else {
        std::cout << "algorithm(algorithm1-rtk) in error." << std::endl;
        errInfo = "rtkFinal";
        whenCrashed();
    }
}
void processBag::_startRtk2nd_output() {
    //QString str = QString(rtk_2nd->readAllStandardOutput());
    QString err = QString(rtk_2nd->readAllStandardError());
    // ================================================ algorithm1 WITH GLOG ================================================
    // for progress bar
    int check = err.lastIndexOf("Progress:");
    if (check > -1) {
        //[I20240105 09:45:13.509135 23752 Ma: 129] Progress: 0.7143% ( 1 / 140 )  Failure rate: 0% (0 / 140 )
        QRegularExpression regex("Progress: (([0-9]*[.])?[0-9]+)%"); 
        QRegularExpressionMatchIterator i = regex.globalMatch(err);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            double current_percentile = match.captured(1).toDouble();
            int current_prog = static_cast<int>(current_percentile);

            parent->setProgressBar(rtkRemaining + (current_prog * 0.5)); // total = ~10 + 50 %
        }
        std::cout << err.toStdString() << std::endl;
    }

}

void processBag::_startGcpMerge() {
    if (process_running) {

    gcpRemaining = parent->progressBar->value();
    QString bagPathGcp;

    if (!gcpCp2ndRound) { 
        // merge the _SLAM.bag
        std::vector<std::string> tempList = parent->getWindowTimer();
        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("slam done in %1").arg(QString::fromStdString(tempList.at(1))));
        parent->startWindowTimer();

        parent->setLog(tr("merging the output.."));
        //gcpMergeBag = parent->getSlamParam().at(16) + parent->getSlamParam().at(15) + QString("_SLAM.bag");
        bagPathGcp = gcpPlyFolder + "/" + parent->getSlamParam().at(14) + QString("_SLAM.bag");
        QFile fi(bagPathGcp);
        if (fi.exists())
            std::cout << "gcp: merging" << std::endl;
        else{
            std::cout << "gcp: bag not found" << std::endl;
        }
    }else{ // 2nd time gcpCp
        parent->setLog(tr("Merging the output.. 2nd"));
        bagPathGcp = gcpPlyFolder2nd + "/" + parent->getSlamParam().at(14) + QString("_SLAM.bag");

        QFile fi(bagPathGcp);
        if (fi.exists()) {
            std::cout << "gcp: merging 2nd" << std::endl;
        }
        else {
            std::cout << "gcp: bag not found" << std::endl;
        }    
    }


    QString bagg = bagPathGcp;

    processM = new QProcess;
    
    //QString merge_str = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib' ../Lib/company/merge %1").arg(bagg);
    QString merge_str = QString("source ../ros/noetic/setup.bash && ../Lib/company/merge %1\n").arg(bagg);
    std::cout << merge_str.toStdString() << std::endl;

    processM->start("bash", QStringList() << "-c" << merge_str);
    connect(processM, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotMergeExitCode(int, QProcess::ExitStatus)));
    connect(processM, SIGNAL(readyReadStandardOutput()), this, SLOT(gcp_merge_output()));

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "merge running (remember to check if this is running in the task manager)" << std::endl;
    std::cout << "============================" << std::endl;
    
    }
    
}
void processBag::slotMergeExitCode(int a, QProcess::ExitStatus e) {
    std::cout << "int "<< a << std::endl;
    if (e == QProcess::ExitStatus::NormalExit){
        std::cout << "merge 1" << std::endl;
    }else{
        std::cout << "merge 0" << std::endl;
    }

    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "merge successfully exits" << std::endl;

        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();
        //return;
        startGcpCP();

    }
    else {
        std::cout << "algorithm(merge) in error." << std::endl;
        errInfo = "merge";
        whenCrashed();
    }

}
void processBag::gcp_merge_output() {


    QString str = QString(processM->readAllStandardOutput());
    int index = str.lastIndexOf("Done!");

    if (index < 0) {

        if (str.lastIndexOf("\r\n") > 0) {
            parent->setProgressBar(gcpRemaining + 3);
        }
    }
    else {
        parent->setLog(tr("Merging done"));
        return;
    }
    std::cout << str.toStdString()<< std::endl;
}
void processBag::_startGcp() {
    //std::cout << "gcp starts" << std::endl;

    //// 1st:
    //QString savingPath = parent->getSlamParam().at(16);
    //QString pathCloud = parent->getBag() + QString("_algorithm1_GtsamPath.ply");
    ////QString savingPath = "E:\\AtestGcp";
    ////QString pathCloud = "E:\\AtestGcp\\2023-07-06-12-13-10.bag_algorithm1_GtsamPath.ply";
    //
    ////std::cout << "cloud: " << cloudPath.toStdString() << std::endl;
    //std::cout << "saving path: " << savingPath.toStdString() << std::endl;
    //std::cout << "path cloud: " << pathCloud.toStdString() << std::endl;
    ////gcpPlyFolder = "E:\\AtestGcp\\2023_07_07_11_35_39";
    //// get all ply files
    //QStringList nameFilter("*.ply");
    //QDir directory(gcpPlyFolder);
    //QStringList allPlyFiles = directory.entryList(nameFilter);
    //QStringList finalPlyFiles;
    //for (QString a : allPlyFiles) {
    //    finalPlyFiles.append(gcpPlyFolder + "/" + a);
    //}

    //std::vector <std::vector<Yuan>> all_output;
    //// get all gcp() output(s)
    //// find all control points
    //int countGcpIndex = 0;
    //for (QString a : finalPlyFiles) {
    //    std::vector<Yuan> output = parent->runGcp(a, pathCloud, parent->getNoOfGcp(), savingPath, countGcpIndex);
    //    //std::vector<Yuan> output = parent->runGcp(a, pathCloud, 10, savingPath,countGcpIndex);
    //    all_output.emplace_back(output);
    //    countGcpIndex += output.size();
    //}
    //// final vector Yuan
    //std::vector<Yuan> finalOutput;
    //for (std::vector<Yuan> v : all_output) {
    //    for (Yuan y : v) {
    //        finalOutput.emplace_back(y);
    //    }
    //}
    //parent->setLog(tr("Getting GCP(s) done, in %1").arg(QString::fromStdString(parent->getWindowTimer())));
    //
    //// check/clear csv file first
    //QString csvFile = parent->getSlamParam().at(16) + "/" + parent->getSlamParam().at(15) + ".csv";
    //csvFile = csvFile.replace('/', '\\');
    //std::cout << "csv: " << csvFile.toStdString() << std::endl;
    ////QString csvFile =  "hi.csv";
    //QFile fi3(csvFile);
    //if (fi3.exists()) {
    //    fi3.remove();
    //    std::cout << "deleted" << std::endl;
    //}
    //// result to csv
    //// use parent->gcpTxtFile to edit csv file  
    //parent->saveCsv(csvFile, finalOutput, parent->gcpTxtFile);
    ////parent->saveCsv(csvFile, finalOutput, "E:\\AtestGcp\\gcptest.txt");
    //// 3rd - 4th
    //// can manual check - edit csv file, etc    
    //parent->setViewGcp(parent->getBag(), pathCloud, csvFile);
    //parent->labelRunning->setText("waiting for procedure..");
    //parent->setProgressBar(gcpRemaining + 10);
    //parent->setLog(tr("You may check the .csv file in the chosen saving path, and press 'continue' in 'GCP'."));

}
void processBag::startGcpCP() {
    if (process_running) {
        std::cout << "gcp starts" << std::endl;
        gcpRemaining = parent->progressBar->value();
        std::vector<std::string> tempList = parent->getWindowTimer();
        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("Merge done in %1").arg(QString::fromStdString(tempList.at(1))));
        parent->startWindowTimer();
        parent->setLog(tr("Getting all control points.."));


        QString savingPath;
        QString pathCloud;
        QString txtPc;
        if (!gcpCp2ndRound){
            // 1st get pathPly from slam
            //savingPath = parent->getSlamParam().at(15)+"/";
            savingPath = gcpPlyFolder;
            pathCloud = parent->getBag() + QString("_algorithm1_companyPath.ply");
            //QString savingPath = "E:\\AtestGcp";
            //QString pathCloud = "E:\\AtestGcp\\2023-07-06-12-13-10.bag_algorithm1_GtsamPath.ply";
            std::cout << "saving path: " << savingPath.toStdString() << std::endl;
            std::cout << "path cloud: " << pathCloud.toStdString() << std::endl;

            // save first realxyz
            parent->saveCsv(parent->gcpTxtFile);

            // 2nd store n ply files into vector
            QStringList nameFilter("*.ply");
            QDir directory(gcpPlyFolder);
            QStringList allPlyFiles = directory.entryList(nameFilter);
            //QStringList finalPlyFiles; // pass into exe's argv
            std::vector<std::string> finalPlyFiles; // pass into exe's argv
            std::cout << "adding ply file(s)" << std::endl;
            for (QString a : allPlyFiles) {
                QString pcStr = QString(gcpPlyFolder + "/" + a);
                //pcStr = pcStr;
                std::string temp = pcStr.toStdString();
                finalPlyFiles.emplace_back(temp);
                std::cout << temp << std::endl;
            }
            // change first argv = ply file(s) into txt
            txtPc = QString(gcpPlyFolder + "/clouds.txt");
            //txtPc = txtPc.replace('/', '\\');
            std::ofstream output_file(txtPc.toStdString());
            std::ostream_iterator<std::string> output_iterator(output_file, "\n");
            std::copy(std::begin(finalPlyFiles), std::end(finalPlyFiles), output_iterator);

        }else{
            savingPath = gcpPlyFolder2nd;
            pathCloud = parent->getBag() + QString("_algorithm1_companyPath.ply");
            std::cout << "saving path: " << savingPath.toStdString() << std::endl;
            std::cout << "path cloud: " << pathCloud.toStdString() << std::endl;

            // 2nd store n ply files into vector
            QStringList nameFilter("*.ply");
            QDir directory(gcpPlyFolder2nd);
            QStringList allPlyFiles = directory.entryList(nameFilter);
            //QStringList finalPlyFiles; // pass into exe's argv
            std::vector<std::string> finalPlyFiles; // pass into exe's argv
            std::cout << "adding ply file(s)" << std::endl;
            for (QString a : allPlyFiles) {
                QString pcStr = QString(gcpPlyFolder2nd + "/" + a);
                //pcStr = pcStr;
                std::string temp = pcStr.toStdString();
                finalPlyFiles.emplace_back(temp);
                std::cout << temp << std::endl;
            }
            // change first argv = ply file(s) into txt
            txtPc = QString(gcpPlyFolder2nd + "/clouds.txt");
            //txtPc = txtPc.replace('/', '\\');
            std::ofstream output_file(txtPc.toStdString());
            std::ostream_iterator<std::string> output_iterator(output_file, "\n");
            std::copy(std::begin(finalPlyFiles), std::end(finalPlyFiles), output_iterator);

        }
        
        // 3rd put argvs into exe
        //bool manualCheck = parent->getManualCheck();
        float radMax = parent->getRadMax();
        float radMin = parent->getRadMin();
        //this->no_of_gcp = parent->getNoOfGcp();
        int intensityMax = parent->getIntensityMax();
        int intensityMin = parent->getIntensityMin();
        int iteration = parent->getIteration();
        int circlePts = parent->getCirclePts();
        bool filterDist = parent->getFilterDist();
        int filterDistance = parent->getFilterDistance();
        bool filterZCoor = parent->getFilterZCoor();
        float filterZCoorMax = parent->getFilterZCoorMax();
        float filterZCoorMin = parent->getFilterZCoorMin();
        // change bool to int
        int filterSwitchkey = 0;
        if (filterDist)
            filterSwitchkey = 1;
        int filterZ = 0;
        if (filterZCoor)
            filterZ = 1;

        // txtPc = QString("/home/xjm/testBag/gcp/2023/clouds.txt");
        // pathCloud = QString("/home/xjm/testBag/gcp/2023-07-06-12-13-10-cut.bag_algorithm1_GtsamPath.ply");
        if (gcpCp2ndRound) {
            filterSwitchkey = 0;
            pathCloud = "N";
            std::cout << "path cloud: " << pathCloud.toStdString() << std::endl;
        }
        // 4th run exe to get csv
        QString gcpStr = QString("../Lib/company/gcpCp %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15").arg(txtPc).arg(QString(savingPath+"/")).arg(pathCloud).arg(parent->gcpTxtFile)
            .arg(circlePts).arg(radMin).arg(radMax).arg(iteration).arg(filterSwitchkey).arg(filterDistance)
            .arg(intensityMin).arg(intensityMax).arg(filterZ).arg(filterZCoorMin).arg(filterZCoorMax);
        //gcpStr = gcpStr.replace('/', '\\');
        std::cout << gcpStr.toStdString() << std::endl;

        gcp_cp = new QProcess;
        gcp_cp->start(gcpStr+"\n");
        connect(gcp_cp, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotGcpcpExitCode(int, QProcess::ExitStatus)));
        connect(gcp_cp, SIGNAL(readyReadStandardOutput()), this, SLOT(_gcpFindPoints()));

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "gcp points finding begins" << std::endl;
        std::cout << "============================" << std::endl;
        // 5th use csv to generate txt -> pass to gcpEE
    
    }
}
void processBag::slotGcpcpExitCode(int a, QProcess::ExitStatus e) {
    //if (e == QProcess::ExitStatus::NormalExit)std::cout << "byebye" << std::endl;
    //if (e == QProcess::ExitStatus::CrashExit)std::cout << "gg" << std::endl;
    //std::cout << "gcpcp exit no.: " << std::to_string(a) << std::endl;
    if (e == QProcess::ExitStatus::NormalExit ) { // && a == 0 ; set to 1 for now. because exit = 1 in windows ?? 0/1??
        std::cout << "gcpCp successfully exits" << std::endl;

        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();

        gcpFindPointsFinished();
    }
    else {
        std::cout << "algorithm(gcpCp) in error." << std::endl;
        errInfo = "gcpCp";
        whenCrashed();
    }

}
void processBag::_gcpFindPoints() {
    
    QString str = QString(gcp_cp->readAllStandardOutput());
    int check = str.lastIndexOf("Progress: ");
    //std::cout << str.toStdString() << std::endl;
    //std::cout << "===check: "<<check << std::endl;
    if (check != -1) {
        std::cout << "------*-----" << std::endl;
        std::cout << str.toStdString() << std::endl;
        std::cout << "------*-----" << std::endl;
        int indexDuration = str.indexOf(check) + 10;
        QString progress = str.mid(indexDuration);
        progress = progress.simplified().section(' ', 0, 0).simplified();
        int oo = progress.toInt();
        int a = (((float)oo / 100) * (float)((100-gcpRemaining)));
        a = a + gcpRemaining;
        //std::cout << "--------------progress---------: " << str.mid(str.indexOf(check)).toStdString() << std::endl;
        //std::cout << "--------------progress + 10---------: " << progress.toStdString() << std::endl;
        //std::cout << "--------------progress: "<<oo << std::endl;
        if (!gcpCp2ndRound) {
            if (a <= 40)
                parent->setProgressBar(a);
        }
        else {
            if (a <= 80)
                parent->setProgressBar(a);
        }
        if (oo == 100) {
            return;
        }
    }

}
void processBag::gcpFindPointsFinished() {

    std::vector<std::string> tempList = parent->getWindowTimer();
    total_time += std::stod(tempList.at(0));
    parent->setLog(tr("Getting GCP(s) done, in %1").arg(QString::fromStdString(tempList.at(1))));

    if (!gcpCp2ndRound) {
        // can manual check - edit csv file, etc    
        QString pathCloud = parent->getBag() + QString("_algorithm1_companyPath.ply");
        // check/clear csv file first
        //QString csvFile = parent->getSlamParam().at(16) + "/" + parent->getSlamParam().at(15) + ".csv";
        std::string csvFileName = parent->gcpTxtFile.toStdString().replace(parent->gcpTxtFile.toStdString().length() -3, 3, "csv");
        //QString csvFile = parent->gcpTxtFile + ".csv";
        QString csvFile = QString::fromStdString(csvFileName);
        //csvFile = csvFile.replace('/', '\\');
        std::cout << "csv: " << csvFile.toStdString() << std::endl;
        ////QString csvFile =  "hi.csv";
        QFile fi3(csvFile);
        if (fi3.exists()) {
            //fi3.remove();
            std::cout << "output csv found" << std::endl;
        }
        else {
            std::cout << "output csv not found" << std::endl;
        }

        parent->setViewGcp(parent->getBag(), pathCloud, csvFile);
        parent->labelRunning->setText("waiting for procedure..");
        parent->setLog(tr("You may check the .csv file in the chosen saving path, and press 'continue' in 'GCP'."));
        gcpRemaining = parent->progressBar->value();

    }else{
        // after 2 rounds of gcpCp, start algorithm2
        parent->startWindowTimer();
        // change 'algorithm2Bag' path OR copy paste out from gcpPlyFolder2nd to bag's saving path
        //matrix vs coor
        std::string csvFileName = parent->gcpTxtFile.toStdString().replace(parent->gcpTxtFile.toStdString().length() - 3, 3, "csv");
        QString csvFile = QString::fromStdString(csvFileName);
        std::cout << "2nd csv: " << csvFile.toStdString() << std::endl;

        QString transTxtPath = gcpPlyFolder2nd + "/" + parent->getSlamParam().at(14) + QString("_SLAMtrans.txt");
        std::cout << "trans: " << transTxtPath.toStdString() << std::endl;
        parent->_prepareGcpReportInputs(transTxtPath, csvFile);
        QApplication::processEvents(); // precaution
        // ========================================================================



        startalgorithm2();

    }

}
void processBag::_gcpEE(QString cloud, QString cloudpath, QString csv) {
    parent->startWindowTimer(); // for timing
    parent->labelRunning->setText("running..");
    std::cout << "gcp ee" << std::endl;
    parent->finalGcpEEcsv = csv;
    //parent->finalGcpEEcsv = "E:\\1a\\testGcp.csv";
    std::cout << "final csv: "<< parent->finalGcpEEcsv.toStdString() << std::endl;
    //runGcpEE();
    //return;   

    // 4th:
    // eliminate error
    parent->setLog(tr("Getting configuration files.."));




    setYamlGcpEE();
    // // rosparam
    // gcpEE_yaml = new QProcess;
    
    // QString _startParams = "";
    // _startParams = tr("rosparam load ../Lib/company/params_gcpEE.yaml && echo CFYcfy");

    // gcpEE_yaml->start(_startParams+"\n");
    // connect(gcpEE_yaml, SIGNAL(readyReadStandardOutput()), this, SLOT(_checkYamlGcpEE()));

    // std::cout << "\n" << std::endl;
    // std::cout << "============================" << std::endl;
    // std::cout << "gcp ee rosparam starting" << std::endl;
    // std::cout << "============================" << std::endl;
}
void processBag::_checkYamlGcpEE() {
    QString str = QString(gcpEE_yaml->readAllStandardOutput());
    int index = str.lastIndexOf("cfy"); // will echo twice
    std::cout << str.toStdString() << std::endl;
    if (index > 1) {
        gcpParamIndex++;
        if (gcpParamIndex > 1) {
            setYamlGcpEE();
            gcpParamIndex = 0;
        }   
    }
}
void processBag::setYamlGcpEE() {
    //std::cout << "hi" << std::endl;
    // set param 
    // QString _indiParams;
    // QProcess* _yamlParam = new QProcess();
    // QEventLoop loop;


    YAML::Node config = YAML::LoadFile("../Lib/company/params_comb.yaml");

    // method to gcp
    config["algorithm1"]["methodToUse"] = 2; // gcp

    // no of cores
    config["algorithm1"]["numberOfCores"] = parent->getSlamParam().at(0).toInt();
    // las
    //if (parent->getSlamParam().at(16) == "true")  // gcp no need las // parent->getType() == process_type::GCP
        //config["algorithm1"]["saveMapToLAS"] = false;
    //else
        config["algorithm1"]["saveMapToLAS"] = true;
    // csv
    //if (parent->getSlamParam().at(2) == "true")
    //    config["algorithm1"]["savePoseToCSV"] = true;
    //else
    //    config["algorithm1"]["savePoseToCSV"] = false;
    // rotating frames
    if (parent->getSlamParam().at(3) == "true")
        config["algorithm1"]["onlyRotationFramesUsedForMapping"] = true;
    else
        config["algorithm1"]["onlyRotationFramesUsedForMapping"] = false;
    // loop closure
    if (parent->getType() == process_type::GCP || parent->getType() == process_type::RTK) { // gcp no need loop closure
        config["algorithm1"]["useLoopClosure"] = false;
    }
    else {
        if (parent->getSlamParam().at(4) == "true") {
            config["algorithm1"]["useLoopClosure"] = true;
            config["algorithm1"]["loopClosureIntervalFrames"] = parent->getSlamParam().at(5).toInt(); // loop closure interval frames
        }
        else
            config["algorithm1"]["useLoopClosure"] = false;
    }

    // narrow space
    if (parent->getSlamParam().at(6) == "true")
        config["algorithm1"]["isNarrowSpace"] = true;
    else
        config["algorithm1"]["isNarrowSpace"] = false;
    // min intensity filter
    config["algorithm1"]["minIntensity"] = parent->getSlamParam().at(7).toFloat();
    // lidar max range
    config["algorithm1"]["lidarMaxRange"] = parent->getSlamParam().at(8).toFloat();
    // lidar min range
    config["algorithm1"]["lidarMinRange"] = parent->getSlamParam().at(9).toFloat();
    // lidar max x
    config["algorithm1"]["lidarMaxXToFilter"] = parent->getSlamParam().at(10).toFloat();
    // lidar min x
    config["algorithm1"]["lidarMinXToFilter"] = parent->getSlamParam().at(11).toFloat();
    // translation threshold
    config["algorithm1"]["translationThreshold"] = parent->getSlamParam().at(12).toFloat();
    // icp iteration
    config["algorithm1"]["icpMaximumIterations"] = parent->getSlamParam().at(13).toInt();
    // loss sigma
    config["algorithm1"]["lossSigma"] = parent->getSlamParam().at(17).toFloat();
    // to laz
    if (parent->getSlamParam().at(18) == "true")
        config["algorithm1"]["compressLas"] = true;
    else
        config["algorithm1"]["compressLas"] = false;
    // map resolution
    config["algorithm1"]["mapResolution"] = parent->getSlamParam().at(19).toFloat();
    // algorithm2: save bag
    //if (parent->getSlamParam().at(17) == "true" || parent->getType() == process_type::GCP) // gcp need this bag for merge: 
    config["algorithm1"]["saveSLAMToBag"] = true;
    //else
        //config["algorithm1"]["saveSLAMToBag"] = false; 
    // voxel size
    config["algorithm1"]["voxelSize"] = parent->getSlamParam().at(21).toFloat();

    //====================================================================================

    if (parent->getType() == process_type::GCP) {
        //// method to gcp
        //config["algorithm1"]["methodToUse"] = 2;
        // useGTSAMOptimizer
        config["algorithm1"]["usecompanyOptimizer"] = true;
        // saveGTSAMPath
        config["algorithm1"]["savecompanyPath"] = true;

        config["algorithm1"]["manualGCP"] = false;

    }


    QString output = "../Lib/company/params.yaml";
    QFile file(output); // remove if exists
    file.remove();
    std::ofstream fout("../Lib/company/params.yaml");
    fout << config << std::endl; // dump it back into the file
    fout.close();

    std::cout << "done gcp params" << std::endl;   

    runGcpEE();
}
void processBag::runGcpEE() {
    if (process_running) {
        // run exe
        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "running gcp: eliminate error" << std::endl;
        std::cout << "============================" << std::endl;

        parent->setLog(tr("eliminating error.."));

        gcpEE_run = new QProcess;
        QString _algorithm1BagStr;
        //gcpEE_run->start("sh");

        // new folder for ply file(s)
        //gcpPlyFolder2nd = QString("controlPts2nd");
        //gcpPlyFolder2nd = parent->getSlamParam().at(15) + "\\" + gcpPlyFolder2nd;
        gcpPlyFolder2nd = parent->getSlamParam().at(15) + "/" ;
        QDir dir(gcpPlyFolder2nd);
        dir.mkdir(QString("controlPts2nd_finalResults"));
        std::cout << "creating a 2nd new folder for ply file(s)" << std::endl;

        gcpPlyFolder2nd += QString("controlPts2nd_finalResults");
        // ================================================================================================================


        
        // output csv to txt  =====================
        //QString csvFile = parent->getSlamParam().at(16) + "/" + parent->getSlamParam().at(15) + ".csv";
        QString csvFile = parent->finalGcpEEcsv;

        //QString csvFile = "E:\\1a\\mel0719\\230719GPS_SCAN.csv";
        //QString gcpEEtxt = "E:\\1a\\mel0719\\2023-07-19-03-00-28Brian_scan.bag_GCPEE.txt";

        std::vector<std::vector<std::string>> tempVec = parent->loadCsv(csvFile);
        QString gcpEEtxt = parent->getSlamParam().at(15) + "/" + parent->getSlamParam().at(14) + "_GCPEE.txt";
        //QFile fi(gcpEEtxt);
        //if (fi.exists()) {
        //    //fi3.remove();
        //    std::cout << "output txt found" << std::endl;
        //}
        //else {
        //    std::cout << "output txt not found" << std::endl;
        //}

        std::ofstream dataFile;
        dataFile.open(gcpEEtxt.toStdString(), std::ofstream::app);
        std::fstream file(gcpEEtxt.toStdString(), std::ios::out);
        dataFile <<std::fixed << std::setprecision(6) << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << parent->rx_gcpee << "," << parent->ry_gcpee << "," << parent->rz_gcpee<< std::endl; // first line
        std::cout << "rx " << parent->rx_gcpee << std::endl;
        std::cout << "ry " << parent->ry_gcpee << std::endl;
        std::cout << "rz " << parent->rz_gcpee << std::endl;
        // i = 1 coz first line = title
        for (int i = 1; i < int(tempVec.size()); i++) {
            std::vector<std::string> temp = tempVec.at(i);
            dataFile << std::fixed << std::setprecision(6) << stod(temp.at(0)) << "," << stod(temp.at(1)) << "," << stod(temp.at(2)) << "," << stod(temp.at(3)) << "," << stod(temp.at(4)) << "," << stod(temp.at(5)) << "," << stod(temp.at(6)) << "," << stod(temp.at(7)) << "," << stod(temp.at(8)) << "," << stod(temp.at(9)) << std::endl;
        }
        // =========================================


        //////  TO BE DELETED
        //gcpEEtxt = "aa.txt";
        //QString _algorithm1BagStr = QString("..\\Lib\\company\\algorithm1_nrt_gcpEE.exe %1 %2 %3 %4 __name:=bagplay && echo CFYplaybagendcfy").arg("E:\\vsTest\\2023-07-08-12-05-16.bag").arg(gcpEEtxt).arg(parent->getSlamParam().at(16)).arg(parent->getSlamParam().at(15));
        //frame_size = 913;
        //gcpRemaining = 60;
        ////// ==============
        QString lidarBag = parent->getBag();
        QString pathBag = "";
        pathBag = lidarBag;

        QString lidarTxt = gcpEEtxt;
        QString pathTxt = "";
        pathTxt = lidarTxt;

        QString path = parent->getSlamParam().at(15) + "/";
        path = gcpPlyFolder2nd + "/";; // for 2nd gcpCp
        QString Fpath = path;

        QString name = parent->getSlamParam().at(14);
        QString Fname = name;

        //QString _algorithm1BagStr = QString("CMAKE_PREFIX_PATH='../ros/noetic' ROS_PACKAGE_PATH='../ros/noetic/share' LD_LIBRARY_PATH='../ros/noetic/lib' ../Lib/company/algorithm1_nrt_gcpEE %1 ./temp.yaml %2 %3 %4 && echo CFYplaybagendcfy").arg(pathBag).arg(pathTxt).arg(Fpath).arg(Fname);
        _algorithm1BagStr = QString("source ../ros/noetic/setup.bash && ../ros/noetic/bin/algorithm1_nrt -bag=%1 -algorithm1=../Lib/company/params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 -log_path=../Log/\n").arg(pathBag).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname).arg(pathTxt);
                
        std::cout << _algorithm1BagStr.toStdString() << std::endl;
        
        algorithm1Index = 0;

        gcpEE_run->start("bash", QStringList() << "-c" << _algorithm1BagStr);

        connect(gcpEE_run, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotGcpeeExitCode(int, QProcess::ExitStatus)));
        //connect(gcpEE_run, SIGNAL(readyReadStandardOutput()), this, SLOT(_startGcpee_output()));
        connect(gcpEE_run, SIGNAL(readyReadStandardError()), this, SLOT(_startGcpee_output()));
        
    }
}
void processBag::slotGcpeeExitCode(int a, QProcess::ExitStatus e) {

    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "gcpEe successfully exits" << std::endl;

        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();

        std::vector<std::string> tempList = parent->getWindowTimer();
        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("Eliminating error done, in %1").arg(QString::fromStdString(tempList.at(1))));
        //allDone();
        parent->startWindowTimer(); // for timing

        //for 2nd round of gcpCp - accuracy report
        gcpCp2ndRound = true;
        _startGcpMerge();
        return;
        //startalgorithm2();

    }
    else {
        std::cout << "algorithm(gcpEe) in error." << std::endl;
        errInfo = "gcpEE";
        whenCrashed();
    }
}
void processBag::_startGcpee_output() {
    //QString str = QString(gcpEE_run->readAllStandardOutput());
    QString err = QString(gcpEE_run->readAllStandardError());

    // for progress bar
    int check = err.lastIndexOf("Progress:");
    if (check > -1) {
        //[I20240105 09:45:13.509135 23752 Ma: 129] Progress: 0.7143% ( 1 / 140 )  Failure rate: 0% (0 / 140 )
        QRegularExpression regex("Progress: (([0-9]*[.])?[0-9]+)%"); 
        QRegularExpressionMatchIterator i = regex.globalMatch(err);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            double current_percentile = match.captured(1).toDouble();
            int current_prog = static_cast<int>(current_percentile);
            current_prog = ((float)current_prog * (float)((70-gcpRemaining)/100.0));
            current_prog += gcpRemaining;
            if(current_prog <= 70)
                parent->setProgressBar(current_prog); // total = ~10 + 40 %
        }

        std::cout << err.toStdString() << std::endl;
    }
  
}

void processBag::startBa() {
    if (process_running) {
        parent->setLog(tr("Starting algorithm1 2nd stage.."));
        baRemaining = parent->progressBar->value();
        baBag = parent->getSlamParam().at(15) + "/" + parent->getSlamParam().at(14) + QString("_SLAM.bag");


        //algorithm2Bag = "E:\\test.bag_SLAM.bag";
        std::cout << "algorithm1 2nd stage bag: " << baBag.toStdString() << std::endl;
        QFile fi(baBag);
        if (fi.exists()) {
            std::cout << "algorithm1 2nd stage starts" << std::endl;
        }
        else {
            std::cout << "slam bag not found" << std::endl;
        }

        //algorithm2_size = frame_size;
        //std::cout << "algorithm1 plus frame: " << std::to_string(algorithm2_size) << std::endl;
        //startalgorithm2Run();
        //baBag = "E:\\vsTest\\2023-12-25-02-43-47_SLAM.bag";

        QString path;
        QString bag = baBag;
        //QString name;
        //name = parent->getSlamParam().at(14);

        //algorithm2bag = "E:\\Atestalgorithm2\\test.bag_SLAM.bag"; // TO BE DELETED ************
        
        QString baSplit; // split normal plane
        if (parent->baSplit.contains("1"))baSplit = "true";
        else baSplit = "false";

        path = QString("source ../ros/noetic/setup.bash && ../ros/noetic/bin/algorithm1_ba -slam=%1 -iter=%2 -voxel_size=%3 -save_pose=true -split_normal=%4 -min_pt=20 -voxel_layer=3 -normal_threshold=0.95\n").arg(bag).arg(parent->baIter).arg(parent->baVoxel).arg(baSplit);             
        std::cout << path.toStdString() << std::endl;

        //algorithm2_path = QString("algorithm1plus.exe %1 %2").arg(algorithm2Bag).arg("parent");
        ba = new QProcess;
        /*algorithm2->start("cmd.exe");
        algorithm2->write(QString(ros_setup + "\r\n").toUtf8());
        algorithm2->write(QString(algorithm2_path + "\r\n").toUtf8());*/
        ba->start("bash", QStringList() << "-c" << path);

        connect(ba, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotbaExitCode(int, QProcess::ExitStatus)));
        connect(ba, SIGNAL(readyReadStandardOutput()), this, SLOT(_startbaOutput()));

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "algorithm1 2nd stage starting" << std::endl;
        std::cout << "============================" << std::endl;


    }
}
void processBag::slotbaExitCode(int a, QProcess::ExitStatus e) {

    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "algorithm1 2nd stage successfully exits" << std::endl;

        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();

        baDone();
    }
    else {
        std::cout << "algorithm(algorithm1 2nd stage) in error." << std::endl;
        errInfo = "algorithm12nd";
        whenCrashed();
    }
}
void processBag::_startbaOutput() {
    QString str = QString(ba->readAllStandardOutput());
    std::cout << "algorithm1 2nd stage is running.." << std::endl;

    //int check = str.lastIndexOf("Current frame is: ");
    //if (check > -1) {
    //    QString text = str;
    //    int frame = 0;
    //    QRegularExpression regex("Current frame is: (\\d+)."); // ƥ   
    //    QRegularExpressionMatchIterator i = regex.globalMatch(text);
    //    while (i.hasNext()) {
    //        QRegularExpressionMatch match = i.next();
    //        frame = match.captured(1).toInt();
    //    }

    //    int oo = frame;
    //    int a = (((float)(oo * 100) / algorithm2_size) / 100) * (float)(100 - algorithm2Remaining + 1);
    //    if ((algorithm2Remaining + 1 + a) <= 98)
    //        parent->setProgressBar(algorithm2Remaining + 1 + a);
    //}
}
void processBag::baDone() {
    std::cout << "algorithm1 2nd stage done" << std::endl;
    std::vector<std::string> tempList = parent->getWindowTimer();
    total_time += std::stod(tempList.at(0));
    parent->setLog(tr("algorithm1 2nd stage in %1").arg(QString::fromStdString(tempList.at(1))));
    parent->startWindowTimer();
    startalgorithm2();

}

void processBag::shutAll(int a) {
    process_running = false;

    // clear all process / .exe
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
    if (algorithm2 != NULL) {

        disconnect(algorithm2, 0, 0, 0);
        algorithm2->kill();
        algorithm2 = NULL;
        std::cout << "algorithm1 plus x " << std::endl;
    }
    if (algorithm2Frame != NULL) {

        disconnect(algorithm2Frame, 0, 0, 0);
        algorithm2Frame->kill();
        algorithm2Frame = NULL;
        std::cout << "algorithm1 plus frame x " << std::endl;
    }
    if (processM != NULL) {

        disconnect(processM, 0, 0, 0);
        processM->kill();
        processM = NULL;
        std::cout << "gcp merge x " << std::endl;
    }
    if (gcpEE_yaml != NULL) {

        disconnect(gcpEE_yaml, 0, 0, 0);
        gcpEE_yaml->kill();
        gcpEE_yaml = NULL;
        std::cout << "gcp ee yaml x " << std::endl;
    }
    if (gcpEE_run != NULL) {

        disconnect(gcpEE_run, 0, 0, 0);
        gcpEE_run->kill();
        gcpEE_run = NULL;
        std::cout << "gcp ee x " << std::endl;
    }
    if (gcp_cp != NULL) {

        disconnect(gcp_cp, 0, 0, 0);
        gcp_cp->kill();
        gcp_cp = NULL;
        std::cout << "gcp cp x " << std::endl;
    }
    if (rtk_2nd != NULL) {

        disconnect(rtk_2nd, 0, 0, 0);
        rtk_2nd->kill();
        rtk_2nd = NULL;
        std::cout << "rtk 2nd x " << std::endl;
    }
    if (ba != NULL) {

        disconnect(ba, 0, 0, 0);
        ba->kill();
        ba = NULL;
        std::cout << "map opti x " << std::endl;
    }


    // ========================================================

    if (a == 1) {      
        QProcess::startDetached("killall algorithm1plus");
        QProcess::startDetached("killall algorithm1plusCuda");
        QProcess::startDetached("killall gcpCp");
        QProcess::startDetached("killall colorization");
        QProcess::startDetached("killall merge");
        QProcess::startDetached("killall mp4_merge");
        QProcess::startDetached("killall algorithm1_nrt");
        QProcess::startDetached("killall algorithm1_ba");


        parent->labelRunning->setText(tr("canceling.."));
        QApplication::processEvents(); // show loading dialog
        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();

        parent->stopWindowTimer();
        parent->labelRunning->setText(tr("canceled"));
        parent->setLog(tr("canceled"));
        
    }
    else if (a == 0) {
        // std::vector<std::string> tempList = parent->getWindowTimer();
        // total_time += std::stod(tempList.at(0));

        // parent->setLog(tr("algorithm1 plus done in %1").arg(QString::fromStdString(tempList.at(1))));

        double time_used_in_s = total_time;
        char* ch = new char[sizeof(time_used_in_s)];
        std::string res = "";
        if (time_used_in_s < 60) { // in s
            sprintf(ch, "%.2f", time_used_in_s);
            res = ch;
            res += "s";
            // time_used_in_s = std::ceil(time_used_in_s * 100.0) / 100.0;
            // res = std::to_string(time_used_in_s) + "s";

        }
        else if (time_used_in_s > 60 && time_used_in_s < 60 * 60) { // in min
            time_used_in_s = time_used_in_s / 60;
            sprintf(ch, "%.2f", time_used_in_s);
            res = ch;
            res += "min";
            // time_used_in_s = std::ceil(time_used_in_s * 100.0) / 100.0;
            // res = std::to_string(time_used_in_s) + "min";
        }
        else { // in hr
            time_used_in_s = time_used_in_s / 60 / 60;
            sprintf(ch, "%.2f", time_used_in_s);
            res = ch;
            res += "hr";
            // time_used_in_s = std::ceil(time_used_in_s * 100.0) / 100.0;
            // res = std::to_string(time_used_in_s) + "hr";
        }


        parent->labelRunning->setText(tr("done in %1").arg(QString::fromStdString(res)));
        parent->setLog(tr("done in %1").arg(QString::fromStdString(res)));

        parent->setProgressBar(100);

    }

    parent->setStatus(running_status::finished);
    parent->windowCanRunBag();

    if (parent->getType() == process_type::GCP) {
        // show accuracy report
        parent->getViewProcess()->onAccuracyReport();
    }

    this->close();
}

void processBag::whenCrashed() {

    process_running = false;

    // clear all process / .exe
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
    if (algorithm2 != NULL) {

        disconnect(algorithm2, 0, 0, 0);
        algorithm2->kill();
        algorithm2 = NULL;
        std::cout << "algorithm1 plus x " << std::endl;
    }
    if (algorithm2Frame != NULL) {

        disconnect(algorithm2Frame, 0, 0, 0);
        algorithm2Frame->kill();
        algorithm2Frame = NULL;
        std::cout << "algorithm1 plus frame x " << std::endl;
    }
    if (processM != NULL) {

        disconnect(processM, 0, 0, 0);
        processM->kill();
        processM = NULL;
        std::cout << "gcp merge x " << std::endl;
    }
    if (gcpEE_yaml != NULL) {

        disconnect(gcpEE_yaml, 0, 0, 0);
        gcpEE_yaml->kill();
        gcpEE_yaml = NULL;
        std::cout << "gcp ee yaml x " << std::endl;
    }
    if (gcpEE_run != NULL) {

        disconnect(gcpEE_run, 0, 0, 0);
        gcpEE_run->kill();
        gcpEE_run = NULL;
        std::cout << "gcp ee x " << std::endl;
    }
    if (gcp_cp != NULL) {

        disconnect(gcp_cp, 0, 0, 0);
        gcp_cp->kill();
        gcp_cp = NULL;
        std::cout << "gcp cp x " << std::endl;
    }
    if (rtk_2nd != NULL) {

        disconnect(rtk_2nd, 0, 0, 0);
        rtk_2nd->kill();
        rtk_2nd = NULL;
        std::cout << "rtk 2nd x " << std::endl;
    }
    if (ba != NULL) {

        disconnect(ba, 0, 0, 0);
        ba->kill();
        ba = NULL;
        std::cout << "map opti x " << std::endl;
    }


    // ========================================================

    if (true) {
        QProcess::startDetached("killall algorithm1plus");
        QProcess::startDetached("killall algorithm1plusCuda");
        QProcess::startDetached("killall gcpCp");
        QProcess::startDetached("killall colorization");
        QProcess::startDetached("killall merge");
        QProcess::startDetached("killall mp4_merge");
        QProcess::startDetached("killall algorithm1_nrt");
        QProcess::startDetached("killall algorithm1_ba");


        parent->labelRunning->setText(tr("canceling.."));
        QApplication::processEvents(); // show loading dialog
        QEventLoop loop;
        QTimer::singleShot(1500, &loop, SLOT(quit()));
        loop.exec();

        parent->stopWindowTimer();
        parent->labelRunning->setText(tr("failed"));
        if (errInfo == "algorithm1") {
            parent->setLog(tr("algorithm1 in error."));
            parent->setLog(tr("Ram or other hardwares may be exceeded its maximum usage.\nPlease check the pc's specification and follows the requirements."));

        }else if (errInfo == "algorithm1Plus") {
            parent->setLog(tr("algorithm1 Plus in error."));
            parent->setLog(tr("Ram or other hardwares may be exceeded its maximum usage.\nPlease check the pc's specification and follows the requirements."));
            parent->setLog(tr("Ensure enough disk space is provided."));

        }else if (errInfo == "rtkFinal") {
            parent->setLog(tr("Rtk Final stage in error."));
            parent->setLog(tr("Ram or other hardwares may be exceeded its maximum usage.\nPlease check the pc's specification and follows the requirements."));
            parent->setLog(tr("Ensure enough disk space is provided."));
            parent->setLog(tr("Or the input files may be invalid."));
        }
        else if (errInfo == "merge") {
            parent->setLog(tr("Merge in error."));
            parent->setLog(tr("Or the input files may be invalid."));
        }
        else if (errInfo == "gcpCp") {
            parent->setLog(tr("gcpCp in error."));
            parent->setLog(tr("Or the input files may be invalid."));
        }
        else if (errInfo == "gcpEE") {
            parent->setLog(tr("gcpEE in error."));
            parent->setLog(tr("Ram or other hardwares may be exceeded its maximum usage.\nPlease check the pc's specification and follows the requirements."));
            parent->setLog(tr("Ensure enough disk space is provided."));
            parent->setLog(tr("Or the input files may be invalid."));
        }
        else if (errInfo == "algorithm12nd") {
            parent->setLog(tr("algorithm1 2nd stage in error."));
            parent->setLog(tr("Ram or other hardwares may be exceeded its maximum usage.\nPlease check the pc's specification and follows the requirements."));
            parent->setLog(tr("Ensure enough disk space is provided."));
        }
        else if (errInfo == "algorithm1Egm") {
            parent->setLog(tr("algorithm1 in error."));
            parent->setLog(tr("Ensure 'egm96-5.exe' is installed with default path."));
            parent->setLog(tr("Or the input files may be invalid."));
        }
        else if (errInfo == "algorithm1Space") {
            parent->setLog(tr("algorithm1 in error."));
            parent->setLog(tr("Ram or other hardwares may be exceeded its maximum usage.\nPlease check the pc's specification and follows the requirements."));
            parent->setLog(tr("Ensure enough disk space is provided."));
        }

        
        parent->setLog(tr("Failed."));

    }
    else if (false) {
    }

    parent->setStatus(running_status::finished);
    parent->windowCanRunBag();
    this->close();
}