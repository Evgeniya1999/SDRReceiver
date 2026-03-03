#include "receivemanager.h"
#include "uhp_iq_stream.h"
#include "uhp_rx_eth.h"

#include <QString>
#include <QHostAddress>
#include <QDebug>

#include <thread>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>


ReceiveManager::ReceiveManager(QObject *parent) : QObject(parent) {

}
void ReceiveManager::runThread(){
    if (initSocket(m_tcpSock, TCP_SOCKET) != 0) return;

    struct sockaddr_in dest_addr {};
    dest_addr.sin_addr.s_addr = htonl(getIp());
    dest_addr.sin_port = htons(getPort());
    dest_addr.sin_family = AF_INET;
    int connect_flag = ::connect(m_tcpSock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (connect_flag == SOCKET_ERROR) {
        int err = WSAGetLastError();
        qDebug() << "Connect failed with error:" << err;
        return;
    }
    if (connectToReceiver() == SOCKET_ERROR){return;}
    while(m_running) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
}
void ReceiveManager::startWork(){
    m_running = true;
    m_workerThread = new std::thread(&ReceiveManager::runThread, this);
    m_workerThread->detach();
}

int ReceiveManager::initSocket(SOCKET &sock, SocketType type){
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        int error_code = WSAGetLastError();
        return -1;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return -1;
    }
    if (type == TCP_SOCKET) {
        sock = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, 0);

    } else if (type == UDP_SOCKET) {
        sock = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    }

    if (sock == INVALID_SOCKET){
        int error_code = WSAGetLastError();
        qDebug() << "create socket failed with error: " << error_code;
        WSACleanup();
        return -1;
    }
    int ttl_value = 110;
    if ( setsockopt( sock, IPPROTO_IP, IP_TTL, (const char*)&ttl_value,  sizeof(ttl_value)) != 0 ) {
        int error_code = WSAGetLastError();
        qDebug() << "setsockopt TTL failed with error: " << error_code;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    return 0;
}

ETH_RX_CTRL::set_log_destination ReceiveManager::packetSetPortCommand(ETH_RX_CTRL::header_req h, uint32_t ip, uint16_t port){
    m_setPortStruct.head = h;
    m_setPortStruct.log_destination_ip = ip;
    m_setPortStruct.log_destination_port = port;

    return m_setPortStruct;
}

ETH_RX_CTRL::set_freq ReceiveManager::packetSetFreqCommand(ETH_RX_CTRL::header_req h, uint32_t f_hz){
    m_setFreq.head = h;
    m_setFreq.carrier_freq_Hz = f_hz;

    return m_setFreq;
}

ETH_RX_CTRL::header_req ReceiveManager::headerReqWrite(uint32_t s, uint32_t m_id, uint16_t t){
    m_headerReq.size = s; //размер всего сообщения с заголовком
    m_headerReq.messid = m_id; //id сообщения для подтверждения, назначается отправителем, если не требуется подтверждение, то равно 0. ПРМ в ответе
    //укажет id, на который отвечает
    m_headerReq.cmd_type = t; //индентификатор команды

    return m_headerReq;
}

int ReceiveManager::sendCommand(SOCKET s, const void* packet, int size){
    int send_bytes = send(s, reinterpret_cast<const char*>(packet), size, 0);
    if (send_bytes == SOCKET_ERROR) {
        qDebug() << "Send error:" << WSAGetLastError();
        return -1;
    }
    return 0;
}

int ReceiveManager::connectToReceiver(){
    auto header = headerReqWrite(sizeof(ETH_RX_CTRL::set_freq), 1, ETH_RX_CTRL::SET_FREQ_REQUEST_0x2);
    auto freqPacket = packetSetFreqCommand(header, m_freq_to_uint32);
    auto addrPacket = packetSetPortCommand(header, m_ip_to_uint32, m_port_to_uint16);
    sendCommand(m_tcpSock, &freqPacket, sizeof(freqPacket));
    sendCommand(m_tcpSock, &addrPacket, sizeof(addrPacket));
    initSocket(m_udpSock, UDP_SOCKET);

    return 0;
}

void ReceiveManager::setFrequency(QString freq){
    m_freq = freq;
    bool l_is_ok;
    m_freq_to_uint32 = freq.toUInt(&l_is_ok);
    if (!l_is_ok) {qDebug() << "freq is not validate";};
}

uint32_t ReceiveManager::getFrequency(){
    return m_freq_to_uint32;
}

void ReceiveManager::setIp(QString ip){
    m_ip = ip;
    m_ip_to_uint32 = QHostAddress(m_ip).toIPv4Address();
}

uint32_t ReceiveManager::getIp(){
    return m_ip_to_uint32;
}

void ReceiveManager::setPort(QString port){
    m_port = port;
    bool l_is_ok;
    m_port_to_uint16 = port.toUShort(&l_is_ok);
    if (!l_is_ok) {qDebug() << "port is not validate";};
}

uint16_t ReceiveManager::getPort(){
    return m_port_to_uint16;
}

ReceiveManager::~ReceiveManager(){
    if (m_workerThread && m_workerThread->joinable()) {
        m_running = false;
        m_workerThread->join();
        delete m_workerThread;
    }
}
