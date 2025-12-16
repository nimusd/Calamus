#include "calamusmain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CalamusMain w;
    w.show();
    return a.exec();
}
