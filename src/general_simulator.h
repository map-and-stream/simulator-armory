#pragma once

#include <string>

#include "simulator.h"

class GeneralSimulator : public ISimulator {
  public:
    GeneralSimulator(SimulatorConfig cfg) : ISimulator(cfg) {}
    ~GeneralSimulator();
};
