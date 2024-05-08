#ifndef LOGINONE_H
#define LOGINONE_H


#include "timer.h"
//#include <QDatastream>
#include <QMessageBox>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <chrono>

// for checking licence
class loginOne :public QObject
{
    Q_OBJECT

public:

    loginOne();
    ~loginOne();


	timer timeChecker;
	// for keep updating startTime - and checking whether expired
	int checkAuthentity();
	// for login page - or updating 
	int checkAuthentity(QString,bool); // 1st = pw, 2nd = from login page OR mainwindow
	int checkPasscode(QString);

	void setKey(int a) { key = a; }

	//void setTimeFile(std::string a) { timeFile = a; }
	void setEncFile(std::string a) { encFile = a; }
	void checkEncFile();

	// for time.txt and encrypt.txt
	void encrypt();
	// to get time information from encrypt.txt and return pair
	bool decrypt();
	// encrypt latest encrypt.txt : residue = temp.txt
	void encrypt(std::string,time_t,time_t);
	// read an encrypted file
	void decrypt(std::string, std::string);

	// get time information from encrypt file - use getTimeList() to return out the str pairs
	bool getCode();
	std::pair<std::vector<std::string>, std::vector<std::string>> getTimeList() { return timeListInStr; }

	std::string remainingTime;

private:
	// for companyE and companyEs files
	int key;
	//std::string timeFile;
	std::string encFile;

	// to store/get start and end time
	std::pair<std::vector<char>, std::vector<char>> timeListInChar;
	std::pair<std::vector<std::string>, std::vector<std::string>> timeListInStr;

	// for passcode
	void splitPw(std::string);
	std::string pwStartTime;
	std::string pwEndTime;

public:
	//// get start time
	//time_t getCurrentTime() { return current; }
	//// set start time
	//void setCurrentTime(time_t t) { current = t; }
	// get the time in nice format
	char* getReadableTime(time_t);
	// set end time
	void setEndTime(time_t t) { end = t; }
	// get end time
	time_t getEndTime() { return end; }
	// get difference btw start and end time : currently in days hrs minutes format
	std::string getElapsedTime(time_t,time_t);
	// update end time (need set start and end time beforehand)
	int updateEndTime(std::string);

	// check if pw is used before
	bool checkPwStorage(QString);
	void updatePwStorage(QString);


private:
	//time_t current;
	time_t end;
	//time_t elapsed;
	time_t pwStart;
	time_t pwEnd;

	// barrier for login page OR mainwindow
	bool first_time;


private Q_SLOTS:
    void slot_updatatime(QString); // get time duration


Q_SIGNALS:

    void timer_signal(int); // give out time duration

};
#endif // LOGINONE_H
