#include "../include/login.h"
#include <QMessageBox>

login::login()
{
   

}

bool login::set_time(int a, QString b) {

    //serialNum = "0578047603777206";  //set pw for customers

    serialNum = b.toStdString();

    
    time(&now);
    std::string str = z.getRegistrationCode(serialNum, now, now + int(a));

    passStr = QString::fromStdString(str);
    if (passStr.isNull()) { return false; }
    return true;
}


int login::dialogcheckcompany()
{
    
    return checkLicense();
    
}


int login::checkLicense() {
    
    if (passStr.size() == 16 && z.tryRegister(passStr.toStdString(), startTime, endTime))
    {

        time(&now);
        timePos = timeStr.toLongLong();

        if (now < timePos)
        {
            QMessageBox::about(NULL, "Error", "Failed!");
            return 3; 
        }
        else if (now > endTime)
        {
            QMessageBox::about(NULL, "Error", "Failed!");
            return 4;    
        }
        else
        {
            timeStr = QString::fromStdString(std::to_string(now));
            timePos = now;
            pauseTIME.run();

            time_t now4 = endTime - timePos;
            time_t now1 = now4 / 86400;
            time_t now2 = now4 % 86400;
            time_t now3 = now2 / 3600;
            time_t now5 = now2 % 3600;
            time_t now6 = now5 / 60;
            maintitleQstr = QString::fromStdString("Remaining Time: " + std::to_string(now1) + " Day(s) " + std::to_string(now3) + " Hour(s) " + std::to_string(now6) + " Minutes ");

            QMessageBox::about(NULL, "Error", "Done!");
            return 6;    
        }

    }
    else
    {
        QMessageBox::about(NULL, "Error", "Failed!");
        return 2;    
    }
}

login::~login()
{}