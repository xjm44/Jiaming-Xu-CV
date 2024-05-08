#include "../include/login.h"
#include <QMessageBox>

login::login()
{
    connect(&pauseTIME, SIGNAL(time_val(QString)), this, SLOT(slot_updatatime(QString)));

}

bool login::set_time(int a) {

    //serialNum = "70007506027B0477";  //set pw for customers


    time(&now);
    std::string str = z.getRegistrationCode(serialNum, now, now + a);
    passStr = QString::fromStdString(str);
    if (passStr.isNull()) { return false; }
    return true;
}

void login::company100()
{
    time(&now);
    std::string str = z.getRegistrationCode(serialNum, now, now + 2592000);
    passStr = QString::fromStdString(str);
}
void login::company10()
{ 
    time(&now);
    //std::string str = z.getRegistrationCode(serialNum, now, now + 60);
    std::string str = z.getRegistrationCode(serialNum, now, now + 5);
    passStr = QString::fromStdString(str);
}

int login::checkIdentity() // if user has entered valid license previously
{
    serialNum = z.serialHex; // each machine has unique serial no.
    QString x("PocoDynamic.dll"); // the file to store the license
    
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



    //QString  passStr;
    //QString  timeStr;

    for (int i = 0; i < rowCount; i++)
    {
        aStream >> passStr;
        //qDebug() << passStr;
       // passStr = QString(String73(passStr.toStdString()).c_str());
        passStr = QString::fromStdString(String73(passStr.toStdString()));

        aStream >> timeStr;
        timeStr = QString::fromStdString(String73(timeStr.toStdString()));

        //qDebug() << passStr;
       // qDebug() << timeStr;
    }
    aFile.close();
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
            str = QString("Str2");//   QString  timeStr;
        }

        aStream << str; 
    }



    for (int i = 0; i < 1; i++)
    {
        // aItem = theModel->item(i, 4); //
        std::string pass = passStr.toStdString(); //std::string pass2{"productPassStr"};
        std::string pass1 = String73(pass);
        QString passStr1 = QString(pass1.c_str());
        aStream << passStr1;
        //qDebug() << "productPassStr";

        std::string pass2 = timeStr.toStdString();//std::string pass2{"productTimeStr"}; //timeStr
        std::string pass3 = String73(pass2);
        QString timeStr1 = QString(pass3.c_str());
        aStream << timeStr1;
        ///qDebug() << "productTimeStr";
       // aItem = theModel->item(i, 5); 
       // bool quYang = (aItem->checkState() == Qt::Checked);
       // aStream << quYang;
    }
    aFile.close();
    return true;
}

std::string login::String73(std::string str)
{

    for (int i = 0; i < str.size(); i++)
    {
        //printf("%02x  %02x\r\n", str[i], str[i] ^ 0xB3);
        str[i] ^= 0x21; // str[i] = str[i] ^ 0x21;  (a ^ b ) ^ b  = a;
    }
    //char a = 's';
   // std::cout  <<  str << "!!! " << str.size() << std::endl;
    return str;
}


void login::slot_updatatime(QString timeString)
{
    
    time(&now);
    /*QMessageBox::information(NULL, "now", QString::fromStdString(std::to_string(now)));
    QMessageBox::information(NULL, "endtime", QString::fromStdString(std::to_string(endTime)));
    QMessageBox::information(NULL, "timepos", QString::fromStdString(std::to_string(timePos)));*/
    if (now < endTime && now>timePos)
    {

        timeStr = QString::fromStdString(std::to_string(now));
        timePos = now;
        QString x("PocoDynamic.dll");
        saveDataAsStream(x);

        time_t now4 = endTime - timePos;
        time_t now1 = now4 / 86400;
        time_t now2 = now4 % 86400;
        time_t now3 = now2 / 3600;
        time_t now5 = now2 % 3600;
        time_t now6 = now5 / 60;
        maintitleQstr = QString::fromStdString("Remaining Time: " + std::to_string(now1) + " Day(s) " + std::to_string(now3) + " Hour(s) " + std::to_string(now6) + " Minutes ");
        emit timer_signal(QString("S6"));

    }
    else if (now >= endTime && suanfaBOOL == false && now > startTime) // expired
    {
        //QMessageBox::information(NULL, "a", "gg2");
        pauseTIME.end();
        timeStr = QString::fromStdString(std::to_string(now));
        timePos = now;
        QString x("PocoDynamic.dll");
        saveDataAsStream(x);
        emit timer_signal(QString("S4"));
        outtime = true;

    }
    else if (now < timePos) // invalid time
    {
        //QMessageBox::information(NULL, "a", "gg3");
        pauseTIME.end();
        emit timer_signal(QString("S3"));
    }
    else
    {
        /*QMessageBox::information(NULL, "a", "gg4");*/
        /*QMessageBox::information(NULL, "now", QString::fromStdString(std::to_string(now)));
        QMessageBox::information(NULL, "endtime", QString::fromStdString(std::to_string(endTime)));
        QMessageBox::information(NULL, "timepos", QString::fromStdString(std::to_string(timePos)));*/
        outtime = true;
        timeStr = QString::fromStdString(std::to_string(now));
        timePos = now;
        QString x("PocoDynamic.dll");
        saveDataAsStream(x);
    }

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
        /*std::cout << "getstartTime:" << startTime << std::endl;
        std::cout << "getendTime:" << endTime << std::endl;
        std::cout << "getnow:" << now << std::endl;*/
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
            QString x("PocoDynamic.dll");
            saveDataAsStream(x);
            pauseTIME.run();

            time_t now4 = endTime - timePos;
            time_t now1 = now4 / 86400;
            time_t now2 = now4 % 86400;
            time_t now3 = now2 / 3600;
            time_t now5 = now2 % 3600;
            time_t now6 = now5 / 60;
            maintitleQstr = QString::fromStdString("Remaining Time: " + std::to_string(now1) + " Day(s) " + std::to_string(now3) + " Hour(s) " + std::to_string(now6) + " Minutes ");

            return 6;     
        }

    }
    else
    {
        // std::cout << "getstartTime111:" << startTime << std::endl;
        // std::cout << "getendTime111:" << endTime << std::endl;
       //  iniDialogUI();
        // milable1->setText("gg");
        return 2;     
    }
}


int login::abortFile() {
    QMessageBox::StandardButton check;
    check = QMessageBox::information(NULL, "The process is running", "Please wait for the current file to be processed.", QMessageBox::Ok );
    return 1;
}


login::~login()
{}