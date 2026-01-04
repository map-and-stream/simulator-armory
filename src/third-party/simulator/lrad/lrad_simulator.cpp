#include "lrad_simulator.h"
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>

constexpr int PORT = 2000;

void handleClient(int clientSock) {
    std::cout << "[SERVER] Client connected.\n";

    std::vector<uint8_t> buffer;
    buffer.reserve(4096);

    uint8_t recvBuf[1024];

    while (true) {
        ssize_t bytesRead = read(clientSock, recvBuf, sizeof(recvBuf));
        if (bytesRead <= 0) {
            std::cerr << "[SERVER] Client disconnected.\n";
            break;
        }

        // Append new bytes
        buffer.insert(buffer.end(), recvBuf, recvBuf + bytesRead);

        // Process all complete frames inside buffer
        while (buffer.size() >= 7) {
            // Check magic
            if (!(buffer[0] == 0x6E && buffer[1] == 0x6E &&
                  buffer[2] == 0x6E && buffer[3] == 0x89)) {
                buffer.erase(buffer.begin());
                continue;
            }

            uint16_t len = (buffer[4] << 8) | buffer[5];
            size_t frameSize = len + 5;

            if (buffer.size() < frameSize)
                break;

            std::vector<uint8_t> frame(buffer.begin(), buffer.begin() + frameSize);
            buffer.erase(buffer.begin(), buffer.begin() + frameSize);

            // Print HEX
            std::cout << "[HEX] ";
            for (auto b : frame)
                std::cout << std::hex << std::uppercase << (int)b << " ";
            std::cout << std::dec << "\n";

            int packetType = frame[6];
            std::cout << "[SERVER] Packet received. Type = "
                      << packetType << "  Size = " << frame.size() << "\n";

            // Dummy response
            std::vector<uint8_t> response = {
                0x01,0x02,0x03,0x04,
                0x05,0x06,0x07,0x08,
                0x09,0x0A
            };
            write(clientSock, response.data(), response.size());
            std::cout << "[SERVER] Sent response packet of size "
                      << response.size() << "\n";
        }
    }

    close(clientSock);
}

void startLradServer() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSock, 5);

    std::cout << "[SERVER] Listening on port " << PORT << "...\n";

    while (true) {
        int clientSock = accept(serverSock, nullptr, nullptr);
        std::thread(handleClient, clientSock).detach();
    }

    close(serverSock);
}
