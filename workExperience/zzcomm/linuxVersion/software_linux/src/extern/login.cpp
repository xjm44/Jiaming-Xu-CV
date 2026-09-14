#include "../include/include_process/login.h"


login::login()
{
    // serialNum = z.serialHex; // each machine has unique serial no.
    serialNum = z.querySerialNum();
    connect(&pauseTIME, SIGNAL(time_val(QString)), this, SLOT(slot_updatatime(QString)));

}

bool login::set_time(int a) {

    time(&now);
    std::string str = z.getRegistrationCode(serialNum, now, now + a);
    passStr = QString::fromStdString(str);
    if (passStr.isNull()) { return false; }
    return true;
}


int login::checkIdentity() // if user has entered valid license previously
{

    QString x("./PocoDynamic"); // the file to store the license

    if (openDataAsStream(x))
    {
        return checkLicense();
    }
    else
    {
        return 1; // cant proceed
    }

}

bool login::openDataAsStream(QString& aFileName)
{ 
    QFile aFile(aFileName); 
    if (!(aFile.open(QIODevice::ReadOnly)))
        return false;

    QDataStream aStream(&aFile); 
    aStream.setVersion(QDataStream::Qt_5_9); 

    qint16  rowCount = 1; 
    qint16  colCount = 2; 

    aStream >> rowCount; 
    aStream >> colCount; 


    QString str;
    for (int i = 0; i < colCount; i++)
        aStream >> str;  


    for (int i = 0; i < rowCount; i++)
    {
        aStream >> passStr;
        passStr = QString::fromStdString(String73(passStr.toStdString()));
        aStream >> timeStr;
        timeStr = QString::fromStdString(String73(timeStr.toStdString()));

    }
    aFile.close();


    // std::cout<<"time: " << timeStr.toStdString() << std::endl;
    // std::cout<<"pass: " << passStr.toStdString() << std::endl;
    // std::cout<<"closed"<<std::endl;
    return true;
}
bool login::saveDataAsStream(QString& aFileName)
{
    QFile aFile(aFileName);  
    if (!(aFile.open(QIODevice::WriteOnly | QIODevice::Truncate)))
        return false;

    QDataStream aStream(&aFile);
    aStream.setVersion(QDataStream::Qt_5_9); 

    qint16  rowCount = 1; 
    qint16  colCount = 2; 

    aStream << rowCount; 
    aStream << colCount;


    for (int i = 0; i < 2; i++)
    {
        QString str;
        if (i == 0)
        {
            str = QString("Str1");  //QString  passStr;
        }
        else
        {
            str = QString("Str2"); //   QString  timeStr;
        }

        aStream << str; 
    }

    for (int i = 0; i < 1; i++)
    {

        std::string pass = passStr.toStdString(); //std::string pass2{"productPassStr"};
        std::string pass1 = String73(pass);
        QString passStr1 = QString(pass1.c_str());
        aStream << passStr1;

        std::string pass2 = timeStr.toStdString();//std::string pass2{"productTimeStr"}; //timeStr
        std::string pass3 = String73(pass2);
        QString timeStr1 = QString(pass3.c_str());
        aStream << timeStr1;
    }
    aFile.close();
    return true;
}

std::string login::String73(std::string str)
{

    for (std::size_t i = 0; i < str.size(); i++)
    {
        //printf("%02x  %02x\r\n", str[i], str[i] ^ 0xB3);
        str[i] ^= 0x21; // str[i] = str[i] ^ 0x21;   (a ^ b ) ^ b  = a;
    }
    return str;
}


void login::slot_updatatime(QString timeString)
{
    //std::cout << timeString.toStdString() << std::endl;
    time(&now);

    if (now < endTime && now>timePos)
    {

        timeStr = QString::fromStdString(std::to_string(now));
        timePos = now;
        QString x("./PocoDynamic");
        saveDataAsStream(x);

        time_t now4 = endTime - timePos;
        time_t now1 = now4 / 86400;
        time_t now2 = now4 % 86400;
        time_t now3 = now2 / 3600;
        time_t now5 = now2 % 3600;
        time_t now6 = now5 / 60;
        maintitleQstr = QString::fromStdString(" " + std::to_string(now1) + " Day(s) " + std::to_string(now3) + " Hour(s) " + std::to_string(now6) + " Minute(s) ");
        emit timer_signal(QString("S6"));

    }
    else if (now >= endTime && now > startTime) // expired
    {
        pauseTIME.end();
        timeStr = QString::fromStdString(std::to_string(now));
        timePos = now;
        QString x("./PocoDynamic");
        saveDataAsStream(x);
        emit timer_signal(QString("S4"));

    }
    else if (now < timePos) // invalid time
    {
        pauseTIME.end();
        emit timer_signal(QString("S3"));
    }
    else
    {
        timeStr = QString::fromStdString(std::to_string(now));
        timePos = now;
        QString x("./PocoDynamic");
        saveDataAsStream(x);
    }

}

int login::dialogcheckcompany()
{
    return checkLicense();
}


int login::checkLicense() {

    // if (passStr.size() == 64 )std::cout << "length passed" << std::endl;
    // std::cout<<passStr.toStdString()<<std::endl;
    // //passStr = "3637549335420D38000C45BE584930E709F4BEB22817422075463337914450D4";
    // if (z.tryRegister(passStr.toStdString(), startTime, endTime))std::cout << "reg passed" << std::endl;

    if (passStr.size() == 64 && z.tryRegister(passStr.toStdString(), startTime, endTime))
    //if (1)
    {
        
        time(&now);
        timePos = timeStr.toLongLong();

        if (now < timePos)
        {
            return 3;  
        }
        else if (now > endTime)
        {
            return 4;   
        }
        else
        {
            timeStr = QString::fromStdString(std::to_string(now));
            timePos = now;
            QString x("./PocoDynamic");
            saveDataAsStream(x);
            pauseTIME.run();

            time_t now4 = endTime - timePos;
            time_t now1 = now4 / 86400;
            time_t now2 = now4 % 86400;
            time_t now3 = now2 / 3600;
            time_t now5 = now2 % 3600;
            time_t now6 = now5 / 60;
            maintitleQstr = QString::fromStdString(" " + std::to_string(now1) + " Day(s) " + std::to_string(now3) + " Hour(s) " + std::to_string(now6) + " Minutes ");
            //std::cout << maintitleQstr.toStdString() << " __ " << std::endl;
            return 6;    
        }
    }
    else
    {
        std::cout <<"failed regis"<<std::endl;
        return 2;    
    }
}



login::~login()
{}