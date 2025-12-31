#ifndef BUZZER_H // 如果沒有定義過 BUZZER_H
#define BUZZER_H // 就定義它

#include <Arduino.h>

namespace Buzzer {

struct Note {
  int frequency;
  int duration;
};

enum Priority { P_LOW = 0, P_MEDIUM = 1, P_HIGH = 2 };

class Player {
public:
  Player(uint8_t pin);

  void init();

  /**
   * @brief Plays a full melody
   * Note the melody will be interrupted by playing sound effect with
   * playShort(), and resuming after sound effect got played.
   */
  template <size_t N> void play(const Note (&melody)[N]) {
    _playInternal(melody, N);
  }

  /**
   * @brief Plays a short UI sound effect with priority handling.
   */
  template <size_t N>
  void playShort(const Note (&note)[N], uint8_t priority = Priority::P_LOW) {
    _playShortInternal(note, N, priority);
  }

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
  boolean is_playing_melody_ = false;
  Priority current_priority_ = Priority::P_LOW;

  void _playTone(const Note &note);
  void _playInternal(const Note *melody, int length);
  void _playShortInternal(const Note *note, int length, uint8_t priority);
};

} // namespace Buzzer

#endif // BUZZER_H