#include "../include/include_process/timer.h"


timer::timer()
{
    ptimer = new QTimer;
    connect(ptimer, SIGNAL(timeout()), this, SLOT(updateTimeAndDisplay()));
}

void timer::updateTimeAndDisplay()
{
    //std::cout << "hi" << std::endl;

    //QTime current = QTime::currentTime();          
    //int t = baseTime.msecsTo(current);      
    //QTime showTime(0, 0, 0, 0);                      
    //showTime = showTime.addMSecs(t);              
    //showStr = showTime.toString("mm:ss:zzz");   //showStr = showTime.toString("hh:mm:ss:zzz");
    showStr = "";
    emit time_val(showStr); 
}

void timer::run()
{
    baseTime = QTime::currentTime();          
    //ptimer->start(600);                     
    ptimer->start(10000);

}
void timer::end()
{
    ptimer->stop();
}
timer::~timer()
{
}
