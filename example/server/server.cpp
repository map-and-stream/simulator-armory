#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

// -------------------------------
// PORTS
// -------------------------------
constexpr int PORT_LRAD  = 2000;
constexpr int PORT_RADAR = 3040;

// -------------------------------
// Frame constants
// -------------------------------
constexpr uint8_t MAGIC[4] = {0x6E, 0x6E, 0x6E, 0x89};
constexpr size_t  HEADER_SIZE = 7; // 4 magic + 2 length + 1 type

// -------------------------------
// Validate frame header
// -------------------------------
bool hasValidMagic(const std::vector<uint8_t>& buf) {
    return buf.size() >= 4 &&
           buf[0] == MAGIC[0] &&
           buf[1] == MAGIC[1] &&
           buf[2] == MAGIC[2] &&
           buf[3] == MAGIC[3];
}

// -------------------------------
// Generic handler (LRAD / RADAR)
// -------------------------------
void connectionHandler(int clientSock, const char* tag) {
    std::vector<uint8_t> buffer;
    std::cout << "[" << tag << "] Client connected\n";

    while (true) {
        uint8_t recvBuf[4096];
        int bytesRead = recv(clientSock, recvBuf, sizeof(recvBuf), 0);

        if (bytesRead <= 0) {
            std::cout << "[" << tag << "] Client disconnected\n";
            break;
        }

        buffer.insert(buffer.end(), recvBuf, recvBuf + bytesRead);

        // Process all complete frames in buffer
        while (buffer.size() >= HEADER_SIZE) {

            // Resync if magic is wrong
            if (!hasValidMagic(buffer)) {
                buffer.erase(buffer.begin());
                continue;
            }

            // Read payload length (big endian)
            uint16_t payloadLen =
                (static_cast<uint16_t>(buffer[4]) << 8) |
                 static_cast<uint16_t>(buffer[5]);

            size_t frameSize = payloadLen + HEADER_SIZE;

            // Wait for full frame
            if (buffer.size() < frameSize)
                break;

            // Extract frame
            std::vector<uint8_t> frame(
                buffer.begin(),
                buffer.begin() + frameSize
            );

            buffer.erase(buffer.begin(), buffer.begin() + frameSize);

            uint8_t type = frame[6];

            std::cout << "[" << tag << "] Frame received:"
                      << " type=" << static_cast<int>(type)
                      << " payloadLen=" << payloadLen << "\n";

            // ------------------------------------------------
            // TRANSPARENT MODE:
            // Send back exactly the same frame
            // ------------------------------------------------
            ssize_t sent = send(clientSock, frame.data(), frame.size(), 0);
            if (sent <= 0) {
                std::cerr << "[" << tag << "] Send failed\n";
                close(clientSock);
                return;
            }
        }
    }

    close(clientSock);
}

// -------------------------------
// Server starter
// -------------------------------
void startServer(int port, const char* tag) {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSock < 0) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(serverSock);
        return;
    }

    if (listen(serverSock, 5) < 0) {
        perror("listen");
        close(serverSock);
        return;
    }

    std::cout << "[SERVER] Listening on port " << port << "\n";

    while (true) {
        int clientSock = accept(serverSock, nullptr, nullptr);
        if (clientSock >= 0) {
            std::thread(connectionHandler, clientSock, tag).detach();
        }
    }
}

// -------------------------------
// MAIN
// -------------------------------
int main() {
    std::thread lradThread(startServer, PORT_LRAD,  "LRAD");
    std::thread radarThread(startServer, PORT_RADAR, "RADAR");

    lradThread.join();
    radarThread.join();
    return 0;
}


