#pragma once

#include <iostream>
#include <string>

enum class SimulatorType { General };

struct SimulatorConfig {
    int x = 0;
};

// --- Abstract Logger Interface ---
class ISimulator {
  public:
    virtual ~ISimulator() = default;
    ISimulator(SimulatorConfig cfg [[maybe_unused]]) {}

  protected:
    SimulatorConfig config;
};
