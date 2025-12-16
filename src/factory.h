#pragma once

#include <string.h>

#include "general_simulator.h"
#include "simulator.h"

class SimulatorFactory {
  public:
    static ISimulator* createLogger(SimulatorType type, SimulatorConfig cfg) {
        if (type == SimulatorType::General) {
            return new GeneralSimulator(cfg);
        } else {
            throw std::invalid_argument("Invalid logger type");
        }
    }
};