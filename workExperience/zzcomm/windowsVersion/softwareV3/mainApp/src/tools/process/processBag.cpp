#include <iomanip>
#include "../../include/include_process/processBag.h"
#include "../../include/include_gui/widget_newProcess.h"
#include "../../include/include_gui/Yuan.h"
#include "../../include/include_gui/gcp.h"
#include <QTimer>
#include <iostream>
#include <QFileInfo>
#include <fstream>
#include <sstream>
#include <QDateTime>
#include <QDir>
#include "yaml-cpp/yaml.h"

processBag::processBag(widget_newProcess* parent)
    : QWidget(parent), type(process_type::Slam),
    paramIndex(0), algorithm1Index(0), frame_size(0), algorithm2Index(0), algorithm2_size(0), algorithm2Remaining(0), gcpRemaining(0), gcpParamIndex(0)
    , total_time(0.0), rtkRemaining(0), rtkIndex(0), process_running(false), baRemaining(0), gcpCp2ndRound(false), notEgm(false)

{
    init();
	this->parent = parent; 
}
processBag::~processBag() {
}
void processBag::init() {
    QFileInfo f("..\\setup.bat");
    ros_setup = f.absoluteFilePath().replace('/', '\\');
    ros_setup = parent->addQuote(ros_setup.toStdString());
    //ros_setup = "..\\setup.bat";
}

void processBag::startTheRun(QString bagPath) {
    //_startGcp();  
    //runGcpEE();
    //_gcpEE("QString cloud", "", "E:\\1a\\testGcp.csv");
    //parent->running = true;
    //QString check = "E:\\vsTest\\ConsoleApplication1\\ConsoleApplication1\\office_tr.ply";
    //parent->manualGcpShow(check);
    //parent->setStatus(running_status::finished);
    //parent->windowCanRunBag();
    //parent->getViewProcess()->onAccuracyReport();
    //return;  

#ifdef _WIN32
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED); // prevent pc from sleeping
#endif

    parent->labelRunning->setText("running..");
    //parent->running = true;
    parent->setStatus(running_status::running);
    parent->startWindowTimerP(); // for timing

    process_running = true;

    std::cout << bagPath.toStdString() << std::endl; // eg. E:/2023-04-26-03-33-48.bag
    this->bagPath = bagPath;
    //std::cout << ros_setup.toStdString() << std::endl;

    parent->setLog(tr("Starting.."));

    QString rosCore = "roscore";
    processCore = new QProcess;
    processCore->start("cmd.exe");
    processCore->write(QString(ros_setup + "\r\n").toUtf8());
    processCore->write(QString(rosCore.toLocal8Bit() + '\r' + '\n').toUtf8());
    //processCore->start(QString(ros_setup + " && " + rosCore + "\n"));
    std::cout << "roscore: " << ros_setup.toStdString() << std::endl;

    connect(processCore, SIGNAL(readyReadStandardOutput()), this, SLOT(_roscoreStarts()));

    //std::cout << "\n" << std::endl;
    //std::cout << "============================" << std::endl;
    //std::cout << "roscore starting" << std::endl;
    //std::cout << "============================" << std::endl;

    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "starting" << std::endl;
    std::cout << "============================" << std::endl;

    parent->windowAddToCmd(QString(tr("starting")));

    //if (parent->getType() == process_type::RTK) {
    //    setYaml_Rtk();
    //}
    //else { // slam type
    //    setYaml();
    //}
    //setYaml();


    //// for log
    //logFile.setFileName("..\\Lib\\company\\algorithm1_Log.txt");
    //logFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);


    //_roscore_output();
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

    }
}
void processBag::_roscore_output() {
    std::cout << "\n" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "roscore started" << std::endl;
    std::cout << "============================" << std::endl;
    parent->setProgressBar(1);
    parent->setLog(tr("core services started."));

    cmd_yaml = new QProcess;
    cmd_yaml->start("cmd.exe");
    QString _startParams = "";

    // ==========================================================================================================================================================================

    if (parent->getType() == process_type::RTK) {
        _startParams = "rosparam load ..\\Lib\\company\\params_rtk.yaml && echo CFYcfy";
    }
    else { // slam type
        //if (parent->getSlamParam().at(17) == "true") {
        //    _startParams = "rosparam load ..\\Lib\\company\\params_plus.yaml && echo CFYcfy";
        //}
        //else {
            _startParams = tr("rosparam load ..\\Lib\\company\\params_comb.yaml && echo CFYcfy");
        //}
    }

    cmd_yaml->write(QString(ros_setup + "\r\n").toUtf8());
    cmd_yaml->write(_startParams.toLocal8Bit() + '\r' + '\n');
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
                    
                //}
            }
            paramIndex = 0;
        }
    }
}
void processBag::setYaml() {

    //// set param 
    //QString _indiParams;
    //QProcess* _yamlParam = new QProcess();
    //_yamlParam->start("cmd.exe");
    //_yamlParam->write(QString(ros_setup + "\r\n").toUtf8());
    //QEventLoop loop; 

    ////====================================================================================
    ////sensor
    //_indiParams = QString("rosparam set /algorithm1/sensor %1").arg(parent->getSlamParam().at(0));
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// no of cores
    //_indiParams = QString("rosparam set /algorithm1/numberOfCores %1").arg(parent->getSlamParam().at(1));
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// las (TRUE FOR NOW)
    ////if (parent->getType() == process_type::GCP) { // gcp no need las
    //    _indiParams = QString("rosparam set /algorithm1/saveMapToLAS false");
    ////}
    ////else {
    ////    //if (parent->getSlamParam().at(2) == "true")
    ////    _indiParams = QString("rosparam set /algorithm1/saveMapToLAS true");
    ////    //else
    ////        //_indiParams = QString("rosparam set /algorithm1/saveMapToLAS false");
    ////}
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// csv
    //if (parent->getSlamParam().at(3) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/savePoseToCSV true");

    //else
    //    _indiParams = QString("rosparam set /algorithm1/savePoseToCSV false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// rotating frames
    //if (parent->getSlamParam().at(4) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/onlyRotationFramesUsedForMapping true");
    //else
    //    _indiParams = QString("rosparam set /algorithm1/onlyRotationFramesUsedForMapping false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// loop closure
    //if (parent->getType() == process_type::GCP) { // gcp no need loop closure
    //    _indiParams = QString("rosparam set /algorithm1/useLoopClosure false");
    //}
    //else {
    //    if (parent->getSlamParam().at(5) == "true")
    //        _indiParams = QString("rosparam set /algorithm1/useLoopClosure true");
    //    else
    //        _indiParams = QString("rosparam set /algorithm1/useLoopClosure false");
    //}
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// loop closure interval frames
    //if (parent->getSlamParam().at(5) == "true") {
    //    _indiParams = QString("rosparam set /algorithm1/loopClosureIntervalFrames %1").arg(parent->getSlamParam().at(6).toInt());
    //    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //    //QApplication::processEvents();
    //    QTimer::singleShot(500, &loop, SLOT(quit())); 
    //    loop.exec(); 
    //}

    //// narrow space
    //if (parent->getSlamParam().at(7) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/isNarrowSpace true");

    //else
    //    _indiParams = QString("rosparam set /algorithm1/isNarrowSpace false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// min intensity filter
    //_indiParams = QString("rosparam set /algorithm1/minIntensity %1").arg(parent->getSlamParam().at(8).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar max range
    //_indiParams = QString("rosparam set /algorithm1/lidarMaxRange %1").arg(parent->getSlamParam().at(9).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar min range
    //_indiParams = QString("rosparam set /algorithm1/lidarMinRange %1").arg(parent->getSlamParam().at(10).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //// QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar max x
    //_indiParams = QString("rosparam set /algorithm1/lidarMaxXToFilter %1").arg(parent->getSlamParam().at(11).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar min x
    //_indiParams = QString("rosparam set /algorithm1/lidarMinXToFilter %1").arg(parent->getSlamParam().at(12).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// translation threshold
    //_indiParams = QString("rosparam set /algorithm1/translationThreshold %1").arg(parent->getSlamParam().at(13).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //// QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// icp iteration
    //_indiParams = QString("rosparam set /algorithm1/icpMaximumIterations %1").arg(parent->getSlamParam().at(14).toInt());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// loss sigma
    //_indiParams = QString("rosparam set /algorithm1/lossSigma %1").arg(parent->getSlamParam().at(18).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// to laz
    //if (parent->getSlamParam().at(19) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/compressLas true");
    //else
    //    _indiParams = QString("rosparam set /algorithm1/compressLas false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// map resolution
    //_indiParams = QString("rosparam set /algorithm1/mapResolution %1").arg(parent->getSlamParam().at(20).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// algorithm2: save bag
    ////if (parent->getSlamParam().at(17) == "true" || parent->getType() == process_type::GCP) // gcp need this bag for merge: 
    //    _indiParams = QString("rosparam set /algorithm1/saveSLAMToBag true");

    ////else
    //    //_indiParams = QString("rosparam set /algorithm1/saveSLAMToBag false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// voxel size
    //_indiParams = QString("rosparam set /algorithm1/voxelSize %1").arg(parent->getSlamParam().at(22).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 
    ////====================================================================================
    //
    //if (parent->getType() == process_type::GCP) {
    //    // useGTSAMOptimizer
    //    _indiParams = QString("rosparam set /algorithm1/useGTSAMOptimizer true");
    //    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //    QTimer::singleShot(500, &loop, SLOT(quit()));    
    //    loop.exec(); 

    //    // saveGTSAMPath
    //    _indiParams = QString("rosparam set /algorithm1/saveGTSAMPath true");
    //    _yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //    QTimer::singleShot(500, &loop, SLOT(quit()));    
    //    loop.exec(); 
    //}
    //
    ////checkbagCheck();
    ////checkbagInfo();
    //_startBag();
    

    parent->setProgressBar(2);
    parent->setLog(tr("Configuration in progress.."));

    YAML::Node config = YAML::LoadFile("..\\Lib\\company\\params_comb.yaml");

    // method to gcp
    config["algorithm1"]["methodToUse"] = 0; // normal algorithm1

    // no of cores
    config["algorithm1"]["numberOfCores"] = parent->getSlamParam().at(0).toInt();
    // las
    //if (parent->getSlamParam().at(16) == "true")  // gcp no need las // parent->getType() == process_type::GCP
    //    config["algorithm1"]["saveMapToLAS"] = false;
    //else 
        config["algorithm1"]["saveMapToLAS"] = true; 
    // csv
    if (parent->getSlamParam().at(2) == "true" && parent->getSlamParam().at(1) != "true")
        config["algorithm1"]["savePoseToCSV"] = true;
    else
        config["algorithm1"]["savePoseToCSV"] = false;
    // rotating frames
    if (parent->getSlamParam().at(3) == "true")
        config["algorithm1"]["onlyRotationFramesUsedForMapping"] = true;
    else
        config["algorithm1"]["onlyRotationFramesUsedForMapping"] = false;
    // loop closure
    if (parent->getType() == process_type::GCP ) { // gcp no need loop closure // || parent->getType() == process_type::RTK
        config["algorithm1"]["useLoopClosure"] = false;
    }
    else {
        if (parent->getSlamParam().at(4) == "true") {
            config["algorithm1"]["useLoopClosure"] = true;
            config["algorithm1"]["loopClosureIntervalFrames"] = parent->getSlamParam().at(5).toInt(); // loop closure interval frames
            if (parent->getSlamParam().at(25) == "true")
                config["algorithm1"]["scanInBuilding"] = true;
            else
                config["algorithm1"]["scanInBuilding"] = false;
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
           
        //config["algorithm1"]["saveMapToLAS"] = false;
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


    QString output = "..\\Lib\\company\\params.yaml";
    QFile file(output); // remove if exists
    file.remove();
    std::ofstream fout("..\\Lib\\company\\params.yaml");
    fout << config << std::endl; // dump it back into the file
    fout.close();


    //checkbagCheck();
    //checkbagInfo();
    std::cout << "done params" << std::endl;
    parent->windowAddToCmd(QString(tr("done params")));
    _startBag();
}
void processBag::setYaml_Rtk() {

    //// set param 
    //QString _indiParams;
    //QProcess* _yamlParam = new QProcess();
    //_yamlParam->start("cmd.exe");
    //_yamlParam->write(QString(ros_setup + "\r\n").toUtf8());
    //QEventLoop loop; 

    ////====================================================================================
    ////sensor
    //_indiParams = QString("rosparam set /algorithm1/sensor %1").arg(parent->getRtkParam().at(0));
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// no of cores
    //_indiParams = QString("rosparam set /algorithm1/numberOfCores %1").arg(parent->getRtkParam().at(1));
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    ////// las (TRUE FOR NOW)
    //////if (parent->getRtkParam().at(2) == "true")
    ////_indiParams = QString("rosparam set /algorithm1/saveMapToLAS true");
    //////else
    ////    //_indiParams = QString("rosparam set /algorithm1/saveMapToLAS false");
    ////_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //////QApplication::processEvents();
    ////QTimer::singleShot(500, &loop, SLOT(quit())); 
    ////loop.exec(); 

    //// csv
    //if (parent->getRtkParam().at(3) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/savePoseToCSV true");
    //else
    //    _indiParams = QString("rosparam set /algorithm1/savePoseToCSV false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// rotating frames
    //if (parent->getRtkParam().at(4) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/onlyRotationFramesUsedForMapping true");
    //else
    //    _indiParams = QString("rosparam set /algorithm1/onlyRotationFramesUsedForMapping false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// narrow space
    //if (parent->getRtkParam().at(5) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/isNarrowSpace true");
    //else
    //    _indiParams = QString("rosparam set /algorithm1/isNarrowSpace false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// min intensity filter
    //_indiParams = QString("rosparam set /algorithm1/minIntensity %1").arg(parent->getRtkParam().at(6).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar max range
    //_indiParams = QString("rosparam set /algorithm1/lidarMaxRange %1").arg(parent->getRtkParam().at(7).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar min range
    //_indiParams = QString("rosparam set /algorithm1/lidarMinRange %1").arg(parent->getRtkParam().at(8).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //// QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar max x
    //_indiParams = QString("rosparam set /algorithm1/lidarMaxXToFilter %1").arg(parent->getRtkParam().at(9).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar min x
    //_indiParams = QString("rosparam set /algorithm1/lidarMinXToFilter %1").arg(parent->getRtkParam().at(10).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// translation threshold
    //_indiParams = QString("rosparam set /algorithm1/translationThreshold %1").arg(parent->getRtkParam().at(11).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //// QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// icp iteration
    //_indiParams = QString("rosparam set /algorithm1/icpMaximumIterations %1").arg(parent->getRtkParam().at(12).toInt());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// outputCoordinateSystem   
    //_indiParams = QString("rosparam set /algorithm1/outputCoordinateSystem \"%1\"").arg(parent->getRtkParam().at(16));
    //std::cout << _indiParams.toStdString() << std::endl; // **************************** TO BE DELETED **************************
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// gpsPositionOffset_x
    //_indiParams = QString("rosparam set /algorithm1/gpsPositionOffset_x %1").arg(parent->getRtkParam().at(17).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// gpsPositionOffset_y
    //_indiParams = QString("rosparam set /algorithm1/gpsPositionOffset_y %1").arg(parent->getRtkParam().at(18).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// gpsPositionOffset_z
    //_indiParams = QString("rosparam set /algorithm1/gpsPositionOffset_z %1").arg(parent->getRtkParam().at(19).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// gpsFrequence
    //_indiParams = QString("rosparam set /algorithm1/Frequency %1").arg(parent->getRtkParam().at(20).toInt());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// algorithm2: save bag
    //_indiParams = QString("rosparam set /algorithm1/saveSLAMToBag true");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 
    ////====================================================================================
    ////checkbagCheck();
    ////checkbagInfo();
    //_startBag();

    parent->setProgressBar(2);
    parent->setLog(tr("configuration in progress.."));

    YAML::Node config = YAML::LoadFile("..\\Lib\\company\\params_rtk.yaml");

    //====================================================================================
    // sensor
    config["algorithm1"]["sensor"] = parent->getRtkParam().at(0).toStdString();
    // no of cores
    config["algorithm1"]["numberOfCores"] = parent->getRtkParam().at(1).toInt();
    // las (TRUE FOR NOW)
    //if (parent->getRtkParam().at(2) == "true")
    //    config["algorithm1"]["saveMapToLAS"] = true; 
    //else
        config["algorithm1"]["saveMapToLAS"] = false;
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
    config["algorithm1"]["gpsFrequency"] = parent->getRtkParam().at(20).toInt();
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
    QString output = "temp.yaml";
    QFile file(output); // remove if exists
    file.remove();
    std::ofstream fout("temp.yaml");

    fout << config << std::endl; // dump it back into the file
    fout.close();
    std::cout << "done rtk" << std::endl;
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
        QTimer::singleShot(500, &loop1, SLOT(quit()));  
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
    processCheck->start("cmd.exe");

    path1 = lidarBag.replace('/', '\\');
    _checkbagInfo = QString("rosbag info %1").arg(path1);

    // write to terminal
    processCheck->write(QString(ros_setup + "\r\n").toUtf8());
    processCheck->write(QString(_checkbagInfo + "\r\n").toUtf8());

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
        //std::cout << "rosbag info checked" << std::endl;
        std::cout << "algorithm1 starting" << std::endl;
        parent->windowAddToCmd(QString(tr("algorithm1 starting")));
        std::cout << "============================" << std::endl;
        parent->setProgressBar(3);
        parent->setLog(tr("algorithm1 in processing.."));

        processStart = new QProcess;
        //processStart->start("cmd.exe");
        QString path1 = "";
        QString  _algorithm1BagStr = "";
        QString lidarBag = bagPath;

        //// get current dir
        //QString path2 = "";
        //int lidarbagIndex = lidarBag.lastIndexOf("/");
        //path2 = lidarBag.left(lidarbagIndex + 1);

        path1 = lidarBag.replace('/', '\\');

        QString path = parent->getSlamParam().at(15) + "/";
        QString Fpath = path.replace('/', '\\');

        QString name = parent->getSlamParam().at(14);
        QString Fname = name.replace('/', '\\');

        if (parent->getType() == process_type::RTK) {
            QString rtkFile = parent->getRtkFile();
            if (rtkFile.isEmpty())rtkFile = "void";
            QString rtkPath = rtkFile.replace('/', '\\');

            //_algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 && echo CFYplaybagendcfy").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname).arg(rtkPath);
            _algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 -log_path=..\\Log\\")
                .arg(parent->addQuote(path1.toStdString()))
                .arg(parent->addQuote(parent->getSlamParam().at(22).toStdString()))
                .arg(parent->addQuote(Fpath.toStdString()))
                .arg(parent->addQuote(Fname.toStdString()))
                .arg(parent->addQuote(rtkPath.toStdString()));

        }
        else { // slam type OR gcp (for merging)

            // for gcp
            if (parent->getType() == process_type::GCP) {
                // new folder for ply file(s)
                gcpPlyFolder = (QDateTime::currentDateTime()).toString("yyyy.MM.dd_hh:mm:ss");
                gcpPlyFolder = gcpPlyFolder.replace('.', '_');
                gcpPlyFolder = gcpPlyFolder.replace(':', '_');
                gcpPlyFolder = QString("controlPts1st");
                gcpPlyFolder = parent->getSlamParam().at(15) + "\\" + gcpPlyFolder;
                //==============================================================================
                QProcess* processMkdir = new QProcess;
                processMkdir->start("cmd.exe");
                QString D3_mkdirBagFile = QString("md %1").arg(parent->addQuote(gcpPlyFolder.toStdString()));
                processMkdir->write(D3_mkdirBagFile.toLocal8Bit() + '\n');
                std::cout << "creating a new folder for ply file(s)" << std::endl;
                QEventLoop loop1; 
                QTimer::singleShot(1000, &loop1, SLOT(quit()));    
                loop1.exec(); 
                // ================================================================================================================

                //_algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 && echo CFYplaybagendcfy").arg(path1).arg(parent->getSlamParam().at(22)).arg(gcpPlyFolder).arg(Fname);
                _algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -log_path=..\\Log\\")
                    .arg(parent->addQuote(path1.toStdString()))
                    .arg(parent->addQuote(parent->getSlamParam().at(22).toStdString()))
                    .arg(parent->addQuote(gcpPlyFolder.toStdString()))
                    .arg(parent->addQuote(Fname.toStdString()));

            }
            else {
                // run the bag
                //_algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 && echo CFYplaybagendcfy").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname);
                _algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -log_path=..\\Log\\")
                    .arg(parent->addQuote(path1.toStdString()))
                    .arg(parent->addQuote(parent->getSlamParam().at(22).toStdString()))
                    .arg(parent->addQuote(Fpath.toStdString()))
                    .arg(parent->addQuote(Fname.toStdString()));

            }

        }
        std::cout << _algorithm1BagStr.toStdString() << std::endl;

        /*processStart->write(QString(ros_setup + "\r\n").toUtf8());
        processStart->write(_algorithm1BagStr.toLocal8Bit() + '\n');
        connect(processStart, SIGNAL(readyReadStandardOutput()), this, SLOT(_startBag_output()));*/

        //_algorithm1BagStr = QString("..\\setup.bat && algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname);
        
        // run with checking of the exit code
        processStart->start(QString(ros_setup + " && "+_algorithm1BagStr+"\n"));
        connect(processStart, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotalgorithm1ExitCode(int, QProcess::ExitStatus)));
        //connect(processStart, SIGNAL(readyReadStandardOutput()), this, SLOT(_startBag_output()));
        connect(processStart, SIGNAL(readyReadStandardError()), this, SLOT(_startBag_output()));

    }
}

void processBag::slotalgorithm1ExitCode(int a, QProcess::ExitStatus e) {

    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "algorithm1 successfully exits" << std::endl;
        parent->windowAddToCmd(QString(tr("algorithm1 done")));


        if (parent->getType() == process_type::RTK) {
            parent->setProgressBar(15); 
        }
        else { // slam / slam + gcp
            if (parent->getType() == process_type::GCP) {
                parent->setProgressBar(28); // total = 3 + 25 %
            }
            else {
                if (parent->getSlamParam().at(16) == "true") // algorithm2
                    parent->setProgressBar(53); // total = 3 + 50 %
                //else // slam
            }
        }



        QEventLoop loop; 
        QTimer::singleShot(1500, &loop, SLOT(quit())); 
        loop.exec(); 

        algorithm1Done();
    }
    else {
        std::cout << "algorithm(algorithm1) in error." << std::endl;
        parent->windowAddToCmd(QString(tr("algorithm1 in error.")));
        errInfo = "algorithm1";
        if (parent->progressBar->value() == 3 && !notEgm ) {
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
    
    //int index1 = str.lastIndexOf("[ Lidar ]");
    //int index2 = str.lastIndexOf(")  ---- /hesai/pandar");

    //std::cout << "*****readAllStandardOutput: " << str.toStdString() << std::endl;
    //std::cout << "*****readAllStandardError: " << err.toStdString() << std::endl;

    //// get total frame number
    //if (index2 > -1 && index1 > -1) {

    //    QString text = str;
    //    int frame = 0;
    //    //QString text = "[ Lidar ]:  [0.049316 ~ 0.100096] avg: 0.050039 ( 1264)  ---- /hesai/pandar";
    //    QRegularExpression regex("\\((.*)\\)  ---- /hesai/pandar"); 
    //    QRegularExpressionMatchIterator i = regex.globalMatch(text);
    //    while (i.hasNext()) {
    //        QRegularExpressionMatch match = i.next();
    //        frame = match.captured(1).toInt();
    //        // std::cout << "Matched string: " << match.captured(0).toStdString()<<std::endl;
    //        // std::cout << "Start position: " << match.capturedStart()<<std::endl;
    //        // std::cout << "End position: " << match.capturedEnd()<<std::endl;
    //    }
    //    std::cout << "frame size: " << frame << " ___" << std::endl;
    //    frame_size = frame;
    //    // std::cout << "l: "<<index <<std::endl;
    //    // std::cout << "h: "<<index1 <<std::endl;
    //    // QString getFrames = str.mid(index,index1).simplified().section('(', 1, -1).simplified();
    //    // getFrames = getFrames.section(')', 0, 0).simplified();
    //    // std::cout<< "____: " <<getFrames.toStdString()<< " ___" <<std::endl;      
    //}

    //// for progress bar
    //int check = str.lastIndexOf("Failure rate");
    //if (check > 0) {
    //    int indexDuration = str.indexOf(" | ") + 2;
    //    QString progress = str.mid(indexDuration);
    //    progress = progress.simplified().section(' ', 0, 0).simplified();
    //    int current_prog = (progress.toInt() * 100 / frame_size);

    //    if (parent->getType() == process_type::RTK) {
    //        parent->setProgressBar(3 + (current_prog * 0.2)); // total = 3 + 20 %
    //    }
    //    else { // slam / slam + gcp
    //        if (parent->getType() == process_type::GCP) {
    //            parent->setProgressBar(3 + (current_prog * 0.25)); // total = 3 + 25 %
    //        }
    //        else {
    //            if (parent->getSlamParam().at(16) == "true") // algorithm2
    //                parent->setProgressBar(3 + (current_prog * 0.5)); // total = 3 + 50 %
    //            else // slam
    //                parent->setProgressBar(3 + (current_prog * 0.90)); // total = 3 + 90 %
    //        }
    //    }

    //    //std::cout << str.toStdString() << std::endl;
    //    parent->windowAddToCmd(str);
    //}

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
        QRegularExpression regex("Valid LiDAR size: (\\d+)"); 
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
        QRegularExpression regex("Progress: (([0-9]*[.])?[0-9]+)%"); 
        QRegularExpressionMatchIterator i = regex.globalMatch(err);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            double current_percentile = match.captured(1).toDouble();
            int current_prog = static_cast<int>(current_percentile);

            if (parent->getType() == process_type::RTK) {
                if((3 + (current_prog * 0.2)) <= 15)
                    parent->setProgressBar(3 + (current_prog * 0.2)); // total = 3 + 20 %
            }
            else { // slam / slam + gcp
                if (parent->getType() == process_type::GCP) {
                    if ((3 + (current_prog * 0.25)) <= 28)
                        parent->setProgressBar(3 + (current_prog * 0.25)); // total = 3 + 25 %
                }
                else {
                    if (parent->getSlamParam().at(16) == "true") // algorithm2
                        if ((3 + (current_prog * 0.5)) <= 53)
                            parent->setProgressBar(3 + (current_prog * 0.5)); // total = 3 + 50 %
                    else // slam
                        parent->setProgressBar(3 + (current_prog * 0.90)); // total = 3 + 90 %
                }
            }

        }
        std::cout << err.toStdString() << std::endl;
        parent->windowAddToCmd(err);

        notEgm = true;
    }
    

}
/*
void processBag::_startBag_output() {
    
    QString str = QString(processStart->readAllStandardOutput());
    int index = str.lastIndexOf("playbagendcfy"); // will echo twice 

    int index1 = str.lastIndexOf("[ Lidar ]");
    int index2 = str.lastIndexOf(")  ---- /hesai/pandar");

    QProcess process;
    QString cmdd = "tasklist";
    process.start(cmdd);
    process.waitForFinished();                   
    QString str1 = process.readAllStandardOutput();

    if (!str1.contains("algorithm1_nrt"))
    {
        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "algorithm1 is not responding yet (remember to check if this is running in the task manager)" << std::endl;
        std::cout << "============================" << std::endl;
    }
    //else {
    if (index2 > -1 && index1 > -1) {
        // _roscore_output();
        // return;

        QString text = str;
        int frame = 0;
        //QString text = "[ Lidar ]:  [0.049316 ~ 0.100096] avg: 0.050039 ( 1264)  ---- /hesai/pandar";
        QRegularExpression regex("\\((.*)\\)  ---- /hesai/pandar"); 
        QRegularExpressionMatchIterator i = regex.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            frame = match.captured(1).toInt();
            // std::cout << "Matched string: " << match.captured(0).toStdString()<<std::endl;
            // std::cout << "Start position: " << match.capturedStart()<<std::endl;
            // std::cout << "End position: " << match.capturedEnd()<<std::endl;
        }
        std::cout << "frame size: " << frame << " ___" << std::endl;

        frame_size = frame;



        // std::cout << "l: "<<index <<std::endl;
        // std::cout << "h: "<<index1 <<std::endl;
        // QString getFrames = str.mid(index,index1).simplified().section('(', 1, -1).simplified();
        // getFrames = getFrames.section(')', 0, 0).simplified();
        // std::cout<< "____: " <<getFrames.toStdString()<< " ___" <<std::endl;      
    }

    //for rtk first stage
    int checkRtk = str.lastIndexOf("RTk align finished, align result saved.");
    if (checkRtk != -1) { 
        index=2; // replacement for the 2nd echo 
    }

    if (index > -1)
    {
        algorithm1Index = algorithm1Index + 1;
        std::cout << "nihao ******************" << std::endl;
        if (algorithm1Index > 1)
        {
            //closeAll(); // close play, record
            QEventLoop loop; 
            QTimer::singleShot(2000, &loop, SLOT(quit())); 
            loop.exec(); 

            // stop cancel btn
            //parent->unableCancel();

            algorithm1Done();
            algorithm1Index = 0;

            process.kill();
        }
    }
    //int check = str.lastIndexOf("failed rate");
    int check = str.lastIndexOf("Failure rate");
    if (check > 0) {
        int indexDuration = str.indexOf(" | ") + 2;
        QString progress = str.mid(indexDuration);
        progress = progress.simplified().section(' ', 0, 0).simplified();
        int current_prog = (progress.toInt() * 100 / frame_size);

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
        //std::cout << "\n" << std::endl;
        //std::cout << "============================" << std::endl;
        //std::cout << "algorithm1 is running" << std::endl;
        //std::cout << "============================" << std::endl;
        //std::cout << str.toStdString() << std::endl;

        //// for log
        //QTextStream stream(&logFile);
        //stream << str;

        
    }

    // } 
    
}
*/
void processBag::algorithm1Done() {
    if (process_running) {
        //// log done
        //logFile.flush();
        //logFile.close();
        if (parent->getType() == process_type::GCP) {
            if (parent->getManualCheck())
                _startGcpManual();
            else
                _startGcpMerge();
        }
        else if (parent->getType() == process_type::RTK) {
            //allDone();
            //parent->setLog(tr("rtk done in %1").arg(QString::fromStdString(parent->getWindowTimer())));
            //parent->startWindowTimer();
            //startalgorithm2();

            std::vector<std::string> tempList = parent->getWindowTimerP();
            total_time += std::stod(tempList.at(0));
            parent->setLog(tr("Rtk's first stage done in %1").arg(QString::fromStdString(tempList.at(1))));
            parent->setLog(tr("Rtk's final stage starts."));
            
            parent->startWindowTimerP();
            start2ndRtk();
            //startalgorithm2();
        }
        else {
            if (parent->getSlamParam().at(1) == "true") {
                std::vector<std::string> tempList = parent->getWindowTimerP();
                total_time += std::stod(tempList.at(0));
                parent->setLog(tr("algorithm1 done in %1").arg(QString::fromStdString(tempList.at(1))));
                parent->startWindowTimerP();
                startBa();

            }
            else if (parent->getSlamParam().at(16) == "true") {
                //parent->setLog(tr("slam done in %1").arg(QString::fromStdString(parent->getWindowTimer())));
                //parent->startWindowTimer();
                //startalgorithm2();

                std::vector<std::string> tempList = parent->getWindowTimerP();
                total_time += std::stod(tempList.at(0));
                parent->setLog(tr("algorithm1 done in %1").arg(QString::fromStdString(tempList.at(1))));
                parent->startWindowTimerP();
                startalgorithm2();
            }
            else {
                std::vector<std::string> tempList = parent->getWindowTimerP();
                total_time += std::stod(tempList.at(0));
                parent->setLog(tr("algorithm1 done in %1").arg(QString::fromStdString(tempList.at(1))));
                allDone();
            }
        }
    }
}

void processBag::startalgorithm2() {
    if (process_running) {
        parent->setProgressBar(parent->progressBar->value()+1);
        parent->setLog(tr("Starting algorithm1 plus.."));
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

        //algorithm2Bag = "E:\\test.bag_SLAM.bag";
        std::cout << "algorithm1 plus bag: " << algorithm2Bag.toStdString() << std::endl;
        QFile fi(algorithm2Bag);
        if (fi.exists()) {
            std::cout << "algorithm1 plus starts" << std::endl;
            parent->windowAddToCmd(QString(tr("algorithm1 plus starting")));
        }
        else {
            std::cout << "slam bag not found" << std::endl;
            parent->windowAddToCmd(QString(tr("slam bag not found, algorithm1 plus cannot be started")));
        }

        //QString lidarBag = algorithm2Bag;
        //QString  _checkbagInfo = "";
        //algorithm2Frame = new QProcess; // preparation
        //algorithm2Frame->start("cmd.exe");
        //QString path1 = lidarBag.replace('/', '\\');
        //_checkbagInfo = QString("rosbag info %1").arg(path1);

        //// write to terminal
        //algorithm2Frame->write(QString(ros_setup + "\r\n").toUtf8());
        //algorithm2Frame->write(QString(_checkbagInfo + "\r\n").toUtf8());

        //connect(algorithm2Frame, SIGNAL(readyReadStandardOutput()), this, SLOT(_getalgorithm2Frames()));

        //std::cout << "\n" << std::endl;
        //std::cout << "============================" << std::endl;
        //std::cout << "getting algorithm1 plus total frames" << std::endl;
        //std::cout << "============================" << std::endl;

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
        QTimer::singleShot(1000, &loop1, SLOT(quit()));   
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
        QString algorithm2bag = algorithm2Bag.replace('/', '\\');
        QString name;
        bool cuda = false;

        name = parent->getSlamParam().at(14);

        if (parent->getSlamParam().at(20) == "true") {
            cuda = true;
            std::cout << "cuda is checked " << std::endl;
        }
        else {
            cuda = false;
            std::cout << "cuda is not checked " << std::endl;
        }


        QString Fname = name.replace('/', '\\');
        //algorithm2bag = "E:\\Atestalgorithm2\\test.bag_SLAM.bag"; // TO BE DELETED ************
        std::cout << "cuda: " << cuda << std::endl;

        if (cuda) {
            //usage: algorithm1plus.exe input_bag [uphold] [output_name] [current_frame_size]
            algorithm2_path = QString("algorithm1plusCuda.exe %1 %2 %3 %4")
                .arg(parent->addQuote(algorithm2bag.toStdString()))
                .arg(parent->getSlamParam().at(24))
                .arg(parent->addQuote(Fname.toStdString()))
                .arg(parent->getSlamParam().at(23));
            //algorithm2_path = QString("algorithm1plusCuda.exe %1 %2").arg(algorithm2bag).arg(Fname);
            parent->windowAddToCmd(QString(tr("cuda is used.")));
        }
        else {
            algorithm2_path = QString("algorithm1plus.exe %1 %2")
                .arg(parent->addQuote(algorithm2bag.toStdString()))
                .arg(parent->addQuote(Fname.toStdString()));
            parent->windowAddToCmd(QString(tr("cuda is not used.")));
        }
        

        //algorithm2_path = QString("algorithm1plus.exe %1 %2").arg(algorithm2Bag).arg("parent");
        algorithm2 = new QProcess;
        /*algorithm2->start("cmd.exe");
        algorithm2->write(QString(ros_setup + "\r\n").toUtf8());
        algorithm2->write(QString(algorithm2_path + "\r\n").toUtf8());*/
        algorithm2->start(QString(ros_setup + " && " + algorithm2_path + "\n"));
        connect(algorithm2, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotalgorithm2ExitCode(int, QProcess::ExitStatus)));
        connect(algorithm2, SIGNAL(readyReadStandardOutput()), this, SLOT(_startalgorithm2Output()));

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
        parent->windowAddToCmd(QString(tr("algorithm1 plus in error.")));
        errInfo = "algorithm1Plus";
        whenCrashed();
    }
}
void processBag::_startalgorithm2Output() {
    QString str = QString(algorithm2->readAllStandardOutput());
    //int index = str.lastIndexOf("finish!!!"); // will echo twice
    //std::cout << str.toStdString() << std::endl;
    //std::cout << index << std::endl;
    //if (index == 0)
    //{
    //    QEventLoop loop; 
    //    QTimer::singleShot(2000, &loop, SLOT(quit())); 
    //    loop.exec();
    //    algorithm2Done();
    //}

    int check = str.lastIndexOf("Current frame is: ");

    if (check > -1) {
        QString text = str;
        int frame = 0;
        QRegularExpression regex("Current frame is: (\\d+)."); 
        QRegularExpressionMatchIterator i = regex.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            frame = match.captured(1).toInt();
        }

        int oo = frame;
        int a = (((float)(oo * 100) / algorithm2_size) / 100) * (float)(100 - algorithm2Remaining+1 );
        if((algorithm2Remaining+1 + a)<=98)
            parent->setProgressBar(algorithm2Remaining+1 +a);

        std::cout << "algorithm1 plus is running.." << std::endl;
        parent->windowAddToCmd(QString(tr("algorithm1 plus is running..")));
    }
}
void processBag::algorithm2Done() {
    std::cout << "algorithm1 plus done" << std::endl;
    parent->windowAddToCmd(QString(tr("algorithm1 plus done.")));
    //parent->setLog(tr("algorithm1 plus done"));
    //parent->setLog(tr("algorithm1 plus done in %1").arg(QString::fromStdString(parent->getWindowTimer())));
    std::vector<std::string> tempList = parent->getWindowTimerP();
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
    processCl = new QProcess;
    processCl->start("cmd.exe");
    processCl->write(QString(ros_setup + "\r\n").toUtf8());
    QString D3_closeAll_str3 = "rosnode kill record_data";
    processCl->write(D3_closeAll_str3.toLocal8Bit() + '\n');
    QString D3_closeAll_str1 = "rosnode kill /ekf_gps";
    processCl->write(D3_closeAll_str1.toLocal8Bit() + '\n');
    QString D3_closeAll_str2 = "rosnode kill bagplay";
    processCl->write(D3_closeAll_str2.toLocal8Bit() + '\n');
}

void processBag::start2ndRtk() {
    if (process_running) {
        parent->setProgressBar(parent->progressBar->value() + 1);
        rtkRemaining = parent->progressBar->value();

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "rtk 2nd stage starting" << std::endl;
        parent->windowAddToCmd(QString(tr("rtk final stage starting")));
        std::cout << "============================" << std::endl;

        rtk_2nd = new QProcess;
        //rtk_2nd->start("cmd.exe");
        QString path1 = "";
        QString  _algorithm1BagStr = "";
        QString lidarBag = bagPath;

        path1 = lidarBag.replace('/', '\\');

        QString path = parent->getSlamParam().at(15) + "/";
        QString Fpath = path.replace('/', '\\');

        QString name = parent->getSlamParam().at(14);
        QString Fname = name.replace('/', '\\');

        QString rtkFile = parent->getRtkFile();
        if (rtkFile.isEmpty())rtkFile = "void";
        QString rtkPath = rtkFile.replace('/', '\\');

        //_algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 && echo CFYplaybagendcfy").arg(path1).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname).arg(rtkPath);
        _algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 -log_path=..\\Log\\")
            .arg(parent->addQuote(path1.toStdString()))
            .arg(parent->addQuote(parent->getSlamParam().at(22).toStdString()))
            .arg(parent->addQuote(Fpath.toStdString()))
            .arg(parent->addQuote(Fname.toStdString()))
            .arg(parent->addQuote(rtkPath.toStdString()));

        std::cout << _algorithm1BagStr.toStdString() << std::endl;

        //rtk_2nd->write(QString(ros_setup + "\r\n").toUtf8());
        //rtk_2nd->write(_algorithm1BagStr.toLocal8Bit() + '\n');
        //connect(rtk_2nd, SIGNAL(readyReadStandardOutput()), this, SLOT(_startRtk2nd_output()));

        rtk_2nd->start(QString(ros_setup + " && " + _algorithm1BagStr + "\n"));
        connect(rtk_2nd, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotRtkExitCode(int, QProcess::ExitStatus)));
        //connect(rtk_2nd, SIGNAL(readyReadStandardOutput()), this, SLOT(_startRtk2nd_output()));
        connect(rtk_2nd, SIGNAL(readyReadStandardError()), this, SLOT(_startRtk2nd_output()));
        
    }
}
void processBag::slotRtkExitCode(int a, QProcess::ExitStatus e) {
    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "algorithm1(rtk) successfully exits" << std::endl;
        parent->windowAddToCmd(QString(tr("rtk final stage done.")));
        std::vector<std::string> tempList = parent->getWindowTimerP();
        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("Rtk's final stage done in %1").arg(QString::fromStdString(tempList.at(1))));
        parent->startWindowTimerP();
        parent->setProgressBar(60);
        QEventLoop loop; 
        QTimer::singleShot(1500, &loop, SLOT(quit())); 
        loop.exec(); 

        startalgorithm2();

    }
    else {
        std::cout << "algorithm(algorithm1-rtk) in error." << std::endl;
        parent->windowAddToCmd(QString(tr("rtk final stage in error.")));
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
            if ((rtkRemaining + (current_prog * 0.5)) <= 59)
                parent->setProgressBar(rtkRemaining + (current_prog * 0.5)); // total = ~10 + 50 %
        }

        parent->windowAddToCmd(err);
        std::cout << err.toStdString() << std::endl;
    }
}


void processBag::_startGcpMerge() {
    if (process_running) {
        parent->setProgressBar(parent->progressBar->value()+1);
        gcpRemaining = parent->progressBar->value();
        QString bagPathGcp;
        

        if (!gcpCp2ndRound) {          
            // merge the _SLAM.bag - first time
            std::vector<std::string> tempList = parent->getWindowTimerP();
            total_time += std::stod(tempList.at(0));
            parent->setLog(tr("Slam done in %1").arg(QString::fromStdString(tempList.at(1))));
            parent->startWindowTimerP();
            parent->setLog(tr("Merging the output.."));
            bagPathGcp = gcpPlyFolder + "/" + parent->getSlamParam().at(14) + QString("_SLAM.bag");

            QFile fi(bagPathGcp);
            if (fi.exists()) {
                std::cout << "gcp: merging" << std::endl;
                parent->windowAddToCmd(QString(tr("gcp: merging.")));
            }
            else {
                std::cout << "gcp: bag not found" << std::endl;
                parent->windowAddToCmd(QString(tr("gcp: failed to merge.")));
            }
        }
        else { // 2nd time gcpCp
            parent->setLog(tr("Merging the output.. 2nd"));
            bagPathGcp = gcpPlyFolder2nd + "/" + parent->getSlamParam().at(14) + QString("_SLAM.bag");

            QFile fi(bagPathGcp);
            if (fi.exists()) {
                std::cout << "gcp: merging 2nd" << std::endl;
                parent->windowAddToCmd(QString(tr("gcp: merging. 2nd")));
            }
            else {
                std::cout << "gcp: bag not found" << std::endl;
                parent->windowAddToCmd(QString(tr("gcp: failed to merge. 2nd")));
            }
        }

        QString bagg = bagPathGcp.replace('/', '\\');

        processM = new QProcess;
        //processM->start("cmd.exe");
        QString merge_str = QString("merge.exe %1").arg(parent->addQuote(bagg.toStdString()));
        std::cout << merge_str.toStdString() << std::endl;
        //processM->write(QString(ros_setup + "\r\n").toUtf8());
        //processM->write(merge_str.toLocal8Bit() + '\r' + '\n');
        //connect(processM, SIGNAL(readyReadStandardOutput()), this, SLOT(gcp_merge_output()));


        processM->start(QString(ros_setup + " && " + merge_str + "\n"));
        connect(processM, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotMergeExitCode(int, QProcess::ExitStatus)));
        connect(processM, SIGNAL(readyReadStandardOutput()), this, SLOT(gcp_merge_output()));

        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "merge is running (remember to check if this is running in the task manager)" << std::endl;
        std::cout << "============================" << std::endl;
    }
}
void processBag::slotMergeExitCode(int a, QProcess::ExitStatus e) {

    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "merge successfully exits" << std::endl;
        parent->windowAddToCmd(QString(tr("merge done.")));
        if (!gcpCp2ndRound) {
            parent->setProgressBar(31);
        }
        else {
            parent->setProgressBar(72);
        }
        
        QEventLoop loop; 
        QTimer::singleShot(1500, &loop, SLOT(quit())); 
        loop.exec(); 
        startGcpCP();

    }
    else {
        std::cout << "algorithm(merge) in error." << std::endl;
        parent->windowAddToCmd(QString(tr("merge in error.")));
        errInfo = "merge";
        whenCrashed();
    }

}
void processBag::gcp_merge_output() {


    QString str = QString(processM->readAllStandardOutput());
    int index = str.lastIndexOf("Done!");

    if (index < 0) {

        if (str.lastIndexOf("\r\n") > 0) {
            if (!gcpCp2ndRound) {
                parent->setProgressBar(31);
            }
            else {
                parent->setProgressBar(72);
            }
        }
    }
    else {
        parent->setLog(tr("Merging done"));
        //_startGcp();
        //startGcpCP();
        //std::cout << "\n" << std::endl;
        //std::cout << "============================" << std::endl;
        //std::cout << "plz continue" << std::endl;
        //std::cout << "============================" << std::endl;
        return;
    }
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
        parent->setProgressBar(parent->progressBar->value()+1);
        gcpRemaining = parent->progressBar->value();
        //parent->setLog(tr("merge done in %1").arg(QString::fromStdString(parent->getWindowTimer()))); // ****** TO BE CHANGED
        //parent->startWindowTimer();

        std::vector<std::string> tempList = parent->getWindowTimerP();
        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("Merge done in %1").arg(QString::fromStdString(tempList.at(1))));
        parent->startWindowTimerP();

        parent->setLog(tr("Getting all control points.."));


        QString savingPath;
        QString pathCloud;
        QString txtPc;
        if (!gcpCp2ndRound) {

            // 1st get pathPly from slam
            //QString savingPath = parent->getSlamParam().at(15);
            savingPath = gcpPlyFolder;
            pathCloud = parent->getBag() + QString("_algorithm1_companyPath.ply");
            //QString savingPath = "E:\\AtestGcp";
            //QString pathCloud = "E:\\AtestGcp\\2023-07-06-12-13-10.bag_algorithm1_GtsamPath.ply";
            std::cout << "saving path: " << savingPath.toStdString() << std::endl;
            std::cout << "path cloud: " << pathCloud.toStdString() << std::endl;

            // save/get first realxyz only , not save csv file
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
                pcStr = pcStr.replace('/', '\\');
                std::string temp = pcStr.toStdString();
                finalPlyFiles.emplace_back(temp);
                std::cout << temp << std::endl;
            }
            // change first argv = ply file(s) into txt
            txtPc = QString(gcpPlyFolder + "\\clouds.txt");
            txtPc = txtPc.replace('/', '\\');
            std::ofstream output_file(txtPc.toStdString());
            std::ostream_iterator<std::string> output_iterator(output_file, "\n");
            std::copy(std::begin(finalPlyFiles), std::end(finalPlyFiles), output_iterator);

        }
        else {

            // 1st get pathPly from slam
            savingPath = gcpPlyFolder2nd;
            pathCloud = parent->getBag() + QString("_algorithm1_companyPath.ply");
            //QString savingPath = "E:\\AtestGcp";
            //QString pathCloud = "E:\\AtestGcp\\2023-07-06-12-13-10.bag_algorithm1_GtsamPath.ply";
            std::cout << "saving path: " << savingPath.toStdString() << std::endl;
            std::cout << "path cloud: " << pathCloud.toStdString() << std::endl;

            //// save first realxyz
            //parent->saveCsv(parent->gcpTxtFile);

            // 2nd store n ply files into vector
            QStringList nameFilter("*.ply");
            QDir directory(gcpPlyFolder2nd);
            QStringList allPlyFiles = directory.entryList(nameFilter);
            //QStringList finalPlyFiles; // pass into exe's argv
            std::vector<std::string> finalPlyFiles; // pass into exe's argv
            std::cout << "adding ply file(s)" << std::endl;
            for (QString a : allPlyFiles) {
                QString pcStr = QString(gcpPlyFolder2nd + "/" + a);
                pcStr = pcStr.replace('/', '\\');
                std::string temp = pcStr.toStdString();
                finalPlyFiles.emplace_back(temp);
                std::cout << temp << std::endl;
            }
            // change first argv = ply file(s) into txt
            txtPc = QString(gcpPlyFolder2nd + "\\clouds.txt");
            txtPc = txtPc.replace('/', '\\');
            std::ofstream output_file(txtPc.toStdString());
            std::ostream_iterator<std::string> output_iterator(output_file, "\n");
            std::copy(std::begin(finalPlyFiles), std::end(finalPlyFiles), output_iterator);

        }

        // 3rd put argvs into exe
        bool manualCheck = parent->getManualCheck();
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

        //txtPc = QString("E:\\AtestGcp\\123\\2023_07_13_15_08_35\\clouds.txt");
        //pathCloud = QString("E:\\AtestGcp\\123\\2023_07_13_15_08_35\\testgcps.bag_SLAM.bag_1_.ply");
        if (gcpCp2ndRound) {
            filterSwitchkey = 0;
            pathCloud = "N";
            std::cout << "path cloud: " << pathCloud.toStdString() << std::endl;
        }
        // 4th run exe to get csv
        QString gcpStr = QString("gcpCP.exe %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15")
            .arg(parent->addQuote(txtPc.toStdString()))
            .arg(parent->addQuote(QString(savingPath + "\\").toStdString()))
            .arg(parent->addQuote(pathCloud.toStdString()))
            .arg(parent->addQuote(parent->gcpTxtFile.toStdString()))
            .arg(circlePts).arg(radMin).arg(radMax).arg(iteration).arg(filterSwitchkey).arg(filterDistance)
            .arg(intensityMin).arg(intensityMax).arg(filterZ).arg(filterZCoorMin).arg(filterZCoorMax);
        gcpStr = gcpStr.replace('/', '\\');
        std::cout << gcpStr.toStdString() << std::endl;

        parent->windowAddToCmd(QString(tr("getting gcps.")));

        gcp_cp = new QProcess;
        //gcp_cp->start("cmd.exe");
        //gcp_cp->write(QString(gcpStr + "\r\n").toUtf8());
        //connect(gcp_cp, SIGNAL(readyReadStandardOutput()), this, SLOT(_gcpFindPoints()));


        gcp_cp->start(QString(gcpStr + "\n"));
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
    if (e == QProcess::ExitStatus::NormalExit)std::cout << "byebye" << std::endl;
    if (e == QProcess::ExitStatus::CrashExit)std::cout << "gg" << std::endl;
    std::cout << "gcpcp exit no.: " << std::to_string(a) << std::endl;
    if (e == QProcess::ExitStatus::NormalExit ) { // && a == 0 // set to 1 for now. because exit = 1 ???? sometimes 1 and sometimes 0
        std::cout << "gcpCp successfully exits" << std::endl;
        parent->windowAddToCmd(QString(tr("gcps found.")));
        
        if (!gcpCp2ndRound) {
            parent->setProgressBar(40);
        }
        else {
            parent->setProgressBar(80);
        }
        QEventLoop loop; 
        QTimer::singleShot(1500, &loop, SLOT(quit())); 
        loop.exec(); 
        //return;
        gcpFindPointsFinished();
    }
    else {
        std::cout << "algorithm(gcpCp) in error." << std::endl;
        parent->windowAddToCmd(QString(tr("finding gcps in error.")));
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
        parent->windowAddToCmd(QString(tr("finding gcps..")));
        parent->windowAddToCmd(str);
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
            //QEventLoop loop; 
            //QTimer::singleShot(2000, &loop, SLOT(quit())); 
            //loop.exec();
            //gcpFindPointsFinished();
            return;
        }
    }

}
void processBag::gcpFindPointsFinished() {
    std::vector<std::string> tempList = parent->getWindowTimerP();
    total_time += std::stod(tempList.at(0));
    parent->setLog(tr("Getting GCP(s) done, in %1").arg(QString::fromStdString(tempList.at(1))));
    if (!gcpCp2ndRound) {
        //parent->setLog(tr("Getting GCP(s) done, in %1").arg(QString::fromStdString(parent->getWindowTimer())));
        //std::vector<std::string> tempList = parent->getWindowTimerP();
        //total_time += std::stod(tempList.at(0));
        //parent->setLog(tr("Getting GCP(s) done, in %1").arg(QString::fromStdString(tempList.at(1))));
        // 
        // can manual check - edit csv file, etc    
        QString pathCloud = parent->getBag() + QString("_algorithm1_companyPath.ply");
        // check/clear csv file first
        //QString csvFile = parent->getSlamParam().at(16) + "/" + parent->getSlamParam().at(15) + ".csv";
        std::string csvFileName = parent->gcpTxtFile.toStdString().replace(parent->gcpTxtFile.toStdString().length() - 3, 3, "csv");
        //QString csvFile = parent->gcpTxtFile + ".csv";
        QString csvFile = QString::fromStdString(csvFileName);
        csvFile = csvFile.replace('/', '\\');
        std::cout << "csv: " << csvFile.toStdString() << std::endl;
        ////QString csvFile =  "hi.csv";
        QFile fi3(csvFile);
        if (fi3.exists()) {
            //fi3.remove();
            std::cout << "output csv found" << std::endl;
            parent->windowAddToCmd(QString(tr("output csv found")));
        }
        else {
            std::cout << "output csv not found" << std::endl;
            parent->windowAddToCmd(QString(tr("output csv not found, cannot continue")));
        }

        parent->setViewGcp(parent->getBag(), pathCloud, csvFile);
        parent->labelRunning->setText("waiting for procedure..");
        parent->setLog(tr("You may check the .csv file in the chosen saving path, and press 'continue' in 'GCP'."));
        gcpRemaining = parent->progressBar->value();
    }
    else {
        // after 2 rounds of gcpCp, start algorithm2
        parent->startWindowTimerP();
        // change 'algorithm2Bag' path OR copy paste out from gcpPlyFolder2nd to bag's saving path
        //matrix vs coor
        std::string csvFileName = parent->gcpTxtFile.toStdString().replace(parent->gcpTxtFile.toStdString().length() - 3, 3, "csv");
        QString csvFile = QString::fromStdString(csvFileName);
        csvFile = csvFile.replace('/', '\\');
        std::cout << "2nd csv: " << csvFile.toStdString() << std::endl;

        QString transTxtPath = gcpPlyFolder2nd + "/" + parent->getSlamParam().at(14) + QString("_SLAMtrans.txt");
        std::cout << "trans: " << transTxtPath.toStdString() << std::endl;
        parent->_prepareGcpReportInputs(transTxtPath, csvFile);
        QApplication::processEvents(); // precaution
        // ========================================================================



        startalgorithm2();
    }
}

void processBag::_startGcpManual() {

    if (process_running) {
        parent->setProgressBar(parent->progressBar->value()+1);
        gcpRemaining = parent->progressBar->value();
        // merge the _SLAM.bag
        std::vector<std::string> tempList = parent->getWindowTimerP();
        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("Slam done in %1").arg(QString::fromStdString(tempList.at(1))));
        //parent->startWindowTimerP();

        parent->setLog(tr("Selecting control points manually.."));
        parent->labelRunning->setText("waiting for selection..");


        // get point cloud
        QString name = parent->getSlamParam().at(14);
        QString Fname = name.replace('/', '\\');
        QString Fpath = gcpPlyFolder.replace('/', '\\');
        QString check = QString(Fpath + "\\" + Fname + ".laz");
        std::cout << check.toStdString() << std::endl;
        QFile fi(check);
        if (fi.exists())std::cout << "point cloud from slam found" << std::endl; 
        else std::cout << "point cloud from slam not found" << std::endl;

        // move to 3d view : check = point cloud
        parent->manualGcpShow(check);

        // return back after selection and end gcp manual addition
        //if (parent->getType() == process_type::GCP) { // if manual check
        //    if (parent->getManualCheck()) {
        //        parent->manualGcpEnd();
        //    }
        //}
    }
}



void processBag::_gcpEE(QString cloud, QString cloudpath, QString csv) {
    //parent->startWindowTimer(); // for timing
    parent->startWindowTimerP(); // for timing
    parent->labelRunning->setText("running..");
    std::cout << "gcp ee" << std::endl;
    parent->finalGcpEEcsv = csv;
    //parent->finalGcpEEcsv = "E:\\1a\\testGcp.csv";
    std::cout << "final csv: "<< parent->finalGcpEEcsv.toStdString() << std::endl;
    //runGcpEE();
    //return;   
    parent->setProgressBar(parent->progressBar->value()+1);
    gcpRemaining = parent->progressBar->value();
    // 4th:
    // eliminate error
    parent->setLog(tr("Getting configuration files.."));
    //// rosparam
    //gcpEE_yaml = new QProcess;
    //gcpEE_yaml->start("cmd.exe");
    //QString _startParams = "";
    //_startParams = tr("rosparam load ..\\Lib\\company\\params_gcpEE.yaml && echo CFYcfy");


    //gcpEE_yaml->write(QString(ros_setup + "\r\n").toUtf8());
    //gcpEE_yaml->write(_startParams.toLocal8Bit() + '\r' + '\n');
    //connect(gcpEE_yaml, SIGNAL(readyReadStandardOutput()), this, SLOT(_checkYamlGcpEE()));

    //std::cout << "\n" << std::endl;
    //std::cout << "============================" << std::endl;
    //std::cout << "gcp ee rosparam starting" << std::endl;
    //std::cout << "============================" << std::endl;

    setYamlGcpEE();
}
void processBag::_gcpEEManual() {

    parent->startWindowTimerP(); // for timing
    //parent->labelRunning->setText("running..");
    std::cout << "gcp ee - manual" << std::endl;

    parent->setLog(tr("Getting configuration files.."));


    setYamlGcpEE();

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
    ////std::cout << "hi" << std::endl;
    //// set param 
    //QString _indiParams;
    //QProcess* _yamlParam = new QProcess();
    //_yamlParam->start("cmd.exe");
    //_yamlParam->write(QString(ros_setup + "\r\n").toUtf8());
    //QEventLoop loop; 

    ////====================================================================================
    ////sensor
    //_indiParams = QString("rosparam set /algorithm1/sensor %1").arg(parent->getSlamParam().at(0));
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// no of cores
    //_indiParams = QString("rosparam set /algorithm1/numberOfCores %1").arg(parent->getSlamParam().at(1));
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// csv
    //_indiParams = QString("rosparam set /algorithm1/savePoseToCSV false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// rotating frames
    //if (parent->getSlamParam().at(4) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/onlyRotationFramesUsedForMapping true");
    //else
    //    _indiParams = QString("rosparam set /algorithm1/onlyRotationFramesUsedForMapping false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// loop closure
    //_indiParams = QString("rosparam set /algorithm1/useLoopClosure false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 


    //// narrow space
    //if (parent->getSlamParam().at(7) == "true")
    //    _indiParams = QString("rosparam set /algorithm1/isNarrowSpace true");

    //else
    //    _indiParams = QString("rosparam set /algorithm1/isNarrowSpace false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// min intensity filter
    //_indiParams = QString("rosparam set /algorithm1/minIntensity %1").arg(parent->getSlamParam().at(8).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar max range
    //_indiParams = QString("rosparam set /algorithm1/lidarMaxRange %1").arg(parent->getSlamParam().at(9).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar min range
    //_indiParams = QString("rosparam set /algorithm1/lidarMinRange %1").arg(parent->getSlamParam().at(10).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //// QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar max x
    //_indiParams = QString("rosparam set /algorithm1/lidarMaxXToFilter %1").arg(parent->getSlamParam().at(11).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// lidar min x
    //_indiParams = QString("rosparam set /algorithm1/lidarMinXToFilter %1").arg(parent->getSlamParam().at(12).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// translation threshold
    //_indiParams = QString("rosparam set /algorithm1/translationThreshold %1").arg(parent->getSlamParam().at(13).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //// QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    //// icp iteration
    //_indiParams = QString("rosparam set /algorithm1/icpMaximumIterations %1").arg(parent->getSlamParam().at(14).toInt());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// loss sigma
    //_indiParams = QString("rosparam set /algorithm1/lossSigma %1").arg(parent->getSlamParam().at(18).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //_indiParams = QString("rosparam set /algorithm1/compressLas false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// map resolution
    //_indiParams = QString("rosparam set /algorithm1/mapResolution %1").arg(parent->getSlamParam().at(20).toFloat());
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// algorithm2: save bag
    //_indiParams = QString("rosparam set /algorithm1/saveSLAMToBag true");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    ////QApplication::processEvents();
    //QTimer::singleShot(500, &loop, SLOT(quit())); 
    //loop.exec(); 

    ////// las
    ////_indiParams = QString("rosparam set /algorithm1/saveMapToLAS true");
    ////_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //////QApplication::processEvents();
    ////QTimer::singleShot(500, &loop, SLOT(quit())); 
    ////loop.exec(); 
    ////====================================================================================

    //// useGTSAMOptimizer
    //_indiParams = QString("rosparam set /algorithm1/useGTSAMOptimizer true");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 

    //// saveGTSAMPath
    //_indiParams = QString("rosparam set /algorithm1/saveGTSAMPath false");
    //_yamlParam->write(_indiParams.toLocal8Bit() + '\r' + '\n');
    //QTimer::singleShot(500, &loop, SLOT(quit()));    
    //loop.exec(); 
    //

    //runGcpEE();


    YAML::Node config = YAML::LoadFile("..\\Lib\\company\\params_comb.yaml");

    // method to gcp
    config["algorithm1"]["methodToUse"] = 2; // gcp

    // no of cores
    config["algorithm1"]["numberOfCores"] = parent->getSlamParam().at(0).toInt();
    // las
    //if (parent->getSlamParam().at(16) == "true")  // gcp no need las // parent->getType() == process_type::GCP
       // config["algorithm1"]["saveMapToLAS"] = false;
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
    if (parent->getType() == process_type::GCP ) { // gcp no need loop closure // || parent->getType() == process_type::RTK
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

        // manualGCP    
        if (parent->getManualCheck()) {
            // plyPath
            QString pathCloud = parent->getBag() + QString("_algorithm1_companyPath.ply");
            config["algorithm1"]["plyPath"] = pathCloud.toStdString();

            config["algorithm1"]["manualGCP"] = true;
        }           
        else {
            config["algorithm1"]["manualGCP"] = false;
        }
            
        
        
    }

    QString output = "..\\Lib\\company\\params.yaml";
    QFile file(output); // remove if exists
    file.remove();
    std::ofstream fout("..\\Lib\\company\\params.yaml");
    fout << config << std::endl; // dump it back into the file
    fout.close();


    std::cout << "done gcp params" << std::endl;
    parent->windowAddToCmd(QString(tr("done gcp params.")));


    runGcpEE();
}
void processBag::runGcpEE() {
    if (process_running) {
        // run exe
        std::cout << "\n" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "running gcp: eliminate error" << std::endl;
        std::cout << "============================" << std::endl;

        parent->setLog(tr("Eliminating error.."));
        gcpEE_run = new QProcess;
        QString _algorithm1BagStr;

        // new folder for ply file(s)
        //gcpPlyFolder2nd = QString("controlPts2nd");
        //gcpPlyFolder2nd = parent->getSlamParam().at(15) + "\\" + gcpPlyFolder2nd;
        gcpPlyFolder2nd = parent->getSlamParam().at(15) + "\\" ;
        QDir dir(gcpPlyFolder2nd);
        dir.mkdir(QString("controlPts2nd_finalResults"));
        std::cout << "creating a 2nd new folder for ply file(s)" << std::endl;
        //QEventLoop loop1; 
        //QTimer::singleShot(1000, &loop1, SLOT(quit()));    
        //loop1.exec(); 
        gcpPlyFolder2nd += QString("controlPts2nd_finalResults");
        // ================================================================================================================


        if (!parent->getManualCheck()) {
            
            //gcpEE_run->start("cmd.exe");

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
            dataFile << std::fixed << std::setprecision(6) << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << 0.000000 << "," << parent->rx_gcpee << "," << parent->ry_gcpee << "," << parent->rz_gcpee << std::endl; // first line
            std::cout << "rx " << parent->rx_gcpee << std::endl;
            std::cout << "ry " << parent->ry_gcpee << std::endl;
            std::cout << "rz " << parent->rz_gcpee << std::endl;
            // i = 1 coz first line = title
            for (int i = 1; i < tempVec.size(); i++) {
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
            pathBag = lidarBag.replace('/', '\\');

            QString lidarTxt = gcpEEtxt;
            QString pathTxt = "";
            pathTxt = lidarTxt.replace('/', '\\');

            QString path = parent->getSlamParam().at(15) + "/";
            path = gcpPlyFolder2nd + "/";; // for 2nd gcpCp
            QString Fpath = path.replace('/', '\\');

            QString name = parent->getSlamParam().at(14);
            QString Fname = name.replace('/', '\\');

            //QString _algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 && echo CFYplaybagendcfy").arg(pathBag).arg(parent->getSlamParam().at(22)).arg(Fpath).arg(Fname).arg(pathTxt);
            _algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 -log_path=..\\Log\\")
                .arg(parent->addQuote(pathBag.toStdString()))
                .arg(parent->addQuote(parent->getSlamParam().at(22).toStdString()))
                .arg(parent->addQuote(Fpath.toStdString()))
                .arg(parent->addQuote(Fname.toStdString()))
                .arg(parent->addQuote(pathTxt.toStdString()));

            std::cout << _algorithm1BagStr.toStdString() << std::endl;
            parent->windowAddToCmd(QString(tr("eliminating error starting.")));

            algorithm1Index = 0;
            //gcpEE_run->write(QString(ros_setup + "\r\n").toUtf8());
            //gcpEE_run->write(_algorithm1BagStr.toLocal8Bit() + '\n');
            //connect(gcpEE_run, SIGNAL(readyReadStandardOutput()), this, SLOT(_startGcpee_output()));


        }
        else {

            QString gcpEEtxt = parent->getSlamParam().at(15) + "/" + parent->getSlamParam().at(14) + "_GCPEE.txt";

            QString lidarBag = parent->getBag();
            QString pathBag = "";
            pathBag = lidarBag.replace('/', '\\');

            QString lidarTxt = gcpEEtxt;
            QString pathTxt = "";
            pathTxt = lidarTxt.replace('/', '\\');

            QString path = parent->getSlamParam().at(15) + "/";
            path = gcpPlyFolder2nd + "/"; // for 2nd gcpCp
            QString Fpath = path.replace('/', '\\');

            QString name = parent->getSlamParam().at(14);
            QString Fname = name.replace('/', '\\');

            _algorithm1BagStr = QString("algorithm1_nrt_comb.exe -bag=%1 -algorithm1=..\\Lib\\company\\params.yaml -product=%2 -save_path=%3 -save_name=%4 -txt=%5 -log_path=..\\Log\\")
                .arg(parent->addQuote(pathBag.toStdString()))
                .arg(parent->addQuote(parent->getSlamParam().at(22).toStdString()))
                .arg(parent->addQuote(Fpath.toStdString()))
                .arg(parent->addQuote(Fname.toStdString()))
                .arg(parent->addQuote(pathTxt.toStdString()));

            std::cout << _algorithm1BagStr.toStdString() << std::endl;
            parent->windowAddToCmd(QString(tr("eliminating error starting.")));

            algorithm1Index = 0;
        }

        gcpEE_run->start(QString(ros_setup + " && " + _algorithm1BagStr + "\n"));
        connect(gcpEE_run, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotGcpeeExitCode(int, QProcess::ExitStatus)));
        //connect(gcpEE_run, SIGNAL(readyReadStandardOutput()), this, SLOT(_startGcpee_output()));
        connect(gcpEE_run, SIGNAL(readyReadStandardError()), this, SLOT(_startGcpee_output()));
        
    }
}
void processBag::slotGcpeeExitCode(int a, QProcess::ExitStatus e) {

    if (e == QProcess::ExitStatus::NormalExit && a == 0) {
        std::cout << "gcpEe successfully exits" << std::endl;
        parent->windowAddToCmd(QString(tr("eliminating errors done.")));
        QEventLoop loop; 
        QTimer::singleShot(1500, &loop, SLOT(quit())); 
        loop.exec(); 

        std::vector<std::string> tempList = parent->getWindowTimerP();
        total_time += std::stod(tempList.at(0));
        parent->setLog(tr("Eliminating error done, in %1").arg(QString::fromStdString(tempList.at(1))));
        //allDone();
        parent->startWindowTimerP(); // for timing

        parent->setProgressBar(70);
        //for 2nd round of gcpCp - accuracy report
        gcpCp2ndRound = true;
        _startGcpMerge();
        return;
        //startalgorithm2();
    }
    else {
        std::cout << "algorithm(gcpEe) in error." << std::endl;
        parent->windowAddToCmd(QString(tr("gcp algorithm in error.")));
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

        parent->windowAddToCmd(err);
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
            parent->windowAddToCmd(QString(tr("algorithm1 2nd stage starting..")));
        }
        else {
            std::cout << "slam bag not found" << std::endl;
            parent->windowAddToCmd(QString(tr("slam bag not found, algorithm1 2nd stage cannot be started")));
        }

        //algorithm2_size = frame_size;
        //std::cout << "algorithm1 plus frame: " << std::to_string(algorithm2_size) << std::endl;
        //startalgorithm2Run();
        //baBag = "E:\\vsTest\\2023-12-25-02-43-47_SLAM.bag";

        QString path;
        QString bag = baBag.replace('/', '\\');
        //QString name;
        //name = parent->getSlamParam().at(14);

        //algorithm2bag = "E:\\Atestalgorithm2\\test.bag_SLAM.bag"; // TO BE DELETED ************
        
        QString baSplit; // split normal plane
        if (parent->baSplit.contains("1"))baSplit = "true";
        else baSplit = "false";

 
        path = QString("algorithm1_ba.exe -slam=%1 -iter=%2 -voxel_size=%3 -save_pose=true -split_normal=%4 -min_pt=20 -voxel_layer=3 -normal_threshold=0.95")
            .arg(parent->addQuote(bag.toStdString()))
            .arg(parent->baIter)
            .arg(parent->baVoxel)
            .arg(baSplit);


        std::cout << path.toStdString() << std::endl;


        //algorithm2_path = QString("algorithm1plus.exe %1 %2").arg(algorithm2Bag).arg("parent");
        ba = new QProcess;
        /*algorithm2->start("cmd.exe");
        algorithm2->write(QString(ros_setup + "\r\n").toUtf8());
        algorithm2->write(QString(algorithm2_path + "\r\n").toUtf8());*/
        ba->start(QString(ros_setup + " && " + path + "\n"));
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
        parent->windowAddToCmd(QString(tr("algorithm1 2nd stage in error.")));
        errInfo = "algorithm12nd";
        whenCrashed();
    }
}
void processBag::_startbaOutput() {
    QString str = QString(ba->readAllStandardOutput());
    std::cout << "algorithm1 2nd stage is running.." << std::endl;
    parent->windowAddToCmd(QString("algorithm1 2nd stage is running.."));

    //int check = str.lastIndexOf("Current frame is: ");
    //if (check > -1) {
    //    QString text = str;
    //    int frame = 0;
    //    QRegularExpression regex("Current frame is: (\\d+)."); 
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
    parent->windowAddToCmd(QString(tr("algorithm1 2nd stage done.")));
    std::vector<std::string> tempList = parent->getWindowTimerP();
    total_time += std::stod(tempList.at(0));
    parent->setLog(tr("algorithm1 2nd stage in %1").arg(QString::fromStdString(tempList.at(1))));
    parent->startWindowTimerP();
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
        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished();                    
        QString str1 = process.readAllStandardOutput();

        //if (str1.contains("roscore.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("roscore.exe"));
        //}
        //if (str1.contains("rosmaster.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("rosmaster.exe"));
        //}
        //if (str1.contains("rosout.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("rosout.exe"));
        //}
        //if (str1.contains("python.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("python.exe"));
        //}
        //if (str1.contains("algorithm1_nrt_rtk.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk.exe"));
        //}
        //if (str1.contains("algorithm1_nrt_gcpEE.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_gcpEE.exe"));
        //}
        if (str1.contains("algorithm1_nrt_comb.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_comb.exe"));
        }
        if (str1.contains("algorithm1plus.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1plus.exe"));
        }
        if (str1.contains("algorithm1plusCuda.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1plusCuda.exe"));
        }
        if (str1.contains("merge.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
        }
        if (str1.contains("gcpCp.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("gcpCp.exe"));
        }
        if (str1.contains("algorithm1_ba.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_ba.exe"));
        }
        process.close();
        std::cout << "all processes closed" << std::endl;


        parent->labelRunning->setText(tr("canceling.."));
        QApplication::processEvents(); // show loading dialog
        QEventLoop loop; 
        QTimer::singleShot(2000, &loop, SLOT(quit())); 
        loop.exec(); 


        if (parent->getType() == process_type::GCP) { // if manual check
            if (parent->getManualCheck()) {
                parent->manualGcpEnd();
            }
        }

        parent->stopWindowTimerP();
        parent->labelRunning->setText(tr("canceled"));
        parent->setLog(tr("Canceled"));
        
    }
    else if (a == 0) {
        //std::vector<std::string> tempList = parent->getWindowTimerP();
        //total_time += std::stod(tempList.at(0));

        //parent->setLog(tr("algorithm1 plus done in %1").arg(QString::fromStdString(tempList.at(1))));

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

        // remove slam.bag ============================================
        QString name = parent->getSlamParam().at(14);
        QString Fname = name.replace('/', '\\');
        QString path = parent->getSlamParam().at(15) + "/";
        QString Fpath = path.replace('/', '\\');
        QString check = QString(Fpath+"\\"+Fname + "_SLAM.bag");
        //check = "E:\\1testBags\\large\\2023-11-25-11-53-54.bag_SLAM.bag";
        std::cout << check.toStdString() << std::endl;
        QFile fi(check);
        if (fi.exists()) { fi.remove(); std::cout << "slam.bag removed" << std::endl;}
        QApplication::processEvents(); // show loading dialog
        QEventLoop loop; 
        QTimer::singleShot(1500, &loop, SLOT(quit())); 
        loop.exec(); 

        QString check1 = QString(Fpath + "\\" + Fname + "_SLAM.bag_optimized.bag");
        //check = "E:\\1testBags\\large\\2023-11-25-11-53-54.bag_SLAM.bag";
        std::cout << check1.toStdString() << std::endl;
        QFile fi1(check1);
        if (fi1.exists()) { fi1.remove(); std::cout << "slam.bag_optimized.bag removed" << std::endl; }
        QApplication::processEvents(); // show loading dialog
        QEventLoop loop1; 
        QTimer::singleShot(1500, &loop1, SLOT(quit())); 
        loop1.exec(); 

        if (parent->getType() == process_type::GCP) {

            QString name = parent->getSlamParam().at(14);
            QString Fname = name.replace('/', '\\');
            QString Fpath = gcpPlyFolder.replace('/', '\\');
            QString check = QString(Fpath + "\\" + Fname + "_SLAM.bag");
            std::cout << check.toStdString() << std::endl;
            QFile fii(check);
            if (fii.exists()) { fii.remove(); std::cout << "gcp slam.bag removed" << std::endl; }
            QApplication::processEvents(); // show loading dialog
            QEventLoop loop; 
            QTimer::singleShot(1500, &loop, SLOT(quit())); 
            loop.exec(); 


            Fpath = gcpPlyFolder2nd.replace('/', '\\');
            check = QString(Fpath + "\\" + Fname + "_SLAM.bag");
            std::cout << check.toStdString() << std::endl;
            QFile fiii(check);
            if (fiii.exists()) { fiii.remove(); std::cout << "gcp 2 slam.bag removed" << std::endl; }
            QApplication::processEvents(); // show loading dialog
            QEventLoop loop1; 
            QTimer::singleShot(1500, &loop1, SLOT(quit())); 
            loop1.exec(); 

        }
        // =============================================================

        parent->labelRunning->setText(tr("done in %1").arg(QString::fromStdString(res)));
        parent->setLog(tr("Done in %1").arg(QString::fromStdString(res)));
        parent->setProgressBar(100);

    }

    parent->setStatus(running_status::finished);
    parent->windowCanRunBag();

    if (parent->getType() == process_type::GCP) {
        // show accuracy report
        parent->getViewProcess()->onAccuracyReport();
    }

    this->close();

#ifdef _WIN32
    SetThreadExecutionState(ES_CONTINUOUS); // end the prevention of the sleep
#endif
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
        QProcess process;
        QString cmdd = "tasklist";
        process.start(cmdd);
        process.waitForFinished();                    
        QString str1 = process.readAllStandardOutput();

        //if (str1.contains("roscore.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("roscore.exe"));
        //}
        //if (str1.contains("rosmaster.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("rosmaster.exe"));
        //}
        //if (str1.contains("rosout.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("rosout.exe"));
        //}
        //if (str1.contains("python.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("python.exe"));
        //}
        //if (str1.contains("algorithm1_nrt_rtk.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_rtk.exe"));
        //}
        //if (str1.contains("algorithm1_nrt_gcpEE.exe")) 
        //{
        //    QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_gcpEE.exe"));
        //}
        if (str1.contains("algorithm1_nrt_comb.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_nrt_comb.exe"));
        }
        if (str1.contains("algorithm1plus.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1plus.exe"));
        }
        if (str1.contains("algorithm1plusCuda.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1plusCuda.exe"));
        }
        if (str1.contains("merge.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("merge.exe"));
        }
        if (str1.contains("gcpCp.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("gcpCp.exe"));
        }
        if (str1.contains("algorithm1_ba.exe")) 
        {
            QProcess::startDetached("taskkill -t  -f /IM " + QString("algorithm1_ba.exe"));
        }
        process.close();
        std::cout << "all processes closed" << std::endl;


        parent->labelRunning->setText(tr("canceling.."));
        QApplication::processEvents(); // show loading dialog
        QEventLoop loop; 
        QTimer::singleShot(2000, &loop, SLOT(quit())); 
        loop.exec(); 


        if (parent->getType() == process_type::GCP) { // if manual check
            if (parent->getManualCheck()) {
                parent->manualGcpEnd();
            }
        }

        parent->stopWindowTimerP();
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

#ifdef _WIN32
    SetThreadExecutionState(ES_CONTINUOUS); // end the prevention of the sleep
#endif
}