#include "secrets.h"
#include "webpage.h"
#include "pixels.h"

#include <AsyncElegantOTA.h>
#include <ezTime.h>

Timezone Pacific;

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
  strip.show();
}

void showDay(int startX, int startY, uint32_t color)
{
  for (int y = 0; y < 5; y++)
    for (int x = 0; x < 6; x++)
    {
      if (days[Pacific.weekday()][x + y * 6])
        strip.setPixelColor(xyToIndex(startX + x, startY + y), color);
      else
        strip.setPixelColor(xyToIndex(startX + x, startY + y), 0);
    }
}

void showSchedule()
{
  for (int i = 0; i < 64; i++)
  {
    switch (schedule[i])
    {
    case 1:
      strip.setPixelColor(xyToIndex(i, 6), neutral);
      break;
    default:
      strip.setPixelColor(xyToIndex(i, 6), 0);
      break;
    }
  }
}

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

void barClock()
{
  showSchedule();
  int endX = (Pacific.hour() + 24 - wakeHour) % 24 * 4 + Pacific.minute() / 15;
  for (int x = 0; x < endX - 1; x++)
    strip.setPixelColor(xyToIndex(x, 7), neutral);
  strip.setPixelColor(xyToIndex(endX, 7), minuteColors[Pacific.minute() % 15]);
}

void pixelClock()
{
  showSchedule();
  int x = (Pacific.hour() + 24 - wakeHour) % 24 * 4 + Pacific.minute() / 15;
  if (x > 0)
    strip.setPixelColor(xyToIndex(x - 1, 7), 0);
  strip.setPixelColor(xyToIndex(x, 7), minuteColors[Pacific.minute() % 15]);
}

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
  showDay(20, 0, neutral);
}

void setup(void)
{
  // Start Strip
  strip.begin();
  strip.clear();
  strip.show();
  strip.setBrightness(50);

  // Start Serial
  Serial.begin(115200);

  // Connect AsyncElegantOTA to Server
  AsyncElegantOTA.begin(&server);
  // Start Server
  startServer();

  // Setup ezTime
  waitForSync();
  Pacific.setLocation("America/Los_Angeles");
  Pacific.setDefault();
  Serial.print("Time and Date: ");
  Serial.println(Pacific.dateTime("l, d-M-y H:i:s.v T"));
}

void loop(void)
{
  // from 9 hours until wakeHour to wakeHour, turn off the clock
  if (Pacific.hour() < wakeHour && Pacific.hour() > (wakeHour - 9) % 24)
  {
    strip.clear();
    strip.show();
    delay(10000);
  }
  // while awake, update the clock display every 1/3 of a second
  else
  {
    digitClock();
    pixelClock();
    strip.show();
    delay(333);
  }
}
