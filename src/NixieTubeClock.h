#ifndef NIXIE_TUBE_CLOCK_H
#define NIXIE_TUBE_CLOCK_H

#include "Buzzer.h"
#include "ClockSystem.h"
#include "NixieTube.h"

class NixieTubeClock : public ClockSystem::ITickCallback {
public:
  NixieTubeClock();

  void init();

  void run();

private:
  const char *TAG = "NixieTubeClock";
  Buzzer::Player buzzer_;
  ClockSystem::RtcController &rtc_ctrl_;
  NixieTube::Controller nt_ctrl_;
  volatile bool need_update_ = false;

  void onTick() override;
};

#endif // NIXIE_TUBE_CLOCK_H
