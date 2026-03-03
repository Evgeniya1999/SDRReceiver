#ifndef RECEIVEMANAGER_H
#define RECEIVEMANAGER_H

#include "qcontainerfwd.h"

#include "uhp_iq_stream.h"
#include "uhp_rx_eth.h"

#include "qobject.h"
#include <cstdint>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
class ReceiveManager : public QObject
{
    Q_OBJECT
public:    
    explicit ReceiveManager(QObject *parent = nullptr);
    ~ReceiveManager();

    enum SocketType {
        TCP_SOCKET,
        UDP_SOCKET
    };

    void setFrequency(QString freq);
    void setIp(QString ip);
    void setPort(QString port);
    uint32_t getFrequency();
    uint32_t getIp();
    uint16_t getPort();
    int initSocket(SOCKET &sock, SocketType type);
    ETH_RX_CTRL::set_log_destination packetSetPortCommand(uint32_t ip, uint16_t port);
    ETH_RX_CTRL::set_freq packetSetFreqCommand(ETH_RX_CTRL::header_req h, uint32_t f_hz);
    ETH_RX_CTRL::header_req headerReqWrite(uint32_t s, uint32_t m_id, uint16_t t);
    int connectToReceiver();
    int sendCommand(SOCKET s, const void* packet, int size);

private:
    QString m_freq;
    uint32_t m_freq_to_uint32;
    QString m_ip;
    uint32_t m_ip_to_uint32;
    QString m_port;
    uint16_t m_port_to_uint16;

    SOCKET m_tcpSock = INVALID_SOCKET;
    SOCKET m_udpSock = INVALID_SOCKET;

    ETH_RX_CTRL::set_log_destination m_setPortStruct;
    ETH_RX_CTRL::header_req  m_headerReq;
    ETH_RX_CTRL::set_freq m_setFreq;

    std::thread* m_workerThread = nullptr;
    std::atomic<bool> m_running;
};


#endif // RECEIVEMANAGER_H
