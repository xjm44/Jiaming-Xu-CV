#ifndef LOGIN_H
#define LOGIN_H


#include "../timer.h"
#include "../../control_authentity/company.h"
#include <QDatastream>
#include <QMessageBox>

// for checking licence
class login :public QObject
{
    Q_OBJECT

public:

    login();
    ~login();

    timer pauseTIME;
    std::string serialNum; //username 
    int checkIdentity(); // check if license has been entered already
    int dialogcheckcompany(); // check if the license is valid at the login page


    bool saveDataAsStream(QString& aFileName);
    bool openDataAsStream(QString& aFileName);
    std::string String73(std::string); // encode + decode
    QString  passStr;  // passcode
    QString  timeStr; // check time duration
    QString  maintitleQstr; // to show time of remaining
    time_t startTime{};
    time_t endTime{};
    time_t now; // current time
    time_t timePos; // time position (between time duration)
    company z;

    int checkLicense(); // for loggin page

    bool set_time(int a); // for setting user duration - testing only

public Q_SLOTS:
    void slot_updatatime(QString); // get time duration
    

Q_SIGNALS:
    void timer_signal(QString); // give out time duration

};
#endif // LOGIN_H
