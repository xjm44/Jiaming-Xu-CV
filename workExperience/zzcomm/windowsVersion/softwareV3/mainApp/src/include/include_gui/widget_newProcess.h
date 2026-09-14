#pragma once

#include <QWidget>
#include "ui_widget_newProcess.h"

#include "widget_viewProcess.h"
#include "../include_process/processBag.h"
#include "../include_gui/mergeMp4.h"
#include "../include_gui/colourise.h"
#include "../include_gui/Yuan.h"
#include "dialog_gcp.h"
#include "dialog_gcp_report.h"

class mainApp;
class dialog_gcp;

enum class running_status {
    running,
    finished,
    waiting
};

// widget in the scroll area
class widget_newProcess : public QWidget,public Ui::widget_newProcessClass
{
	Q_OBJECT

public:

	widget_newProcess(QWidget *parent);
	~widget_newProcess();
	void init();
    void startWindowTimer();
    std::string getWindowTimer();

    void startWindowTimerP(); // for process
    std::vector<std::string> widget_newProcess::getWindowTimerP(); // for process
    void stopWindowTimerP(); // when cancel btn pressed

    void run(); // start the process
    void runColour(); // start the process - colourisation

    // colourisation - merge mp4(s)
    mergeMp4 merge_mp4;
    void set_colour_mp4(QStringList l); 
    void set_colour_pc(QString p) { colorFilePc = p; }
    colourise _colour;
    void doneColor();
    //QStringList getColourMp4() { return colorFileMp4; }
    //QString getColourPc() { return colorFilePc; }
    QString mp4FilePath;
    QString getColorErr() { return colorErr; }
    void setColorErr(QString a) { colorErr = a; }
    void colorCrash();

    // continue run remaining bag
    void windowCanRunBag();

    // for process the bag (one processBag - one newProcess)
    processBag process;

    // for view_process (set type name, for now)
	void setViewProcess();
    widget_viewProcess* getViewProcess() { return viewProcess; }

    // for in line-up
	//bool running; // waiting/running
    running_status getStatus() { return status; }
    void setStatus(running_status s) { status = s; }
    

    // type
    void setType(process_type);
    process_type getType();

    // show bag name
    void setBag(QString a);
    QString getBagPath();
    QString getBag();

    // no_of_cores,ba,csv,rotatingFrames,loopClosure,loopClosureIntervalFrames,narrowSpace,minIntensityFilter,lidarMaxRange,lidarMinRange,lidarMaxXToFilter,lidarMinXToFilter,translationThreshold,icpMaximumIterations,bagName,bagPath,algorithm2,sigma,laz,resolution,cuda,voxel,productParams
    QStringList getSlamParam();
    // set slam param
    void setSlamParam(
        int no_of_cores,
        bool ba,
        bool csv,
        bool rfo,
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
        float voxelSize,
        QString productParams,
        int algorithm2Frame,
        float algorithm2Uphold,
        bool loopInterior
    );

    // geoid, outputCoordinateSystem,gpsPositionOffset_x,gpsPositionOffset_y,gpsPositionOffset_z,spinBoxGpsFreq, hemi(QString), hemi(Bool)
    QStringList getRtkParam();
    // set rtk param
    void setRtkParam(
        bool geoid,
        QString outputCoordinateSystem,
        float doubleSpinBoxXRtk,
        float doubleSpinBoxYRtk,
        float doubleSpinBoxZRtk,
        int spinBoxGpsFreq,
        QString hemisphere,
        bool hemi
    );
    
    void setRtkFile(QString file);
    QString getRtkFile() { return current_rtkFile; }


    // projDir,imu,traj,cali,frameskip,firstframe,lastframe,visiblerange,voxel,uncolor,penetration
    void setColourParam(
        QString proj,
        QString imu,
        QString traj,
        QString cali,
        double frameskip,
        double firstframe,
        double lastframe,
        float visiblerange,
        float quality,
        bool uncolorPtss,
        QString penetrationn,
        QString maskk
    );

    // manualcheck, radmax, radmin, noofgcp, intensitymax, intensitymin, iteration, circlepts, filterdist, filterdistance, filterzcoor, filterzcoormax, filterzcoormin
    void setGcpParam(
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
    );


    // gcp manual
    dialog_gcp *dialogGcp;
    // move to 3d view
    void manualGcpShow(QString); 
    void manualGcpEnd();
    
    void _prepareGcpReportInputs(QString,QString);
    std::vector<std::string> splitString(const std::string& str);
    std::vector<std::vector<double>> finalXyz; // after cp's xyz is transformed 

    // for viewProcess
    void setProgressBar(int i);
    void setLog(QString);


    void test();

    // for gcp: csv files -> changed : get first realxyz only
    void saveCsv(QString gcpTxt); // (QString csvFile, std::vector<Yuan> o, 
    std::vector<std::vector<std::string>> loadCsv(QString csvFile);
    //std::vector<Yuan> runGcp(QString pc, QString pcpath, int no, QString savingPath, int gcpIndex);
    QString gcpTxtFile;
    void setViewGcp(QString, QString, QString);
    double rx_gcpee;
    double ry_gcpee;
    double rz_gcpee;

    // cancel btn
    void unableCancel() { btnCancel->setEnabled(false); }
    void enableCancel() { btnCancel->setEnabled(true); }

    // set unique name
    QString name;
    QString getName() { return name; }


    // for terminal
    void windowAddToCmd(QString);


    // hba to be deleted
    int baIter = 0;
    double baVoxel = 0;
    QString baSplit;

    // prevent spaces, spe chars, etc in arguments
    QString addQuote(std::string);

private:
	widget_viewProcess* viewProcess;
    running_status status;

private Q_SLOTS:
	void _viewProcess(); // view process progress
    void _proceedToColourisation(); // mp4 ready -> start to colourise point cloud
    void _proceedToColourisationMulti(); // mp4 ready -> start to colourise point cloud
    void _proceedToEE(); // gcp ee
    void _closeProcess(); // close btn
    void _proceedToGcpReport(); // accuracy report
private:
	mainApp* window_;

    process_type current_type;
    QString current_bag;
    QString current_rtkFile;

    QString colorErr;

private: // slam param
    int no_of_cores;
    bool loopClosure;
    int loopClosureIntervalFrames;
    bool narrowSpace;
    float minIntensityFilter;
    float lidarMaxRange;
    float lidarMinRange;
    float lidarMaxXToFilter;
    float lidarMinXToFilter;
    float translationThreshold;
    int icpMaximumIterations;
    bool csv;
    bool ba;
    bool rotatingFrames;
    QString bagName;
    QString bagPath;
    bool algorithm1;
    QString lossSigma;
    bool checkBoxtolaz;
    float doubleSpinBoxResolution;
    bool cuda;
    float voxelSize;
    QString productParams;
    int algorithm2Frame;
    float algorithm2Uphold;
    bool loopInterior;
    

    QStringList list_slamParam;

private: // rtk param
    bool geoid;
    QString outputCoordinateSystem;
    float doubleSpinBoxXRtk;
    float doubleSpinBoxYRtk;
    float doubleSpinBoxZRtk;
    int spinBoxGpsFreq;
    QString hemisphere;
    bool hemi;

    QStringList list_rtkParam;

private: // color param
    QStringList colorFileMp4;
    QString finalMp4;
    QString colorFilePc;
    //colorize.exe <project_dir> <point_cloud> <video> <imu_source> <traj_source> <calibration_file>[--config config_file][other options...]
    QString projDir;
    QString imuSrc;
    QString trajSrc;
    QString calibrationFile;
    double frameSkip;
    double firstFrame;
    double lastFrame;
    float visibleRange;
    float quality;
    bool uncolorPts;
    QString penetration;
    QString mask;
public: // for colourisation
    void setFinalMp4(QString a) { finalMp4 = a; }
    QString getprojDir() { return projDir; }
    QString getpointCloud() { return colorFilePc; }
    QString getVideo() { return finalMp4; }
    QString getImu() { return imuSrc; }
    QString getTraj() { return trajSrc; }
    void setCali(QString s) { calibrationFile = s; }
    QString getCali() { return calibrationFile; }
    QString getMask() { return mask; }
    QString getYaml() { return QString(""); } //  TO BE CHANGED    
    double getframeSkip() { return frameSkip; }
    double getfirstFrame() { return firstFrame; }
    double getlastFrame() { return lastFrame; }
    float getvisibleRange() { return visibleRange; }
    float getQuality() { return quality; }
    bool getUncolorPts() { return uncolorPts; }
    QString getPenetration() { return penetration; }
    QString config; // to be deleted, used for manual inputs now
private: // gcp param
    bool manualCheck;
    float radMax;
    float radMin;
    int no_of_gcp;
    int intensityMax;
    int intensityMin;
    int iteration;
    int circlePts;
    bool filterDist;
    int filterDistance;
    bool filterZCoor;
    float filterZCoorMax;
    float filterZCoorMin;
public: // gcp
    QString finalGcpEEcsv;
    bool getManualCheck() { return manualCheck; }
    float getRadMax() { return radMax; }
    float getRadMin() { return radMin; }
    int getNoOfGcp() { return no_of_gcp; }
    int getIntensityMax() { return intensityMax; }
    int getIntensityMin() { return intensityMin; }
    int getIteration() { return iteration; }
    int getCirclePts() { return circlePts; }
    bool getFilterDist() { return filterDist; }
    int getFilterDistance() { return filterDistance; }
    bool getFilterZCoor() { return filterZCoor; }
    float getFilterZCoorMax() { return filterZCoorMax; }
    float getFilterZCoorMin() { return filterZCoorMin; }



private Q_SLOTS:

    // manual gcp
    void _dialogGcpContinue();
    void _dialogGcpClose();
    void _dialogGcpxyz(double, double, double);
};