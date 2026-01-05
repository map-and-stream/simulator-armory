#include "radar_simulator.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
bool sweepHitsTarget(int sweepAz, int targetAz) { return std::abs(sweepAz - targetAz) <= 3; // beam width 
    }
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
    {3, 2300, 1700, false},   // 170.0°
    {4, 2400, 1800, false},   // 180.0°
    {5, 2500, 1900, false},   // 190.0°
    {6, 2600, 2000, false},   // 200.0°
    {7, 2700, 2100, false},   // 210.0°
    {8, 2800, 2200, false},   // 220.0°
    {9, 2900, 2400, false},   // 240.0°
    {10, 3000, 2700, false},  // 270.0°
    {11, 3100, 2800, false},  // 280.0°
    {12, 3200, 2900, false},  // 290.0°
    {13, 3300, 3000, false},  // 300.0°
    {14, 3400, 3100, false},  // 310.0°
    {15, 3500, 3200, false},  // 320.0°
    {16, 3600, 3300, false},  // 330.0°
    {17, 2100, 3400, false},  // 340.0°
    {18, 2200, 3500, false},  // 350.0°
    {19, 2300, 1000,false},   //100.0°
    {20, 2400, 1100,false},   //110.0°
    {21, 2500, 1200,false},   //120.0°
    {22, 2600, 1300,false},   //130.0°
    {23, 2700, 1400,false},   //140.0°
    {24, 2800, 1500,false},   //150.0°
    {25, 2900, 1600,false},   //160.0°
    {26, 3000, 1700,false},   //170.0°
    {27, 3100, 1800,false},   //180.0°
    {28, 3200, 1900,false},   //190.0°
    {29, 3300, 100,false},    //10.0°
    {30, 3400, 200,false},    //20.0°
    {31, 3500, 300,false},    //30.0°
    {32, 3600, 400,false},    //40.0°
    {33, 2100, 500,false},    //50.0°
    {34, 2200, 600,false},    //60.0°
    {35, 2300, 700,false},    //70.0°
    {36, 2400, 800,false},    //80.0°
    {37, 2500, 900,false},    //90.0°
    {38, 2600, 100,false},    //10.0°
    {39, 2700, 200,false},    //20.0°
    {40, 2800, 300,false},    //30.0°
    {41, 2900, 400,false},    //40.0°
    {42, 3000, 500,false},    //50.0°
    {43, 3100, 600,false},    //60.0°
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
// ================================
// CAT101 – target track message
// ================================
CAT101 makeCAT101FromTarget(const SimTarget& t) {
    CAT101 cat{};
    cat.radarid = 1;
    cat.tracknumber = t.tracknumber;
    cat.range = t.range;
    cat.azimuth = t.azimuth;   // 0.1 deg
    cat.elevation = 5;
    cat.latitude = 0;
    cat.longitude = 0;
    cat.altitude = 0;
    cat.speed = 250;
    cat.heading = 1800;        // 180.0 deg
    cat.rcs = 10;
    cat.type = 1;
    cat.real_simulation = 1;
    return cat;
}

// ================================
// CAT102 – arc (you already had this)
// ================================
CAT102 generateCAT102(int sweepAzimuth) {
    CAT102 cat{};
    cat.radarid = 1;
    cat.azimuth = sweepAzimuth;
    cat.elevation = 10;
    cat.resolution = 50;       // meters/bin
    cat.arcdatalength = 200;   // 10 km if 50 m/bin
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

// ================================
// CAT103 – cluster info example
// ================================
CAT103 generateCAT103() {
    CAT103 cat{};
    cat.radarid = 1;
    cat.clusterx = 10000;  // arbitrary units
    cat.clustery = 5000;
    return cat;
}

// ================================
// CAT104 – radar health/status
// ================================
CAT104 generateCAT104Status() {
    CAT104 cat{};
    cat.radarid = 1;
    cat.latitude = 523700000;   // e.g. 52.37 deg * 1e7
    cat.longitude = 485200000;  // e.g. 4.852 deg * 1e7
    cat.altitude = 50;          // meters
    cat.status = 0;             // 0 = OK
    cat.temperature = 25;       // °C
    cat.sectorleft = 0;
    cat.sectorright = 3599;
    cat.rpm = 12;               // rotations/min
    cat.maxrange = 20000;       // meters
    return cat;
}

// ================================
// CAT105 – parameter/LED control
// ================================
CAT105 generateCAT105() {
    CAT105 cat{};
    cat.radarid = 1;
    cat.parameter = 1; // e.g. "GAIN"
    cat.led = 1;       // ON
    return cat;
}

// ================================
// CAT106 – pointing / beam control
// ================================
CAT106 generateCAT106(int azimuth) {
    CAT106 cat{};
    cat.radarid = 1;
    cat.azimuth = azimuth;
    cat.elevation = 5;
    return cat;
}

// ================================
// CAT107 – TX control
// ================================
CAT107 generateCAT107() {
    CAT107 cat{};
    cat.radarid = 1;
    cat.tx = 1;   // TX ON
    return cat;
}

// ================================
// CAT108 – IFF / identification
// ================================
CAT108 generateCAT108(const SimTarget& t) {
    CAT108 cat{};
    cat.radarid = 1;
    cat.tracknumber = t.tracknumber;
    cat.type = 1;     // e.g. aircraft
    cat.foe = 0;      // 0 = friend, 1 = foe
    return cat;
}

// ================================
// CAT109 – simple track confirm
// ================================
CAT109 generateCAT109(const SimTarget& t) {
    CAT109 cat{};
    cat.radarid = 1;
    cat.tracknumber = t.tracknumber;
    return cat;
}

// ================================
// CAT110 – 3D Cartesian position
// ================================
CAT110 generateCAT110(const SimTarget& t) {
    CAT110 cat{};
    cat.radarid = 1;
    cat.x = t.range;   // fake mapping
    cat.y = 0;         // simplification
    cat.z = 1000;      // altitude
    return cat;
}

// ================================
// CAT111 – sector / mode control
// ================================
CAT111 generateCAT111() {
    CAT111 cat{};
    cat.radarid = 1;
    cat.sectorleft = 0;
    cat.sectorright = 3599;
    cat.rpm = 12;
    cat.mode = 1;          // e.g. "SURVEILLANCE"
    cat.enable_disable = 1;
    return cat;
}

// ================================
// CAT112 – wind / motion vector
// ================================
CAT112 generateCAT112() {
    CAT112 cat{};
    cat.radarid = 1;
    cat.speed = 15;       // m/s
    cat.direction = 900;  // 90.0°
    return cat;
}

// ================================
// CAT113 – threshold setting
// ================================
CAT113 generateCAT113() {
    CAT113 cat{};
    cat.radarid = 1;
    cat.threshold = 42;   // arbitrary
    return cat;
}

// ================================
// CAT114 – message counter / heartbeat
// ================================
CAT114 generateCAT114(int msgNumber) {
    CAT114 cat{};
    cat.radarid = 1;
    cat.messagenumber = msgNumber;
    return cat;
}


// -------------------------------
// Broadcaster loop
// -------------------------------
void radarBroadcasterLoop(int interval_ms) {
    Radar radar;
    int sweepAzimuth = 0;
    int msgCounter = 0;

    while (g_running) {
        ++msgCounter;

        // ---------------------
        // CAT102 – rotating sweep
        // ---------------------
        CAT102 sweep = generateCAT102(sweepAzimuth);
        std::vector<uint8_t> payload102;
        radar.encode(sweep, payload102);
        auto frame102 = buildFrame(102, payload102);

        std::cout << "[RADAR] CAT102: arc len=" << sweep.arcdata.size()
                  << " payload=" << payload102.size()
                  << " frame=" << frame102.size() << "\n";

        broadcastToRadarClients(frame102);

        // ---------------------
        // CAT101 – detection-based tracks
        // ---------------------
        for (auto& t : g_targets) {
            if (!t.detectedThisRotation &&
                sweepHitsTarget(sweepAzimuth, t.azimuth)) {

                CAT101 track = makeCAT101FromTarget(t);
                std::vector<uint8_t> payload101;
                radar.encode(track, payload101);
                broadcastToRadarClients(buildFrame(101, payload101));

                // Optional: also send CAT108 (IFF) and CAT109 (track confirm)
                CAT108 cat108 = generateCAT108(t);
                std::vector<uint8_t> payload108;
                radar.encode(cat108, payload108);
                broadcastToRadarClients(buildFrame(108, payload108));

                CAT109 cat109 = generateCAT109(t);
                std::vector<uint8_t> payload109;
                radar.encode(cat109, payload109);
                broadcastToRadarClients(buildFrame(109, payload109));

                t.detectedThisRotation = true;
            }
        }

        // ---------------------
        // Low-rate "system" messages
        // ---------------------

        // Every 10 sweeps: send radar status (CAT104) and control (CAT111)
        if (msgCounter % 10 == 0) {
            CAT104 status = generateCAT104Status();
            std::vector<uint8_t> p104;
            radar.encode(status, p104);
            broadcastToRadarClients(buildFrame(104, p104));

            CAT111 ctrl = generateCAT111();
            std::vector<uint8_t> p111;
            radar.encode(ctrl, p111);
            broadcastToRadarClients(buildFrame(111, p111));
        }

        // Every 20 sweeps: send environment/motion (CAT112) and threshold (CAT113)
        if (msgCounter % 20 == 0) {
            CAT112 wind = generateCAT112();
            std::vector<uint8_t> p112;
            radar.encode(wind, p112);
            broadcastToRadarClients(buildFrame(112, p112));

            CAT113 thr = generateCAT113();
            std::vector<uint8_t> p113;
            radar.encode(thr, p113);
            broadcastToRadarClients(buildFrame(113, p113));
        }

        // Every 30 sweeps: send TX status (CAT107) and parameter (CAT105)
        if (msgCounter % 30 == 0) {
            CAT107 tx = generateCAT107();
            std::vector<uint8_t> p107;
            radar.encode(tx, p107);
            broadcastToRadarClients(buildFrame(107, p107));

            CAT105 param = generateCAT105();
            std::vector<uint8_t> p105;
            radar.encode(param, p105);
            broadcastToRadarClients(buildFrame(105, p105));
        }

        // Always send heartbeat/message counter (CAT114)
        {
            CAT114 hb = generateCAT114(msgCounter);
            std::vector<uint8_t> p114;
            radar.encode(hb, p114);
            broadcastToRadarClients(buildFrame(114, p114));
        }

        // Optional: CAT103 and CAT110 from a sample target
        if (!g_targets.empty() && msgCounter % 15 == 0) {
            const auto& t = g_targets.front();

            CAT103 c103 = generateCAT103();
            std::vector<uint8_t> p103;
            radar.encode(c103, p103);
            broadcastToRadarClients(buildFrame(103, p103));

            CAT110 c110 = generateCAT110(t);
            std::vector<uint8_t> p110;
            radar.encode(c110, p110);
            broadcastToRadarClients(buildFrame(110, p110));
        }

        // ---------------------
        // Advance sweep
        // ---------------------
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
