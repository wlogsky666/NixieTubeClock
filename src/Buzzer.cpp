#include <Arduino.h>

#include "Buzzer.h"
#include "Config.h"

namespace Buzzer {

Player::Player(uint8_t pin = CONFIG::BUZZER_PIN) : pin_(pin) {}

void Player::init() {
  pinMode(pin_, OUTPUT);
  noTone(pin_);
}

void Player::play(const Note *melody, int length) {
  if (!(melody && length > 0)) {
    return;
  }

  current_melody_ = MelodyState(melody, length, 0);
  saved_melody_ = MelodyState(nullptr, 0, -1);
  playTone(current_melody_.melody[0]);
}

void Player::playShort(const Note *note, int length) {
  if (!note) {
    return;
  }

  if (current_melody_.melody && current_melody_.melody != note) {
    saved_melody_ = current_melody_;
    if (next_note_time_ > millis()) {
      saved_remaining_note_time_ = next_note_time_ - millis();
    } else {
      saved_remaining_note_time_ = 0;
    }
  }

  current_melody_ = MelodyState(note, length, 0);
  playTone(current_melody_.melody[0]);
}

void Player::run() {
  if (current_melody_.melody == nullptr) {
    return;
  }

  if (millis() >= next_note_time_) {
    current_melody_.step++;
    if (current_melody_.step < current_melody_.len) {
      playTone(current_melody_.melody[current_melody_.step]);
    } else {
      // Resuming interrupted melody if existed
      if (saved_melody_.melody != nullptr) {
        current_melody_ = saved_melody_;
        saved_melody_ = MelodyState(nullptr, 0, -1);

        playTone(current_melody_.melody[current_melody_.step]);
        next_note_time_ = millis() + saved_remaining_note_time_;
      } else {
        noTone(pin_);
        current_melody_ = MelodyState(nullptr, 0, -1);
      }
    }
  }
}

void Player::playTone(const Note &note) {
  if (note.frequency > 0) {
    tone(pin_, note.frequency);
  } else {
    noTone(pin_);
  }
  next_note_time_ = millis() + note.duration;
}

} // namespace Buzzer