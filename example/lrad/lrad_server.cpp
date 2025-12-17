#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

constexpr int PORT = 2000;

void handleClient(int clientSock) {
    std::cout << "[SERVER] Client connected.\n";

    char buffer[1024];
    while (true) {
        ssize_t bytesRead = read(clientSock, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            std::cerr << "[SERVER] Client disconnected.\n";
            break;
        }

        // Print received data in hex
        std::vector<uint8_t> data(buffer, buffer + bytesRead);
        std::cout << "[HEX] ";
        for (auto b : data) {
            std::cout << std::hex << std::uppercase << (int)b << " ";
        }
        std::cout << std::dec << "\n";

        // Extract packet type (example: 7th byte)
        int packetType = (data.size() > 6) ? data[6] : -1;
        std::cout << "[SERVER] Packet received. Type = " << packetType << "  Size = " << data.size() << "\n";

        // Respond with a dummy frame (example 10 bytes)
        std::vector<uint8_t> response = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
        write(clientSock, response.data(), response.size());
        std::cout << "[SERVER] Sent response packet of size " << response.size() << "\n";
    }

    close(clientSock);
}

int main() {
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
    return 0;
}
