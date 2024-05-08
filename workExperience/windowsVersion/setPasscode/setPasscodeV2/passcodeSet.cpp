#include "passcodeSet.h"
#include <QMessageBox>
#include <QClipboard>
passcodeSet::passcodeSet(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);


    connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(_setTime()),Qt::UniqueConnection);
    connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(_copy()), Qt::UniqueConnection);
}

passcodeSet::~passcodeSet()
{}


void passcodeSet::_setTime() {

    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (!(re.exactMatch(ui.editdays->text()) && re.exactMatch(ui.edithours->text()) && re.exactMatch(ui.editminutes->text()))) {
        QMessageBox::information(NULL, "Error", "Only Integers");
    }
    else {

        //int days_to_sec = ((ui.editdays->text()).toInt() * 24);
        //int hours_to_sec = ((ui.edithours->text().toInt() + days_to_sec) * 60);
        //int mins_to_sec = (ui.editminutes->text().toInt() + hours_to_sec) * 60;

        // convert to s 1st
        time_t endTime = convertTosec(ui.editdays->text().toInt(), ui.edithours->text().toInt(), ui.editminutes->text().toInt());

        time_t currentTime = time(0);
        endTime += currentTime;


        ////1704869007 + 1000000999 = 2704870006
        ////1705872007 + 0010870888 = 2015872895
        std::string passcode = std::to_string(currentTime + 1000000999) + std::to_string(endTime + 10870888);
        std::string final_passcode;
        // using ascii
        for (char c : passcode) {
            int check = (int)c;
            int temp;
            if (check > 53) {
                temp = (c - 6);
            }
            else {
                temp = (c + 19);
            }

            final_passcode += (char)temp;
        }

        std::cout << "passcode: " << passcode << std::endl;
        std::cout << "final_passcode: " << final_passcode << std::endl;

        ui.lineEdit->setText(QString::fromStdString(final_passcode));

    }

    

}
void passcodeSet::_copy() {

    QClipboard* cb = QApplication::clipboard();

    cb->setText(ui.lineEdit->text());

    QString a = QString("Copied! '%1'").arg(ui.lineEdit->text());

    QMessageBox::information(NULL, "Info", a);

}

long long passcodeSet::convertTosec(int d, int h, int m) {

    int64_t days_to_sec = (d * 24);
    int64_t hours_to_sec = (h + days_to_sec) * 60;
    int64_t mins_to_sec = (m + hours_to_sec) * 60;
    long long total = days_to_sec + mins_to_sec + days_to_sec;
    std::cout << total << std::endl;
    std::cout << "--total--" << std::endl;
    std::vector<int> num;
    long long temp = total;
    while (temp)
    {
        int u = temp % 10;
        num.push_back(u);
        temp = temp / 10;
    }
   /* reverse(num.begin(), num.end());
    for (int a : num) {
        std::cout << a << std::endl;
    }*/
    int lastDigit = num[0];
    int toAdd = 0;
    if (lastDigit != 0) {
        toAdd = 10 - lastDigit;
        std::cout <<"to add: " << toAdd << std::endl;
    }
    total += toAdd;
    std::cout << total << std::endl;
    std::cout << "--new total--" << std::endl;
    return total;

}