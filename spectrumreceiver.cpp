#include "spectrumreceiver.h"
#include "receivemanager.h"
#include "ui_spectrumreceiver.h"
#include <QtMath>

SpectrumReceiver::SpectrumReceiver(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SpectrumReceiver)
{
    ui->setupUi(this);

    receive_manager = new ReceiveManager(this);

    connect(ui->ip_edit, &QLineEdit::textChanged, this, &SpectrumReceiver::ipWrite);
    connect(ui->port_edit, &QLineEdit::textChanged, this, &SpectrumReceiver::portWrite);
    connect(ui->freq_edit, &QLineEdit::textChanged, this, &SpectrumReceiver::freqWrite);
    connect(ui->start_btn, &QPushButton::clicked, this, &SpectrumReceiver::connectToReceive);


    ui->widget->addGraph();
    QVector<double> x(101), y(101);
    for (int i=0; i<101; ++i) {
        x[i] = i;
        y[i] = qSin(i/10.0);
    }
    ui->widget->graph(0)->setData(x, y);
    ui->widget->rescaleAxes();
    ui->widget->replot();
}

void SpectrumReceiver::ipWrite(const QString ip){
    receive_manager->setIp(ip);
}
void SpectrumReceiver::portWrite(const QString port){
    receive_manager->setPort(port);
}
void SpectrumReceiver::freqWrite(const QString freq){
    receive_manager->setFrequency(freq);
}
void SpectrumReceiver::connectToReceive(){

}

SpectrumReceiver::~SpectrumReceiver()
{
    delete ui;
}


