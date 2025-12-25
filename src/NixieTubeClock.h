#ifndef NIXIE_TUBE_CLOCK_H
#define NIXIE_TUBE_CLOCK_H

#include "NixieTube.h"

class NixieTubeClock {
public:
  NixieTubeClock();

  void init();

  void test();

private:
  const char *TAG = "NixieTubeClock";
  NixieTube::Controller nt_ctrl_;
};

#endif // NIXIE_TUBE_CLOCK_H
