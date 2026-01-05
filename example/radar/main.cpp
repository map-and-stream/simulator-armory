// #include <csignal>
// #include <thread>

// #include "../../src/third-party/simulator/radar/radar_simulator.h"


// int main() {
//     std::signal(SIGINT, handleSignal);
//     std::signal(SIGTERM, handleSignal);

//     std::thread serverThread(startRadarServer);
//     std::thread broadcasterThread(radarBroadcasterLoop, 1000);

//     serverThread.join();
//     broadcasterThread.join();

//     return 0;
// }
#include <csignal>
#include <thread>

#include "radar_simulator.h"

int main() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    std::thread serverThread(startRadarServer);
    std::thread broadcasterThread(radarBroadcasterLoop, 100);

    serverThread.join();
    broadcasterThread.join();
    return 0;
}
