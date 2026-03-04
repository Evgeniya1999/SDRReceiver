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

void ReceiveManager::startWork(){
    m_running = true;
    m_workerThread = new std::thread(&ReceiveManager::runThread, this);
}

using Header_Ans = ETH_RX_CTRL::header_ans;


void ReceiveManager::runThread(){
    const int IQ_COUNT = 1024;
    const int IQ_SIZE = 2 * sizeof(int);
    m_iqBuffer.reserve(IQ_COUNT * IQ_SIZE);
    while(m_running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_udpSock, &readfds);

        timeval tv{0, 100000};  // 100ms таймаут

        int sel = select(0, &readfds, NULL, NULL, &tv);

        if (sel > 0 && FD_ISSET(m_udpSock, &readfds)) {
            char temp[65536];
            sockaddr_in sender;
            int sender_size = sizeof(sender);

            int bytes = recvfrom(m_udpSock, temp, sizeof(temp), 0,
                                 (sockaddr*)&sender, &sender_size);

            if (bytes > 0) {

                qDebug() << "Received UDP packet:" << bytes << "bytes";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
int ReceiveManager::configReceiver(){

    if (initSocket(m_udpSock, UDP_SOCKET) != 0) return -1;

    struct sockaddr_in dest_addr {};
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_port = htons(42000);
    dest_addr.sin_family = AF_INET;
    int bind_flag = ::bind(m_udpSock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (bind_flag == SOCKET_ERROR) {
        int err = WSAGetLastError();
        qDebug() << "Bind failed with error:" << err;
        return -1;
    }
    int freq_mess_id = htons(static_cast<uint32_t>(GetCurrentProcessId()));
    auto headerForFreq = headerReqWrite(sizeof(ETH_RX_CTRL::set_freq), freq_mess_id, ETH_RX_CTRL::SET_FREQ_REQUEST_0x2);
    auto freqPacket = packetSetFreqCommand(headerForFreq, m_freq_to_uint32);
    if (sendCommand(m_tcpSock, &freqPacket, sizeof(freqPacket)) == SOCKET_ERROR){
        qDebug() << "Failed to send frequency command";
    };
    if (!waitForResponse(freq_mess_id, 10000)) {
        qDebug() << "No response for frequency command";
        return -1;
    }
    int port_mess_id = htons(static_cast<uint32_t>(GetCurrentProcessId())+1);
    auto headerPort = headerReqWrite(sizeof(ETH_RX_CTRL::set_freq), port_mess_id, ETH_RX_CTRL::SET_LOG_DESTINATION_0x17);
    auto headerPacket = packetSetPortCommand(headerPort, m_ip_to_uint32, m_port_to_uint16);
    if (sendCommand(m_tcpSock, &headerPacket, sizeof(headerPacket)) == SOCKET_ERROR){
        qDebug() << "Failed to send frequency command";
    };
    if (!waitForResponse(port_mess_id, 10000)) {
        qDebug() << "No response for frequency command";
        return -1;
    }
    return 0;
}
int ReceiveManager::waitForResponse(int mess_id, int timeout){
    auto start_time = std::chrono::steady_clock::now();
    while(true){
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
        if (elapsed > timeout) {
            qDebug() << "Timeout waiting for response";
            return false;
        }
        fd_set readfs;
        FD_ZERO(&readfs);
        FD_SET(m_tcpSock, &readfs);
        timeval select_timeout{0, 100000}; // 100ms
        int sel = select(0, &readfs, NULL, NULL, &select_timeout);
        if (sel == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            qDebug() << "select failed with error: " << error_code;
            break;
        }
        if (FD_ISSET(m_tcpSock, &readfs)) {
            readSocket(m_tcpSock, m_tcpBuffer, mess_id);
        } else {
            //cout << "socket is not ready!" << endl;
        }
    }
    return 0;
}
void print_hex(const char* data, int len, int countBytes)
{
    if (countBytes > 0) {
        qDebug() << "size:" << countBytes << "bytes";
    }
    int to_show = std::min(len, 60);

    QByteArray bytes(data, to_show);
    QByteArray hexData = bytes.toHex(' ').toUpper();
    qDebug() << "send Hex:" << hexData;
}
void ReceiveManager::readSocket(SOCKET &s, std::vector<char>& buffer, int expected_mess_id){
    char temp_buffer[4096];
    int bytes;

    bytes = ::recv(s, temp_buffer, sizeof(temp_buffer), 0);
    if (bytes == SOCKET_ERROR) {
        int error_code = WSAGetLastError();
            qDebug() << "recv failed with error: " << error_code;
        return;
    }
    if (bytes == 0) {
        qDebug() << "Connection closed by receiver";
        return;
    }
    buffer.insert(buffer.end(), temp_buffer, temp_buffer + bytes);
    qDebug() << "Received TCP packet:" << bytes << "bytes";
    print_hex(buffer.data(), bytes, bytes);
    while (true){
        if (buffer.size() < sizeof(ETH_RX_CTRL::header_ans)){
            qDebug() << "Not enough data for header, waiting for more";
            break;
        }

        auto* header = reinterpret_cast<ETH_RX_CTRL::header_ans*>(buffer.data());
        uint32_t msg_size = header->size;
        if (msg_size > 102400) { // максимальный размер из протокола
            qDebug() << "Invalid message size:" << msg_size << ", clearing buffer";
            buffer.clear();
            break;
        }
        if (buffer.size() < msg_size){
            qDebug() << "Not enough data for full message (" << msg_size
                     << "bytes), waiting for more";
            break;
        }

        if (header->messid == expected_mess_id) {
            buffer.erase(buffer.begin(), buffer.begin() + msg_size);
            if (header->cmd_complete == ETH_RX_CTRL::good) {

                qDebug() << "Command" << header->cmd_type << "completed successfully";
                return;
            } else {
                qDebug() << "Command failed with code:" << header->cmd_complete;
                return;
            }
        }
        buffer.erase(buffer.begin(), buffer.begin() + msg_size);
    }
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

    if (initSocket(m_tcpSock, TCP_SOCKET) != 0) return -1;

    struct sockaddr_in dest_addr {};
    dest_addr.sin_addr.s_addr = htonl(getIp());
    dest_addr.sin_port = htons(getPort());
    dest_addr.sin_family = AF_INET;
    int connect_flag = ::connect(m_tcpSock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (connect_flag == SOCKET_ERROR) {
        int err = WSAGetLastError();
        qDebug() << "Connect failed with error:" << err;
        return 0;
    }

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
    m_running = false;
    if (m_workerThread && m_workerThread->joinable()) {       
        m_workerThread->join();
        delete m_workerThread;
    }
    if (m_udpSock != INVALID_SOCKET) closesocket(m_udpSock);
    if (m_tcpSock != INVALID_SOCKET) closesocket(m_tcpSock);
    WSACleanup();
}
