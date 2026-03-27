#include <vector>
#include <time.h>
#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "network.h"
#include "display.h"
#include "config.h"

TriState _nightMode = TriState::Unknown; // Unknown at start, will be set in loop
uint8_t _currentBrightness = _brightnessMedium;

const int _numThemes = _colorThemes.size();
int _currentTheme = 0;

// Button state variables
bool _buttonState = false;
bool _lastButtonState = false;
bool _toggledState = false; // This is the variable to toggle

const uint32_t _pastelRed = 0xFF443C; // pastel red
const uint32_t _pastelYellow = 0xFF953B; // pastel yellow

// Germany TZ string: CET/CEST with automatic DST
BrightnessLevel _brightnessLevel = MEDIUM;
ConnectionState _connectionState = NONE;

std::array<int, 4> getTime()
{
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  int h = timeinfo.tm_hour;
  int m = timeinfo.tm_min;

  // 12h mode conversion
  if (!_use24h) {
    h = h % 12;
    if (h == 0) h = 12;
  }

  std::array<int, 4>  digits_to_display = { h / 10, h % 10, m / 10, m % 10 };

  // Hide leading zero in hour if needed
  if (!_showLeadingZero && digits_to_display[0] == 0)
    digits_to_display[0] = -1; // Use -1 as "blank"
  
  return digits_to_display;
}

void toggleColorTheme()
{
  _currentTheme = (_currentTheme + 1) % _numThemes;
}

void SetBrightness(BrightnessLevel level, bool nightMode)
{
  _currentBrightness = level;
  if (_nightMode == TriState::True && _allowNightMode)
    _currentBrightness *= 0.50f;
}

void toggleBrightness(bool nightTime = false)
{
  if (_brightnessLevel == MIN)
    _brightnessLevel = MEDIUM;
  else if (_brightnessLevel == MEDIUM)
    _brightnessLevel = MAX;
  else
    _brightnessLevel = MIN;

  SetBrightness(_brightnessLevel, _nightMode == TriState::True);
}

void toggleButtonState(bool value)
{
  if (value && !_buttonState)
  {
    if (_buttonMode == ButtonMode::COLOR && _nightMode == TriState::False)
      toggleColorTheme();
    else
      toggleBrightness(_allowNightMode);
  }
  _buttonState = value;
}

void listenToButtonPress()
{
  // Button logic: detect touch start (rising edge from HIGH to LOW) with debounce
  static unsigned long lastDebounceTime = 0;
  bool value = digitalRead(BUTTON_PIN) == LOW;
  if (value != _lastButtonState)
    lastDebounceTime = millis();

  if ((millis() - lastDebounceTime) > _debounceDelayMs)
    toggleButtonState(value);
  _lastButtonState = value;
}

void AdjustNightMode(int hour)
{
    if (hour >= _beginNightHour && _nightMode != TriState::True)
    {
        _nightMode = TriState::True;
        SetBrightness(_brightnessLevel, _nightMode == TriState::True);
    }
    else if (hour < _endNightHour && _nightMode != TriState::False)
    {
        _nightMode = TriState::False;
        SetBrightness(_brightnessLevel, _nightMode == TriState::True);
    }
}

void ShowWifiConnectionState()
{
    uint32_t color = hexToColor(_pastelRed);
    showWormMode(color, _brightnessMedium, 5000);
}

void ShowTimeSyncState()
{
    uint32_t color = hexToColor(_pastelYellow);
    showWormMode(color, _brightnessMedium, 5000);
}

void setup() 
{
  Serial.begin(115200);
  initialize();
  initializeWifi();
  connectWiFi();

  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button pin as input with pullup
}

void loop() 
{
  _connectionState = checkTimeSync();

  if (_connectionState == NONE)
  {
    ShowWifiConnectionState();
    return;
  }
  else if (_connectionState == WIFI_CONNECTED)
  {
    ShowTimeSyncState();
    return;
  }


  listenToButtonPress();

  // Build digits_to_display as std::vector
  int hour, m;
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  hour = timeinfo.tm_hour;
  m = timeinfo.tm_min;
  if (!_use24h) {
    hour = hour % 12;
    if (hour == 0) hour = 12;
  }

  if (_allowNightMode)
    AdjustNightMode(hour);

  std::vector<int> digits_to_display = { hour / 10, hour % 10, m / 10, m % 10 };
  if (!_showLeadingZero && digits_to_display[0] == 0) {
    digits_to_display[0] = -1;
  }

  uint32_t currentColor = GetColor(_currentTheme, _nightMode);

  showStaticClock(digits_to_display, currentColor, _currentBrightness);

  delay(100);
}