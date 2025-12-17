#include <arpa/inet.h>
#include <unistd.h>

#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "third-party/protocol-armory/src/radar/radar.h"
#include "third-party/protocol-armory/src/radar/struct.h"

// -------------------------------
// PORTS
// -------------------------------
constexpr int PORT_RADAR = 3040;

// -------------------------------
// Frame constants
// -------------------------------
constexpr uint8_t MAGIC[4] = {0x6E, 0x6E, 0x6E, 0x89};
constexpr size_t HEADER_SIZE = 7;  // 4 magic + 2 length + 1 type

// -------------------------------
// Globals
// -------------------------------
std::vector<int> g_radarClients;
std::mutex g_radarMutex;
bool g_running = true;

// -------------------------------
// Helper: build frame (MAGIC + LEN + TYPE + PAYLOAD)
// -------------------------------
std::vector<uint8_t> buildFrame(uint8_t type, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> frame;

    // MAGIC
    frame.insert(frame.end(), MAGIC, MAGIC + 4);

    // LENGTH (payload only, big-endian)
    uint16_t len = static_cast<uint16_t>(payload.size());
    frame.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
    frame.push_back(static_cast<uint8_t>(len & 0xFF));

    // TYPE
    frame.push_back(type);

    // PAYLOAD
    frame.insert(frame.end(), payload.begin(), payload.end());

    return frame;
}

// -------------------------------
// Helper: broadcast frame to all radar clients
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
// Client connection handler
// (we keep it simple: just read and ignore data)
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
        // You can log or process input if needed,
        // but for "send only CAT101" we ignore it.
    }

    // Remove from client list
    {
        std::lock_guard<std::mutex> lock(g_radarMutex);
        auto it = std::find(g_radarClients.begin(), g_radarClients.end(), clientSock);
        if (it != g_radarClients.end()) {
            g_radarClients.erase(it);
        }
    }

    close(clientSock);
}

// -------------------------------
// Server starter (RADAR on 3040)
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
// CAT101 generator (example data)
// -------------------------------
CAT101 generateTestCAT101() {
    static int track = 1;
    CAT101 cat{};

    cat.radarid = 1;
    cat.tracknumber = track++;
    cat.range = 1000 + (track % 500);
    cat.azimuth = (track * 10) % 3600;  // 0.1 deg units if you want
    cat.elevation = 5;
    cat.latitude = 35000000;   // dummy
    cat.longitude = 51000000;  // dummy
    cat.altitude = 10000;
    cat.speed = 250;
    cat.heading = (track * 20) % 3600;
    cat.rcs = 10;
    cat.type = 1;
    cat.real_simulation = 0;  // real = 0, sim = 1 (for example)

    return cat;
}

// -------------------------------
// Periodic broadcaster thread
// -------------------------------
void radarBroadcasterLoop(int interval_ms) {
    Radar radar;

    while (g_running) {
        CAT101 msg = generateTestCAT101();

        // Encode CAT101 using your Payam-based Radar encoder
        std::vector<uint8_t> payload;
        radar.encode(msg, payload);

        // Wrap into your custom frame
        // Here, we use type = 101 for CAT101
        auto frame = buildFrame(101, payload);

        std::cout << "[RADAR] Broadcasting CAT101: track " << msg.tracknumber << ", payload size = " << payload.size()
                  << ", frame size = " << frame.size() << "\n";

        broadcastToRadarClients(frame);

        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
}

// -------------------------------
// Signal handler to exit cleanly
// -------------------------------
void handleSignal(int) {
    g_running = false;
}

// -------------------------------
// MAIN
// -------------------------------
int main() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    std::thread serverThread(startRadarServer);

    // Broadcast CAT101 every 1000 ms (1 second)
    std::thread broadcasterThread(radarBroadcasterLoop, 1000);

    serverThread.join();
    broadcasterThread.join();

    return 0;
}
