#include "webpage.h"
#include "pixels.h"
#include "clock.h"

Timezone Pacific;

IPAddress store;
// use only after WiFi has been setup
void IP(int startX)
{
  if (WiFi.localIP() == store)
    return;

  store = WiFi.localIP();
  int x = startX;
  for (char c : WiFi.localIP().toString())
  {
    if (c == '.')
    {
      strip.setPixelColor(xyToIndex(x, 5), minuteColors[14]);
      x += 2;
    }
    else
    {
      showDigit(c - '0', x, 1, minuteColors[14]);
      x += 4;
    }
  }
}

// show
void showSchedule()
{
  for (int i = 0; i < 64; i++)
  {
    switch (schedule[i])
    {
    case 0:
      strip.setPixelColor(xyToIndex(i, 6), 0);
      break;
    default:
      strip.setPixelColor(xyToIndex(i, 6), neutral);
      break;
    }
  }
}

// fill the bottom row throughout the day, with the end pixel showing minute with color
void barClock()
{
  showSchedule();
  int endX = (Pacific.hour() + 24 - wakeHour) % 24 * 4 + Pacific.minute() / 15;
  for (int x = 0; x < endX - 1; x++)
    strip.setPixelColor(xyToIndex(x, 7), neutral);
  strip.setPixelColor(xyToIndex(endX, 7), minuteColors[Pacific.minute() % 15]);
}

// show movement across the day, with the pixel showing minute with color
void pixelClock()
{
  showSchedule();
  int x = (Pacific.hour() + 24 - wakeHour) % 24 * 4 + Pacific.minute() / 15;
  if (x > 0)
    strip.setPixelColor(xyToIndex(x - 1, 7), 0);
  strip.setPixelColor(xyToIndex(x, 7), minuteColors[Pacific.minute() % 15]);
}

// show the current time and day of week on the upper left
void digitClock()
{
  if (Pacific.hour() >= 10)
    showDigit(Pacific.hour() / 10, 0, 0, neutral);
  else
    for (int y = 0; y < 5; y++)
      for (int x = 0; x < 3; x++)
        strip.setPixelColor(xyToIndex(x, y), 0);
  showDigit(Pacific.hour() % 10, 4, 0, neutral);
  strip.setPixelColor(65, neutral);
  strip.setPixelColor(67, neutral);
  showDigit(Pacific.minute() / 10, 10, 0, neutral);
  showDigit(Pacific.minute() % 10, 14, 0, neutral);
  showDay(Pacific.weekday(), 20, 0, neutral);
}
