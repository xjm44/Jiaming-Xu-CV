#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <QtWidgets/QMainWindow>
#include "ui_passcodeSet.h"

class passcodeSet : public QMainWindow
{
    Q_OBJECT

public:
    passcodeSet(QWidget *parent = nullptr);
    ~passcodeSet();

    void setTime(long long l);
	// days & hrs & mins to s(time_t)
	long long convertTosec(int, int, int);





public slots:

    void _setTime(); // set user duration - for testing only
    void _copy(); // c to clipboard


private:
    Ui::passcodeSetClass ui;
};
