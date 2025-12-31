#include <Arduino.h>

#include "Buzzer.h"
#include "Config.h"

namespace Buzzer {

Player::Player(uint8_t pin = CONFIG::BUZZER_PIN) : pin_(pin) {}

void Player::init() {
  pinMode(pin_, OUTPUT);
  noTone(pin_);
}

void Player::_playInternal(const Note *melody, int length) {
  if (!(melody && length > 0)) {
    return;
  }

  current_melody_ = MelodyState(melody, length, 0);
  saved_melody_ = MelodyState(nullptr, 0, -1);
  is_playing_melody_ = true;
  _playTone(current_melody_.melody[0]);
}

void Player::_playShortInternal(const Note *note, int length,
                                uint8_t priority) {
  if (!note) {
    return;
  }

  unsigned long now = millis();
  if (current_melody_.melody && !is_playing_melody_) {
    if (current_priority_ > priority) {
      return;
    }
  }

  if (current_melody_.melody && is_playing_melody_) {
    saved_melody_ = current_melody_;
    unsigned long elapsed = now - last_note_ms_;
    saved_remaining_note_time_ = (elapsed < current_note_duration_)
                                     ? (current_note_duration_ - elapsed)
                                     : 0;
  }

  current_melody_ = MelodyState(note, length, 0);
  is_playing_melody_ = false;
  _playTone(current_melody_.melody[0]);
}

void Player::run() {
  if (current_melody_.melody == nullptr) {
    return;
  }

  unsigned long now = millis();
  if (now - last_note_ms_ >= current_note_duration_) {
    current_melody_.step++;
    if (current_melody_.step < current_melody_.len) {
      _playTone(current_melody_.melody[current_melody_.step]);
    } else {
      // Resuming interrupted melody if existed
      if (saved_melody_.melody != nullptr) {
        current_melody_ = saved_melody_;
        saved_melody_ = MelodyState(nullptr, 0, -1);

        _playTone(current_melody_.melody[current_melody_.step]);
        current_note_duration_ = saved_remaining_note_time_;
        is_playing_melody_ = true;
      } else {
        noTone(pin_);
        current_melody_ = MelodyState(nullptr, 0, -1);
        current_priority_ = Priority::P_LOW;
      }
    }
  }
}

void Player::_playTone(const Note &note) {
  noTone(pin_);
  if (note.frequency > 0) {
    tone(pin_, note.frequency);
  } else {
    noTone(pin_);
  }
  last_note_ms_ = millis();
  current_note_duration_ = note.duration;
}

} // namespace Buzzer