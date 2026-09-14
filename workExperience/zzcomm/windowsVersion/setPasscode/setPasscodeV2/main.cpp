#include "passcodeSet.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    passcodeSet w;
    w.show();

    return a.exec();
}
