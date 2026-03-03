#ifndef SPECTRUMRECEIVER_H
#define SPECTRUMRECEIVER_H

#include <QMainWindow>
#include <QObject>

#include "qcustomplot.h"
#include "receivemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {

class SpectrumReceiver;
}
QT_END_NAMESPACE

class SpectrumReceiver : public QMainWindow
{
    Q_OBJECT

public:
    SpectrumReceiver(QWidget *parent = nullptr);
    ~SpectrumReceiver();
private slots:
    void ipWrite(const QString ip);
    void portWrite(const QString port);
    void freqWrite(const QString freq);
    void connectToReceive();

private:
    Ui::SpectrumReceiver *ui;
    ReceiveManager *receive_manager;
};
#endif // SPECTRUMRECEIVER_H
