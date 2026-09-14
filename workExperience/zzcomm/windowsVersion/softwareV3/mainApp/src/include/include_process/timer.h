#ifndef TIMER_H
#define TIMER_H


#include <QObject>
#include <iostream>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <sstream>
#include <QFile>
#include <string>
#include <stdio.h>

// get the remaining time of the licence
class timer : public QThread
{
    Q_OBJECT
public:
    timer();
    ~timer();

    void run(); // update time 
    void end(); // time ended

Q_SIGNALS:
    void time_val(QString); // display time in a certain format

private slots:
    void updateTimeAndDisplay(); // display time in a certain format
private:
    QTimer* ptimer;    
    QTime baseTime;     
    QString showStr;    // to show to time duration
};


#endif // TIMER_H


