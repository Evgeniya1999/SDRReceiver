#include "spectrumreceiver.h"
#include "receivemanager.h"
#include "ui_spectrumreceiver.h"
#include <QtMath>
#include <QIntValidator>

SpectrumReceiver::SpectrumReceiver(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SpectrumReceiver)
{
    ui->setupUi(this);

    receive_manager = new ReceiveManager(this);

    connect(ui->ip_edit, &QLineEdit::textChanged, this, &SpectrumReceiver::ipWrite);
    connect(ui->port_edit, &QLineEdit::textChanged, this, &SpectrumReceiver::portWrite);
    ui->port_edit->setValidator(new QIntValidator(0, 65535, this));
    connect(ui->freq_edit, &QLineEdit::textChanged, this, &SpectrumReceiver::freqWrite);
    ui->freq_edit->setValidator(new QDoubleValidator(3000000, 30000000, 0, this));
    connect(ui->start_btn, &QPushButton::clicked, this, &SpectrumReceiver::connectToReceive);

    ui->ip_edit->setInputMask("000.000.000.000;_");
    ui->port_edit->setInputMask("0000;_");
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
    if (ui->start_btn->text() == "Старт") {
        ui->start_btn->setText("Стоп");

        if (receive_manager->connectToReceiver() != 0) {
            qDebug() << "Failed to connect to receiver";
            receive_manager->stopWork();
        } else {
            if (receive_manager->configReceiver() != 0) {
                qDebug() << "Failed to configure receiver";
                receive_manager->stopWork();
            } else {
                receive_manager->startWork();
            }
        }

    } else if (ui->start_btn->text() == "Стоп"){
        ui->start_btn->setText("Старт");

        receive_manager->stopWork();
        ui->ip_edit->clear();
        ui->port_edit->clear();
        ui->freq_edit->clear();
    }
}

SpectrumReceiver::~SpectrumReceiver()
{
    delete ui;
}


