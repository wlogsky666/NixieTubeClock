#ifndef BUZZER_H // 如果沒有定義過 BUZZER_H
#define BUZZER_H // 就定義它

#include <Arduino.h>

namespace Buzzer {

struct Note {
  int frequency;
  int duration;
};

class Player {
public:
  Player(uint8_t pin);

  void init();

  void play(const Note *melody, int length);

  void playShort(const Note *note, int length);

  void run();

private:
  struct MelodyState {
    explicit MelodyState(const Note *m, int length, int s)
        : melody(m), len(length), step(s) {}

    const Note *melody;
    int len;
    int step;
  };

  const uint8_t pin_;
  MelodyState current_melody_{nullptr, 0, -1};
  MelodyState saved_melody_{nullptr, 0, -1};
  unsigned long last_note_ms_ = 0;
  unsigned long current_note_duration_ = 0;
  unsigned long saved_remaining_note_time_ = 0;

  void playTone(const Note &note);
};

} // namespace Buzzer

#endif // BUZZER_H