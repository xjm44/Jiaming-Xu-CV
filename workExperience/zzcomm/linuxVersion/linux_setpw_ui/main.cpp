#include "company.hpp"
#include "windowLogin.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QFile>
#include "login.h"
#include <stdio.h>
#include <QMessageBox>


using namespace std;


int main(int argc, char** argv) {
    // cout << "You have entered " << argc << " arguments" << endl;
    // cout << "Max no. of input arguments are 1." << endl;
    // if(argc!=2){
    //     cout << "Please enter only one argument. Re-run the application" << endl;
    //     return -99;
    // }

    cout << "*********** Setting Pw ***************" << endl;

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);


    windowLogin l;
    l.show();

    return a.exec();

    
    // for (int i = 0; i < argc; ++i)
    //     cout << argv[i] << "\n";

    //std::string serial = argv[1];


    // company zz;

    // // Get current time and end time
    // time_t startTime{}, endTime{};
    // time(&startTime);
    // endTime = startTime + 3600 * 24 * 365 * 10; // 10 years
    // printf("startTime: %ld  || endTime: %ld \n", startTime, endTime);

    // // query serial number ( this used to display on the screen, in order to make sure the user can see it)
    // std::string serial = zz.querySerialNum();
    // std::cout << "serial: " << serial << std::endl;

    // // get registration code, this code is used to register the software for specific time period
    // // the registration code is a 64 bytes hex string, that you should offer to the user
    // std::string regCode = zz.getRegistrationCode(serial, startTime, endTime);
    // std::cout << "regCode: " << regCode << std::endl;


    // // try to register the software, if the registration code is valid, the software will be registered
    // time_t regStartTime{}, regEndTime{};
    // auto success = zz.tryRegister(regCode, regStartTime, regEndTime);
    // if(success)
    // {
    //     std::cout << "register success ..." << std::endl;
    //     printf("regStartTime: %ld  || regEndTime: %ld \n", regStartTime, regEndTime);
    // } else {
    //     std::cout << "register failed ..." << std::endl;
    // }

    return 0;
}