#ifndef PIXELS_H
#define PIXELS_H

#include <Adafruit_NeoPixel.h>

// x must be less than 64
// y must be less than 8

// c 0 1  2  3  4  5  6
//   0 15 16 31 32 47 48
//   1 14 17 30 33 46 49
//   2 13 18 29 34 45 50
//   3 12 19 28 35 44 51
//   4 11 20 27 36 43 52
//   5 10 21 26 37 42 53
//   6 9  22 25 38 41 54
//   7 8  23 24 39 40 55
extern Adafruit_NeoPixel strip;

// pixel vars
const bool nums[10][15] = {
    {1, 1, 1, // 0
     1, 0, 1,
     1, 0, 1,
     1, 0, 1,
     1, 1, 1},
    {0, 1, 0, // 1
     1, 1, 0,
     0, 1, 0,
     0, 1, 0,
     1, 1, 1},
    {1, 1, 1, // 2
     0, 0, 1,
     1, 1, 1,
     1, 0, 0,
     1, 1, 1},
    {1, 1, 1, // 3
     0, 0, 1,
     1, 1, 1,
     0, 0, 1,
     1, 1, 1},
    {1, 0, 1, // 4
     1, 0, 1,
     1, 1, 1,
     0, 0, 1,
     0, 0, 1},
    {1, 1, 1, // 5
     1, 0, 0,
     1, 1, 1,
     0, 0, 1,
     1, 1, 1},
    {1, 1, 1, // 6
     1, 0, 0,
     1, 1, 1,
     1, 0, 1,
     1, 1, 1},
    {1, 1, 1, // 7
     0, 0, 1,
     0, 0, 1,
     0, 0, 1,
     0, 0, 1},
    {1, 1, 1, // 8
     1, 0, 1,
     1, 1, 1,
     1, 0, 1,
     1, 1, 1},
    {1, 1, 1, // 9
     1, 0, 1,
     1, 1, 1,
     0, 0, 1,
     1, 1, 1}};

const bool days[8][30] = {
    {1, 1, 1, 0, 0, 0, // Er (error)
     1, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1,
     1, 0, 0, 1, 0, 0,
     1, 1, 1, 1, 0, 0},
    {1, 1, 1, 0, 0, 0, // Su
     1, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 0, 1,
     0, 0, 1, 1, 0, 1,
     1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, // M
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0},
    {1, 1, 1, 0, 0, 0, // Tu
     0, 1, 0, 0, 0, 0,
     0, 1, 0, 1, 0, 1,
     0, 1, 0, 1, 0, 1,
     0, 1, 0, 1, 1, 1},
    {1, 0, 1, 0, 1, 0, // W
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0,
     1, 0, 1, 0, 1, 0,
     1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 0, 0, // Th
     0, 1, 0, 1, 0, 0,
     0, 1, 0, 1, 1, 1,
     0, 1, 0, 1, 0, 1,
     0, 1, 0, 1, 0, 1},
    {1, 1, 1, 0, 0, 0, // Fr
     1, 0, 0, 0, 0, 0,
     1, 1, 0, 1, 1, 1,
     1, 0, 0, 1, 0, 0,
     1, 0, 0, 1, 0, 0},
    {1, 1, 1, 0, 0, 0, // St
     1, 0, 0, 0, 1, 0,
     1, 1, 1, 1, 1, 1,
     0, 0, 1, 0, 1, 0,
     1, 1, 1, 0, 1, 1}};

// color vars
const uint32_t neutral = strip.Color(6, 6, 6);

const uint32_t minuteColors[15] = {
    strip.Color(255, 12, 0),   // 0  red
    strip.Color(237, 36, 0),   // 1  orange
    strip.Color(219, 73, 0),   // 2  yellow-orange
    strip.Color(200, 109, 0),  // 3  yellow
    strip.Color(164, 146, 0),  // 4  light yellow
    strip.Color(109, 182, 0),  // 5  yellow-green
    strip.Color(55, 200, 0),   // 6  light green
    strip.Color(36, 219, 0),   // 7  green
    strip.Color(18, 255, 36),  // 8  cyan
    strip.Color(36, 219, 73),  // 9  kinda blue
    strip.Color(55, 164, 128), // 10 blue
    strip.Color(73, 109, 200), // 11 kinda blue
    strip.Color(91, 73, 255),  // 12 blue-purple
    strip.Color(109, 36, 191), // 13 purple
    strip.Color(128, 12, 128)  // 14 violet
};

int xyToIndex(short x, short y);

void showDigit(short digit, int startX, int startY, uint32_t color);

void showDay(short day, int startX, int startY, uint32_t color);

#endif /* PIXELS_H */