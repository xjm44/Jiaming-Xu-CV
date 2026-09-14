#pragma once
#include <QWidget>
#include <QProcess>

enum class process_type {
    Slam,
    Colourisation,
    RTK,
    GCP
};

class widget_newProcess;

// process bag
class processBag: public QWidget
{
	Q_OBJECT

public:
	processBag(widget_newProcess* parent);
	~processBag();
	void init();

	void startTheRun(QString bagPath); // start to process

    void shutAll(int); // end all processes; 0 = own, 1 = from parent signal

private Q_SLOTS:
    void _roscoreStarts();  // roscore
    void _checkYaml();      // rosparam
    void _bagcheck_output();// check
    void _baginfo_output(); // info
    void _startBag_output();// algorithm1
    void _startalgorithm2Output();
    void _getalgorithm2Frames();
    void gcp_merge_output();
    void _checkYamlGcpEE();
    void _startGcpee_output();
    void _gcpFindPoints();
    void _startRtk2nd_output();
    void _startbaOutput();

    void slotalgorithm1ExitCode(int a, QProcess::ExitStatus e);
    void slotalgorithm2ExitCode(int, QProcess::ExitStatus);
    void slotRtkExitCode(int, QProcess::ExitStatus);
    void slotMergeExitCode(int, QProcess::ExitStatus);
    void slotGcpcpExitCode(int, QProcess::ExitStatus);
    void slotGcpeeExitCode(int, QProcess::ExitStatus);
    void slotbaExitCode(int, QProcess::ExitStatus);

private:
    void _roscore_output(); // check roscore
    int paramIndex;         // check yaml
    void setYaml();         // set rosparam
    void setYaml_Rtk();
    void checkbagCheck();   // rosbag check
    void checkbagInfo();    // rosbag info
    void _startBag();       // finish checking
    int algorithm1Index;        // check algorithm1

    void algorithm1Done();      // done algorithm1
    void closeAll();        // remove play/record etc
    void allDone();
    //algorithm2
    void startalgorithm2();
    int algorithm2Index;
    void algorithm2Done();
    QString algorithm2Bag;
    int algorithm2_size;
    void startalgorithm2Run();
    int algorithm2Remaining;
    //gcp
    void _startGcpMerge();
    void _startGcp();
    QString gcpMergeBag;
    QString gcpPlyFolder;
    QString gcpPlyFolder2nd;
    bool gcpCp2ndRound;
    int gcpRemaining;
    void setYamlGcpEE();
    void runGcpEE();
    int gcpParamIndex;
    void startGcpCP(); // points classification
    void gcpFindPointsFinished();
    //rtk 
    void start2ndRtk();
    int rtkRemaining;
    int rtkIndex;
    // hba
    void startBa();
    int baRemaining;
    QString baBag;
    void baDone();

public:
    void _gcpEE(QString,QString,QString); // gcp ee
    
    

private:
	widget_newProcess* parent;
	QString bagPath;
	process_type type;
    int frame_size;

    double total_time;

    void whenCrashed();
    QString errInfo;

private:
    QString ros_setup ; // for ros (setup.bat)
    bool process_running;
    QProcess* processCore = NULL; // start roscore
    QProcess* processCheck = NULL; // info
    QProcess* processCheckC = NULL; // check
    QProcess* processStart = NULL; // algorithm1
    QProcess* processCl = NULL; // close
    QProcess* cmd_yaml = NULL; // .yaml
    QProcess* algorithm2 = NULL;
    QProcess* algorithm2Frame = NULL;
    QProcess* processM = NULL;
    QProcess* gcpEE_yaml = NULL;
    QProcess* gcpEE_run = NULL;
    QProcess* gcp_cp = NULL;
    QProcess* rtk_2nd = NULL;
    QProcess* ba = NULL;
};
