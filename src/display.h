#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include <cstdint>
#include "config.h"

// Function declarations
uint32_t GetColor(int theme, TriState nightMode);
uint32_t hexToColor(uint32_t hex);
uint32_t interpolateColor(uint32_t c1, uint32_t c2, float t);
void initialize();
void showSegment(uint8_t segment, uint8_t pos, uint32_t color);
void showColons(uint32_t color);
void showStaticClock(const std::vector<int>& digits_to_display, uint32_t color, uint8_t brightness);
void showWormMode(const std::vector<int>& digits_to_display, uint32_t color, uint8_t brightness);

#endif