#include "display.h"
#include "config.h"
#include <Adafruit_NeoPixel.h>

// Include extern declarations or pass as parameters
Adafruit_NeoPixel _strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t _neutralWhite = 0xFCFAF6;
constexpr int _numberOfDigits = 4;
constexpr int _numberOfSegments = 7;

// Segment mapping for all 4 digits (from original sketch)
const uint16_t _segGroups[_numberOfDigits * _numberOfSegments][_numberOfDigits] = {
  {    4,   7,  16,  19 }, {    0,   3,  20,  23 }, {  232, 235, 236, 239 }, {  228, 231, 240, 243 }, {  224, 227, 244, 247 }, {    8,  11,  12,  15 }, {   24,  27, 248, 251 },
  {   34,  37,  46,  49 }, {   30,  33,  50,  53 }, {  202, 205, 206, 209 }, {  198, 201, 210, 213 }, {  194, 197, 214, 217 }, {   38,  41,  42,  45 }, {   54,  57, 218, 221 }, 
  {   72,  75,  84,  87 }, {   68,  71,  88,  91 }, {  164, 167, 168, 171 }, {  160, 163, 172, 175 }, {  156, 159, 176, 179 }, {   76,  79,  80,  83 }, {   92,  95, 180, 183 },
  {  102, 105, 114, 117 }, {   98, 101, 118, 121 }, {  134, 137, 138, 141 }, {  130, 133, 142, 145 }, {  126, 129, 146, 149 }, {  106, 109, 110, 113 }, {  122, 125, 150, 153 }
};
const uint8_t _digitPositions[_numberOfDigits] = {3, 2, 1, 0}; // rightmost is 0

// colon between minutes and seconds (if needed)
const uint16_t _segGroupsColons[12] = {
  60, 61, 62, 63, 64, 65,
  186, 187, 188, 189, 190, 191 
};

const byte _digits[10][_numberOfSegments] = {
  {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1}, {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
};

void initialize() {
  _strip.begin();
  _strip.show(); // Initialize all pixels to 'off'
}

// Helper to convert 0xRRGGBB to NeoPixel color
uint32_t hexToColor(uint32_t hex) {
  uint8_t r = (hex >> 16) & 0xFF;
  uint8_t g = (hex >> 8) & 0xFF;
  uint8_t b = hex & 0xFF;
  return _strip.Color(r, g, b);
}

uint32_t interpolateColor(uint32_t c1, uint32_t c2, float t) {
  uint8_t r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
  uint8_t r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;
  uint8_t r = r1 + (r2 - r1) * t;
  uint8_t g = g1 + (g2 - g1) * t;
  uint8_t b = b1 + (b2 - b1) * t;
  return _strip.Color(r, g, b);
}

uint32_t GetColor(int theme, TriState nightMode)
{
  if (_buttonMode == ButtonMode::BRIGHTNESS )
    return _neutralWhite;

  unsigned long nowMillis = millis();
  const std::vector<uint32_t>& colorList = _colorThemes[theme];

  uint32_t currentColor = _colorThemes[theme][0];
  
  if (nightMode == TriState::False)
  {
    int numColorsPerTheme = colorList.size();
    int colorIdx = (nowMillis / _transitionTime) % numColorsPerTheme;
    int nextIdx = (colorIdx + 1) % numColorsPerTheme;
    float t = (nowMillis % _transitionTime) / (float)_transitionTime;
    uint32_t c1 = hexToColor(colorList[colorIdx]);
    uint32_t c2 = hexToColor(colorList[nextIdx]);
    currentColor = interpolateColor(c1, c2, t);
  }


  return currentColor;
}

void showSegment(uint8_t segment, uint8_t pos, uint32_t color, uint8_t brightness) {
  if (pos >= _numberOfDigits || segment >= _numberOfSegments) 
    return;

  uint8_t idx = segment + _digitPositions[pos] * _numberOfSegments;
  if (idx >= _numberOfDigits * _numberOfSegments) 
    return;

  for (int i = _segGroups[idx][0]; i <= _segGroups[idx][1]; i++)
  {
    if (i >= NUM_LEDS) 
      continue;

    _strip.setPixelColor(i, color);
    _strip.setBrightness(brightness);
  }
  for (int i = _segGroups[idx][2]; i <= _segGroups[idx][3]; i++)
  {
    if (i >= NUM_LEDS) 
      continue;

    _strip.setPixelColor(i, color);
    _strip.setBrightness(brightness);
  }
}

void showColons(uint32_t color, uint8_t brightness) {
  for (int i = 0; i < _colonCount; i++) {
    if (_segGroupsColons[i] >= NUM_LEDS) 
      continue;

    _strip.setPixelColor(_segGroupsColons[i], color);
    _strip.setBrightness(brightness);
  }
}

void showStaticClock(const std::vector<int>& digits_to_display, uint32_t color, uint8_t brightness)
{
  _strip.clear();
  for (int d = 0; d < _numberOfDigits; d++) {
    if (d >= (int)digits_to_display.size()) 
      continue;

    int digitIdx = digits_to_display[d];
    if (digitIdx < 0 || digitIdx >= 10) 
      continue;

    for (int s = 0; s < _numberOfSegments; s++) {
      if (_digits[digitIdx][s]) {
        showSegment(s, d, color, brightness);
      }
    }
  }
  showColons(color, brightness);

  _strip.show();
}

void showWormMode(const std::vector<int>& digits_to_display, uint32_t color, uint8_t brightness)
{
  _strip.clear();
  
  // Get all active LED indices for the current time
  std::vector<int> activeLeds;
  for (int d = 0; d < _numberOfDigits; d++) {
    if (d >= (int)digits_to_display.size()) continue;
    
    int digitIdx = digits_to_display[d];
    if (digitIdx < 0 || digitIdx >= 10) continue;
    
    for (int s = 0; s < _numberOfSegments; s++) {
      if (_digits[digitIdx][s]) {
        uint8_t idx = s + _digitPositions[d] * _numberOfSegments;
        if (idx < _numberOfDigits * _numberOfSegments) {
          for (int i = _segGroups[idx][0]; i <= _segGroups[idx][1]; i++) {
            if (i < NUM_LEDS) activeLeds.push_back(i);
          }
          for (int i = _segGroups[idx][2]; i <= _segGroups[idx][3]; i++) {
            if (i < NUM_LEDS) activeLeds.push_back(i);
          }
        }
      }
    }
  }
  
  // Add colons
  for (int i = 0; i < _colonCount; i++) {
    if (_segGroupsColons[i] < NUM_LEDS) activeLeds.push_back(_segGroupsColons[i]);
  }
  
  // Calculate worm position (completes one cycle per 5 seconds = 5000ms)
  unsigned long cycleMillis = (millis() % 5000);
  int wormPosition = (cycleMillis / (5000 / NUM_LEDS)) % NUM_LEDS;
  
  // Draw worm with fade trail
  for (int i = 0; i < _wormLength; i++) {
    int ledIdx = (wormPosition + i) % NUM_LEDS;
    
    // Check if this LED is part of the active time display
    bool isActive = false;
    for (int activeLed : activeLeds) {
      if (activeLed == ledIdx) {
        isActive = true;
        break;
      }
    }
    
    if (isActive) {
      // Brightest at the front of the worm, dimmer towards the tail
      float brightness = 1.0f - (float)i / _wormLength;
      uint8_t r = ((color >> 16) & 0xFF) * brightness;
      uint8_t g = ((color >> 8) & 0xFF) * brightness;
      uint8_t b = (color & 0xFF) * brightness;
      uint32_t dimmedColor = _strip.Color(r, g, b);
      _strip.setPixelColor(ledIdx, dimmedColor);
    }
  }
  
  _strip.setBrightness(brightness);

  _strip.show();
}