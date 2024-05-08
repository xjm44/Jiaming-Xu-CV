#include "../../include/include_gui/widget_newProcess.h"
#include "../../include/include_gui/mainApp.h"
//#include "../../include/include_gui/gcp.h"
#include <iostream>
#include <fstream>
#include <QDir>
#include <QTextStream>
#include <QGenericMatrix>

widget_newProcess::widget_newProcess(QWidget *parent)
	: QWidget(parent),
	merge_mp4(this),_colour(this),mp4FilePath(QString()),
	process(this),
    gcpTxtFile(QString()),
    rx_gcpee(0.000000), ry_gcpee(0.000000), rz_gcpee(0.000000),
    name(QString()),
    viewProcess(nullptr)
    , status(running_status::waiting)
    , uncolorPts(false)
       
{
	setupUi(this);

	window_ = dynamic_cast<mainApp*>(parent);

	init();

}
void widget_newProcess::startWindowTimer() {
    if (window_)
        window_->startTimer();
}
std::vector<std::string> widget_newProcess::getWindowTimer() {
    if (window_)
        return window_->getTimer();

    std::vector<std::string> a;
    return a; // just for 'warning'
}
void widget_newProcess::stopWindowTimer() {
    if (window_)
        window_->stopTimer();
}
widget_newProcess::~widget_newProcess()
{}

void widget_newProcess::run()
{
    window_->availableForBagRunning = false;
    process.startTheRun(current_bag);
}

void widget_newProcess::runColour() {
    // merge mp4 first
    // colourisation

    window_->availableForBagRunning = false;
    merge_mp4.startMerging(colorFileMp4);
}

void widget_newProcess::init() {

    //to see log + param
	viewProcess = new widget_viewProcess(this);
    process.init();

	connect(btnViewProcess, SIGNAL(clicked()), this, SLOT(_viewProcess()), Qt::UniqueConnection);
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(_closeProcess()), Qt::UniqueConnection);

    // initial status
	labelRunning->setText("waiting..");

    // colourisation
    connect(&merge_mp4, SIGNAL(_startColouriseMultiMp4()), this, SLOT(_proceedToColourisationMulti()), Qt::UniqueConnection);
    connect(&merge_mp4, SIGNAL(_startColourise()), this, SLOT(_proceedToColourisation()), Qt::UniqueConnection);

    // gcp
    connect(viewProcess, SIGNAL(_gcpEE()), this, SLOT(_proceedToEE()), Qt::UniqueConnection);
    connect(viewProcess, SIGNAL(_gcpReport()), this, SLOT(_proceedToGcpReport()), Qt::UniqueConnection);
}

void widget_newProcess::setBag(QString a) {
    this->current_bag = a; 
    lineEditBagName->setText(window_->getFileName(current_bag));
    lineEditBagName->setStyleSheet("QLineEdit{font-size:16px}");
    setViewProcess();
}

void widget_newProcess::_viewProcess() {
	
	viewProcess->show();

    
}

void widget_newProcess::setViewProcess() {
    if (current_type == process_type::Slam) 
        viewProcess->labelBagName->setText("SLAM");
    else if (current_type == process_type::Colourisation) 
        viewProcess->labelBagName->setText("Colourisation"); 
    else if (current_type == process_type::RTK) 
        viewProcess->labelBagName->setText("RTK"); 
    else if (current_type == process_type::GCP) 
        viewProcess->labelBagName->setText("Slam + GCP"); // for view btn -> view process -> typr    

    viewProcess->_showParam(current_type);
}

// ======================================================== SLAM ======================================================================
QStringList widget_newProcess::getSlamParam() {
    //std::cout << "---  --- " << std::endl;
    //std::cout << "sensor: " << sensor.toStdString() << std::endl;
    //std::cout << "core: " << std::to_string(no_of_cores) << std::endl;
    //std::cout << "las: " << std::to_string(las) << std::endl;
    //std::cout << "csv: " << std::to_string(csv) << std::endl;
    //std::cout << "rotatingFrames: " << std::to_string(rotatingFrames) << std::endl;
    //std::cout << "loopClosure: " << std::to_string(loopClosure) << std::endl;
    //std::cout << "loopClosureIntervalFrames: " << std::to_string(loopClosureIntervalFrames) << std::endl;
    //std::cout << "narrowSpace: " << std::to_string(narrowSpace) << std::endl;
    //std::cout << "minIntensityFilter: " << std::to_string(minIntensityFilter) << std::endl;
    //std::cout << "lidarMaxRange: " << std::to_string(lidarMaxRange) << std::endl;
    //std::cout << "lidarMinRange: " << std::to_string(lidarMinRange) << std::endl;
    //std::cout << "lidarMaxXToFilter: " << std::to_string(lidarMaxXToFilter) << std::endl;
    //std::cout << "lidarMinXToFilter: " << std::to_string(lidarMinXToFilter) << std::endl;
    //std::cout << "translationThreshold: " << std::to_string(translationThreshold) << std::endl;
    //std::cout << "icpMaximumIterations: " << std::to_string(icpMaximumIterations) << std::endl;


    return list_slamParam;


}
void widget_newProcess::setSlamParam(
    int no_of_cores,
    bool ba,
    bool csv,
    bool rotatingFrames,
    bool loopClosure,
    int loopClosureIntervalFrames,
    bool narrowSpace,
    float minIntensityFilter,
    float lidarMaxRange,
    float lidarMinRange,
    float lidarMaxXToFilter,
    float lidarMinXToFilter,
    float translationThreshold,
    int icpMaximumIterations,
    QString bagName,
    QString bagPath,
    bool algorithm1,
    QString lossSigma,
    bool checkBoxtolaz1,
    float doubleSpinBoxResolution1,
    bool cuda,
    float voxel,
    QString productParams,
    int algorithm2Frame,
    float algorithm2Uphold
) {
    this->no_of_cores = no_of_cores;
    this->csv = csv;
    this->ba = ba;
    this->rotatingFrames = rotatingFrames;
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
    this->bagName = bagName;
    this->bagPath = bagPath;
    this->algorithm1 = algorithm1;
    this->lossSigma = lossSigma;
    this->checkBoxtolaz = checkBoxtolaz1;
    this->doubleSpinBoxResolution = doubleSpinBoxResolution1;
    this->cuda = cuda;
    this->voxelSize = voxel;
    this->productParams = productParams;
    this->algorithm2Frame = algorithm2Frame;
    this->algorithm2Uphold = algorithm2Uphold;

    list_slamParam.append(QString::number(this->no_of_cores));
    list_slamParam.append(QVariant(this->ba).toString());
    list_slamParam.append(QVariant(this->csv).toString());
    list_slamParam.append(QVariant(this->rotatingFrames).toString());
    list_slamParam.append(QVariant(this->loopClosure).toString());
    list_slamParam.append(QString::number(this->loopClosureIntervalFrames));
    list_slamParam.append(QVariant(this->narrowSpace).toString());
    list_slamParam.append(QString::number(this->minIntensityFilter));
    list_slamParam.append(QString::number(this->lidarMaxRange));
    list_slamParam.append(QString::number(this->lidarMinRange));
    list_slamParam.append(QString::number(this->lidarMaxXToFilter));
    list_slamParam.append(QString::number(this->lidarMinXToFilter));
    list_slamParam.append(QString::number(this->translationThreshold));
    list_slamParam.append(QString::number(this->icpMaximumIterations));
    list_slamParam.append(this->bagName);
    list_slamParam.append(this->bagPath);
    list_slamParam.append(QVariant(this->algorithm1).toString());
    list_slamParam.append(this->lossSigma);
    list_slamParam.append(QVariant(this->checkBoxtolaz).toString());
    list_slamParam.append(QString::number(this->doubleSpinBoxResolution));
    list_slamParam.append(QVariant(this->cuda).toString());
    list_slamParam.append(QString::number(this->voxelSize));
    list_slamParam.append(this->productParams);
    list_slamParam.append(QString::number(this->algorithm2Frame));
    list_slamParam.append(QString::number(this->algorithm2Uphold));
}
// ======================================================================================================================================
// ===============================================     RTK     ===========================================================================

QStringList widget_newProcess::getRtkParam() {
    return list_rtkParam;
}
// set slam param
void widget_newProcess::setRtkParam(
    bool geoid,
    QString outputCoordinateSystem,
    float doubleSpinBoxXRtk,
    float doubleSpinBoxYRtk,
    float doubleSpinBoxZRtk,
    int spinBoxGpsFreq,
    QString hemisphere,
    bool hemi
) {

    this->geoid = geoid;
    this->outputCoordinateSystem = outputCoordinateSystem;
    this->doubleSpinBoxXRtk = doubleSpinBoxXRtk;
    this->doubleSpinBoxYRtk = doubleSpinBoxYRtk;
    this->doubleSpinBoxZRtk = doubleSpinBoxZRtk;
    this->spinBoxGpsFreq = spinBoxGpsFreq;
    this->hemisphere = hemisphere;
    this->hemi = hemi;

    list_rtkParam.append(QVariant(this->geoid).toString());
    list_rtkParam.append(this->outputCoordinateSystem);
    list_rtkParam.append(QString::number(this->doubleSpinBoxXRtk));
    list_rtkParam.append(QString::number(this->doubleSpinBoxYRtk));
    list_rtkParam.append(QString::number(this->doubleSpinBoxZRtk));
    list_rtkParam.append(QString::number(this->spinBoxGpsFreq));
    list_rtkParam.append(this->hemisphere);
    list_rtkParam.append(QVariant(this->hemi).toString());
    //std::cout << "rtkbagName: " <<this->bagName_rtk.toStdString() << std::endl;
}
// ======================================================================================================================================
// ===============================================     Colourisation     ================================================================
void widget_newProcess::set_colour_mp4(QStringList l) {
    colorFileMp4 = l;
    mp4FilePath = window_->getFilePath(l.at(0)); 
}
void widget_newProcess::setColourParam(
    QString proj,
    QString imu,
    QString traj,
    QString cali,
    int frameskip,
    int firstframe,
    int lastframe,
    float visiblerange,
    float quality_,
    bool uncolorPtss,
    QString penetrationn
) {
    projDir = proj;
    imuSrc = imu;
    trajSrc = traj;
    calibrationFile = cali;
    frameSkip = frameskip;
    firstFrame = firstframe;
    lastFrame = lastframe;
    visibleRange = visiblerange;
    quality = quality_;
    uncolorPts = uncolorPtss;
    penetration = penetrationn;
}

void widget_newProcess::_proceedToColourisation() {
    std::cout << "done single" << std::endl;
    std::cout << finalMp4.toStdString() << std::endl;
    setLog(tr("Configuration.."));
    setProgressBar(10);
    _colour.run();
}
void widget_newProcess::_proceedToColourisationMulti() {
    std::cout << "done multi mp4" << std::endl;
    std::cout << finalMp4.toStdString() << std::endl;
    setLog(tr("Configuration.."));
    setProgressBar(10);
    _colour.run();
}
void widget_newProcess::doneColor() {
    window_->availableForColourRunning = true;
}
// ======================================================================================================================================
// ==================================================       GCP     ===================================================================
void widget_newProcess::setGcpParam(
    bool manualCheck,
    float radMax,
    float radMin,
    int no_of_gcp,
    int intensityMax,
    int intensityMin,
    int iteration,
    int circlePts,
    bool filterDist,
    int filterDistance,
    bool filterZCoor,
    float filterZCoorMax,
    float filterZCoorMin
) {
    this->manualCheck = manualCheck;
    this->radMax = radMax;
    this->radMin = radMin;
    this->no_of_gcp = no_of_gcp;
    this->intensityMax = intensityMax;
    this->intensityMin = intensityMin;
    this->iteration = iteration;
    this->circlePts = circlePts;
    this->filterDist = filterDist;
    this->filterDistance = filterDistance;
    this->filterZCoor = filterZCoor;
    this->filterZCoorMax = filterZCoorMax;
    this->filterZCoorMin = filterZCoorMin;

}

//std::vector<Yuan> widget_newProcess::runGcp(QString pc, QString pcpath, int no, QString savingPath, int gcpIndex) {
//    //std::vector<Yuan> output;
//    //gcp *_gcp = new gcp(this);
//    //_gcp->gcpIndex = gcpIndex;
//    //output = _gcp->get_gcp(pc.toStdString(), QString(savingPath+"\\").toStdString(), no,pcpath.toStdString()).first;
//    //return output;
//
//}
void widget_newProcess::saveCsv(QString gcpTxt) { //QString csvFile, std::vector<Yuan> o, 
    std::cout << "saving csv" << std::endl;
    // get value from txt file first
    QStringList rx;
    QStringList ry;
    QStringList rz;
    QFile file(gcpTxt);
    if (!file.open(QIODevice::ReadOnly)) {
    }
    std::cout << "txt file: " << gcpTxt.toStdString() << " opened." << std::endl;
    QTextStream in(&file);
    int line1 = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        std::cout << "line: " << line.toStdString() << std::endl;
        // saving first point
        if (line1 == 0) {
            QStringList fields = line.split(",");

            std::stringstream sstrm;
            sstrm << std::setprecision(10) << std::fixed << fields.at(1).toDouble() << std::endl;
            double dx;
            sstrm >> dx;
            rx_gcpee = dx;

            std::stringstream sstrm1;
            sstrm1 << std::setprecision(10) << std::fixed << fields.at(0).toDouble() << std::endl;
            double dy;
            sstrm1 >> dy;
            ry_gcpee = dy;

            std::stringstream sstrm2;
            sstrm2 << std::setprecision(10) << std::fixed << fields.at(2).toDouble() << std::endl;
            double dz;
            sstrm2 >> dz;
            rz_gcpee = dz;
            std::cout << "first xyz: " << rx_gcpee<< " "<< ry_gcpee<< " "<< rz_gcpee << std::endl;

            break;
        }


        //// get all real xyz, exclude starting point
        //if (line1 != 0) {
        //    
        //    QStringList fields = line.split(",");

        //    std::stringstream sx;
        //    sx << std::setprecision(10) << std::fixed << fields.at(1).toDouble() << std::endl;
        //    double dx;
        //    sx >> dx;
        //    rx.append(QString::number(dx, 'f', 10));

        //    std::stringstream sy;
        //    sy << std::setprecision(10) << std::fixed << fields.at(0).toDouble() << std::endl;
        //    double dy;
        //    sy >> dy;
        //    ry.append(QString::number(dy, 'f', 10));

        //    std::stringstream sz;
        //    sz << std::setprecision(10) << std::fixed << fields.at(2).toDouble() << std::endl;
        //    double dz;
        //    sz >> dz;
        //    rz.append(QString::number(dz, 'f', 10));

        //    //rx.append(fields.at(1));
        //    //ry.append(fields.at(0));
        //    //rz.append(fields.at(2));
        //    //std::cout << "-----------" << std::endl;
        //    //std::cout << QString::number(dx, 'f', 10).toStdString() << std::endl;
        //    //std::cout << QString::number(dy, 'f', 10).toStdString() << std::endl;
        //    //std::cout << QString::number(dz, 'f', 10).toStdString() << std::endl;
        //    //std::cout << "-----------" << std::endl;
        //}
        line1++;
    }
    file.close();



    //// save csv 
    //// check/clear csv file first
    //QFile fi(csvFile);
    //if (fi.exists()) {
    //    fi.remove();
    //    std::cout << "deleted" << std::endl;
    //}
    //std::cout << "csv file: " << csvFile.toStdString() << " opened." << std::endl;
    //// create a new csv file
    //std::fstream fout;
    //fout.open(csvFile.toStdString(), std::ios::out | std::ios::app);
    //// insert first row (xyztrealxyz)
    //fout << "x" << ","
    //    << "y" << ","
    //    << "z" << ","
    //    << "time" << ","
    //    << "x_path" << ","
    //    << "y_path" << ","
    //    << "z_path" << ","
    //    << "east" << ","
    //    << "north" << ","
    //    << "up" << ","
    //    << "\n";
    //// add data from vector<Yuan>
    //for (int i = 0; i < o.size(); i++) {
    //    if (i >= rx.size()) {
    //        rx.append("0.000000");
    //        ry.append("0.000000");
    //        rz.append("0.000000");
    //    }
    //    o[i].rx = rx.at(i).toDouble();
    //    o[i].ry = ry.at(i).toDouble();
    //    o[i].rz = rz.at(i).toDouble();
    //    //std::cout << o[i].rx << std::endl;
    //    //std::cout << o[i].ry << std::endl;
    //    //std::cout << o[i].rz << std::endl;

    //    fout << std::fixed << std::setprecision(10) 
    //        << o[i].x << ","
    //        << o[i].y << ","
    //        << o[i].z << ","
    //        << o[i].t << ","
    //        << (double)o[i].x_path << ","
    //        << (double)o[i].y_path << ","
    //        << (double)o[i].z_path << ","
    //        << std::fixed << std::setprecision(10)
    //        << (double)o[i].rx << ","
    //        << (double)o[i].ry << ","
    //        << (double)o[i].rz << ","
    //        << "\n";

    //    std::cout << "saving to csv file: " << " ===" << std::endl;
    //}
    //std::cout << "save csv done" << std::endl;
}
std::vector<std::vector<std::string>> widget_newProcess::loadCsv(QString csvFile) {
    // load csv
    std::vector<std::vector<std::string>> content;
    std::vector<std::string> row;
    std::string line, word;

    std::fstream file(csvFile.toStdString(), std::ios::in);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            row.clear();

            std::stringstream str(line);

            while (getline(str, word, ','))
                row.push_back(word);
            content.push_back(row);
        }
    }
    else
        std::vector<std::vector<std::string>> content1; // cannot open file - no such file

    for (int i = 0; i < (int)content.size(); i++)
    {
        for (int j = 0; j < (int)content[i].size(); j++)
        {
            std::cout << content[i][j] << " _ ";
        }
        std::cout << "\n";
    }
    return content;
}

void widget_newProcess::_proceedToEE() { // gcp ee
    
    finalGcpEEcsv = viewProcess->lineEditCoor->text();
    process._gcpEE(viewProcess->lineEditPc->text(), viewProcess->lineEditPcPath->text(), viewProcess->lineEditCoor->text());
}
void widget_newProcess::setViewGcp(QString cloud, QString pathcloud, QString csv) {
    viewProcess->btnContinue->setEnabled(1);
    viewProcess->lineEditCoor->setText(csv);
    viewProcess->lineEditPc->setText(cloud);
    viewProcess->lineEditPcPath->setText(pathcloud);
}


// ======================================================================================================================================
void widget_newProcess::_proceedToGcpReport() {
    gcp_report* report = new gcp_report(this);
    report->setOriGpsFile(gcpTxtFile);  
    report->setFinalXyz(finalXyz);
    report->addItem();
    report->show();
}

std::vector<std::string> widget_newProcess::splitString(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

void widget_newProcess::_prepareGcpReportInputs(QString trans, QString cpCsv) {
    QFile f(trans);
    if(f.exists())std::cout << "trans found " << std::endl;
    QFile f1(cpCsv);
    if (f1.exists())std::cout << "2nd csv found " << std::endl;

    std::ifstream transFile(trans.toStdString());

    std::string line;
    std::vector<double> num; // get trans info
    while (std::getline(transFile, line))
    {
        std::vector<std::string> words = splitString(line);

        for (const std::string& word : words) {
            std::cout << word << std::endl;
            double temp = stod(word);
            num.emplace_back(temp);
        }
        std::cout << "---------" << std::endl;
    }
    transFile.close();

    //double matrix[4][4];
    //double vect[4];
    //vect[0] = 6.870499;
    //vect[1] = -7.885124;
    //vect[2] = -0.439918;
    //vect[3] = 1.0;

    //for (int i = 0; i < 4; i++) {
    //    for (int j = 0; j < 4; j++) {
    //        if (i == 0) {
    //            matrix[i][j] = num.at(i + j);
    //        }
    //        if (i == 1) {
    //            matrix[i][j] = num.at(i + j + 3);
    //        }
    //        if (i == 2) {
    //            matrix[i][j] = num.at(i + j + 6);
    //        }
    //        if (i == 3) {
    //            matrix[i][j] = num.at(i + j + 9);
    //        }
    //    }
    //}
    //std::vector<double> ress;
    //for (int x = 0; x < 4; x++) {
    //    double temp = 0;
    //    for (int y = 0; y < 4; y++) {
    //        temp += matrix[x][y] * vect[y];

    //    }
    //    ress.emplace_back(temp);
    //}

    double values[16] = { num.at(0), num.at(1), num.at(2), num.at(3),
    num.at(4), num.at(5), num.at(6), num.at(7),
    num.at(8), num.at(9), num.at(10), num.at(11),
    num.at(12), num.at(13), num.at(14), num.at(15)
    };
    QGenericMatrix<4, 4, double> gmatrix(values); // initialize a 4x4 matrix - trans txt



    // get xyz from csv ===================================================

    // load csv
    std::vector<std::vector<std::string>> content;
    std::vector<std::string> row;
    std::string lineC, word;
    int total_cp = 0;

    std::fstream file(cpCsv.toStdString(), std::ios::in);
    if (file.is_open())
    {
        while (getline(file, lineC))
        {
            row.clear();

            std::stringstream str(lineC);

            while (getline(str, word, ','))
                row.push_back(word);
            content.push_back(row);
            total_cp++;
        }
    }
    file.close();
    //else
    //    std::vector<std::vector<std::string>> content1; // cannot open file - no such file
    std::vector<std::vector<double>> csvRes;
    for (int i = 1; i < content.size(); i++)
    {
        std::vector<double> tt;
        for (int j = 0; j < 3; j++)
        {
            tt.emplace_back(stod(content[i][j]));
            std::cout << content[i][j] << " _ ";
        }
        csvRes.emplace_back(tt);
        tt.clear();
        std::cout << "\n";
    }
    std::cout << "control points: "<<total_cp<<std::endl;




    // change coor for all xyz
    //std::vector<std::vector<double>> finalXyz;

    for (std::vector<double> a : csvRes) {
        double tempXyz[4];
        //for (double zz : a) {
        for(int i=0;i<3;i++){
            std::cout << "before : " << a.at(i) << " ";
            tempXyz[i] = a.at(i);

        }
        tempXyz[3] = 1.0;
        QGenericMatrix<1, 4, double> gvect(tempXyz);
        QGenericMatrix<1, 4, double> res = gmatrix * gvect;
        std::cout << "  " << std::endl;
        std::cout << " -----After-------  " << std::endl;
        std::cout << "x " << std::setprecision(10) << res(0, 0) << std::endl;
        std::cout << "y " << std::setprecision(10) << res(1, 0) << std::endl;
        std::cout << "z " << std::setprecision(10) << res(2, 0) << std::endl;
        std::cout << std::endl;
        std::vector<double> finalTempXyz;
        finalTempXyz.emplace_back(res(0, 0));
        finalTempXyz.emplace_back(res(1, 0));
        finalTempXyz.emplace_back(res(2, 0));
        finalXyz.emplace_back(finalTempXyz);
    }


    //double v1[4] = { 6.870499, -7.885124, -0.439918, 1.0 };
    //QGenericMatrix<1, 4, double> gvect(v1);
    //QGenericMatrix<1, 4, double> res = gmatrix * gvect;
    //std::cout << "  " << std::endl;
    //std::cout << " ------------  " << std::endl;
    //std::cout << "x " << std::setprecision(10) << res(0, 0) << std::endl;
    //std::cout << "y " << std::setprecision(10) << res(1, 0) << std::endl;
    //std::cout << "z " << std::setprecision(10) << res(2, 0) << std::endl;
    for (std::vector<double> a : finalXyz) {
        for (double b : a) {
            std::cout << " .....  " << b;
        }
        std::cout << std::endl;
    }

}
// ======================================================================================================================================





void widget_newProcess::setType(process_type t) {
    current_type = t;
}
process_type widget_newProcess::getType() {
    if (current_type == process_type::Slam) {
        //std::cout << "SLAM" << std::endl;
        return process_type::Slam;
    }
    else if (current_type == process_type::Colourisation) {
        //std::cout << "COLOR" << std::endl;
        return process_type::Colourisation;
    }
    else if (current_type == process_type::RTK) {
        //std::cout << "RTK" << std::endl;
        return process_type::RTK;
    }
    else if (current_type == process_type::GCP) {
        //std::cout << "GCP" << std::endl;
        return process_type::GCP;
    }

    return process_type::Slam; // just for 'warning'
}

void widget_newProcess::setProgressBar(int i) {
    progressBar->setValue(i);
}
void widget_newProcess::setLog(QString s) {
    viewProcess->listWidget->addItem(s);
}
void widget_newProcess::windowCanRunBag() {
    window_->availableForBagRunning = true;
    window_->checkAllNewProcess();
}
void widget_newProcess::setRtkFile(QString file) {
    current_rtkFile = file;
}
QString widget_newProcess::getBagPath() {
    //this->current_bag;
    QDir d = QFileInfo(this->current_bag).absoluteDir();
    QString absolute = d.absolutePath();
    return absolute;
}
QString widget_newProcess::getBag() { 
    return this->current_bag;
}


void widget_newProcess::test() {
    //gcp _gcp(this);
    //_gcp.get_gcp("E:\\200\\qq.ply", "E:\\200\\");
    //_gcp.add(12, 32, 1, "E:\\200\\qq.ply", "E:\\200\\");
}

void widget_newProcess::_closeProcess() {


    if (getStatus() == running_status::running) {
        QMessageBox::StandardButton check;
        check = QMessageBox::question(0, QString(tr("Confirmation")), QString(tr("Stop the process?")), QMessageBox::Yes | QMessageBox::No);
        if (check == QMessageBox::Yes) {

            if (current_type != process_type::Colourisation) {
                process.shutAll(1);
            }
            else {
                merge_mp4.closeBtnPressed();
                _colour.closeBtnPressed();
            }     
        }      
    }
    else if (getStatus() == running_status::waiting || getStatus() == running_status::finished) {

        // remove newProcess from list

        int size = window_->getNewProcessList().size();
        //std::cout <<"Project's name: " << name.toStdString() << std::endl;
        for (int i = 0; i < size; i++) {
            QString temp_name = window_->getNewProcessList().at(i)->getName();
            if (temp_name == this->name) {
                std::cout << "found at index of: " << i << std::endl;
                window_->removeNewProcess(this->name);
                //window_->getNewProcessList().erase(window_->getNewProcessList().begin()+i);
                //std::cout << "hi " << i << std::endl;
                //window_->update();
                return;
            }
        }
    }
        
        
    

}
