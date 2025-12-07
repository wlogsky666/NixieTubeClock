#ifndef NIXIE_TUBE_CLOCK_H
#define NIXIE_TUBE_CLOCK_H

#include "ShiftRegister.h"

class NixieTubeClock {
public:
  NixieTubeClock();

  void init();

private:
  ShiftRegister::Controller sr_ctrl_;
};

#endif // NIXIE_TUBE_CLOCK_H
