#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <stdarg.h>

#include "Config.h"

enum LogLevel {
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_NONE
};

class Logger {
public:
  static Logger &getInstance() {
    static Logger instance; // Singleton
    return instance;
  }

  void setSystemLevel(LogLevel level) { _currentSystemLevel = level; }

  void init(uint32_t baudRate = CONFIG::BAUD_RATE) {
    if (is_init_)
      return;

    Serial.begin(baudRate);

    is_init_ = true;
    info(TAG, "Serial initialized with %ld baudrate.", baudRate);
  }

  void debug(const char *tag, const char *format, ...) {
    if (_currentSystemLevel <= LOG_LEVEL_DEBUG) {
      va_list args;
      va_start(args, format);
      logFormat("DEBUG", tag, format, args);
      va_end(args);
    }
  }

  void info(const char *tag, const char *format, ...) {
    if (_currentSystemLevel <= LOG_LEVEL_INFO) {
      va_list args;
      va_start(args, format);
      logFormat("INFO", tag, format, args);
      va_end(args);
    }
  }

  void warn(const char *tag, const char *format, ...) {
    if (_currentSystemLevel <= LOG_LEVEL_WARN) {
      va_list args;
      va_start(args, format);
      logFormat("WARN", tag, format, args);
      va_end(args);
    }
  }

  void error(const char *tag, const char *format, ...) {
    if (_currentSystemLevel <= LOG_LEVEL_ERROR) {
      va_list args;
      va_start(args, format);
      logFormat("ERROR", tag, format, args);
      va_end(args);
    }
  }

private:
  const char *TAG = "Logger";
  LogLevel _currentSystemLevel = LOG_LEVEL_DEBUG;
  boolean is_init_ = false;

  Logger() {}
  Logger(Logger const &) = delete;
  void operator=(Logger const &) = delete;

  void logFormat(const char *levelStr, const char *tag, const char *format,
                 va_list args) {
    if (!is_init_) {
      return;
    }

    char buffer[128];
    vsnprintf(buffer, sizeof(buffer), format, args);

    Serial.print(F("["));
    Serial.print(levelStr);
    Serial.print(F("]["));
    Serial.print(tag);
    Serial.print(F("] "));
    Serial.println(buffer);
  }
};

#define Log Logger::getInstance()

#endif