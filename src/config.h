#ifndef CONFIG_H
#define CONFIG_H

#include <time.h>
#include <vector>

constexpr int DATA_PIN = 13;
constexpr int BUTTON_PIN = 12;
constexpr int NUM_LEDS = 252;
constexpr int _wormLength = 3;
constexpr int _colonCount = 12;

constexpr bool _showLeadingZero = false; // Show leading zero in hour if true
constexpr bool _use24h = true;          // Use 24h mode if true, else 12h mode
constexpr bool _showPoints = true;      // Show points between hours and minutes

enum BrightnessLevel { MIN = 30, MEDIUM = 50, MAX = 75 };

enum class TriState { False, True, Unknown };
enum ButtonMode { COLOR, BRIGHTNESS };
constexpr ButtonMode _buttonMode = COLOR; // Button toggles color themes or brightness levels

constexpr uint8_t _brightnessMax = 75;
constexpr uint8_t _brightnessMedium = 50;
constexpr uint8_t _brightnessMin = 30;
constexpr bool _allowNightMode = true;

constexpr u_int8_t _beginNightHour = 19; // 19 PM
constexpr u_int8_t _endNightHour = 06; // 6 AM

constexpr int _transitionTime = 10000; // ms for each color transition
constexpr int _debounceDelayMs = 50;

const std::vector<std::vector<uint32_t>> _colorThemes = {
  { 0x2364aa, 0x3da5d9, 0x73bfb8, 0xfec601, 0xea7317 },
  { 0xFF6F00, 0xFFA058, 0xFFC69A, 0xFFA058, 0xFF7D19 },
  { 0xFFA700, 0xFFC558, 0xFFDC9A, 0xFFC558, 0xFFB019 },
  { 0xFE000D, 0xFE5760, 0xFF9A9F, 0xFE5760, 0xFE1924 },
  //{ 0xffffff, 0xcccccc, 0x888888, 0x444444, 0x222222 }
};

#endif