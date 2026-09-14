#ifndef LOGIN_H
#define LOGIN_H


#include <iostream>
#include <QString>
#include <QTime>
#include "timer.h"
#include <string>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QThread>
#include "../control/company.h"
#include "timer.h"
#include <QtDebug>
#include <QDataStream>
#include <sstream>
#include <QFile>
#include <string>
#include <stdio.h>


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

    // fake license============
    void company100();
    void company10();
    //==========================

    bool saveDataAsStream(QString &aFileName);//�����ݱ���Ϊ�������ļ�
    bool openDataAsStream(QString &aFileName);//��ȡ�������ļ�
    std::string String73(std::string); // encode + decode
    QString  passStr;  // (password)
    QString  timeStr;
    QString  maintitleQstr; // time
    bool passBool;
    time_t startTime{};
    time_t endTime{};
    time_t now;
    time_t timePos;
    bool outtime;
    double StartTime;
    double EndTime;
    bool suanfaStart;
    company z;
    bool suanfaBOOL;

    int checkLicense(); // for loggin page

    int abortFile(); // for preventing file insertion when process is running

    bool set_time(int a); // for setting user duration - testing only

public Q_SLOTS:
	void slot_updatatime(QString);

    

Q_SIGNALS:
	void timer_signal(QString); //�źŴ�ֵ

};
#endif // LOGIN_H
