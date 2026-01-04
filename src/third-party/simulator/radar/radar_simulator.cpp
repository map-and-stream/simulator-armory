#include "radar_simulator.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

// -------------------------------
// PORTS
// -------------------------------
constexpr int PORT_RADAR = 3040;

// -------------------------------
// Frame constants
// -------------------------------
constexpr uint8_t MAGIC[4] = {0x6E, 0x6E, 0x6E, 0x89};

// -------------------------------
// Globals
// -------------------------------
std::vector<int> g_radarClients;
std::mutex g_radarMutex;
bool g_running = true;

// -------------------------------
// Build frame
// -------------------------------
std::vector<uint8_t> buildFrame(uint8_t type, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> frame;

    frame.insert(frame.end(), MAGIC, MAGIC + 4);

    uint16_t len = payload.size();
    frame.push_back((len >> 8) & 0xFF);
    frame.push_back(len & 0xFF);

    frame.push_back(type);
    frame.insert(frame.end(), payload.begin(), payload.end());

    return frame;
}

// -------------------------------
// Broadcast
// -------------------------------
void broadcastToRadarClients(const std::vector<uint8_t>& frame) {
    std::lock_guard<std::mutex> lock(g_radarMutex);

    for (auto it = g_radarClients.begin(); it != g_radarClients.end();) {
        int sock = *it;
        ssize_t sent = send(sock, frame.data(), frame.size(), 0);

        if (sent <= 0) {
            std::cerr << "[RADAR] Send failed, removing client\n";
            close(sock);
            it = g_radarClients.erase(it);
        } else {
            ++it;
        }
    }
}

// -------------------------------
// Client handler
// -------------------------------
void connectionHandler(int clientSock) {
    std::cout << "[RADAR] Client connected\n";

    uint8_t recvBuf[4096];
    while (g_running) {
        int bytesRead = recv(clientSock, recvBuf, sizeof(recvBuf), 0);
        if (bytesRead <= 0) {
            std::cout << "[RADAR] Client disconnected\n";
            break;
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_radarMutex);
        g_radarClients.erase(std::remove(g_radarClients.begin(), g_radarClients.end(), clientSock),
                             g_radarClients.end());
    }

    close(clientSock);
}

// -------------------------------
// Start server
// -------------------------------
void startRadarServer() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_RADAR);
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

    std::cout << "[SERVER] RADAR listening on port " << PORT_RADAR << "\n";

    while (g_running) {
        int clientSock = accept(serverSock, nullptr, nullptr);
        if (clientSock >= 0) {
            {
                std::lock_guard<std::mutex> lock(g_radarMutex);
                g_radarClients.push_back(clientSock);
            }
            std::thread(connectionHandler, clientSock).detach();
        }
    }

    close(serverSock);
}

// -------------------------------
// Generate CAT101
// -------------------------------
CAT101 generateTestCAT101() {
    static int track = 1;
    CAT101 cat{};

    cat.radarid = 1;
    cat.tracknumber = track++;
    cat.range = 1000 + (track % 500);
    cat.azimuth = (track * 10) % 3600;
    cat.elevation = 5;
    cat.latitude = 35000000;
    cat.longitude = 51000000;
    cat.altitude = 10000;
    cat.speed = 250;
    cat.heading = (track * 20) % 3600;
    cat.rcs = 10;
    cat.type = 1;
    cat.real_simulation = 0;

    return cat;
}

CAT102 generateTestCAT102() {
    static int arcid = 1;
    CAT102 cat{};
    cat.radarid = 1;
    cat.azimuth = (arcid * 5) % 3600;
    cat.elevation = 10;
    cat.resolution = 2048;
    cat.arcdatalength = 32;
    cat.arcdata = std::string(cat.arcdatalength, '\xAA'); // dummy data of length 32
    arcid++;
    return cat;
}


// -------------------------------
// Broadcaster loop
// -------------------------------

void radarBroadcasterLoop(int interval_ms) {
    Radar radar;

    while (g_running) {

        // -------------------------
        // CAT101 (existing)
        // -------------------------
        CAT101 msg101 = generateTestCAT101();
        std::vector<uint8_t> payload101;
        radar.encode(msg101, payload101);
        auto frame101 = buildFrame(101, payload101);
        broadcastToRadarClients(frame101);

        // -------------------------
        // CAT102 (NEW)
        // -------------------------
        CAT102 msg102 = generateTestCAT102();
        std::vector<uint8_t> payload102;
        radar.encode(msg102, payload102);
        auto frame102 = buildFrame(102, payload102);

        std::cout << "[RADAR] Broadcasting CAT102: arc len = "
                  << msg102.arcdatalength
                  << ", payload size = " << payload102.size()
                  << ", frame size = " << frame102.size() << "\n";

        broadcastToRadarClients(frame102);

        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
}

// -------------------------------
// Signal handler
// -------------------------------
void handleSignal(int) {
    g_running = false;
}
