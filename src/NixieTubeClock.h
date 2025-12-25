#ifndef NIXIE_TUBE_CLOCK_H
#define NIXIE_TUBE_CLOCK_H

#include "ClockSystem.h"
#include "NixieTube.h"

class NixieTubeClock : public ClockSystem::ITickCallback {
public:
  NixieTubeClock();

  void init();

  void onTick() override;

  void test();

private:
  const char *TAG = "NixieTubeClock";
  ClockSystem::RtcController &rtc_ctrl_;
  NixieTube::Controller nt_ctrl_;
  volatile bool need_update_ = false;
};

#endif // NIXIE_TUBE_CLOCK_H
