#include "RdkQTcpClientWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RdkQTcpClientWidget w;
    w.show();
    return a.exec();
}
