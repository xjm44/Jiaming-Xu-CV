#include "../include/include_process/loginOne.h"
#include <QSettings>
#include <QStandardPaths>
#include <qdir.h>
#include <QEventLoop>

bool is_digits(const std::string& str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

loginOne::loginOne()
    :end(0)
    , first_time(false)
{

    // pre-preparation: set key, enc file , etc
    QString output = "..\\Lib\\company\\setting.ini";
    QSettings setting(output, QSettings::IniFormat);    
    //setting.setValue("kp", 100110110);
    setKey(setting.value("kp").toInt() - 100110000 );
    std::cout << "key: " << key << std::endl;

    
    checkEncFile();

    connect(&timeChecker, SIGNAL(time_val(QString)), this, SLOT(slot_updatatime(QString)));

}
loginOne::~loginOne()
{
}
void loginOne::checkEncFile() {

    QString dir_path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
    QString eFile = dir_path + "\\companyE.txt";

    QFile file(eFile);
    if (file.exists()) {
        std::cout << "eFile found " << std::endl;
    }
    else {
        // copy from ..//Lib//company
        // this is because the endTime in ..//Lib//company is definitely < curTime
        bool check = QFile::copy("..\\Lib\\company\\companyE.txt", eFile);
        QEventLoop loop1;
        QTimer::singleShot(1000, &loop1, SLOT(quit()));
        loop1.exec();
        if (check) {
            std::cout << "copied "<< std::endl;
        }
        else {
            std::cout << "fail to copy "<< std::endl;
        }
    }



    //setEncFile("..\\Lib\\company\\companyE.txt");
    setEncFile(eFile.toStdString());
}

void loginOne::slot_updatatime(QString qs) {
    //std::cout << "qs: " << qs.toStdString() << std::endl;
    
    time_t cur_time = time(0);
    //std::cout << "cur: " << cur_time << std::endl;
    //std::cout << "end: " << end << std::endl;

    if (cur_time >= end) {

        //expired
        timeChecker.end();
        emit timer_signal(3); // need to show on the login page
    }
    else {
        // update startTime in the encFile
        encrypt(encFile, cur_time, end);
        // return remaining time 
        remainingTime = getElapsedTime(end, cur_time);
        emit timer_signal(6);
    }
}

int loginOne::checkAuthentity() {

    // check enc file startTime and endTime
    if (decrypt()) {
        if (getCode()) {

            std::pair<std::vector<std::string>, std::vector<std::string>> timeList = getTimeList();
            long long firstTime = std::stoll(timeList.first.at(0));
            long long secondTime = std::stoll(timeList.second.at(0));
            setEndTime(secondTime); // end time is 0 at first
            time_t cur_time = time(0);
            if (cur_time >= secondTime) {
                //expired
                return 3;
            }
            else {
                // update startTime in the encFile
                encrypt(encFile, cur_time, secondTime);
                // return remaining time 
                timeChecker.run();
                remainingTime = getElapsedTime(secondTime, cur_time);            
                return 6;
            }

        }
        else {
            return 4;
        }
    }
    else {
        return 4;
    }
}

int loginOne::checkAuthentity(QString a, bool b) {
    first_time = b;

    // if companyE.txt not exist
    QFile file(QString::fromStdString(encFile));
    if (file.exists()) {
        std::cout << "companyE file exists" << std::endl;

        // check for pw's validity
        // OR
        // update enc file
        int res = checkPasscode(a);
        return res;
    }
    else {
        std::cout << "companyE file does not exist" << std::endl;
        // create a new enc file
        // OR
        // raise error
        //return 1;
    }
    return 1;
}

int loginOne::checkPasscode(QString a) {

    // check if passcode is used before 
    if (checkPwStorage(a)) { // will return true if matched - used before
        std::cout << "code used before" << std::endl;
        return 2;
    }
    // ===============================

    // cannot use the diff between code's start/end time with the time in the enc file
    /*
    For example,
    1st: 01.06 - 01.16
    set 01.08 - 01.10
    2nd 01.09 - 01.12
    hence, the starting time is keep updating
    */
    
    // simply check the endTime - startTime > 0 in the code.
    // 1 - passcode = startTime + endTime
    //1704869007 + 1000000999 = 2704870006
    //1705872007 + 0010870888 = 2015872895
    // check for pw validity
    switch (updateEndTime(a.toStdString()))
    {
    case 201:

        //if (decrypt()) {
        //    if (getCode()) {
        //        std::pair<std::vector<std::string>, std::vector<std::string>> timeList = getTimeList();
        //        long long firstTime = std::stoll(timeList.first.at(0));
        //        long long secondTime = std::stoll(timeList.second.at(0));

        //        std::cout << "" << std::endl;
        //        std::cout << "At first: " << std::endl;
        //        std::cout << "starting time: " << getCurrentTime() << std::endl;
        //        std::cout << "end time: " << getEndTime() << std::endl;
        //        std::cout << "elapsed time: " << getElapsedTime() << std::endl;
        //        std::cout << "" << std::endl;

        //    }
        //}

        // store the pw
        updatePwStorage(a);
        timeChecker.run();
        return 6;
    case 401:
        std::cout << "passcode is invalid " << std::endl;
        return 2;
    case 400:
        std::cout << "passcode is invalid " << std::endl;
        return 2;
    default:
        break;
    }


    return 2;
}

bool loginOne::decrypt()
{
    timeListInChar.first.clear();
    timeListInChar.second.clear();

    std::fstream fin;
    //std::fstream fout;
    fin.open(encFile, std::fstream::in);
    //fout.open(decFile, std::fstream::out | std::fstream::trunc);

    char c;
    int count = 0;

    while (fin >> std::noskipws >> c) {
        // Remove the key from the character
        int temp = (c - key);

        if (count == 0) {
            if ((char)temp == NULL) {
                return false;
            }
            //get first line of code only
            if ((char)temp != '\n') {
                timeListInChar.first.emplace_back((char)temp);
                //std::cout << "cc " << std::to_string((char)temp) << std::endl;
            }

        }
        else if (count == 1) {
            if ((char)temp == NULL) {
                return false;
            }
            //get 2nd line of code only
            if ((char)temp != '\n') {
                timeListInChar.second.emplace_back((char)temp);
            }
        }

        if ((char)temp == '\n') {
            count++;
            //std::cout << "next line " << count << std::endl;
        }

        //fout << (char)temp;
    }

    fin.close();
    //fout.close();


    std::cout << "decrypt first size: " << timeListInChar.first.size() << std::endl;
    std::cout << "decrypt second size: " << timeListInChar.first.size() << std::endl;
    return true;
}


bool loginOne::getCode() {

    if (timeListInChar.first.size() == 0 || timeListInChar.second.size() == 0) {
        return false;
    }

    timeListInStr.first.clear();
    timeListInStr.second.clear();

    std::string str1, str2;

    for (char a : timeListInChar.first) {
        str1 += a;
        //std::cout << "a " << a << std::endl;
    }

    //std::cout <<"str1 " << (str1) << std::endl;
    for (char a : timeListInChar.second) {
        str2 += a;
        //std::cout << "a " << a << std::endl;
    }
    //std::cout << "str2 " << (str2) << std::endl;

    //// string to number
    //std::stringstream ss(str);
    //long long number;
    //ss >> number;
    //std::cout << "number " << number << std::endl;


    timeListInStr.first.emplace_back(str1);
    timeListInStr.second.emplace_back(str2);
    std::cout << "getCode first size: " << timeListInStr.first.size() << std::endl;
    std::cout << "getCode second size: " << timeListInStr.first.size() << std::endl;

    return true;

}

char* loginOne::getReadableTime(time_t cur) {

    char* time_str = ctime(&cur);
    time_str[strlen(time_str) - 1] = '\0';
    //printf("Current Time : %s\n", time_str);
    return time_str;
}

std::string loginOne::getElapsedTime(time_t end, time_t current) {

    time_t now4 = end - current;
    time_t now1 = now4 / 86400;
    time_t now2 = now4 % 86400;
    time_t now3 = now2 / 3600;
    time_t now5 = now2 % 3600;
    time_t now6 = now5 / 60;
    std::string remainingTime = (" " + std::to_string(now1) + " Day(s) " + std::to_string(now3) + " Hour(s) " + std::to_string(now6) + " Minute(s) ");

    return remainingTime;

}
int loginOne::updateEndTime(std::string pw) {
    /*
    get diff btw the passcode
    update if valid
    */

    // splite pw first - get start and end time
    splitPw(pw);
    if ((!is_digits(pwStartTime)) || (!is_digits(pwEndTime))) {
        std::cout << "not valid number after decrypted "<< std::endl;
        return 400;
    }

    //1704869007 + 1000000999 = 2704870006
    //1705872007 + 0310000888 = 2015872895
    pwStart = std::stoll(pwStartTime) - 1000000999;
    pwEnd = std::stoll(pwEndTime) - 10870888; // 0010870888
    std::cout << "passcode: " << pw << std::endl;
    std::cout << "s1 in ll " << pwStart << std::endl;
    std::cout << "s2 in ll " << pwEnd << std::endl;

    // compare start and end time with original (encrypt) info.
    //if (pwStart > current && pwEnd > end) {
    // compare startTime < endTime in passcode
    if (pwStart < pwEnd) {

        //get difference in the passcode
        long long toBeAdded = pwEnd - pwStart;
        std::cout << "time to be added " << toBeAdded << std::endl;

        if (toBeAdded < 0 || toBeAdded%10 != 0) {
            std::cout << "cant %10 " << std::endl;
            return 400;
        }

        //std::cout << "ori-end " << end << std::endl;
        // edit end time in the original encrypt txt file

        if (first_time){
            end = time(0); // if at login page, means expired
            first_time = false;    
        }

        time_t finalEndTime = end + toBeAdded;
        setEndTime(finalEndTime);
        time_t cur_time = time(0);
        // update startTime in the encFile
        encrypt(encFile, cur_time, finalEndTime);


        return 201;
    }
    else {
        return 401;
    }


}

void loginOne::splitPw(std::string pw) {
    std::string final_passcode1;
    for (char c : pw) {
        int check = (int)c;
        int temp;
        if (check < 53) {
            temp = (c + 6);
        }
        else {
            temp = (c - 19);
        }

        final_passcode1 += (char)temp;
    }

    pwStartTime = final_passcode1.substr(0, 10);
    pwEndTime = final_passcode1.substr(10, 10);
    //std::cout << "s1 " << pwStartTime << std::endl;
    //std::cout << "s2 " << pwEndTime << std::endl;


}
void loginOne::encrypt(std::string in , time_t start , time_t end) {

    // Input stream
    std::fstream f;
    f.open("temp.txt", std::fstream::out | std::fstream::trunc);
    f << start << "\n";
    f << end;
    f.close();

    // Input stream
    std::fstream fin, fout;
    char c;

    fin.open("temp.txt", std::fstream::in); // ********* TO BE DELETED
    fout.open(in, std::fstream::out | std::fstream::trunc);
    // Reading original file till end of file
    while (fin >> std::noskipws >> c) {
        int temp = (c + key);

        // Write temp as char in output file
        fout << (char)temp;
    }

    // Closing both files
    fin.close();
    fout.close();


    if (std::remove("temp.txt") == 0) {
        //std::cout << "residue T deleted" << std::endl;
    }
    else {
        //std::cout << "residue T not deleted" << std::endl;
    }


}

void loginOne::decrypt(std::string in, std::string out) {

    std::fstream fin;
    std::fstream fout;
    fin.open(in, std::fstream::in);
    fout.open(out, std::fstream::out | std::fstream::trunc);

    char c;
    int count = 0;

    while (fin >> std::noskipws >> c) {
        // Remove the key from the character
        int temp = (c - key);
        fout << (char)temp;
    }

    fin.close();
    fout.close();
}
void loginOne::encrypt()
{

    //if (key == NULL || timeFile.empty() || encFile.empty()) {
    //    std::cout << "no key / txtfile " << std::endl;
    //    return;
    //}


    //// Input stream
    //std::fstream fin, fout;
    //char c;

    //fin.open(timeFile, std::fstream::in);
    //fout.open(encFile, std::fstream::out | std::fstream::trunc);
    //// Reading original file till end of file
    //while (fin >> std::noskipws >> c) {
    //    int temp = (c + key);

    //    // Write temp as char in output file
    //    fout << (char)temp;
    //}

    //// Closing both files
    //fin.close();
    //fout.close();

}
bool loginOne::checkPwStorage(QString a) {
    //QFile fq("..\\Lib\\company\\companyEs.txt");
    //if (!fq.exists())return true;

    QString dir_path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
    //if (QDir(dir_path).exists()) {
    std::cout <<"check: " << dir_path.toStdString() << std::endl;
    std::string esFile = dir_path.toStdString() + "\\companyEs.txt";

    //}
    // decrypt first
    decrypt(esFile, "tempO.txt");
    std::fstream fout;
    fout.open("tempO.txt", std::fstream::in); // read only

    std::string word = a.toStdString();
    std::string line;
    bool check = false;
    while (std::getline(fout, line))
    {
        size_t pos = line.find(word);
        if (pos != std::string::npos) {
            check = true;
            break;
        }         
    }
    fout.close();

    if (std::remove("tempO.txt") == 0) {
        std::cout << "residue O (c) deleted" << std::endl;
    }
    else {
        std::cout << "residue O (c) not deleted" << std::endl;
    }

    return check;
}

void loginOne::updatePwStorage(QString a) {
    QString dir_path = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0);
    std::string esFile = dir_path.toStdString() + "\\companyEs.txt";

    // decrypt first
    decrypt(esFile, "tempO.txt");
    // add new pw
    std::fstream ft;
    ft.open("tempO.txt", std::fstream::app);
    std::string s = a.toStdString()+"\n"; // \n add to the right because the Es file is empty at first
    ft << s;
    ft.close();
    //encrypt back
    std::fstream fin, fout;
    char c;
    fin.open("tempO.txt", std::fstream::in);
    fout.open(esFile, std::fstream::out | std::fstream::trunc);
    // Reading original file till end of file
    while (fin >> std::noskipws >> c) {
        int temp = (c + key);

        // Write temp as char in output file
        fout << (char)temp;
    }

    // Closing both files
    fin.close();
    fout.close();


    if (std::remove("tempO.txt") == 0) {
        std::cout << "residue O (u) deleted" << std::endl;
    }
    else {
        std::cout << "residue O (u) not deleted" << std::endl;
    }
   
}