#include "../../src/third-party/simulator/radar/radar_simulator.h"
#include <thread>
#include <csignal>

int main() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    std::thread serverThread(startRadarServer);
    std::thread broadcasterThread(radarBroadcasterLoop, 1000);

    serverThread.join();
    broadcasterThread.join();

    return 0;
}
