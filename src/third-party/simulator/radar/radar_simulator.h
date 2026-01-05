#pragma once

#include <cstdint>
#include <mutex>
#include <vector>

#include "../../protocol-armory/src/radar/radar.h"
#include "../../protocol-armory/src/radar/struct.h"
bool sweepHitsTarget(int sweepAz, int targetAz);
// -------------------------------
// Globals
// -------------------------------
extern std::vector<int> g_radarClients;
extern std::mutex g_radarMutex;
extern bool g_running;
// -------------------------------
// Functions
// -------------------------------
void startRadarServer();
void radarBroadcasterLoop(int interval_ms);
void handleSignal(int);

std::vector<uint8_t> buildFrame(uint8_t type,
                                const std::vector<uint8_t>& payload);
void broadcastToRadarClients(const std::vector<uint8_t>& frame);
void connectionHandler(int clientSock);
