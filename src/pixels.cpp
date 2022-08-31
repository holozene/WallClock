#include "pixels.h"

Adafruit_NeoPixel strip(512, 13, NEO_GRB + NEO_KHZ800);

int xyToIndex(short x, short y)
{
  return x * 8 + ((x % 2 == 1) ? 7 - y : y);
}

void showDigit(short digit, int startX, int startY, uint32_t color)
{
  for (int y = 0; y < 5; y++)
    for (int x = 0; x < 3; x++)
    {
      if (nums[digit][x + y * 3])
        strip.setPixelColor(xyToIndex(startX + x, startY + y), color);
      else
        strip.setPixelColor(xyToIndex(startX + x, startY + y), 0);
    }
}

void showDay(short day, int startX, int startY, uint32_t color)
{
  for (int y = 0; y < 5; y++)
    for (int x = 0; x < 6; x++)
    {
      if (days[day][x + y * 6])
        strip.setPixelColor(xyToIndex(startX + x, startY + y), color);
      else
        strip.setPixelColor(xyToIndex(startX + x, startY + y), 0);
    }
}

void testColors() {
    // test color Pallette
    for (int x = 0; x < 15; x++)
      strip.setPixelColor(xyToIndex(31 + x, 4), minuteColors[x]);
}