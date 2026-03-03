#include "spectrumreceiver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpectrumReceiver w;
    w.show();
    return a.exec();
}
