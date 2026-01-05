#include "radar_simulator.h"

#include <csignal>
#include <iostream>
#include <thread>

int main() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    std::cout << "[MAIN] Starting radar simulator...\n";

    // Start server thread
    std::thread serverThread(startRadarServer);

    // Start broadcaster loop (e.g., 100 ms sweep step)
    radarBroadcasterLoop(100);

    if (serverThread.joinable()) {
        serverThread.join();
    }

    std::cout << "[MAIN] Radar simulator stopped.\n";
    return 0;
}
