#include "../include/timer.h"

timer::timer()
{
    ptimer = new QTimer;
    connect(ptimer, SIGNAL(timeout()), this, SLOT(updateTimeAndDisplay()));
}

void timer::updateTimeAndDisplay()
{
    QTime current = QTime::currentTime();         
    int t = baseTime.msecsTo(current);       
    QTime showTime(0, 0, 0, 0);                     
    showTime = showTime.addMSecs(t);               
    showStr = showTime.toString("mm:ss:zzz");     //showStr = showTime.toString("hh:mm:ss:zzz");
    emit time_val(showStr);
}

void timer::run()
{
    baseTime = QTime::currentTime();       
    ptimer->start(60000);                      
    //ptimer->start(30000);
}
void timer::end()
{
    ptimer->stop();
}
timer::~timer()
{
    qDebug() << "~timer()";
}


