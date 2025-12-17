// radar_simulator.h  (you just posted)
#pragma once

#include <vector>
#include <mutex>
#include <cstdint>

#include "../../protocol-armory/src/radar/radar.h"
#include "../../protocol-armory/src/radar/struct.h"

// Globals
extern std::vector<int> g_radarClients;
extern std::mutex g_radarMutex;
extern bool g_running;

// Functions
void startRadarServer();
void radarBroadcasterLoop(int interval_ms);

std::vector<uint8_t> buildFrame(uint8_t type, const std::vector<uint8_t>& payload);
void broadcastToRadarClients(const std::vector<uint8_t>& frame);
void connectionHandler(int clientSock);
CAT101 generateTestCAT101();
void handleSignal(int);
