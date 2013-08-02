#include "codewizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CodeWizard w;
    w.show();
    
    return a.exec();
}
