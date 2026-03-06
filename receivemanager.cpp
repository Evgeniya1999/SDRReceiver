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
void ReceiveManager::stopWork(){
    if (m_running) {
        int stop_stream_mess_id = htons(static_cast<uint32_t>(GetCurrentProcessId()+3));
        auto headerForStopStream = headerReqWrite(sizeof(ETH_RX_CTRL::stop_iq_stream), stop_stream_mess_id, ETH_RX_CTRL::STOP_IQ_STREAM_0xE);
        auto stopStreamPacket = packetStopStreamCommand(headerForStopStream);
        if (sendCommand(m_udpSock, &stopStreamPacket, sizeof(stopStreamPacket), ETH_RX_CTRL::STOP_IQ_STREAM_0xE) == SOCKET_ERROR){
            qDebug() << "Failed to send stop stream command";
        };
        if (waitForResponse(stop_stream_mess_id, 10000) == SOCKET_ERROR) {
            qDebug() << "No response for stop stream command";
        }
        m_running = false;
        closesocket(m_udpSock);
    }
    closesocket(m_tcpSock);
    WSACleanup();

}

using Header_Ans = ETH_RX_CTRL::header_ans;


void ReceiveManager::runThread(){

    if (initSocket(m_udpSock, UDP_SOCKET) != 0) return;

    struct sockaddr_in dest_addr {};
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_port = htons(42000);
    dest_addr.sin_family = AF_INET;
    int bind_flag = ::bind(m_udpSock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (bind_flag == SOCKET_ERROR) {
        int err = WSAGetLastError();
        qDebug() << "Bind failed with error:" << err;
        return;
    }

    int start_stream_mess_id = htons(static_cast<uint32_t>(GetCurrentProcessId()+4));
    auto headerForStartStream = headerReqWrite(sizeof(ETH_RX_CTRL::ctrl_iq_stream_now), start_stream_mess_id, ETH_RX_CTRL::CTRL_IQ_STREAM_NOW_0xC);

    auto startStreamPacket = packetStartStreamCommand(headerForStartStream, dest_addr.sin_addr.s_addr, dest_addr.sin_port);
    if (sendCommand(m_tcpSock, &startStreamPacket, sizeof(startStreamPacket), ETH_RX_CTRL::CTRL_IQ_STREAM_NOW_0xC) == SOCKET_ERROR){
        qDebug() << "Failed to send start stream command";
    };
    //if (waitForResponse(start_stream_mess_id, 10000) == SOCKET_ERROR) {
    //    qDebug() << "No response for start stream command";

    //}
    const int IQ_COUNT = 1024;
    const int IQ_SIZE = 2 * sizeof(int);
    m_iqBuffer.reserve(IQ_COUNT * IQ_SIZE);
    while(m_running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_udpSock, &readfds);
        FD_SET(m_tcpSock, &readfds);

        timeval tv{0, 100000};  // 100ms таймаут

        int sel = select(0, &readfds, NULL, NULL, &tv);

        if (sel > 0 && FD_ISSET(m_udpSock, &readfds)) {
            char temp[65536];
            sockaddr_in sender;
            int sender_size = sizeof(sender);

            int bytes = recvfrom(m_udpSock, temp, sizeof(temp), 0,
                                 (sockaddr*)&sender, &sender_size);

            if (bytes > 0 && bytes != 80) {
                std::vector<char> packetData(temp, temp + bytes);
                // Вызываем парсер
                int result = parseUdpPacket(packetData);
                if (result != 0) {
                    qDebug() << "parseUdpPacket returned error:" << result;
                }
                //if (bytes > 0 && bytes != 80) {

                //    qDebug() << "Received UDP packet:" << bytes << "bytes";
                //}
            }

        }
        if (FD_ISSET(m_tcpSock, &readfds)) {
            if (readSocket(m_tcpSock, m_tcpBuffer, start_stream_mess_id) == 0) {
                break;
            }
        } else {
            //cout << "socket is not ready!" << endl;
        }
    }
}
int ReceiveManager::configReceiver(){

    int freq_mess_id = htons(static_cast<uint32_t>(GetCurrentProcessId()));
    auto headerForFreq = headerReqWrite(sizeof(ETH_RX_CTRL::set_freq), freq_mess_id, ETH_RX_CTRL::SET_FREQ_REQUEST_0x2);
    auto freqPacket = packetSetFreqCommand(headerForFreq, m_freq_to_uint32);
    if (sendCommand(m_tcpSock, &freqPacket, sizeof(freqPacket), ETH_RX_CTRL::SET_FREQ_REQUEST_0x2) == SOCKET_ERROR){
        qDebug() << "Failed to send frequency command";
    };
    if (waitForResponse(freq_mess_id, 10000) == SOCKET_ERROR) {
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
            return -1;
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
            if (readSocket(m_tcpSock, m_tcpBuffer, mess_id) == 0) {
                break;
            }
        } else {
            //cout << "socket is not ready!" << endl;
        }
    }
    return 0;
}
int ReceiveManager::parseUdpPacket(std::vector<char> &buffer){
    if (buffer.size() < sizeof(UDP_IQ::header_t)) return -1;
    UDP_IQ::header_t* header = reinterpret_cast<UDP_IQ::header_t*>(buffer.data());
    if ((header->mas_str[0] != 'h' and header->mas_str[1] != 'f') & (header->mas_str[0] != '68' and header->mas_str[1] != '66')) return -1;
            qDebug() << "header->mas_str: " << header->mas_str[0] << header->mas_str[1];
    uint32_t n_samples = header->n_samples;
    if (n_samples == 0) return 0;
    const char* iq_data = buffer.data() + sizeof(UDP_IQ::header_t);
    int iq_data_len = buffer.size() - sizeof(UDP_IQ::header_t);
    int byte_convert = 0;
    switch (header->iq_format) {
    case 0x82: // int16
        byte_convert = 4; // I (2) + Q (2)
        break;
    case 0x84: // int24
        byte_convert = 6; // I (3) + Q (3)
        break;
    default:
        qDebug() << "Unsupported IQ format:" << header->iq_format;
        return -1;
    }
    if (iq_data_len < static_cast<int>(n_samples * byte_convert))
        return -1;
    int samples_to_show = qMin(5, (int)n_samples);
    for (auto i = 0; i < n_samples; i++){
        float i_val = 0.0f, q_val = 0.0f;
        const char* samplePtr = iq_data + i * iq_data_len;
        if (header->iq_format == 0x82) {
            const int32_t* i32 = reinterpret_cast<const int32_t*>(samplePtr);
            i_val = i32[0] / 2147483648.0f;
            q_val = i32[1] / 2147483648.0f;
        } else if (header->iq_format == 0x84) {
            const uint8_t* u8 = reinterpret_cast<const uint8_t*>(samplePtr);
            int32_t i_raw = u8[0] | (u8[1] << 8) | (u8[2] << 16);
            if (i_raw & 0x800000) i_raw |= 0xFF000000;
            i_val = i_raw / 8388608.0f;

            int32_t q_raw = u8[3] | (u8[4] << 8) | (u8[5] << 16);
            if (q_raw & 0x800000) q_raw |= 0xFF000000;
            q_val = q_raw / 8388608.0f;
        }
        if (i < samples_to_show) {
            qDebug() << "  sample" << i << ": I =" << i_val << ", Q =" << q_val;
        }

        // Запись в кольцевой буфер
        m_ringBuffer[m_writeIndex] = std::complex<float>(i_val, q_val);
        m_writeIndex = (m_writeIndex + 1) % m_ringBuffer.size();
        if (m_availableSamples < m_ringBuffer.size())
            m_availableSamples++;
    }
    //if (m_availableSamples >= FFT_SIZE) {
    //    processFft();
    //}
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
    qDebug() << "Hex:" << hexData;
}
int ReceiveManager::readSocket(SOCKET &s, std::vector<char>& buffer, int expected_mess_id){
    char temp_buffer[4096];
    int bytes;

    bytes = ::recv(s, temp_buffer, sizeof(temp_buffer), 0);
    if (bytes == SOCKET_ERROR) {
        int error_code = WSAGetLastError();
            qDebug() << "recv failed with error: " << error_code;
        return -1;
    }
    if (bytes == 0) {
        qDebug() << "Connection closed by receiver";
        return -1;
    }
    buffer.insert(buffer.end(), temp_buffer, temp_buffer + bytes);
    qDebug() << "read socket. buffer size: " << buffer.size();
    qDebug() << "Received TCP packet:" << bytes << "bytes";
    print_hex(buffer.data(), bytes, bytes);
    while (true){
        if (buffer.size() < sizeof(ETH_RX_CTRL::header_ans)){
            qDebug() << "min size for buffer is size header: " << sizeof(ETH_RX_CTRL::header_ans);
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
                return 0;
            } else {
                qDebug() << "Command failed with code:" << header->cmd_complete;
                return -1;
            }
        }

        buffer.erase(buffer.begin(), buffer.begin() + msg_size);
    }
    return 0;
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
    //int ttl_value = 110;
    //if ( setsockopt( sock, IPPROTO_IP, IP_TTL, (const char*)&ttl_value,  sizeof(ttl_value)) != 0 ) {
    //    int error_code = WSAGetLastError();
    //    qDebug() << "setsockopt TTL failed with error: " << error_code;
    //    closesocket(sock);
    //    WSACleanup();
    //    return -1;
    //}

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
ETH_RX_CTRL::stop_iq_stream ReceiveManager::packetStopStreamCommand(ETH_RX_CTRL::header_req h){
    m_stopStream.head = h;
    return m_stopStream;
}
ETH_RX_CTRL::ctrl_iq_stream_now ReceiveManager::packetStartStreamCommand(ETH_RX_CTRL::header_req h, uint32_t ip, uint16_t port){
    m_startStream.head = h;
    m_startStream.IP_stream = ip;
    m_startStream.port_stream = port;
    m_startStream.preset_num = 0xffff;
    return m_startStream;
}

ETH_RX_CTRL::header_req ReceiveManager::headerReqWrite(uint32_t s, uint32_t m_id, uint16_t t){
    m_headerReq.size = s; //размер всего сообщения с заголовком
    m_headerReq.messid = m_id; //id сообщения для подтверждения, назначается отправителем, если не требуется подтверждение, то равно 0. ПРМ в ответе
    //укажет id, на который отвечает
    m_headerReq.cmd_type = t; //индентификатор команды

    return m_headerReq;
}

int ReceiveManager::sendCommand(SOCKET s, const void* packet, int size , ETH_RX_CTRL::COMMAND command){
    int send_bytes = send(s, reinterpret_cast<const char*>(packet), size, 0);
    if (send_bytes == SOCKET_ERROR) {
        qDebug() << "Send error:" << WSAGetLastError();
        return -1;
    }
    qDebug() << "send " << "0x" + QString::number(command, 16).toUpper() << " command. command size: " << send_bytes << "bytes";
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
        return -1;
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
