// #include "radar_simulator.h"

// #include <arpa/inet.h>
// #include <unistd.h>

// #include <algorithm>
// #include <chrono>
// #include <iostream>
// #include <thread>

// // -------------------------------
// // PORTS
// // -------------------------------
// constexpr int PORT_RADAR = 3040;

// // -------------------------------
// // Frame constants
// // -------------------------------
// constexpr uint8_t MAGIC[4] = {0x6E, 0x6E, 0x6E, 0x89};

// // -------------------------------
// // Globals
// // -------------------------------
// std::vector<int> g_radarClients;
// std::mutex g_radarMutex;
// bool g_running = true;

// // -------------------------------
// // Build frame
// // -------------------------------
// std::vector<uint8_t> buildFrame(uint8_t type, const std::vector<uint8_t>& payload) {
//     std::vector<uint8_t> frame;

//     frame.insert(frame.end(), MAGIC, MAGIC + 4);

//     uint16_t len = payload.size();
//     frame.push_back((len >> 8) & 0xFF);
//     frame.push_back(len & 0xFF);

//     frame.push_back(type);
//     frame.insert(frame.end(), payload.begin(), payload.end());

//     return frame;
// }

// // -------------------------------
// // Broadcast
// // -------------------------------
// void broadcastToRadarClients(const std::vector<uint8_t>& frame) {
//     std::lock_guard<std::mutex> lock(g_radarMutex);

//     for (auto it = g_radarClients.begin(); it != g_radarClients.end();) {
//         int sock = *it;
//         ssize_t sent = send(sock, frame.data(), frame.size(), 0);

//         if (sent <= 0) {
//             std::cerr << "[RADAR] Send failed, removing client\n";
//             close(sock);
//             it = g_radarClients.erase(it);
//         } else {
//             ++it;
//         }
//     }
// }

// // -------------------------------
// // Client handler
// // -------------------------------
// void connectionHandler(int clientSock) {
//     std::cout << "[RADAR] Client connected\n";

//     uint8_t recvBuf[4096];
//     while (g_running) {
//         int bytesRead = recv(clientSock, recvBuf, sizeof(recvBuf), 0);
//         if (bytesRead <= 0) {
//             std::cout << "[RADAR] Client disconnected\n";
//             break;
//         }
//     }

//     {
//         std::lock_guard<std::mutex> lock(g_radarMutex);
//         g_radarClients.erase(std::remove(g_radarClients.begin(), g_radarClients.end(), clientSock),
//                              g_radarClients.end());
//     }

//     close(clientSock);
// }

// // -------------------------------
// // Start server
// // -------------------------------
// void startRadarServer() {
//     int serverSock = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSock < 0) {
//         perror("socket");
//         return;
//     }

//     int opt = 1;
//     setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     sockaddr_in addr{};
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(PORT_RADAR);
//     addr.sin_addr.s_addr = INADDR_ANY;

//     if (bind(serverSock, (sockaddr*)&addr, sizeof(addr)) < 0) {
//         perror("bind");
//         close(serverSock);
//         return;
//     }

//     if (listen(serverSock, 5) < 0) {
//         perror("listen");
//         close(serverSock);
//         return;
//     }

//     std::cout << "[SERVER] RADAR listening on port " << PORT_RADAR << "\n";

//     while (g_running) {
//         int clientSock = accept(serverSock, nullptr, nullptr);
//         if (clientSock >= 0) {
//             {
//                 std::lock_guard<std::mutex> lock(g_radarMutex);
//                 g_radarClients.push_back(clientSock);
//             }
//             std::thread(connectionHandler, clientSock).detach();
//         }
//     }

//     close(serverSock);
// }

// // -------------------------------
// // Generate CAT101
// // -------------------------------
// // CAT101 generateTestCAT101() {
// //     static int track = 1;
// //     CAT101 cat{};

// //     cat.radarid = 1;
// //     cat.tracknumber = track++;
// //     cat.range = 1000 + (track % 500);
// //     cat.azimuth = (track * 10) % 3600;
// //     cat.elevation = 5;
// //     cat.latitude = 35000000;
// //     cat.longitude = 51000000;
// //     cat.altitude = 10000;
// //     cat.speed = 250;
// //     cat.heading = (track * 20) % 3600;
// //     cat.rcs = 10;
// //     cat.type = 1;
// //     cat.real_simulation = 0;

// //     return cat;
// // }
// CAT101 generateTestCAT101() {
//     static int track = 1;
//     static int fixedAzimuth = 900; // 90.0°

//     CAT101 cat{};
//     cat.radarid = 1;
//     cat.tracknumber = track;
//     cat.range = 1500;
//     cat.azimuth = fixedAzimuth;   // 🔥 FIXED
//     cat.elevation = 5;
//     cat.speed = 250;
//     cat.heading = 1800;

//     return cat;
// }

// CAT102 generateTestCAT102() {
//     static int arcid = 1;
//     CAT102 cat{};
//     cat.radarid = 1;
//     cat.azimuth = (arcid * 5) % 3600;
//     cat.elevation = 10;
//     cat.resolution = 2048;
//     cat.arcdatalength = 32;
//     cat.arcdata = std::string(cat.arcdatalength, '\xAA'); // dummy data of length 32
//     arcid++;
//     return cat;
// }


// // -------------------------------
// // Broadcaster loop
// // -------------------------------

// void radarBroadcasterLoop(int interval_ms) {
//     Radar radar;

//     while (g_running) {

//         // -------------------------
//         // CAT101 (existing)
//         // -------------------------
//         CAT101 msg101 = generateTestCAT101();
//         std::vector<uint8_t> payload101;
//         radar.encode(msg101, payload101);
//         auto frame101 = buildFrame(101, payload101);
//         broadcastToRadarClients(frame101);

//         // -------------------------
//         // CAT102 (NEW)
//         // -------------------------
//         CAT102 msg102 = generateTestCAT102();
//         std::vector<uint8_t> payload102;
//         radar.encode(msg102, payload102);
//         auto frame102 = buildFrame(102, payload102);

//         std::cout << "[RADAR] Broadcasting CAT102: arc len = "
//                   << msg102.arcdatalength
//                   << ", payload size = " << payload102.size()
//                   << ", frame size = " << frame102.size() << "\n";

//         broadcastToRadarClients(frame102);

//         std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
//     }
// }

// // -------------------------------
// // Signal handler
// // -------------------------------
// void handleSignal(int) {
//     g_running = false;
// }
#include "radar_simulator.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
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
// Target model (FIXED azimuth)
// -------------------------------
struct SimTarget {
    int tracknumber;
    int range;        // meters
    int azimuth;      // 0–3599 (0.1 deg)
    bool detectedThisRotation;
};

std::vector<SimTarget> g_targets = {
    {1, 1500,  900, false},   // 90.0°
    {2, 2200, 1600, false},   // 160.0°
    {3, 2300, 1700, false},    // 170.0°
    {4, 2400, 1800, false},    // 180.0°
    {5, 2500, 1900, false},    // 1900.0°
    {6, 2600, 2000, false},    // 200.0°
    {7, 2700, 2100, false},    // 210.0°
    {8, 2800, 2200, false},    // 220.0°
    {9, 2900, 2400, false},    // 240.0°
    {10, 3000, 2700, false},   // 270.0°
    {11, 3100, 2800, false},   // 280.0°
    {12, 3200, 2900, false},   // 290.0°
    {13, 3300, 3000, false},   // 300.0°
    {14, 3400, 3100, false},   // 310.0°
    {15, 3500, 3200, false},   // 320.0°
    {16, 3600, 3300, false},   // 330.0°
    {17, 2100, 3400, false},   // 340.0°
    {18, 2200, 3500, false},   // 350.0°
    {19, 2300, 1000,false}, //100.0°
    {20, 2400, 1100,false}, //110.0°
    {21, 2500, 1200,false}, //120.0°
    {22, 2600, 1300,false}, //130.0°
    {23, 2700, 1400,false}, //140.0°
    {24, 2800, 1500,false}, //150.0°
    {25, 2900, 1600,false}, //160.0°
    {26, 3000, 1700,false}, //170.0°
    {27, 3100, 1800,false}, //180.0°
    {28, 3200, 1900,false}, //190.0°
    {29, 3300, 100,false}, //10.0°
    {30, 3400, 200,false}, //20.0°
    {31, 3500, 300,false}, //30.0°
    {32, 3600, 400,false}, //40.0°
    {33, 2100, 500,false}, //50.0°
    {34, 2200, 600,false}, //60.0°
    {35, 2300, 700,false}, //70.0°
    {36, 2400, 800,false}, //80.0°
    {37, 2500, 900,false}, //90.0°
    {38, 2600, 100,false}, //10.0°
    {39, 2700, 200,false}, //20.0°
    {40, 2800, 300,false}, //30.0°
    {41, 2900, 400,false}, //40.0°
    {42, 3000, 500,false}, //50.0°
    {43, 3100, 600,false}, //60.0°
};

// -------------------------------
// Build frame
// -------------------------------
std::vector<uint8_t> buildFrame(uint8_t type,
                                const std::vector<uint8_t>& payload) {
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
        ssize_t sent = send(*it, frame.data(), frame.size(), 0);
        if (sent <= 0) {
            close(*it);
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
    uint8_t buf[1024];

    while (g_running) {
        if (recv(clientSock, buf, sizeof(buf), 0) <= 0) {
            break;
        }
    }

    close(clientSock);
    std::cout << "[RADAR] Client disconnected\n";
}

// -------------------------------
// Start server
// -------------------------------
void startRadarServer() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_RADAR);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSock, (sockaddr*)&addr, sizeof(addr));
    listen(serverSock, 5);

    std::cout << "[RADAR] Listening on port " << PORT_RADAR << "\n";

    while (g_running) {
        int clientSock = accept(serverSock, nullptr, nullptr);
        if (clientSock >= 0) {
            std::lock_guard<std::mutex> lock(g_radarMutex);
            g_radarClients.push_back(clientSock);
            std::thread(connectionHandler, clientSock).detach();
        }
    }

    close(serverSock);
}

// -------------------------------
// Radar helpers
// -------------------------------
bool sweepHitsTarget(int sweepAz, int targetAz) {
    return std::abs(sweepAz - targetAz) <= 3;  // beam width
}

CAT101 makeCAT101FromTarget(const SimTarget& t) {
    CAT101 cat{};
    cat.radarid = 1;
    cat.tracknumber = t.tracknumber;
    cat.range = t.range;
    cat.azimuth = t.azimuth;   // 🔥 FIXED
    cat.elevation = 5;
    cat.speed = 250;
    cat.heading = 1800;
    cat.rcs = 10;
    cat.type = 1;
    cat.real_simulation = 1;
    return cat;
}

CAT102 generateCAT102(int sweepAzimuth) {
    CAT102 cat{};
    cat.radarid = 1;
    cat.azimuth = sweepAzimuth;
    cat.elevation = 10;
    cat.resolution = 50;       // meters/bin
    cat.arcdatalength = 200;   // 10 km
    cat.arcdata.assign(cat.arcdatalength, 0x00);

    // Inject echoes
    for (const auto& t : g_targets) {
        if (sweepHitsTarget(sweepAzimuth, t.azimuth)) {
            int bin = t.range / cat.resolution;
            if (bin >= 0 && bin < cat.arcdatalength) {
                cat.arcdata[bin] = 0xFF;
            }
        }
    }
    return cat;
}

// -------------------------------
// Broadcaster loop (CORRECT)
// -------------------------------
void radarBroadcasterLoop(int interval_ms) {
    Radar radar;
    int sweepAzimuth = 0;

    while (g_running) {

        // CAT102 – rotating sweep
        CAT102 sweep = generateCAT102(sweepAzimuth);
        std::vector<uint8_t> payload102;
        radar.encode(sweep, payload102);

        auto frame102 = buildFrame(102, payload102);

        std::cout << "[RADAR] Broadcasting CAT102: arc len = "
                << sweep.arcdata.size()
                << ", payload size = "
                << payload102.size()
                << ", frame size = "
                << frame102.size()
                << "\n";

        broadcastToRadarClients(frame102);

        // CAT101 – detection-based
        for (auto& t : g_targets) {
            if (!t.detectedThisRotation &&
                sweepHitsTarget(sweepAzimuth, t.azimuth)) {

                CAT101 track = makeCAT101FromTarget(t);
                std::vector<uint8_t> payload101;
                radar.encode(track, payload101);
                broadcastToRadarClients(buildFrame(101, payload101));

                t.detectedThisRotation = true;
            }
        }

        // Advance sweep
        sweepAzimuth = (sweepAzimuth + 5) % 3600;

        // Reset once per rotation
        if (sweepAzimuth == 0) {
            for (auto& t : g_targets) {
                t.detectedThisRotation = false;
            }
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(interval_ms));
    }
}

// -------------------------------
// Signal handler
// -------------------------------
void handleSignal(int) {
    g_running = false;
}
