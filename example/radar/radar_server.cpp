#include <arpa/inet.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

constexpr int SERVER_PORT = 3040;

bool parsePacketHeader(const std::vector<uint8_t>& data, uint8_t& type) {
    if (data.size() < 7)
        return false;
    type = data[6];
    return true;
}

void clientHandler(int clientSock) {
    std::vector<uint8_t> buffer;

    std::cout << "[SERVER] Client connected.\n";

    while (true) {
        uint8_t recvBuf[4096];
        int bytesRead = recv(clientSock, recvBuf, sizeof(recvBuf), 0);

        if (bytesRead <= 0) {
            std::cout << "[SERVER] Client disconnected.\n";
            break;
        }

        buffer.insert(buffer.end(), recvBuf, recvBuf + bytesRead);

        // Hex dump
        std::cerr << "[HEX] ";
        for (auto b : buffer)
            std::cerr << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
        std::cerr << std::dec << std::endl;

        // Packet framing 6E 6E 6E 89
        while (buffer.size() >= 7) {
            if (!(buffer[0] == 0x6E && buffer[1] == 0x6E && buffer[2] == 0x6E &&
                  buffer[3] == 0x89)) {
                buffer.erase(buffer.begin());
                continue;
            }

            // Get length
            uint16_t size = ((buffer[4] << 8) | buffer[5]) + 5;

            if (buffer.size() < size)
                break;  // need more bytes

            std::vector<uint8_t> pkt(buffer.begin(), buffer.begin() + size);
            buffer.erase(buffer.begin(), buffer.begin() + size);

            uint8_t type = 0;
            if (!parsePacketHeader(pkt, type))
                continue;

            std::cout << "[SERVER] Packet received. Type = " << (int)type << "  Size = " << size
                      << "\n";
            // After detecting a valid packet and printing its type:
            std::cout << "[SERVER] Packet received. Type = " << (int)type << "  Size = " << size
                      << "\n";

            // Build a response packet manually
            std::vector<uint8_t> responsePkt;  // Framing header
            responsePkt.push_back(0x6E);
            responsePkt.push_back(0x6E);
            responsePkt.push_back(0x6E);
            responsePkt.push_back(0x89);

            // Length field (big endian)
            uint16_t length = 10;  // payload size
            responsePkt.push_back((length >> 8) & 0xFF);
            responsePkt.push_back(length & 0xFF);

            // Type field
            responsePkt.push_back(0x96);  // example type

            // Payload
            responsePkt.push_back(0x01);  // lrad_id
            responsePkt.push_back(0x02);  // status
            responsePkt.push_back(0x03);  // range

            // Send back to client
            send(clientSock, responsePkt.data(), responsePkt.size(), 0);
            std::cout << "[SERVER] Sent response packet of size " << responsePkt.size() << "\n";
        }
    }

    close(clientSock);
    std::cout << "[SERVER] Client thread exit.\n";
}

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        return 1;
    }

    listen(serverSock, 5);

    std::cout << "[SERVER] Listening on port " << SERVER_PORT << "...\n";

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t len = sizeof(clientAddr);

        int clientSock = accept(serverSock, (sockaddr*)&clientAddr, &len);

        if (clientSock < 0)
            continue;

        std::thread(clientHandler, clientSock).detach();
    }

    close(serverSock);
    return 0;
}
