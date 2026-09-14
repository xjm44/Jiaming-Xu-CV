#ifndef TIMER_H
#define TIMER_H


#include <QString>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QDebug>


class timer : public QThread
{
    Q_OBJECT
public:
    timer();
    ~timer();

    void run();
    void end();

Q_SIGNALS:
    void time_val(QString); 

private slots:
    void updateTimeAndDisplay();
private:
    QTimer* ptimer;     
    QTime baseTime;    
    QString showStr;   


#endif // TIMER_H


