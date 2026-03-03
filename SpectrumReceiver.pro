QT       += core gui
QT += core gui widgets printsupport
QT += core gui widgets network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    iqbuffer.cpp \
    main.cpp \
    qcustomplot.cpp \
    receivemanager.cpp \
    signalconverter.cpp \
    spectrumreceiver.cpp

HEADERS += \
    iqbuffer.h \
    receivemanager.h \
    signalconverter.h \
    spectrumreceiver.h \
    qcustomplot.h \
    uhp_iq_stream.h \
    uhp_rx_eth.h

FORMS += \
    spectrumreceiver.ui

win32-g++ {
    QMAKE_CXXFLAGS += -Wa,-mbig-obj
}
win32 {
    LIBS = -lrpcrt4 -lws2_32 -lole32
}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
